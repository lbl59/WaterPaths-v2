//;;
// Created by bernardoct on 8/26/17.
//

#include <fstream>
#include <iomanip>
#include <sys/stat.h>
#include <numeric>
#include <random>
#include <algorithm>

// ADDED PARQUET HEADERS ON 06-02-2026 by Lillian Lau.
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/writer.h>
#include <parquet/file_writer.h>

#include "MasterDataCollector.h"
#include "../Utils/ObjectivesCalculator.h"
#include "../Utils/Utils.h"
#include "../DroughtMitigationInstruments/Transfers.h"
#include "TransfersDataCollector.h"
#include "../SystemComponents/WaterSources/Quarry.h"
#include "../SystemComponents/WaterSources/WaterReuse.h"
#include "../SystemComponents/WaterSources/AllocatedReservoir.h"
#include "ReservoirDataCollector.h"
#include "IntakeDataCollector.h"
#include "QuaryDataCollector.h"
#include "WaterReuseDataCollector.h"
#include "AllocatedReservoirDataCollector.h"
#include "EmptyDataCollector.h"

using namespace Constants;
using namespace std;

int MasterDataCollector::seed = NON_INITIALIZED;

MasterDataCollector::MasterDataCollector(vector<unsigned long> &realizations_to_run)
        : n_realizations(*max_element(realizations_to_run.begin(), realizations_to_run.end()) + 1),
        realizations_ran(realizations_to_run) {}

MasterDataCollector::~MasterDataCollector() {
    for (vector<DataCollector *> dcs : water_source_collectors)
        for (DataCollector *dc : dcs)
            delete dc;

    for (vector<DataCollector *> dcs : drought_mitigation_policy_collectors)
        for (DataCollector *dc : dcs)
            delete dc;

    for (vector<UtilitiesDataCollector *> dcs : utility_collectors)
        for (UtilitiesDataCollector *dc : dcs)
            delete dc;
}
/**
 * @brief Gets only the column names for the data needed to calculate time-varying performance
 * ADDED on 06-19-2025 by Lillian Lau
 * 
 * @return vector<string> 
 */
vector<string> MasterDataCollector::getSubsetColumnNames() {
    vector<string> column_names;

    // push utility data column names to column_names
    if (!utility_collectors.empty()) {
        for (vector<UtilitiesDataCollector *> p_vec : utility_collectors) {
            UtilitiesDataCollector* dc = p_vec[0];
            string hdr = dc->printSubsetCompactStringHeader(); 

            // Remove trailing comma if present:
            if (!hdr.empty() && hdr.back() == ',') {
                hdr.pop_back();
            }

            // Split hdr by ',' into individual column names:
            stringstream ss(hdr);
            string token;
            while (std::getline(ss, token, ',')) {
                column_names.push_back(token);
            }
        }
    } else {
        throw runtime_error("No utility collectors found.");
    }

    // push transfer data column names to column_names
    if (!drought_mitigation_policy_collectors.empty()) {
        // Create a string of all the column names 
        for (vector<DataCollector *> p_vec : drought_mitigation_policy_collectors) {
            DataCollector* dc = p_vec[0];
            string hdr = dc->printCompactStringHeader();

            // Remove trailing comma if present:
            if (!hdr.empty() && hdr.back() == ',') {
                hdr.pop_back();
            }

            // Split hdr by ',' into individual column names:
            stringstream ss(hdr);
            string token;
            while (std::getline(ss, token, ',')) {

                // Check if token contains 'transf'
                if (token.find("transf") != string::npos) {
                    // If it does, add it to column_names
                    column_names.push_back(token);
                }
            }
        }
    } else {
        throw runtime_error("No drought mitigation policy collectors found.");
    }
    return column_names;

}

/**
 * @brief Returns the column names for the specified data collector type.
 * 
 * @param d_type The data collector type for which to get column names
 * @return vector<string> 
 * ADDED on 06-02-2026 by Lillian Lau
 */
