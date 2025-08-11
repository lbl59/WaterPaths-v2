//
// Created by bernardoct on 5/1/17.
//

/*
 * CLASS INTENDED USAGE ANALYSIS:
 * 
 * The InsuranceStorageToROF class implements a drought insurance mechanism that:
 * 1. Inherits from both DroughtMitigationPolicy and ContinuityModelROF
 * 2. Provides insurance payouts to utilities when their Risk-of-Failure (ROF) exceeds trigger thresholds
 * 3. Prices insurance annually by running ROF simulations to estimate payout probability
 * 4. Uses storage-to-ROF lookup tables to quickly calculate risk levels
 * 
 * WORKFLOW:
 * - Constructor: Sets up internal copies of water system components for ROF calculations
 * - addSystemComponents(): Links to the main simulation's utilities and water sources
 * - setRealization(): Should configure the class for different uncertainty scenarios (CURRENTLY BROKEN)
 * - applyPolicy(): Called weekly during simulation to:
 *   a) Update utility revenue tracking
 *   b) Price insurance annually (first week of year)
 *   c) Make payouts when ROF > trigger threshold
 * - priceInsurance(): Runs multiple short-term ROF simulations to estimate expected payouts
 * 
 * MAJOR ISSUES IDENTIFIED:
 * 1. [RESOLVED] setRealization() functionality is completely commented out, breaking uncertainty analysis
 * 2. Potential memory leaks from drought_mitigation_policies filtering
 * 3. Index mismatches between different utility vectors (utilities_ids, continuity_utilities, realization_utilities)
 * 4. ROF trigger values accumulate without reset mechanism
 * 5. Unused pricing_utilities copies created for each pricing cycle
 * 6. No bounds checking on ROF table tier calculations
 * 7. Inconsistent week parameter usage in pricing loops
 */

#include <iostream>
#include "InsuranceStorageToROF.h"
#include "../Utils/Utils.h"

InsuranceStorageToROF::InsuranceStorageToROF(const int id, const vector<int>&action_multiplier_ids, // ADDED ON 2025-04-28 by Lillian Lau.
                                             vector<WaterSource *> &water_sources,
                                             const Graph &water_sources_graph,
                                             const vector<vector<int>> &water_sources_to_utilities,
                                             vector<Utility *> &utilities,
                                             vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
                                             vector<MinEnvFlowControl *> min_env_flow_controls,
                                             vector<vector<double>>& utilities_rdm,
                                             vector<vector<double>>& water_sources_rdm,
                                             vector<vector<double>>& policy_rdm, vector<vector<double>>& actions_rdm, 
                                             vector<double> &rof_triggers,
                                             const double insurance_premium, const vector<double> &fixed_payouts,
                                             unsigned long total_simulation_time)
        : DroughtMitigationPolicy(id, INSURANCE_STORAGE_ROF),
        ContinuityModelROF(Utils::copyWaterSourceVector(water_sources), water_sources_graph,
                             water_sources_to_utilities, Utils::copyUtilityVector(utilities, true),
                             Utils::copyMinEnvFlowControlVector(min_env_flow_controls), 
                             utilities_rdm.at(0), water_sources_rdm.at(0), actions_rdm.at(0),   // ADDED ON 2025-04-28 by Lillian Lau.
                             total_simulation_time, false, (unsigned int) NON_INITIALIZED),
          rof_triggers(rof_triggers),
          total_simulation_time(total_simulation_time),
          insurance_premium(insurance_premium),
          insurance_price(NONE),
          fixed_payouts(fixed_payouts),
          action_multiplier_ids(action_multiplier_ids),  // ADDED ON 2025-04-28 by Lillian Lau.
          utilities_revenue_update(vector<double>((unsigned long) n_utilities, 0.)),
          utilities_revenue_last_year(vector<double>((unsigned long) n_utilities, 0.)),
          drought_mitigation_policies(Utils::copyDroughtMitigationPolicyVector(drought_mitigation_policies))
{

    for (Utility *u : utilities) utilities_ids.push_back(u->id);
    // CORRECTION 14/18/2025 by Lillian Lau:
    // continuity_utilities are already being deleted in PaperTestProblem 
    // when it calls ContinuityModel destructor when it instantiates a problem
    /**
    for (Utility *u : continuity_utilities) {
        u->clearWaterSources();
        u->resetTotal_storage_capacity();
    }
    */
    
    // POTENTIAL ISSUE: The utilities_ids vector is being populated from the input utilities parameter,
    // but the class will later use both continuity_utilities (copied utilities) and realization_utilities
    // (references to original utilities). This creates potential for index mismatches.

    // Remove financial instruments, if any.
    auto it = this->drought_mitigation_policies.begin();
    while(it != this->drought_mitigation_policies.end()) {
        if((*it)->type > SUPPLY_INSTRUMENT_LIM) {
            it = this->drought_mitigation_policies.erase(it);
        }
        else ++it;
    }
    
    // POTENTIAL ISSUE: This modifies the drought_mitigation_policies vector by removing certain types
    // but doesn't handle the memory management of the removed policies. They may leak memory or 
    // cause double-deletion issues depending on who owns these pointers.

    insurance_price = vector<double>((unsigned long) n_utilities, 0.);
    payout_multiplier = vector<double>((unsigned long) n_utilities, 1.);
}

