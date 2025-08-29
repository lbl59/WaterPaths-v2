//
// Created by bernardoct on 8/25/17.
//

#include <numeric>
#include <algorithm>
#include "ObjectivesCalculator.h"
#include "Utils.h"

double ObjectivesCalculator::calculateReliabilityObjective(
        const vector<UtilitiesDataCollector *>& utility_collector, vector<unsigned long> realizations) {
    unsigned long n_realizations = utility_collector.size();
    if (realizations.empty()) {
        realizations = vector<unsigned long>(n_realizations);
        iota(realizations.begin(), realizations.end(), 0);
    } else {
        n_realizations = realizations.size();
    }

    unsigned long n_weeks = utility_collector[realizations[0]]->getCombined_storage().size();
    unsigned long n_years = (unsigned long) round(n_weeks / WEEKS_IN_YEAR);

    vector<vector<int>> realizations_year_reliabilities(
            utility_collector.size(), vector<int>(n_years, NON_INITIALIZED));
    vector<int> year_reliabilities(n_years, 0);

    /// Creates a table with years that failed in each realization.
    for (const unsigned long &r : realizations) {
        for (unsigned long y = 0; y < n_years; ++y) {
            for (int w = (int) round(y * WEEKS_IN_YEAR);
                 w < (int) min((int) n_weeks, (int) round((y + 1) * WEEKS_IN_YEAR));
                 ++w) {
                if (utility_collector[r]->getCombined_storage()[w] /
                    utility_collector[r]->getCapacity()[w] <
                    STORAGE_CAPACITY_RATIO_FAIL) {
                    realizations_year_reliabilities[r][y] = FAILURE;
                }
            }
        }
    }

    /// Creates a vector with the number of realizations that failed for each year.
    for (unsigned long y = 0; y < n_years; ++y) {
        for (const unsigned long &r : realizations) {
            if (realizations_year_reliabilities[r][y]==FAILURE)
                year_reliabilities[y]++;
        }
    }

    double check_non_zero = accumulate(year_reliabilities.rbegin(),
                                       year_reliabilities.rend(),
                                       0.0);

    /// Returns year with most realization failures, divided by the number of realizations (reliability objective).
    if (check_non_zero > 0) {
        // Take the worst year (highest number of failures) across all years and realizations.
        vector<double> year_realiabilities_allreals;
        for (const unsigned long &r : realizations) {
            year_realiabilities_allreals.push_back(
                    1. - (double) *max_element(realizations_year_reliabilities[r].begin(),
                                         realizations_year_reliabilities[r].end()));
        }
        // sort to get the 90th percentile.
        sort(year_realiabilities_allreals.begin(),
             year_realiabilities_allreals.end());
        double obj_value = year_realiabilities_allreals.at(
                (unsigned long) floor(0.90 * n_realizations));

        /**
        double obj_value =
                1. - (double) *max_element(year_reliabilities.begin(),
                                           year_reliabilities.end()) /
                     n_realizations;
         */
        if (std::isinf(obj_value)) {
            string error_inf = "Infinite reliability.";
            throw logic_error(error_inf.c_str());
        } else {
            return obj_value;
        }
    } else {
        return 1.;
    }
}