vector<string> MasterDataCollector::getColumnNames(string d_type) {
    vector<string> column_names;
    if (d_type == "Utilities") {
        if (!utility_collectors.empty()) {
            for (vector<UtilitiesDataCollector *> p_vec : utility_collectors) {
                UtilitiesDataCollector* dc = p_vec[0];
                string hdr = dc->printCompactStringHeader(); 

                // Remove trailing comma if present:
                if (!hdr.empty() && hdr.back() == ',') {
                    hdr.pop_back();
                }

                // Split hdr by ',' into individual column names:
                stringstream ss(hdr);
                string token;
                while (std::getline(ss, token, ',')) {
                    column_names.push_back(token);
                }
            }
        } else {
            throw runtime_error("No utility collectors found.");
        }

    } else if (d_type == "Policies") {
        if (!drought_mitigation_policy_collectors.empty()) {
            // Create a string of all the column names 
            for (vector<DataCollector *> p_vec : drought_mitigation_policy_collectors) {
                DataCollector* dc = p_vec[0];
                string hdr = dc->printCompactStringHeader(); 

                // Remove trailing comma if present:
                if (!hdr.empty() && hdr.back() == ',') {
                    hdr.pop_back();
                }

                // Split hdr by ',' into individual column names:
                stringstream ss(hdr);
                string token;
                while (std::getline(ss, token, ',')) {
                    column_names.push_back(token);
                }
            }
        } else {
            throw runtime_error("No drought mitigation policy collectors found.");
        }

    } else if (d_type == "WaterSources") {
        if (!water_source_collectors.empty()) {
            for (vector<DataCollector *> p_vec : water_source_collectors) {
                DataCollector* dc = p_vec[0];
                string hdr = dc->printCompactStringHeader(); 

                // Remove trailing comma if present:
                if (!hdr.empty() && hdr.back() == ',') {
                    hdr.pop_back();
                }

                // Split hdr by ',' into individual column names:
                stringstream ss(hdr);
                string token;
                while (std::getline(ss, token, ',')) {
                    column_names.push_back(token);
                }
            }
        } else {
            throw runtime_error("No water source collectors found.");
        }
    }
    return column_names;
}

void MasterDataCollector::printSubsetOutputParquet(
    int week_i, int week_f, string file_name) {

    // ADDED ON 07-16-2025 by Lillian Lau
    string timeseries_directory = output_directory + "timeseries/";
    Utils::createDir(timeseries_directory);

    vector<string> column_names = getSubsetColumnNames();
    vector<vector<DataCollector *>> p_matrix;
    vector<vector<UtilitiesDataCollector *>> u_matrix;

    // assign u_matrix to utility_collectors if it is not empty
    if (!utility_collectors.empty()) {
        u_matrix = utility_collectors;
    }

    // assign p_matrix to utility_collectors if it is not empty
    if (!drought_mitigation_policy_collectors.empty()) {
        p_matrix = drought_mitigation_policy_collectors;
    }
    
    bool failure_flag = false;
    string failure_msg;

    #pragma omp parallel for default(none) \
        shared(timeseries_directory, file_name, week_i, week_f, column_names, p_matrix, u_matrix, failure_flag, failure_msg)
    for (int rr = 0; rr < (int) realizations_ran.size(); ++rr) {
        int r = realizations_ran[rr];

        int num_columns = column_names.size();
        int num_rows = week_f - week_i;

        arrow::MemoryPool *pool = arrow::default_memory_pool();
        vector<shared_ptr<arrow::DoubleBuilder>> builders;
        builders.reserve(num_columns);
        for (int i = 0; i < num_columns; ++i) {
            builders.push_back(make_shared<arrow::DoubleBuilder>(pool));
        }

        for (int w = week_i; w < week_f; ++w) {
            vector<double> row_data;
            row_data.reserve(num_columns);
            // If p_matrix is not empty, iterate through it
            if (!p_matrix.empty() && !u_matrix.empty()) {
                for (vector<UtilitiesDataCollector *> u_vec : u_matrix) {
                    UtilitiesDataCollector* dc = u_vec[r];
                    string data_w = dc-> printSubsetCompactString(w);

                    // Split data_w by ',' into individual values:
                    stringstream ss(data_w);
                    string token;

                    while(std::getline(ss, token, ',')) {
                        double d = 0.0;
                        // Convert token to double and append to the builder:
                        try {
                            d = std::stod(token);
                        } catch (const std::exception &e) {
                            d = std::numeric_limits<double>::quiet_NaN();
                        }
                        row_data.push_back(d);
                    }
                }

                for (vector<DataCollector *> p_vec : p_matrix) {
                    DataCollector* dc = p_vec[r];
                    string data_w = dc-> printCompactString(w);

                    // Split data_w by ',' into individual values:
                    stringstream ss(data_w);
                    string token;
                    int token_count = 0;

                    while(std::getline(ss, token, ',')) {
                        token_count++;
                        // get the last three tokens
                        if (token_count > 2) {
                            double d = 0.0; 
                            // Convert token to double and append to the builder:
                            try {
                                d = std::stod(token);
                            } catch (const std::exception &e) {
                                d = std::numeric_limits<double>::quiet_NaN();
                            }
                            row_data.push_back(d);
                        }
                    }
                }
            }
            
            for (int c = 0; c < num_columns; ++c) {
                // Append the value to the corresponding column builder
                arrow::Status st = builders[c]->Append(row_data[c]);
                if (!st.ok()) {
                    failure_flag = true;
                    failure_msg = st.ToString();
                    break;
                }
            }
        }

        vector<shared_ptr<arrow::Array>> columns;
        columns.reserve(num_columns);
        for (int c = 0; c < num_columns; ++c) {
            shared_ptr<arrow::Array> array;
            PARQUET_THROW_NOT_OK(builders[c]->Finish(&array));
            columns.push_back(array);
        }

        // Build the schema for the parquet file
        vector<shared_ptr<arrow::Field>> fields;
        fields.reserve(num_columns);
        for (int c = 0; c < num_columns; ++c) {
            // Create a field for each column
            fields.push_back(arrow::field(column_names[c], arrow::float64()));
        }

        auto schema = make_shared<arrow::Schema>(fields);
        auto table = arrow::Table::Make(schema, columns);

        // MODIFIED BY 07-16-2025 by Lillian Lau
        string outfile_name = timeseries_directory + file_name + "_r"
                            + std::to_string(r) + ".parquet";
        shared_ptr<arrow::io::FileOutputStream> output_file;
        
        PARQUET_ASSIGN_OR_THROW(
            output_file,
            arrow::io::FileOutputStream::Open(outfile_name));
        
        auto writer_properties = parquet::WriterProperties::Builder()
            .compression(parquet::Compression::SNAPPY)
            ->build();

        int chunk_size = num_rows;
        PARQUET_THROW_NOT_OK(
            parquet::arrow::WriteTable(*table, pool, output_file, 
                chunk_size, writer_properties));
        PARQUET_THROW_NOT_OK(output_file->Close());
    }
    
    if (failure_flag) {
        throw std::runtime_error("Arrow Append failed: " + failure_msg);
    }
    
}