InsuranceStorageToROF::InsuranceStorageToROF(
        InsuranceStorageToROF &insurance) :
        DroughtMitigationPolicy(insurance.id, insurance.type),
        ContinuityModelROF(Utils::copyWaterSourceVector(insurance.continuity_water_sources),
                           insurance.water_sources_graph,
                           insurance.water_sources_to_utilities,
                           Utils::copyUtilityVector(insurance.continuity_utilities, true),
                           Utils::copyMinEnvFlowControlVector(insurance.min_env_flow_controls),
                           insurance.utilities_rdm,
                           insurance.water_sources_rdm,
                           insurance.actions_rdm,  // ADDED ON 2025-04-28 by Lillian Lau
                           insurance.total_simulation_time,
                           false, insurance.realization_id),
        rof_triggers(insurance.rof_triggers),
        total_simulation_time(insurance.total_simulation_time),
        insurance_premium(insurance.insurance_premium),
        insurance_price(insurance.insurance_price),
        payout_multiplier(insurance.payout_multiplier),
        fixed_payouts(insurance.fixed_payouts),
        action_multiplier_ids(insurance.action_multiplier_ids),  // ADDED ON 2025-04-28 by Lillian Lau.
        utilities_revenue_update(vector<double>((unsigned long) n_utilities, 0.)),
        utilities_revenue_last_year(vector<double>((unsigned long) n_utilities, 0.)),
        drought_mitigation_policies(Utils::copyDroughtMitigationPolicyVector(insurance.drought_mitigation_policies)) {

    utilities_ids = insurance.utilities_ids;
    
    // POTENTIAL ISSUE: Copy constructor creates deep copies of most components but resets revenue vectors.
    // This may not be the intended behavior - should revenue state be preserved in copies?
}

InsuranceStorageToROF::~InsuranceStorageToROF() {
    /**
    // CORRECTION MADE ON 04-23-2025 BY LILLIAN LAU
    // Removing manual deletion of data collectors.
    // Delete drought mitigation policies.
    for (auto dmp : drought_mitigation_policies) {
        delete dmp;
    }
    */
}


