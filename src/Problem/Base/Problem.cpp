//
// Created by Bernardo on 11/24/2017.
//

#include <algorithm>
#include <numeric>
#include <random>
#include <set>
#include "Problem.h"
#include "../../Utils/Utils.h"
#include <omp.h>

// Import Parquet libraries
#include <arrow/io/api.h>
#include <parquet/arrow/reader.h>

vector<double> Problem::calculateAndPrintObjectives(bool print_files) {
    if (this->master_data_collector != nullptr) {
        if (print_files) {
            this->master_data_collector->setOutputDirectory(output_dir);
        }
        string fo = "Objectives";
        objectives = this->master_data_collector->calculatePrintObjectives(
                fo + "_s" + std::to_string(solution_no) + fname_sufix, print_files);
        return objectives;
    } else {
        objectives = vector<double>(25, 1e5);
        return objectives;
    }
}

void Problem::printTimeSeriesAndPathways(bool print_timeseries, bool parquet_format) {
    // Calculate objective values.
    if (this->master_data_collector != nullptr) {

        // Print output files.
        string fu = "Utilities";
        string fws = "WaterSources";
        string fp = "Policies";
        string fpw = "Pathways";

        // ADDED ON 2025-07-16 by Lillian Lau.
        string timeseries_pathways_output_directory = io_directory + output_dir;
        Utils::createDir(timeseries_pathways_output_directory);

        cout << "Printing Pathways" << endl;
        //this->master_data_collector->setOutputDirectory(io_directory); 
        this->master_data_collector->setOutputDirectory(timeseries_pathways_output_directory);  // MODIFIED ON 2025-07-16 by Lillian Lau.
        this->master_data_collector->printPathways(
                fpw + "_s" + std::to_string(solution_no) + fname_sufix);
        if (print_timeseries) {
            cout << "Printing time series" << endl;
            // ADDED ON 2025-06-19 by Lillian Lau.
            if (parquet_format) {
                this->master_data_collector->printSubsetOutputParquet(
                        0, (int) n_weeks, "subset_s" + std::to_string(solution_no) +
                                            fname_sufix);
            } else {
                /**
                this->master_data_collector->printSubsetOutputCSV(
                        0, (int) n_weeks, "subset_s" + std::to_string(solution_no) +
                                            fname_sufix);
                */
                this->master_data_collector->printUtilitiesOutputCompact(
                    0, (int) n_weeks, fu + "_s" + std::to_string(solution_no) +
                                    fname_sufix);
                this->master_data_collector->printWaterSourcesOutputCompact(
                    0, (int) n_weeks, fws + "_s" + std::to_string(solution_no) +
                                    fname_sufix);
                this->master_data_collector->printPoliciesOutputCompact(
                    0, (int) n_weeks, fp + "_s" + std::to_string(solution_no) +
                                    fname_sufix);
            }
        }
    } else {
        printf("Trying to print pathways but data collector is empty. Either your simulation crashed or you deleted the data collector too early.\n");
    }
}

vector<int> Problem::vecInfraRankToVecInt(vector<infraRank> v) {
    vector<int> sorted;
    for (infraRank ir : v) {
        sorted.push_back(ir.id);
    }
    return sorted;
}

double Problem::checkAndFixInfraExpansionHighLowOrder(
        vector<int> *order, vector<double> *triggers, int id_low,
        int id_high, double capacity_low, double capacity_high) {

    auto pos_low = distance(order->begin(),
                            find(order->begin(),
                                 order->end(),
                                 id_low));

    auto pos_high = distance(order->begin(),
                             find(order->begin(),
                                  order->end(),
                                  id_high));

    if (pos_high < pos_low) {
        capacity_high += capacity_low;
        order->erase(order->begin() + pos_low);
        triggers->erase(triggers->begin() + pos_low);
    }

    return capacity_high;
}


void Problem::setN_weeks(unsigned long n_weeks) {
    Problem::n_weeks = n_weeks;
}

void Problem::setSol_number(unsigned long sol_number) {
    Problem::solution_no = sol_number;
}

void Problem::setIODirectory(const string &io_directory) {
    this->io_directory = io_directory;
}