/**
 * @brief Prints the output of the specified data collector type to Parquet files.
 * 
 * @param week_i Starting week
 * @param week_f Ending week
 * @param d_type The data collector type (e.g., "utilities", "policies", "water_source")
 * @param file_name The output file name prefix
 * 
 * ADDED on 06-02-2026 by Lillian Lau
 */
void MasterDataCollector::printOutputParquet(
    int week_i, int week_f, string d_type, string file_name) {

    vector<string> column_names = getColumnNames(d_type);
    vector<vector<DataCollector *>> p_matrix;
    vector<vector<UtilitiesDataCollector *>> u_matrix;

    if (d_type == "Utilities") {
        // assign u_matrix to utility_collectors if it is not empty
        if (!utility_collectors.empty()) {
            u_matrix = utility_collectors;
        }
    } else if (d_type == "Policies") {
        // assign p_matrix to utility_collectors if it is not empty
        if (!drought_mitigation_policy_collectors.empty()) {
            p_matrix = drought_mitigation_policy_collectors;
        }
    } else if (d_type == "WaterSources") {
        // assign p_matrix to water_source_collectors if it is not empty
        if (!water_source_collectors.empty()) {
            p_matrix = water_source_collectors;
        }
    }   

    bool failure_flag = false;
    string failure_msg;

    #pragma omp parallel for default(none) \
        shared(file_name, week_i, week_f, d_type, column_names, p_matrix, u_matrix, failure_flag, failure_msg)
    for (int rr = 0; rr < (int) realizations_ran.size(); ++rr) {
        int r = realizations_ran[rr];

        int num_columns = column_names.size();
        int num_rows = week_f - week_i;

        arrow::MemoryPool *pool = arrow::default_memory_pool();
        vector<shared_ptr<arrow::DoubleBuilder>> builders;
        builders.reserve(num_columns);
        for (int i = 0; i < num_columns; ++i) {
            builders.push_back(make_shared<arrow::DoubleBuilder>(pool));
        }

        for (int w = week_i; w < week_f; ++w) {
            vector<double> row_data;
            row_data.reserve(num_columns);
            // If p_matrix is not empty, iterate through it
            if (!p_matrix.empty() && ((d_type == "Policies") || (d_type == "WaterSources"))) {
                for (vector<DataCollector *> p_vec : p_matrix) {
                    DataCollector* dc = p_vec[r];
                    string data_w = dc-> printCompactString(w);

                    // Split data_w by ',' into individual values:
                    stringstream ss(data_w);
                    string token;

                    while(std::getline(ss, token, ',')) {
                        double d = 0.0;
                        // Convert token to double and append to the builder:
                        try {
                            d = std::stod(token);
                        } catch (const std::exception &e) {
                            d = std::numeric_limits<double>::quiet_NaN();
                        }
                        row_data.push_back(d);
                    }
                }

            } else if (!u_matrix.empty() && d_type == "Utilities") {
                // If u_matrix is not empty, iterate through it
                for (vector<UtilitiesDataCollector *> u_vec : u_matrix) {
                    UtilitiesDataCollector* udc = u_vec[r];
                    string data_w = udc-> printCompactString(w);

                    // Split data_w by ',' into individual values:
                    stringstream ss(data_w);
                    string token;

                    while(std::getline(ss, token, ',')) {
                        double d = 0.0;
                        // Convert token to double and append to the builder:
                        try {
                            d = std::stod(token);
                        } catch (const std::exception &e) {
                            d = std::numeric_limits<double>::quiet_NaN();
                        }
                        row_data.push_back(d);
                    }
                }
            }
            
            for (int c = 0; c < num_columns; ++c) {
                // Append the value to the corresponding column builder
                arrow::Status st = builders[c]->Append(row_data[c]);
                if (!st.ok()) {
                    failure_flag = true;
                    failure_msg = st.ToString();
                    break;
                }
            }
        }

        vector<shared_ptr<arrow::Array>> columns;
        columns.reserve(num_columns);
        for (int c = 0; c < num_columns; ++c) {
            shared_ptr<arrow::Array> array;
            PARQUET_THROW_NOT_OK(builders[c]->Finish(&array));
            columns.push_back(array);
        }

        // Build the schema for the parquet file
        vector<shared_ptr<arrow::Field>> fields;
        fields.reserve(num_columns);
        for (int c = 0; c < num_columns; ++c) {
            // Create a field for each column
            fields.push_back(arrow::field(column_names[c], arrow::float64()));
        }

        auto schema = make_shared<arrow::Schema>(fields);
        auto table = arrow::Table::Make(schema, columns);

        string outfile_name = output_directory + file_name + "_r"
                            + std::to_string(r) + ".parquet";
        shared_ptr<arrow::io::FileOutputStream> output_file;
        
        PARQUET_ASSIGN_OR_THROW(
            output_file,
            arrow::io::FileOutputStream::Open(outfile_name));
        
        auto writer_properties = parquet::WriterProperties::Builder()
            .compression(parquet::Compression::SNAPPY)
            ->build();

        int chunk_size = num_rows;
        PARQUET_THROW_NOT_OK(
            parquet::arrow::WriteTable(*table, pool, output_file, 
                chunk_size, writer_properties));
        PARQUET_THROW_NOT_OK(output_file->Close());
    }
    
    if (failure_flag) {
        throw std::runtime_error("Arrow Append failed: " + failure_msg);
    }
    
}