double ObjectivesCalculator::calculateRestrictionFrequencyObjective(
        const vector<RestrictionsDataCollector *>& restriction_data, vector<unsigned long> realizations) {

    unsigned long n_realizations = restriction_data.size();
    if (realizations.empty()) {
        realizations = vector<unsigned long>(n_realizations);
        iota(realizations.begin(), realizations.end(), 0);
    } else {
        n_realizations = realizations.size();
    }
    vector<double> restriction_freq;

    /// Check if there were restriction policies in place.
    if (!restriction_data.empty()) {
        unsigned long n_weeks = restriction_data[realizations[0]]->getRestriction_multipliers().size();
        unsigned long n_years = (unsigned long) round(n_weeks / WEEKS_IN_YEAR);

        double restriction_frequency = 0;
        
        vector<double> restriction_freq_per_realization;
        /// Counts how many years across all realizations had restrictions.
        for (const unsigned long &r : realizations) {
            vector<double> restriction_freq_y;

            for (unsigned long y = 0; y < n_years; ++y) {
                double restriction_freq_w = 0;

                for (int w = (int) round(y * WEEKS_IN_YEAR);
                     w < (int) min((int) n_weeks,
                             (int) round((y + 1) * WEEKS_IN_YEAR)); ++w) {
                    if (restriction_data[r]->getRestriction_multipliers()[w] !=
                        1.0) {
                        //restriction_frequency++;
                        restriction_freq_w++;
                        break;
                    }
                    restriction_freq_y.push_back(restriction_freq_w);
                }
                sort(restriction_freq_y.begin(), restriction_freq_y.end());
                restriction_freq_per_realization.push_back(
                        restriction_freq_y.at((unsigned long) floor(0.90 * n_years)));
            }
        }
        sort(restriction_freq_per_realization.begin(),
             restriction_freq_per_realization.end());
        double obj_value = restriction_freq_per_realization.at(
                (unsigned long) floor(0.90 * n_realizations));
        //double obj_value = restriction_frequency / (n_realizations * n_years);

        if (std::isinf(obj_value)) {
            string error_inf = "Infinite restriction frequency.";
            throw logic_error(error_inf.c_str());
        } else {
            return obj_value;
        }
    } else
        return NONE;
}

double ObjectivesCalculator::calculateNetPresentCostInfrastructureObjective(
        const vector<UtilitiesDataCollector *>& utility_data, vector<unsigned long> realizations) {

    unsigned long n_realizations = utility_data.size();
    vector<double> infrastructure_npc_per_realization;

    if (realizations.empty()) {
        realizations = vector<unsigned long>(n_realizations);
        iota(realizations.begin(), realizations.end(), 0);
    } else {
        n_realizations = realizations.size();
    }

    //double infrastructure_npc = 0;
    for (const unsigned long &r : realizations) {
	    auto realization = utility_data[r];
        infrastructure_npc_per_realization.push_back(accumulate(
                realization->getNet_present_infrastructure_cost().begin(),
                realization->getNet_present_infrastructure_cost().end(),
                0.));
        /**
        infrastructure_npc += accumulate(
                realization->getNet_present_infrastructure_cost().begin(),
                realization->getNet_present_infrastructure_cost().end(),
                0.);
        */
    }
    // sort infrastructure npcs to get the top 10 percentile.
    sort(infrastructure_npc_per_realization.begin(),
         infrastructure_npc_per_realization.end());
    double infrastructure_npc = infrastructure_npc_per_realization.at(
            (unsigned long) floor(0.90 * n_realizations));
    // return average infrastructure npcs across realizations.
    //return infrastructure_npc / n_realizations;
    return infrastructure_npc;
}

