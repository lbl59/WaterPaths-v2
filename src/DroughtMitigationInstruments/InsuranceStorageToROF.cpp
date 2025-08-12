//
// Created by bernardoct on 5/1/17.
// Simplified by Lillian Lau on 8/12/2025.
//

#include <iostream>
#include <cmath>
#include "InsuranceStorageToROF.h"
#include "../Utils/Utils.h"

InsuranceStorageToROF::InsuranceStorageToROF(
    const int id, const double insurance_premium, 
    const vector<int>&action_multiplier_ids, // ADDED ON 2025-04-28 by Lillian Lau.
    vector<Utility *> &utilities,
    vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
    vector<MinEnvFlowControl *> min_env_flow_controls,
    vector<vector<double>>& utilities_rdm,
    vector<vector<double>>& water_sources_rdm,
    vector<vector<double>>& policy_rdm, 
    vector<vector<double>>& actions_rdm, 
    vector<double> &rof_triggers,
    const vector<double> &payout_percentage_of_revenue)
        : DroughtMitigationPolicy(id, INSURANCE_STORAGE_ROF),
          rof_triggers(rof_triggers),
          insurance_premium(insurance_premium),
          insurance_cost(NONE),
          payout_percentage_of_revenue(payout_percentage_of_revenue),
          action_multiplier_ids(action_multiplier_ids),  // ADDED ON 2025-04-28 by Lillian Lau.
          utilities_revenue_update(vector<double>((unsigned long) utilities.size(), 0.)),
          utilities_revenue_last_year(vector<double>((unsigned long) utilities.size(), 0.)),
          drought_mitigation_policies(vector<DroughtMitigationPolicy *>())
{
    for (Utility *u : utilities) {
        utilities_ids.push_back(u->id);
    }
    
    insurance_cost = vector<double>((unsigned long) utilities.size(), 0.);
    payout_multiplier = vector<double>((unsigned long) utilities.size(), 1.);
}

InsuranceStorageToROF::InsuranceStorageToROF(
        InsuranceStorageToROF &insurance) :
        DroughtMitigationPolicy(insurance.id, insurance.type),
        rof_triggers(insurance.rof_triggers),
        insurance_premium(insurance.insurance_premium),
        insurance_cost(insurance.insurance_cost),
        payout_multiplier(insurance.payout_multiplier),
        payout_percentage_of_revenue(insurance.payout_percentage_of_revenue),
        action_multiplier_ids(insurance.action_multiplier_ids),  // ADDED ON 2025-04-28 by Lillian Lau.
        utilities_revenue_update(vector<double>((unsigned long) insurance.utilities_ids.size(), 0.)),
        utilities_revenue_last_year(vector<double>((unsigned long) insurance.utilities_ids.size(), 0.)),
        drought_mitigation_policies(vector<DroughtMitigationPolicy *>()) {

    utilities_ids = insurance.utilities_ids;
}

InsuranceStorageToROF::~InsuranceStorageToROF() {}

void InsuranceStorageToROF::applyPolicy(int week) {
    // Update utilities year revenue.
    for (unsigned long u = 0; u < DroughtMitigationPolicy::realization_utilities.size(); ++u) {
        utilities_revenue_update[u] +=
                DroughtMitigationPolicy::realization_utilities[u]->getGrossRevenue();
    }

    // If first week of the year, price insurance for coming year and update
    // gross revenue to base payouts.
    if (Utils::isFirstWeekOfTheYear(week + 1)) {
        utilities_revenue_last_year = utilities_revenue_update;
        std::fill(utilities_revenue_update.begin(), utilities_revenue_update.end(), NONE);
        priceInsurance(week);
    }

    // Only make payouts AFTER the first year
    // No insurance was purchased during the first year
    if (week > WEEKS_IN_YEAR) {
        // Make payouts, if needed.
        for (unsigned long u = 0; u < utilities_ids.size(); ++u) {
            // Get storage-based ROFs for the utility
            double utility_rof = realization_utilities[u]->getRisk_of_failure();
            if (utility_rof > rof_triggers[u]) {
                // payout multiplier adjusts for partially paid insurance price due to insufficient contingency fund.
                DroughtMitigationPolicy::realization_utilities[u]->addInsurancePayout(
                        payout_percentage_of_revenue[u] * utilities_revenue_last_year[u] * payout_multiplier[u]);
            } else {
                DroughtMitigationPolicy::realization_utilities[u]->addInsurancePayout(NONE);
            }
        }
    }
}

void InsuranceStorageToROF::addSystemComponents(vector<Utility *> utilities,
                                                vector<WaterSource *> water_sources,
                                                vector<MinEnvFlowControl *> min_env_flow_controls) {

    DroughtMitigationPolicy::realization_utilities = vector<Utility *>(utilities.size());
    for (int i : utilities_ids) {
        DroughtMitigationPolicy::realization_utilities[i] = utilities[i];
    }
}

/**
 * Prices insurance based on current storage levels relative to capacity.
 * Uses storage-to-ROF scaling instead of recalculating ROF tables.
 * @param week
 */
void InsuranceStorageToROF::priceInsurance(int week) {
    // Reset prices.
    for (int u : utilities_ids) insurance_cost[u] = 0;

    // Price insurance based on current storage-based ROF levels
    for (int u : utilities_ids) {
        double utility_rof = realization_utilities[u]->getRisk_of_failure();
        // If current ROF exceeds trigger, expect higher payout probability
        if (utility_rof > rof_triggers[u]) {
            // Increase total cost spent on insurance if insurance payout has been triggered
            insurance_cost[u] += (payout_percentage_of_revenue[u] * utilities_revenue_last_year[u]) * insurance_premium;
        } 
    }
}

void InsuranceStorageToROF::setRealization(unsigned long realization_id, const vector<double> &utilities_rdm,
                                           const vector<double> &water_sources_rdm, const vector<double> &policy_rdm,
                                           const vector<double> &actions_rdm) { // ADDED ON 2025-04-28 by Lillian Lau.
    
    for (unsigned long u = 0; u < utilities_ids.size(); ++u) {
        int action_multiplier_idx = action_multiplier_ids.at(u);

        // Capping the trigger between 0 and 1
        rof_triggers[u] = max(0.0, min(1.0, rof_triggers[u] + actions_rdm.at(action_multiplier_idx)));
    }
}