void MasterDataCollector::printPoliciesOutputCompact(
        int week_i, int week_f, string file_name) {
    if (!drought_mitigation_policy_collectors.empty()) {
#pragma omp parallel for
        for (int rr = 0; rr < (int) realizations_ran.size(); ++rr) {
            auto r = realizations_ran[rr];
            std::ofstream out_stream;
            out_stream.open(output_directory + file_name + "_r"
                            + std::to_string(r) + ".csv");

            string line;
            for (vector<DataCollector *> p : drought_mitigation_policy_collectors)
                line += p[r]->printCompactStringHeader();
            line.pop_back();
            out_stream << line << endl;

            for (int w = week_i; w < week_f; ++w) {
                line = "";
                for (vector<DataCollector *> p : drought_mitigation_policy_collectors)
                    line += p[r]->printCompactString(w);
                line.pop_back();
                out_stream << line << endl;
            }

            out_stream.close();
        }
    }
}

void MasterDataCollector::printUtilitiesOutputCompact(
        int week_i, int week_f, string file_name) {
#pragma omp parallel for
    for (int rr = 0; rr < (int) realizations_ran.size(); ++rr) {
        auto r = realizations_ran[rr];
        std::ofstream out_stream;
        out_stream.open(output_directory + file_name + "_r"
                        + std::to_string(r) + ".csv");

        string line;
        for (vector<UtilitiesDataCollector *> &p : utility_collectors)
            line += p[r]->printCompactStringHeader();
        line.pop_back();
        out_stream << line << endl;

        for (int w = week_i; w < week_f; ++w) {
            line = "";
            for (vector<UtilitiesDataCollector *> &p : utility_collectors)
                line += p[r]->printCompactString(w);
            line.pop_back();
            out_stream << line << endl;
        }

        out_stream.close();
    }
}

