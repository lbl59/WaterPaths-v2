//
// Created by Bernardo on 11/24/2017.
//

#ifndef TRIANGLEMODEL_PROBLEM_H
#define TRIANGLEMODEL_PROBLEM_H

#include <vector>
#include "../../DataCollector/Base/DataCollector.h"
#include "../../DataCollector/MasterDataCollector.h"
#include "../../Utils/Utils.h"
#include "../../SystemComponents/WaterSources/Reservoir.h"

struct infraRank {
    int id;
    double xreal;

    infraRank(int id, double xreal) {
        this->id = id;
        this->xreal = xreal;
    }
};

struct by_xreal {
    bool operator()(const infraRank &ir1, const infraRank &ir2) {
        return ir1.xreal < ir2.xreal;
    }
};

class Problem {
protected:
    unsigned long n_realizations;
    unsigned long  n_weeks;
    unsigned long  solution_no;
    unsigned long n_threads;
    int n_utilities = NON_INITIALIZED;

    string io_directory;
    string output_dir;  // ADDED ON 2025-07-16 by Lillian Lau.
    string rdm_directory;  // ADDED ON 2025-05-27 by Lillian Lau.
    string fname_sufix;
    string rof_tables_directory;  // ADDED ON 2025-05-27 by Lillian Lau.
    string inflow_demand_evap_suffix; // CHANGED ON 2025-05-27 by Lillian Lau.

    vector<unsigned long > realizations_to_run;
    MasterDataCollector* master_data_collector = nullptr;
    vector<double> objectives;
    bool print_output_files = true;

    unsigned long  rdm_no;
    int import_export_rof_tables;
    double table_gen_storage_multiplier;
    vector<vector<double>> utilities_rdm;
    vector<vector<double>> water_sources_rdm;
    vector<vector<double>> policies_rdm;
    vector<vector<double>> actions_rdm;  // ADDED ON 2025-04-28 by Lillian Lau.
    vector<vector<Matrix2D<double>>> rof_tables;

    double checkAndFixInfraExpansionHighLowOrder(vector<int> *order, vector<double> *trigger, int id_low, int id_high, double capacity_low,
                                                 double capacity_high);

    vector<int> vecInfraRankToVecInt(vector<infraRank> v);

public:
    Problem(unsigned long n_weeks);

    virtual ~Problem();

    virtual int functionEvaluation(double* vars, double* objs, double* consts)=0;

    void setN_weeks(unsigned long n_weeks);

    void setSol_number(unsigned long sol_number);

    void setIODirectory(const string &io_directory);

    void setRDMDirectory(const string &rdm_directory);  // ADDED ON 2025-05-27 by Lillian Lau.

    const vector<double> &getObjectives() const;

    vector<double> calculateAndPrintObjectives(bool print_files);

    void setRDMOptimization(vector<vector<double>> &utilities_rdm, 
        vector<vector<double>> &water_sources_rdm,
        vector<vector<double>> &policies_rdm, 
        vector<vector<double>> &actions_rdm); // ADDED ON 2025-04-28 by Lillian Lau.

    void setRDMReevaluation(unsigned long rdm_no, 
        vector<vector<double>> &utilities_rdm,
        vector<vector<double>> &water_sources_rdm, 
        vector<vector<double>> &policies_rdm, 
        vector<vector<double>> &actions_rdm); // ADDED ON 2025-04-28 by Lillian Lau.

    void setN_threads(unsigned long n_threads);

    void setPrint_output_files(bool print_output_files);

    void setN_realizations(unsigned long n_realizations);

    void setOutputDirectory(string output_dir);  // ADDED ON 2025-07-16 by Lillian Lau.

    void setRealizationsToRun(vector<unsigned long>& realizations_to_run);

    void set_inflow_evap_demand_suffix(const string &inflow_demand_evap_suffix);  // MODIFIED ON 2025-05-27 by Lillian Lau.

    void setFname_sufix(const string &fname_sufix);

    MasterDataCollector* getMaster_data_collector();

    void destroyDataCollector();

    void printTimeSeriesAndPathways(bool print_timeseries, bool parquet_format);

    void setRofTables(unsigned long n_realizations, string rof_tables_directory);

    void setRofTablesParquet(unsigned long n_realizations, string rof_tables_directory); // ADDED ON 2025-05-27 by Lillian Lau.

    void setImport_export_rof_tables(int import_export_rof_tables, int n_weeks, int mode, string rof_tables_directory);

    void runBootstrapRealizationThinning(int standard_solution, int n_sets, int n_bs_samples,
                                         int threads, vector<vector<int>> &realizations_to_run);

};


#endif //TRIANGLEMODEL_PROBLEM_H
