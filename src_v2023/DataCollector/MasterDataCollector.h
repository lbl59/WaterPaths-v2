//
// Created by bernardoct on 8/26/17.
//

#ifndef TRIANGLEMODEL_MASTERDATACOLLECTOR_H
#define TRIANGLEMODEL_MASTERDATACOLLECTOR_H


#include <vector>
#include "Base/DataCollector.h"
#include "UtilitiesDataCollector.h"
#include "../DroughtMitigationInstruments/Base/DroughtMitigationPolicy.h"
#include "RestrictionsDataCollector.h"

class MasterDataCollector {
private:
    string output_directory;
    unsigned long n_realizations;

    vector<vector<DataCollector *>> water_source_collectors;
    vector<vector<DataCollector *>> drought_mitigation_policy_collectors;
    vector<vector<UtilitiesDataCollector *>> utility_collectors;
    vector<unsigned long> crashed_realizations;
    vector<unsigned long> realizations_ran;

    static int seed;

public:

    MasterDataCollector(vector<unsigned long> &realizations_to_run);

    vector<string> getSubsetColumnNames();  // ADDED on 06-19-2026 by Lillian Lau    

    vector<string> getColumnNames(string d_type);  // ADDED on 06-02-2026 by Lillian Lau

    vector<double> calculatePrintObjectives(string file_name, bool print);

    virtual ~MasterDataCollector();

    // ADDED on 06-19-2026 by Lillian Lau
    void printSubsetOutputParquet(int week_i, int week_f, string file_name);

    // TO IMPLEMENT: Add in function to print subset output to CSV
    // void printSubsetOutputCSV(int week_i, int week_f, string file_name);

    // ADDED on 06-02-2026 by Lillian Lau
    void printOutputParquet(int week_i, int week_f, string d_type, string file_name);

    void printPoliciesOutputCompact(int week_i, int week_f, string file_name);
    // ADDED on 06-02-2026 by Lillian Lau
    void printUtilitiesOutputCompact(int week_i, int week_f, string file_name);

    void printWaterSourcesOutputCompact(int week_i, int week_f, string file_name);

    void printWaterSourcesOutputTabular(int week_i, int week_f, string file_name);

    void setOutputDirectory(string io_directory);

    void printPathways(string file_name);

    void addRealization(
            vector<WaterSource *> water_sources_realization,
            vector<DroughtMitigationPolicy *> drought_mitigation_policies_realization,
            vector<Utility *> utilities_realization, unsigned long r);

    void removeRealization(unsigned long r);

    void cleanCollectorsOfDeletedRealizations();

    void collectData(unsigned long r);

    void performBootstrapAnalysis(int sol_id, int n_sets, int n_samples, int n_threads,
            vector<vector<int>> bootstrap_samples = vector<vector<int>>());

    DataCollector* createPolicyDataCollector(DroughtMitigationPolicy* dmp, unsigned long r);

    DataCollector* createWaterSourceDataCollector(WaterSource* ws, unsigned long r);

    void printUtilityObjectivesToRowOutStream(vector<UtilitiesDataCollector *> &u, std::ofstream &outStream,
            vector<double> &objectives);

    void readOrCreateBSSamples(int sol_id, int n_sets, int n_samples, const vector<vector<int>> &bootstrap_samples,
                               vector<vector<int>> &bootstrap_sample_sets) const;

    void printObjsBSSamples(int sol_id, int n_sets, int n_samples, vector<vector<double>> &objectives);

    void printObjectivesOfAllRealizationsForBSAnalysis(int sol_id, int n_sets, int n_samples);

    static void setSeed(int seed);

    static void unsetSeed();

    void printBSSamples(int sol_id, int n_sets, int n_samples, const vector<vector<int>> &bootstrap_sample_sets) const;

    void isolateRestrictionDataCollectors(vector<UtilitiesDataCollector *> &u,
                                          vector<RestrictionsDataCollector *> &utility_restrictions) const;
};


#endif //TRIANGLEMODEL_MASTERDATACOLLECTOR_H