void MasterDataCollector::printWaterSourcesOutputCompact(
        int week_i, int week_f, string file_name) {
#pragma omp parallel for
    for (int rr = 0; rr < (int) realizations_ran.size(); ++rr) {
        auto r = realizations_ran[rr];
        try {
            std::ofstream out_stream;
            out_stream.open(output_directory + file_name + "_r"
                            + std::to_string(r) + ".csv");

            string line;
            for (vector<DataCollector *> p : water_source_collectors)
                line += p[r]->printCompactStringHeader();
            line.pop_back();
            out_stream << line << endl;

            for (int w = week_i; w < week_f; ++w) {
                line = "";
                for (vector<DataCollector *> p : water_source_collectors)
                    line += p[r]->printCompactString(w);
                line.pop_back();
                out_stream << line << endl;
            }

            out_stream.close();
        } catch (...) {
            printf("Warning: water sources data for realization %lu not saved due to error.\n", r);
        }
    }
}

void MasterDataCollector::printUtilityObjectivesToRowOutStream(vector<UtilitiesDataCollector *> &u,
        std::ofstream &outStream, vector<double> &objectives) {
    // Create vector with restriction policies pertaining only to the
    // utility whose objectives are being calculated.
    vector<RestrictionsDataCollector *> utility_restrictions(
            *max_element(realizations_ran.begin(), realizations_ran.end()) + 1
    );
    isolateRestrictionDataCollectors(u, utility_restrictions);

    // Reliability
    double reliability = ObjectivesCalculator::calculateReliabilityObjective(u, realizations_ran);
    /// Restriction Frequency
    double restriction_freq = ObjectivesCalculator::
    calculateRestrictionFrequencyObjective(utility_restrictions, realizations_ran);
    /// Infrastructure NPC
    double inf_npc = ObjectivesCalculator::
    calculateNetPresentCostInfrastructureObjective(u, realizations_ran);
    /// Peak Financial Cost
    double financial_cost = ObjectivesCalculator::
    calculatePeakFinancialCostsObjective(u, realizations_ran);
    /// Worse Case Costs
    double worse_cost = ObjectivesCalculator::calculateWorseCaseCostsObjective(u, realizations_ran);

    outStream << setw(COLUMN_WIDTH) << u[realizations_ran[0]]->name
              /// Reliability
              << setw(COLUMN_WIDTH * 2)
              << setprecision(COLUMN_PRECISION)
              << reliability
              /// Restriction Frequency
              << setw(COLUMN_WIDTH * 2)
              << setprecision(COLUMN_PRECISION)
              << restriction_freq
              /// Infrastructure NPC
              << setw(COLUMN_WIDTH * 2)
              << setprecision(COLUMN_PRECISION)
              << inf_npc
              /// Peak Financial Cost
              << setw(COLUMN_WIDTH * 2)
              << setprecision(COLUMN_PRECISION)
              << financial_cost
              /// Worse Case Costs
              << setw(COLUMN_WIDTH * 2)
              << setprecision(COLUMN_PRECISION)
              << worse_cost
              << endl;

    objectives.push_back(reliability);
    objectives.push_back(restriction_freq);
    objectives.push_back(inf_npc);
    objectives.push_back(financial_cost);
    objectives.push_back(worse_cost);
}

vector<double> MasterDataCollector::calculatePrintObjectives(string file_name, bool print) {
    vector<double> objectives;
    /**
    if (print) {
        cout << "Calculating and printing Objectives" << endl;
        string obj_file_path = output_directory + file_name + ".out";
        std::ofstream outStream;
        outStream.open(obj_file_path);

        outStream << setw(COLUMN_WIDTH) << "      " << setw((COLUMN_WIDTH * 2))
                  << "Reliability"
                  << setw(COLUMN_WIDTH * 2) << "Restriction Freq."
                  //              << setw(COLUMN_WIDTH * 2) << "Jordan Lake Alloc."
                  << setw(COLUMN_WIDTH * 2) << "Infrastructure NPC"
                  << setw(COLUMN_WIDTH * 2) << "Peak Financial Cost"
                  << setw(COLUMN_WIDTH * 2) << "Worse Case Costs" << endl;

        for (auto &u : utility_collectors) {
            printUtilityObjectivesToRowOutStream(u, outStream, objectives);
        }

        outStream.close();

        for (int i = 0; i < (int) objectives.size(); ++i) {
            double o = objectives.at(i);
            if (o > 10e10 || o < -0.1) {
                char error[512];
                sprintf(error, "Objective %d has absurd value of %f. Aborting.\n", i, o);
                throw_with_nested(runtime_error(error));
            }
        }
    } else {
      */
    for (auto &u : utility_collectors) {
        // Create vector with restriction policies pertaining only to the
        // utility whose objectives are being calculated.
        vector<RestrictionsDataCollector *> utility_restrictions(
                *max_element(realizations_ran.begin(), realizations_ran.end()) + 1
                );
        isolateRestrictionDataCollectors(u, utility_restrictions);

        objectives.push_back
                (ObjectivesCalculator::calculateReliabilityObjective(u, realizations_ran));
        objectives.push_back
                (ObjectivesCalculator::calculateRestrictionFrequencyObjective(utility_restrictions, realizations_ran));
        objectives.push_back
                (ObjectivesCalculator::calculateNetPresentCostInfrastructureObjective(u, realizations_ran));
        objectives.push_back
                (ObjectivesCalculator::calculatePeakFinancialCostsObjective(u, realizations_ran));
        objectives.push_back
                (ObjectivesCalculator::calculateWorseCaseCostsObjective(u, realizations_ran));
    }
    return objectives;
}