void InsuranceStorageToROF::applyPolicy(int week) {
    // Update utilities year revenue.
    for (unsigned long u = 0; u < continuity_utilities.size(); ++u) {
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

    // Do not make payouts during the first year, when no insurance was purchased.
    if (week > WEEKS_IN_YEAR) {
        vector<double> utilities_rof(DroughtMitigationPolicy::realization_utilities.size(), NONE);

        // Get utilities ROFs
        for (unsigned long u = 0; u < DroughtMitigationPolicy::realization_utilities.size(); ++u) {
            utilities_rof[u] = DroughtMitigationPolicy::realization_utilities[u]->getRisk_of_failure();
        }

        // Make payouts, if needed.
        for (unsigned long u = 0; u < continuity_utilities.size(); ++u) {
            if (utilities_rof[u] > rof_triggers[u]) {
                // payout multiplier adjusts for partially paid insurance price due to insufficient contingency fund.
                DroughtMitigationPolicy::realization_utilities[u]->addInsurancePayout(
                        fixed_payouts[u] * utilities_revenue_last_year[u] * payout_multiplier[u]);
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

    for (Utility *u : utilities) {
        utility_base_storage_capacity.push_back(u->getTotal_storage_capacity());
    }
    current_storage_table_shift = vector<double>(utilities.size());

    connectRealizationWaterSources(water_sources);
}

/**
 * Runs a ROF set of 50 year long simulations in order to estimate how likely payouts are expected
 * to occur. The price of the insurance is set as the average sum of payouts across all 50 years
 * times the insurance premium.
 * @param week
 */
void InsuranceStorageToROF::priceInsurance(int week) {

    // Reset prices.
    for (int u : utilities_ids) insurance_price[u] = 0;

    // checks if new infrastructure became available and, if so, set the corresponding realization
    // infrastructure online.
    updateOnlineInfrastructure(week);

    for (int r = 0; r < NUMBER_REALIZATIONS_ROF; ++r) {
        // reset reservoirs' and utilities' storage and combined storage, respectively, they currently
        // have in the corresponding realization simulation.
        
        // ADDED ON 04-24-2025 BY LILLIAN LAU: Create an independent working vector of utility pointers
        vector<Utility*> pricing_utilities(continuity_utilities.size());

        for (int u = 0; u < continuity_utilities.size(); ++u) {
            pricing_utilities[u] = new Utility(*continuity_utilities[u]); // invokes copy constructor
        }
        
        // POTENTIAL ISSUE: The pricing_utilities are created but not used for ROF calculations.
        // The calculateShortTermROFTable still uses continuity_utilities, making this copy operation wasteful.
        // Also, the ROF values are saved to pricing_utilities but never read from them.

        int start_week = std::max(0, week - (int) WEEKS_IN_YEAR + 1);
        for (int w = start_week; w <= week; ++w) {
            
            continuityStep(w, r);

            // Get utilities' approximate rof from storage-rof-table.
            auto utilities_rofs = InsuranceStorageToROF::calculateShortTermROFTable(
                        w, continuity_utilities, n_utilities);

            // Save ROFs to pricing_utilities for analysis
            for (int u = 0; u < pricing_utilities.size(); ++u)
                pricing_utilities[u]->setRisk_of_failure(utilities_rofs[u]);

            // apply supply drought mitigation instruments.
            for (DroughtMitigationPolicy* dmp : drought_mitigation_policies) {
                dmp->applyPolicy(w);
            }

            // Increase the price of the insurance if payout is triggered 
            // Reset dmp utility-variables.
            for (const int &u : utilities_ids) {
                // Get utilities ROFs
                if (utilities_rofs[u] > rof_triggers[u]) {
                    insurance_price[u] += fixed_payouts[u] *
                            utilities_revenue_last_year[u] * insurance_premium;
                }
                continuity_utilities[u]->resetDroughtMitigationVariables();
            }
        }
        // Clean up temporary clones
        for (auto *u : pricing_utilities)
            delete u;
    }

    // Average out insurance price across realizations
    for (int u : utilities_ids) {
        insurance_price[u] /= NUMBER_REALIZATIONS_ROF;

        if (insurance_price[u] == 0) {
            payout_multiplier[u] = 0.;
        } else {
            payout_multiplier[u] = 1.;
        }
        DroughtMitigationPolicy::realization_utilities[u]->
                purchaseInsurance(insurance_price[u]);
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

/**
 * Runs one full ROF calculations for realization_id for a given week.
 * @param week for which ROF is to be calculated.
 * @todo This is mostly a copy and paste from ContinuityModelROF.cpp, so
 * although I'm now on a fix this should be done in a better way at some point.
 */
vector<double> InsuranceStorageToROF::calculateShortTermROFTable(int week,
                                                              const vector<Utility *> &utilities,
                                                              const int &n_utilities) {
    // vector where risks of failure will be stored.
    vector<double> risk_of_failure((unsigned long) n_utilities, 0.0);
    double m;
    for (int u = 0; u < n_utilities; ++u) {
        // Get current stored volume for utility u.
        double utility_storage =
                utilities[u]->getTotal_stored_volume();
        // Ratio of current and status-quo utility storage capacities
        m = utilities[u]->getTotal_storage_capacity() / utility_base_storage_capacity[u];
        // Calculate base table tier that contains the desired ROF by
        // shifting the table around based on new infrastructure -- the
        // shift is made by the part (m - 1) * STORAGE_CAPACITY_RATIO_FAIL *
        // utility_base_storage_capacity[u] - current_storage_table_shift[u]
        double storage_convert = utility_storage +
                                 STORAGE_CAPACITY_RATIO_FAIL * 
                                 utility_base_storage_capacity[u] *
                                 (1. - m) + current_storage_table_shift[u];
        int tier = (int) (storage_convert * NO_OF_INSURANCE_STORAGE_TIERS /
                          utility_base_storage_capacity[u]);
        
                          // Mean ROF between the two tiers of the ROF table where
        // current storage is located.
        risk_of_failure[u] = (getRofFromRealizationTable(u, week, tier) +
            getRofFromRealizationTable(u, week, min(NO_OF_INSURANCE_STORAGE_TIERS, tier + 1))) / 2;
    }

    return risk_of_failure;
}

void InsuranceStorageToROF::updateOnlineInfrastructure(int week) {
    ContinuityModelROF::updateOnlineInfrastructure(week);

    // Use capacity multiplier if using pre-computed tables (which are created with such multiplier)
    if (week < WEEKS_IN_YEAR + 1 && use_imported_tables) {
        for (double &u : utility_base_storage_capacity) {
            u *= BASE_STORAGE_CAPACITY_MULTIPLIER;
        }
    } else if (!use_imported_tables) {
        for (int u = 0; u < n_utilities; ++u) {
            utility_base_storage_capacity[u] = continuity_utilities[u]->getTotal_storage_capacity();
        }
    }
}