double ObjectivesCalculator::calculatePeakFinancialCostsObjective(
        const vector<UtilitiesDataCollector *>& utility_data, vector<unsigned long> realizations) {

    unsigned long n_realizations = utility_data.size();
    if (realizations.empty()) {
        realizations = vector<unsigned long>(n_realizations);
        iota(realizations.begin(), realizations.end(), 0);
    } else {
        n_realizations = realizations.size();
    }

    unsigned long n_weeks = utility_data[realizations[0]]->getGross_revenues().size();
    unsigned long n_years = (unsigned long) round(n_weeks / WEEKS_IN_YEAR);

    double realizations_year_debt_payment = 0;
    double realizations_year_cont_fund_contribution = 0;
    double realizations_year_gross_revenue = 1e-6;
    double realizations_year_insurance_contract_cost = 0;
    vector<double> year_financial_costs;
    vector<double> realization_financial_costs(utility_data.size(), 0);

    /// Creates a table with years that failed in each realization.
    int y;
    for (const unsigned long &r : realizations) {
        year_financial_costs.assign(n_years,  0.0);
        y = 0;
        for (unsigned long w = 0; w < n_weeks; ++w) {
            // accumulate year's info by summing weekly amounts.
            realizations_year_debt_payment +=
                    utility_data[r]->getDebt_service_payments()[w];
            realizations_year_cont_fund_contribution +=
                    utility_data[r]->getContingency_fund_contribution()[w];
            realizations_year_gross_revenue +=
                    utility_data[r]->getGross_revenues()[w];
            realizations_year_insurance_contract_cost +=
                    utility_data[r]->getInsurance_contract_cost()[w];

            // if last week of the year, close the books and calculate
            // financial cost for the year.
            if (Utils::isFirstWeekOfTheYear(w + 1)) {
                year_financial_costs[y] +=
                        (realizations_year_debt_payment +
                         realizations_year_cont_fund_contribution +
                         realizations_year_insurance_contract_cost) /
                        realizations_year_gross_revenue;
                // update year count.
                y++;

                // reset accounts.
                realizations_year_debt_payment = 0;
                realizations_year_cont_fund_contribution = 0;
                realizations_year_gross_revenue = 1e-6;
                realizations_year_insurance_contract_cost = 0;
            }
        }
        // sort the year's costs to get the worse 10th percentile.
        sort(year_financial_costs.begin(), year_financial_costs.end());

        // store highest year cost as the cost financial cost of the realization.
        realization_financial_costs[r] = year_financial_costs.at(
                (unsigned long) floor(0.90 * n_years));
        /**
        realization_financial_costs[r] =
                *max_element(year_financial_costs.begin(),
                             year_financial_costs.end());
        if (realization_financial_costs[r] > 1e10) {
            printf("Absurdly high financial cost in realization %lu.\n", r);
        }
         */
    }
    // sort the realization's costs to get the peak financial cost.
    sort(realization_financial_costs.begin(), realization_financial_costs.end());
    double obj_value = realization_financial_costs.at(
            (unsigned long) floor(0.90 * n_realizations));
    /**
    double obj_value = accumulate(realization_financial_costs.begin(),
                                  realization_financial_costs.end(),
                                  0.0) / n_realizations;
    */
    if (std::isinf(obj_value)) {
        string error_inf = "Infinite peak financial cost.";
        throw logic_error(error_inf.c_str());
    } else {
        return obj_value;
    }
}

double ObjectivesCalculator::calculateWorseCaseCostsObjective(
        const vector<UtilitiesDataCollector *>& utility_data, vector<unsigned long> realizations) {

    unsigned long n_realizations = utility_data.size();
    if (realizations.empty()) {
        realizations = vector<unsigned long>(n_realizations);
        iota(realizations.begin(), realizations.end(), 0);
    } else {
        n_realizations = realizations.size();
    }

    unsigned long n_weeks = utility_data[realizations[0]]->getGross_revenues().size();
    unsigned long n_years = (unsigned long) round(n_weeks / WEEKS_IN_YEAR);

    vector<double> worse_year_financial_costs;
    vector<double> year_financial_costs;
    double year_drought_mitigation_cost = 0;
    double year_gross_revenue = 1e-6;

    // Creates a table with years that failed in each realization.
    int y;
    for (const unsigned long &r : realizations) {
        y = 0;
        year_financial_costs.assign(n_years, 0);
        for (unsigned long w = 0; w < n_weeks; ++w) {
            // accumulate year's info by summing weekly amounts.
            year_drought_mitigation_cost +=
                    utility_data[r]->getDrought_mitigation_cost()[w];
            year_gross_revenue += utility_data[r]->getGross_revenues()[w];

            // if last week of the year, close the books and calculate financial cost for the year.
            if (Utils::isFirstWeekOfTheYear(w + 1)) {
                year_financial_costs[y] =
                        max(year_drought_mitigation_cost
                            - utility_data[r]->getContingency_fund_size()[w],
                            0.0) / year_gross_revenue;

                year_gross_revenue = 1e-6;
                year_drought_mitigation_cost = 0;
                // update year count.
                y++;
            }
        }
        // store highest year cost as the drought mitigation cost of the realization.
        worse_year_financial_costs.push_back(*max_element(
                year_financial_costs.begin(),
                year_financial_costs.end()));
    }

    /// sort costs to get the worse 1 percentile.
    sort(worse_year_financial_costs.begin(),
         worse_year_financial_costs.end());

    double obj_value = worse_year_financial_costs.at(
            (unsigned long) floor(WORSE_CASE_COST_PERCENTILE * n_realizations));

    if (std::isinf(obj_value)) {
        string error_inf = "Infinite worse case cost.";
        throw logic_error(error_inf.c_str());
    } else {
        return obj_value;
    }
}