void MasterDataCollector::isolateRestrictionDataCollectors(vector<UtilitiesDataCollector *> &u,
                                                           vector<RestrictionsDataCollector *> &utility_restrictions) const {
    for (auto &p : drought_mitigation_policy_collectors)
                if (p.at(realizations_ran.at(0))->type == RESTRICTIONS && p[realizations_ran[0]]->id == u.at(realizations_ran[0])->id)
                    for (auto i : realizations_ran) {
                        utility_restrictions.at(i) =
                                dynamic_cast<RestrictionsDataCollector *>(p.at(i));
                    }
}

void MasterDataCollector::performBootstrapAnalysis(
		int sol_id, int n_sets, int n_samples, int n_threads, vector<vector<int>> bootstrap_samples) {
    printf("Running bootstrap samples.\n");
    vector<vector<int>> bootstrap_sample_sets((unsigned long) n_sets, vector<int>((unsigned long) n_samples));

    // Create or use specified bootstrap samples
    readOrCreateBSSamples(sol_id, n_sets, n_samples, bootstrap_samples, bootstrap_sample_sets);

    vector<vector<double>> objectives((unsigned long) n_sets);
    for (unsigned long &r : crashed_realizations) {
        for (vector<int> &bs : bootstrap_sample_sets) {
            bs.erase(remove(bs.begin(), bs.end(), r), bs.end());
        }
    }

//#pragma omp parallel for num_threads(n_threads) shared(objectives)
    for (int set = 0; set < n_sets; ++set) {
        // Calculate objectives for the set of bootstrapped realizations.
        vector<unsigned long> bootstrap_sample_set = vector<unsigned long>(
                bootstrap_sample_sets[set].begin(),
                bootstrap_sample_sets[set].end());

        for (unsigned long &r : crashed_realizations) {
            for (unsigned long &bs : bootstrap_sample_set) {
                if (bs >= r) {
                    --bs;
                }
            }
        }

        for (auto &u : utility_collectors) {
            // Create vector with restriction policies pertaining only to the
            // utility whose objectives are being calculated.
            vector<RestrictionsDataCollector *> utility_restrictions(
                    *max_element(realizations_ran.begin(), realizations_ran.end()) + 1
            );
            isolateRestrictionDataCollectors(u, utility_restrictions);

            // Populate vector of objectives for each corresponding set of bootstrap samples.
            objectives[set].push_back
                    (ObjectivesCalculator::calculateReliabilityObjective(u, bootstrap_sample_set));
            objectives[set].push_back
                    (ObjectivesCalculator::calculateRestrictionFrequencyObjective(utility_restrictions, bootstrap_sample_set));
            objectives[set].push_back
                    (ObjectivesCalculator::calculateNetPresentCostInfrastructureObjective(u, bootstrap_sample_set));
            objectives[set].push_back
                    (ObjectivesCalculator::calculatePeakFinancialCostsObjective(u, bootstrap_sample_set));
            objectives[set].push_back
                    (ObjectivesCalculator::calculateWorseCaseCostsObjective(u, bootstrap_sample_set));
        }
    }

    // Print objectives of bootstrap samples
    printObjsBSSamples(sol_id, n_sets, n_samples, objectives);

    // Print objectives of all realizations
    printObjectivesOfAllRealizationsForBSAnalysis(sol_id, n_sets, n_samples);

    // Print bootstrap samples file.
    printBSSamples(sol_id, n_sets, n_samples, bootstrap_sample_sets);

}

void MasterDataCollector::printBSSamples(int sol_id, int n_sets, int n_samples,
                                         const vector<vector<int>> &bootstrap_sample_sets) const {
    ofstream outStream_realizations; // Either read samples from file or create new ones.
    outStream_realizations.open(output_directory + "bootstrap_realizations_" +
                                to_string(n_sets) + "_" + to_string(n_samples) + "_S" +
                                to_string(sol_id) + ".csv");

    string line;
    for (int set = 0; set < n_sets; ++set) {
        // Generate one set of bootstrapped realizations, if none was specified.
        line = "";
        for (int s : bootstrap_sample_sets[set]) {
            line += to_string(s) + ",";
        }
        line.pop_back();
        outStream_realizations << line << endl;
    }

    outStream_realizations.close();
}

