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
#ifdef  PARALLEL
#include "../../../Borg/borgms.h"
#endif

class Problem {
protected:
    unsigned long n_realizations;
    unsigned long  n_weeks;
    unsigned long  solution_no;
    unsigned long n_threads;
    unsigned long n_dec_vars = NON_INITIALIZED;
    unsigned long n_objectives = 5; // Number of objectives implemented in current version.
    int n_utilities = NON_INITIALIZED;

    string io_directory;
    string output_dir;  // ADDED ON 2025-07-16 by Lillian Lau.
    string rdm_directory;  // ADDED ON 2025-05-27 by Lillian Lau.
    string fname_sufix;
    string rof_tables_directory;
    string inflow_demand_evap_suffix;

    vector<unsigned long > realizations_to_run;
    MasterDataCollector* master_data_collector = nullptr;
    vector<double> objectives;
    bool print_output_files = true;

    int seed;
    int rdm_no = NON_INITIALIZED;
    int import_export_rof_tables;
    double table_gen_storage_multiplier;

    // RDM DU factor vectors
    vector<vector<double>> utilities_rdm;
    vector<vector<double>> water_sources_rdm;
    vector<vector<double>> policies_rdm;
    vector<vector<double>> actions_rdm;  // ADDED ON 2025-04-28 by Lillian Lau.

    vector<vector<Matrix2D<double>>> rof_tables;
    vector<vector<unsigned long>> bs_realizations;
    vector<int> solutions_to_run_range;
    string system_io, solutions_file, bootstrap_file;
    int n_sets, n_bs_samples;

    bool plotting, print_obj_row;

    virtual void readInputData();

public:
    explicit Problem();

    Problem(unsigned long n_weeks, int n_realizations,
            int import_export_rof_table, string system_io,
            string &rof_tables_directory, int seed,
            unsigned long n_threads, 
            int rdm_no, string rdm_directory, // ADDED ON 2025-05-27 by Lillian Lau.
            string output_dir, // ADDED ON 2025-07-16 by Lillian Lau.
            string bootstrap_file,
            string &utilities_rdm_file, string &policies_rdm_file,
            string &water_sources_rdm_file, string &actions_rdm_file,  // ADDED ON 2025-04-28 by Lillian Lau.
            int n_sets, int n_bs_samples, 
            string &solutions_file, vector<int> &solutions_to_run_range, 
            bool plotting, bool print_obj_row, 
            string &inflows_demand_evap_directory_suffix);

    virtual ~Problem();

    virtual int functionEvaluation(double* vars, double* objs, double* consts)=0;

    vector<double> calculateAndPrintObjectives(bool print_files);

    vector<int> vecInfraRankToVecInt(vector<infraRank> v);

    MasterDataCollector* getMaster_data_collector();

    const vector<double> &getObjectives() const;

    void setN_weeks(unsigned long n_weeks);

    void setSol_number(unsigned long sol_number);

    void setIODirectory(const string &io_directory);

    void setN_threads(unsigned long n_threads);

    void setPrint_output_files(bool print_output_files);

    void setN_realizations(unsigned long n_realizations);

    void setOutputDirectory(string output_dir);  // ADDED ON 2025-07-16 by Lillian Lau.

    void setRealizationsToRun(vector<unsigned long>& realizations_to_run);

    void setFname_sufix(const string &fname_sufix);

    void destroyDataCollector();

    void printTimeSeriesAndPathways(bool plot_time_series = true);

    void setRofTables(unsigned long n_realizations, string rof_tables_directory);
    
    void setRofTablesParquet(unsigned long n_realizations, string rof_tables_directory);

    void setImport_export_rof_tables(int import_export_rof_tables,
                                     string rof_tables_directory);

    void runBootstrapRealizationThinning(int standard_solution, int n_sets,
                                         int n_bs_samples,
                                         int threads,
                                         const vector<vector<unsigned long>> &bs_realizations);
    
    void setRDMOptimization(vector<vector<double>> &utilities_rdm,
                            vector<vector<double>> &water_sources_rdm,
                            vector<vector<double>> &policies_rdm,
                            vector<vector<double>> &actions_rdm);  // ADDED ON 2025-04-28 by Lillian Lau.
    
    void setRDMReevaluation(int rdm_no, 
                            vector<vector<double>> &utilities_rdm,
                            vector<vector<double>> &water_sources_rdm,
                            vector<vector<double>> &policies_rdm,
                            vector<vector<double>> &actions_rdm);  // ADDED ON 2025-04-28 by Lillian Lau.
    
    static double checkAndFixInfraExpansionHighLowOrder(vector<int> &order,
                                                        vector<double> &triggers,
                                                        int id_low, int id_high,
                                                        double capacity_low,
                                                        double capacity_high);
    
    virtual void runSimulation() = 0;

    virtual unsigned long getNDecVars() const;

    virtual unsigned long getNObjectives() const;

    void set_inflow_evap_demand_suffix(const string &inflow_demand_evap_suffix);

#ifdef PARALLEL
    virtual void setProblemDefinition(BORG_Problem &problem) = 0;
#endif
};


#endif //TRIANGLEMODEL_PROBLEM_H