void Problem::setRDMDirectory(const string &rdm_directory) {
    this->rdm_directory = rdm_directory;
}

void Problem::setRDMOptimization(vector<vector<double>> &utilities_rdm,
                                 vector<vector<double>> &water_sources_rdm,
                                 vector<vector<double>> &policies_rdm,
                                 vector<vector<double>> &actions_rdm) {
    this->utilities_rdm = utilities_rdm;
    this->water_sources_rdm = water_sources_rdm;
    this->policies_rdm = policies_rdm;
    this->actions_rdm = actions_rdm;  // ADDED ON 2025-04-28 by Lillian Lau.
}

void Problem::setRDMReevaluation(unsigned long rdm_no, 
                                 vector<vector<double>> &utilities_rdm,
                                 vector<vector<double>> &water_sources_rdm,
                                 vector<vector<double>> &policies_rdm,
                                 vector<vector<double>> &actions_rdm) {
    this->rdm_no = rdm_no;
    this->utilities_rdm = utilities_rdm;
    this->water_sources_rdm = water_sources_rdm;
    this->policies_rdm = policies_rdm;
    this->actions_rdm = actions_rdm;  // ADDED ON 2025-04-28 by Lillian Lau.
}

void Problem::setFname_sufix(const string &fname_sufix) {
    Problem::fname_sufix = fname_sufix;
}

void Problem::set_inflow_evap_demand_suffix(const string &inflow_demand_evap_suffix) {
    Problem::inflow_demand_evap_suffix = inflow_demand_evap_suffix;
}

void Problem::setN_threads(unsigned long n_threads) {
    Problem::n_threads = n_threads;
}

// ADDED ON 2025-07-16 by Lillian Lau.
void Problem::setOutputDirectory(string output_dir) {
    Problem::output_dir = output_dir;
}

const vector<double> &Problem::getObjectives() const {
    return objectives;
}

void Problem::setPrint_output_files(bool print_output_files) {
    Problem::print_output_files = print_output_files;
}

void Problem::setN_realizations(unsigned long n_realizations) {
    Problem::n_realizations = n_realizations;

    if (realizations_to_run.empty()) {
        realizations_to_run = vector<unsigned long>(n_realizations);
        iota(begin(realizations_to_run), end(realizations_to_run), 0);
    }
}

void Problem::setRealizationsToRun(vector<unsigned long> &realizations_to_run) {
    this->realizations_to_run = realizations_to_run;
}

MasterDataCollector *Problem::getMaster_data_collector() {
    return master_data_collector;
}

Problem::~Problem() {}

void Problem::destroyDataCollector() {
    if (master_data_collector != nullptr) {
        delete master_data_collector;
        master_data_collector = nullptr;
    } else {
        cerr << "Tried to delete nullptr master data collector.\n";
    }
}

Problem::Problem(unsigned long n_weeks) : n_weeks(n_weeks) {
    Reservoir::unsetSeed();
}

/**
 * Read pre-computed ROF tables.
 * @param folder Folder containing the ROF tables.
 * @param n_realizations number of realizations.
 */
void Problem::setRofTables(unsigned long n_realizations, string rof_tables_directory) {
    double start_time = omp_get_wtime();
    printf("Reading ROF tables.\n");
    string file_name = rof_tables_directory + "tables_r0_u0.csv";
    auto data_r0_u0 = Utils::parse2DCsvFile(file_name);
    auto n_weeks_in_table = (int) data_r0_u0.size(); 
    auto n_tiers = (int) data_r0_u0.at(0).size();

    if (n_tiers != NO_OF_INSURANCE_STORAGE_TIERS) {
        char error[75];
        sprintf(error, "Number of tiers in tables does not match number of tiers for this problem.");
    }

    n_utilities = 0;
    string fname = rof_tables_directory + "tables_r0_u0.csv";
    //fstream f;
    std::ifstream ifile(fname.c_str());
    while ((bool) ifile) {
        n_utilities += 1;
        fname = rof_tables_directory + "tables_r0_u" + to_string(n_utilities) + ".csv";
        ifile = std::ifstream(fname.c_str());
    }

    rof_tables = vector<vector<Matrix2D<double>>>(
            n_realizations,
            vector<Matrix2D<double>>((unsigned long) n_utilities,
                                     Matrix2D<double>(n_weeks_in_table, n_tiers)));

    for (unsigned long r = 0; r < n_realizations; ++r) {

        for (int u = 0; u < n_utilities; ++u) {
            string file_name = rof_tables_directory + "tables_r" + to_string(r) + "_u" + to_string(u) + ".csv";
            auto tables_utility_week = Utils::parse2DCsvFile(file_name);

            for (unsigned long w = 0; w < n_weeks; ++w) {
                rof_tables[r][u].setPartialData(w, tables_utility_week[w].data(), tables_utility_week[w].size());
            }
        }
    }

    printf("Loading CSV ROF tables took %f s.\n", omp_get_wtime() - start_time);
}