void MasterDataCollector::printObjectivesOfAllRealizationsForBSAnalysis(int sol_id, int n_sets, int n_samples) {
    string file_name = output_directory + "objectives_all_reals_" + to_string(n_sets) +
                       "_" + to_string(n_samples) + "_S" + to_string(sol_id) + ".csv";
    vector<double> objectives_all_reals = calculatePrintObjectives("", false);

    string line;
    line = "";
    for (double &o : objectives_all_reals) {
	    line += to_string(o) + ",";
    }
    line.pop_back();

    ofstream outStream_objs_all_reals;
    outStream_objs_all_reals.open(file_name);
    outStream_objs_all_reals << line << endl;

    outStream_objs_all_reals.close();
}

void MasterDataCollector::printObjsBSSamples(int sol_id, int n_sets, int n_samples,
                                              vector<vector<double>> &objectives) {// Print objectives.
    ofstream outStream_objs;
    string objectives_file_name = output_directory + "bootstrap_objs_" + to_string(n_sets) + "_" +
                        to_string(n_samples) + "_S" + to_string(sol_id) + ".csv";
    outStream_objs.open(objectives_file_name);
    printf("Bootstrap objectives files will be printed at %s\n", objectives_file_name.c_str());

    string line;
    for (int set = 0; set < n_sets; ++set) {
        line = "";
        for (double &o : objectives[set]) {
            line += to_string(o) + ",";
        }
        line.pop_back();
        outStream_objs << line << endl;
    }
    outStream_objs.close();
}

void MasterDataCollector::readOrCreateBSSamples(int sol_id, int n_sets, int n_samples,
                                                const vector<vector<int>> &bootstrap_samples,
                                                vector<vector<int>> &bootstrap_sample_sets) const {
    random_device rd;     // only used once to initialise (seed) engine
    mt19937 rng((seed == NON_INITIALIZED ? rd() : seed));    // random-number engine used (Mersenne-Twister in this case)

    int min = 0;
    int max = (int) n_realizations - 1;
    uniform_int_distribution<int> uni(min, max); // guaranteed unbiased
    string line;
    if (!bootstrap_samples.empty()) {
	    bootstrap_sample_sets = bootstrap_samples;
    } else {
        for (int set = 0; set < n_sets; ++set) {
            // Generate one set of bootstrapped realizations, if none was specified.
            for (int &s : bootstrap_sample_sets[set]) {
                s = uni(rng);
            }
        }
    }
}

void MasterDataCollector::printPathways(string file_name) {
    std::ofstream outStream;
    // ADDED ON 07-16-2025 by Lillian Lau
    string pathways_directory = output_directory + "pathways/";
    //outStream.open(output_directory + file_name + ".out");
    // MODIFIED BY LILLIAN LAU ON 07-16-2025
    outStream.open(pathways_directory + file_name + ".out");

    outStream << "Realization\tutility\tweek\tinfra." << endl;

    for (auto &uc : utility_collectors)
        for (int rr = 0; rr < (int) realizations_ran.size(); ++rr) {
            auto r = realizations_ran[rr];
            for (vector<int> infra : uc[r]->getPathways()) {
                outStream << r << "\t" << infra[0] << "\t" << infra[1] << "\t"
                          << infra[2] << endl;
            }
        }

    outStream.close();
}

void MasterDataCollector::setOutputDirectory(string io_directory) {
    // MODIFIED BY LILLIAN LAU ON 07-16-2025
    // Check if io_directory is not being set for the same io_directory it is already set. Avoids unnecessary verbose.
    /**
    if (io_directory != output_directory) {
        output_directory = io_directory + DEFAULT_OUTPUT_DIR;
        Utils::createDir(output_directory);
        cout << "Output will be printed to folder " << output_directory << endl;
    }
    */
   output_directory = io_directory;
}

DataCollector* MasterDataCollector::createPolicyDataCollector(DroughtMitigationPolicy* dmp, unsigned long r) {
    if (dmp->type == RESTRICTIONS)
        return new RestrictionsDataCollector(dynamic_cast<Restrictions *> (dmp), r);
    else if (dmp->type == TRANSFERS)
        return new TransfersDataCollector(dynamic_cast<Transfers *> (dmp), r);
    else if (dmp->type == INSURANCE_STORAGE_ROF)
        return new EmptyDataCollector();
    else
        throw invalid_argument("Drought mitigation policy not recognized. "
                                 "Did you forget to add it to the "
                                 "MasterDataCollector::addRealization"
                                 " function?");
}

