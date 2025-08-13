//
// Created by bernardoct on 5/1/17.
//

#ifndef TRIANGLEMODEL_INSURANCESTORAGETOROF_H
#define TRIANGLEMODEL_INSURANCESTORAGETOROF_H


#include "Base/DroughtMitigationPolicy.h"
#include "../Utils/Constants.h"

class InsuranceStorageToROF : public DroughtMitigationPolicy {
private:
    vector<double> rof_triggers;// = vector<double>(40, 0.);
    //const double insurance_premium;
    double insurance_premium;
    vector<int> action_multiplier_ids;  // ADDED ON 2025-04-28 by Lillian Lau.
    vector<double> payout_multiplier;
    vector<double> insurance_cost;
    const vector<double> &payout_percentage_of_revenue;   // Percentage of the utility's revenue last year.
    vector<double> utilities_revenue_update;
    vector<double> utilities_revenue_last_year;
    vector<DroughtMitigationPolicy *> drought_mitigation_policies;

public:
    InsuranceStorageToROF(const int id, double insurance_premium,
                              const vector<int> &action_multiplier_ids, // ADDED ON 2025-04-28 BY LILLIAN LAU
                              vector<Utility *> &utilities,
                              vector<DroughtMitigationPolicy *> &drought_mitigation_policies,
                              vector<MinEnvFlowControl *> min_env_flow_controls,
                              vector<vector<double>>& utilities_rdm,
                              vector<vector<double>>& water_sources_rdm,
                              vector<vector<double>>& policy_rdm, 
                              vector<vector<double>>& actions_rdm, 
                              vector<double> &rof_triggers,
                              const vector<double> &payout_percentage_of_revenue);

    InsuranceStorageToROF(InsuranceStorageToROF &insurance);

    ~InsuranceStorageToROF() override;

    //void priceInsurance(int week);

    void applyPolicy(int week) override;

    void addSystemComponents(vector<Utility *> utilities,
                                 vector<WaterSource *> water_sources,
                                 vector<MinEnvFlowControl *> min_env_flow_controls) override;

    void setRealization(unsigned long realization_id, 
        const vector<double> &utilities_rdm,
        const vector<double> &water_sources_rdm, 
        const vector<double> &policy_rdm,
        const vector<double> &actions_rdm) override;
};


#endif //TRIANGLEMODEL_INSURANCESTORAGETOROF_H