// ADDED ON 2025-05-30 by Lillian Lau.
void Problem::setRofTablesParquet(unsigned long n_realizations, string rof_tables_directory) {

    double start_time = omp_get_wtime();
    printf("Reading ROF tables.\n");
    string file_name = rof_tables_directory + "tables_r0_u0.parquet";

    // Get table dimensions
    auto data_r0_u0 = Utils::parse2DParquetFile(file_name, n_realizations);
    int n_weeks_in_table = static_cast<int>(data_r0_u0.size());
    auto n_tiers = static_cast<int>(data_r0_u0.at(0).size());

    if (n_tiers != NO_OF_INSURANCE_STORAGE_TIERS) {
        throw runtime_error(
            "Mismatch in number of tiers: file has " + std::to_string(n_tiers) + 
            " but problem expects " + std::to_string(NO_OF_INSURANCE_STORAGE_TIERS));
    }

    // Count number of utilities
    n_utilities = 0;
    std::ifstream ifile(file_name.c_str());

    while ((bool) ifile) {
        n_utilities += 1;
        string fname = rof_tables_directory + "tables_r0_u" + to_string(n_utilities) + ".parquet";
        ifile = std::ifstream(fname.c_str());
    }

    rof_tables = vector<vector<Matrix2D<double>>>(
            n_realizations,
            vector<Matrix2D<double>>((unsigned long) n_utilities,
                                     Matrix2D<double>(n_weeks_in_table, n_tiers)));

    for (unsigned long r = 0; r < n_realizations; ++r) {
        for (int u = 0; u < n_utilities; ++u) {
            string file_name = rof_tables_directory + "tables_r" + to_string(r) + "_u" + to_string(u) + ".parquet";
            auto table = Utils::parse2DParquetFile(file_name, n_weeks_in_table);

            for (unsigned long w = 0; w < n_weeks; ++w) {
                rof_tables[r][u].setPartialData(
                    w, 
                    table[w].data(), 
                    static_cast<size_t>(table[w].size()));
            }
        }
    }

    printf("Loading Parquet ROF tables took %f s.\n", omp_get_wtime() - start_time);
}

void Problem::setImport_export_rof_tables(int import_export_rof_tables, int n_weeks, int mode, string rof_tables_directory) {
    if (std::abs(import_export_rof_tables) > 1)
        throw invalid_argument("Import/export ROF tables can be assigned as:\n"
                               "-1 - import tables\n"
                               "0 - ignore tables\n"
                               "1 - export tables.\n"
                               "The value entered is invalid.");
    this->import_export_rof_tables = import_export_rof_tables;
    this->rof_tables_directory = rof_tables_directory;

    if (import_export_rof_tables == IMPORT_ROF_TABLES) {
        if (mode == 0) {
            setRofTables(n_realizations, rof_tables_directory);
        } else if (mode == 1) {
            setRofTablesParquet(n_realizations, rof_tables_directory);
        }
    } else {
        Utils::createDir(rof_tables_directory);
    }
}

void Problem::runBootstrapRealizationThinning(int standard_solution, int n_sets, int n_bs_samples,
                                              int threads, vector<vector<int>> &realizations_to_run) {
    master_data_collector->setOutputDirectory(output_dir);
    master_data_collector->performBootstrapAnalysis(standard_solution, n_sets, n_bs_samples, threads, realizations_to_run);
}