DataCollector* MasterDataCollector::createWaterSourceDataCollector(WaterSource* ws, unsigned long r) {
    if (ws->source_type == RESERVOIR)
        return new ReservoirDataCollector(dynamic_cast<Reservoir *> (ws), r);
    else if (ws->source_type == INTAKE)
        return new IntakeDataCollector(dynamic_cast<Intake *> (ws), r);
    else if (ws->source_type == QUARRY)
        return new QuaryDataCollector(dynamic_cast<Quarry *> (ws), r);
    else if (ws->source_type == WATER_REUSE)
        return new WaterReuseDataCollector(dynamic_cast<WaterReuse *> (ws), r);
    else if (ws->source_type == ALLOCATED_RESERVOIR)
        return new AllocatedReservoirDataCollector(dynamic_cast<AllocatedReservoir *> (ws), r);
    else if (ws->source_type ==
             RESERVOIR_EXPANSION ||
             ws->source_type ==
             NEW_WATER_TREATMENT_PLANT ||
             ws->source_type ==
             SOURCE_RELOCATION)
        return new EmptyDataCollector();
    else
        throw invalid_argument("Water source not recognized. "
                                 "Did you forget to add it to the "
                                 "MasterDataCollector::addRealization"
                                 " function?");
}

void MasterDataCollector::addRealization(
        vector<WaterSource *> water_sources_realization,
        vector<DroughtMitigationPolicy *> drought_mitigation_policies_realization,
        vector<Utility *> utilities_realization,
        unsigned long r) {
    // If collectors vectors have not yet been initialized, initialize them.
#pragma omp critical
    {
        if (water_source_collectors.empty()) {
            water_source_collectors = vector<vector<DataCollector *>>
                    (water_sources_realization.size(), vector<DataCollector *>(n_realizations));
            drought_mitigation_policy_collectors = vector<vector<DataCollector *>>
                    (drought_mitigation_policies_realization.size(), vector<DataCollector *>(n_realizations));
            utility_collectors = vector<vector<UtilitiesDataCollector *>>
                    (utilities_realization.size(), vector<UtilitiesDataCollector *>(n_realizations));
        }
    };

    // Create utilities data collectors
    for (int u = 0; u < (int) utilities_realization.size(); ++u) {
        utility_collectors[u][r] = new UtilitiesDataCollector(utilities_realization[u], r);
    }

    // Create drought mitigation policies data collector
    for (int dmp = 0; dmp < (int) drought_mitigation_policies_realization.size(); ++dmp)
        drought_mitigation_policy_collectors[dmp][r] =
                createPolicyDataCollector(drought_mitigation_policies_realization[dmp], r);

    // Create water sources data collectors
    for (int ws = 0; ws < (int) water_sources_realization.size(); ++ws) {
        water_source_collectors[ws][r] = createWaterSourceDataCollector(water_sources_realization[ws], r);
    }
} 

void MasterDataCollector::removeRealization(unsigned long r) {
    for (int u = 0; u < (int) utility_collectors.size(); ++u) {
        delete utility_collectors[u][r];
        utility_collectors[u][r] = nullptr;
    }
    for (int dmp = 0; dmp < (int) drought_mitigation_policy_collectors.size(); ++dmp) {
	delete drought_mitigation_policy_collectors[dmp][r];
        drought_mitigation_policy_collectors[dmp][r] = nullptr;
    }
    for (int ws = 0; ws < (int) water_source_collectors.size(); ++ws) {
	delete water_source_collectors[ws][r];
        water_source_collectors[ws][r] = nullptr;
    }

    crashed_realizations.push_back(r);
}

void MasterDataCollector::cleanCollectorsOfDeletedRealizations() {
    for (auto &v : utility_collectors) {
        v.erase(remove_if(v.begin(), v.end(), [](const void *x) { return x == nullptr; }), v.end());
    }
    for (auto &v : drought_mitigation_policy_collectors) {
        v.erase(remove_if(v.begin(), v.end(), [](const void *x) { return x == nullptr; }), v.end());
    }
    for (auto &v : water_source_collectors) {
        v.erase(remove_if(v.begin(), v.end(), [](const void *x) { return x == nullptr; }), v.end());
    }
}


void MasterDataCollector::collectData(unsigned long r) {
    for (vector<UtilitiesDataCollector *> &uc : utility_collectors)
        uc[r]->collect_data();
    for (vector<DataCollector *> dmp : drought_mitigation_policy_collectors)
        dmp[r]->collect_data();
    for (vector<DataCollector *> ws : water_source_collectors)
        ws[r]->collect_data();
}

void MasterDataCollector::setSeed(int seed) {
    MasterDataCollector::seed = seed;
}

void MasterDataCollector::unsetSeed() {
    MasterDataCollector::seed = NON_INITIALIZED;
}
