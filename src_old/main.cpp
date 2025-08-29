#include "SystemComponents/WaterSources/Base/WaterSource.h"
#include "Utils/QPSolver/QuadProg++.h"
#include "Utils/Solutions.h"
#include "Problem/PaperTestProblem.h"

#ifdef  PARALLEL
#include <mpi.h>
#include "../Borg/borgmm.h"
//#include "../Borg/borgms.h"
#endif

#include <algorithm>
#include <getopt.h>
#include <fstream>
#include <filesystem>

using namespace filesystem;
using namespace std;
using namespace Constants;
using namespace Solutions;

Problem *problem_ptr;
int failures = 0;

void eval(double *vars, double *objs, double *consts) {
    for (int i = 0; i < NUM_DEC_VAR; ++i) {
        if (isnan(vars[i])) {
            char error[50];
            sprintf(error, "Nan in decision variable %d", i);
            throw invalid_argument(error);
        }
    }
    failures += problem_ptr->functionEvaluation(vars, objs, consts);
    problem_ptr->destroyDataCollector();
    printf("\n");
}

int main(int argc, char *argv[]) {
    const int c_num_dec = NUM_DEC_VAR;
    const int c_num_obj = NUM_OBJECTIVES;
    int c_num_constr = 0;
    double c_obj[c_num_obj];
    double c_constr[0];

    unsigned long n_realizations = 1000;
    unsigned long n_weeks = 1565;

    string system_io = DEFAULT_DATA_DIR;
    string output_dir = DEFAULT_OUTPUT_DIR;  // ADDED ON 2025-07-16 by Lillian Lau.
    string solution_file;
    string uncertainty_file;
    string bootstrap_file;
    string utilities_rdm_file;
    string policies_rdm_file;
    string water_sources_rdm_file;
    string actions_rdm_file;  // ADDED ON 2025-04-25 by Lillian Lau.
    string inflows_demand_evap_directory_suffix;
    string rof_tables_directory = DEFAULT_ROF_TABLES_DIR;
    string rdm_directory = DEFAULT_RDM_TSERIES_DIR;  // ADDED ON 2025-05-27 by Lillian Lau.

    int standard_solution = NON_INITIALIZED;
    int n_threads = 2;
    int standard_rdm = 0;
    int first_solution = NON_INITIALIZED;
    int last_solution = NON_INITIALIZED;
    int import_export_rof_table = 0;
    //bool verbose = false; // REMOVED ON 2025-07-16 by Lillian Lau.  
    //bool tabular = false;
    bool plotting = false;
    bool run_optimization = false;
    bool print_objs_row = false;
    unsigned long n_islands = 2;
    unsigned long nfe = 1000;
    unsigned long output_frequency = 200;
    int seed = -1;
    int rdm_no = -1;
    int n_sets = -1;
    int n_bs_samples = -1;
    vector<vector<int>> realizations_to_run;
    vector<vector<double>> utilities_rdm;
    vector<vector<double>> water_sources_rdm;
    vector<vector<double>> policies_rdm;
    vector<vector<double>> actions_rdm;

    int c;
    while ((c = getopt(argc, argv,
                       "?s:u:T:r:t:d:f:l:m:v:c:p:b:i:n:o:e:y:S:A:D:R:U:P:W:I:C:O:B:")) !=
           -1) {
        switch (c) {
            case '?':
                fprintf(stdout,
                        "%s\n"
                        "\t-?: print this error_message\n"
                        "\t-s: solutions file (hard coded solutions)\n"
                        "\t-u: uncertain factors file (hard coded values)\n"
                        "\t-T: number of threads (auto)\n"
                        "\t-r: number of realizations (%lu)\n"
                        "\t-t: total simulated time in weeks (%lu)\n"
                        "\t-d: directory for system I/O (%s)\n"
                        "\t-f: first solution number\n"
                        "\t-l: last solution number\n"
                        "\t-m: individual solution number\n"
                        //"\t-v: verbose mode (false)\n"  // CHANGED ON 2025-07-16 BY LILLIAN LAU 
                        "\t-v: output file name\n"
                        "\t-p: output for plotting (smaller csv files)\n"
                        "\t-b: run optimization with Borg (false)\n"
                        "\t-i: number of islands if using Borg (2)\n"
                        "\t-n: number of function evaluations if using"
                        " Borg (1000)\n"
                        "\t-o: output frequency if using Borg (200)\n"
                        "\t-e: seed number (none)\n"
                        "\t-y: file with bootstrap samples\n"
                        "\t-S: number of bootstrap samples per set for "
                        "bootstrap analysis.\n"
                        "\t-B: number of sets of bootstrap samples for "
                        "bootstrap analysis.\n"
                        "\t-D: RDM data directory.\n"  // ADDED ON 2025-05-27 by Lillian Lau.
                        "\t-R: RDM sample number\n"
                        "\t-U: UtilityParser RDM file\n"
                        "\t-P: Policies RDM file\n"
                        "\t-W: Water sources RDM file\n"
                        "\t-A: Actions RDM file\n"
                        "\t-I: Inflows, demand, and evaporation folder suffix to"
                        " be added to folder names (e.g., _high or _inflows_bs)\n"  // CHANGED ON 2025-04-17 by Lillian Lau.
                        "\t-O: Directory containing the pre-computed "
                        "ROF table CSV files\n"
                        "\t-C: Import/export rof tables (1: export, 0:"
                        " do nothing (standard), -1: import)\n",
                        //"\t-B: Export objectives for all utilities on a single "
                        //"line",  // REMOVED ON 2025-04-25 by Lillian Lau.
                        argv[0], n_realizations, n_weeks, system_io.c_str());
                return -1;
            case 's':
                solution_file = optarg;
                break;
            case 'u':
                uncertainty_file = optarg;
                break;
            case 'T':
                n_threads = atoi(optarg);
                break;
            case 'r':
                n_realizations = (unsigned long) atoi(optarg);
                break;
            case 't':
                n_weeks = (unsigned long) atoi(optarg);
                break;
            case 'd':
                system_io = optarg;
                break;
            case 'f':
                first_solution = atoi(optarg);
                break;
            case 'l':
                last_solution = atoi(optarg);
                break;
            case 'm':
                standard_solution = atoi(optarg);
                break;
            case 'v':
                //verbose = static_cast<bool>(atoi(optarg));  // CHANGED ON 2025-07-16 by Lillian Lau.
                output_dir = optarg;  // ADDED ON 2025-07-16 by Lillian Lau.
                break;
            case 'p':
                plotting = static_cast<bool>(atoi(optarg));
                //cout << "Plotting output set to " << plotting << endl;
                break;
            case 'b':
                run_optimization = strncmp(optarg, "true", 4) == 0;
                break;
            case 'i':
                n_islands = (unsigned long) atoi(optarg);
                break;
            case 'n':
                nfe = (unsigned long) atoi(optarg);
                break;
            case 'o':
                output_frequency = (unsigned long) atoi(optarg);
                break;
            case 'e':
                seed = atoi(optarg);
                break;
            case 'y':
                bootstrap_file = optarg;
                break;
            case 'D':
                rdm_directory = optarg; // ADDED ON 2025-05-27 by Lillian Lau.
                break;
            case 'R':
                rdm_no = atoi(optarg);
                break;
            case 'U':
                utilities_rdm_file = optarg;
                break;
            case 'P':
                policies_rdm_file = optarg;
                break;
            case 'W':
                water_sources_rdm_file = optarg;
                break;
            case 'A':
                actions_rdm_file = optarg;
                break;
            case 'I':
                inflows_demand_evap_directory_suffix = optarg;
                break;
            case 'C':
                import_export_rof_table = atoi(optarg);
                break;
            case 'S':
                n_bs_samples = atoi(optarg);
                break;
            case 'B':
                n_sets = atoi(optarg);
                break;
            case 'O':
                rof_tables_directory = optarg;
                break;
            default:
                fprintf(stderr, "Unknown option (-%c)\n", c);
                return -1;
        }
    }
    
    vector<int> solutions_to_run_range;
    if (last_solution != NON_INITIALIZED) {
        solutions_to_run_range = {first_solution, last_solution};
    } else {
        solutions_to_run_range = {standard_solution, standard_solution};
    }
    
    printf("Setting up PaperTestProblem.\n"); 
    problem_ptr = new PaperTestProblem(n_weeks, n_realizations,
                                        import_export_rof_table,
                                        system_io, rof_tables_directory,
                                        seed, n_threads,
                                        rdm_no, rdm_directory, // ADDED ON 2025-05-27 by Lillian Lau.
                                        output_dir, // ADDED ON 2025-07-16 by Lillian Lau.
                                        bootstrap_file, 
                                        utilities_rdm_file,
                                        policies_rdm_file,
                                        water_sources_rdm_file,
                                        actions_rdm_file,  // ADDED ON 2025-04-25 by Lillian Lau
                                        n_sets, n_bs_samples, solution_file,
                                        solutions_to_run_range, plotting,
                                        print_objs_row, 
                                        inflows_demand_evap_directory_suffix);  // ADDED ON 2025-04-22 by Lillian Lau.
    
    // If Borg is not called, run in simulation mode
    if (!run_optimization) {
        printf("Running in simulation mode.\n");
        problem_ptr->runSimulation();
        return 0;

    } else {

    #ifdef PARALLEL
        // for debugging borg, creating file to print each ranks DVs which isdone in Eval function

        BORG_Algorithm_ms_startup(&argc, &argv);
        BORG_Algorithm_ms_islands((int) n_islands);
        BORG_Algorithm_ms_initialization(INITIALIZATION_LATIN_GLOBAL);
        BORG_Algorithm_ms_max_evaluations((int) nfe);
        BORG_Algorithm_output_frequency((int) output_frequency);

        // Define the problem.
        BORG_Problem problem = BORG_Problem_create(c_num_dec, c_num_obj,
                                                   c_num_constr,
                                                   eval);
        
        // Get rank 
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        
        if (rank == 0){ 
            cout << "Setting up optimization" << endl;
            printf("Running Borg with:\n"
                "n_islands: %lu\n"
                "nfe: %lu\n"
                "output freq.: %lu\n"
                "n_weeks: %lu\n"
                "n_realizations: %lu\n\n",
                n_islands, nfe, output_frequency, n_weeks, n_realizations);
            
            int world_size;
            MPI_Comm_size( MPI_COMM_WORLD, &world_size );
            cout << "Total number of processes launched: " << world_size << endl;
        }

        // Set all the parameter bounds and epsilons
        problem_ptr->setProblemDefinition(problem);

        if (seed != NON_INITIALIZED) {
            if (seed < 0) throw invalid_argument("See must be greater than 0.");
            srand(seed);
            WaterSource::setSeed(seed);
            //BORG_Random_seed(seed);
        }
        //char output_directory[256]
        char io_directory[256];
        char output_file_name[256], runtime_file_name[256];

        FILE *outputFile = nullptr;

        // convert output_dir to char array
        sprintf(io_directory, "%s", output_dir.c_str());

        // Create the output directory if it does not exist
        if (exists(output_dir)) {
            Utils::createDir(string(output_dir));
        }

        sprintf(output_file_name, "%srefset_MM4_N%lu_bs200_seed%lu.set", io_directory, nfe, seed);
        // sprintf(output_file_name, "%soutput/refset_MM4_N%lu_bs200_noIns_seed%lu.set", output_directory, nfe, seed);
        
        sprintf(runtime_file_name, "%sruntime_MM4_N%lu_M%lu_bs200_seed%lu.runtime", io_directory, nfe, rank, seed);
        //sprintf(runtime, "%soutput/runtime_MM4_N%lu_M%lu_bs200_noIns_seed%lu.runtime", output_directory, nfe, rank, seed);

        BORG_Algorithm_output_runtime(runtime_file_name);
        
        BORG_Random_seed(37*seed*(rank+1));

        BORG_Archive result = BORG_Algorithm_ms_run(problem); // this actually runs the optimization
        
        // If this is the master node, print out the final archive
        if (result != nullptr) {
            outputFile = fopen(output_file_name, "w");
            cout << "master node print, should see only once" << endl;
            cout << output_file_name << endl;
            if (!outputFile) {
                BORG_Debug("Unable to open final output file\n");
            }
            BORG_Archive_print(result, outputFile);
            BORG_Archive_destroy(result);
            fclose(outputFile);
        }

        printf("Number of failed function evaluations: %d.\n", failures);

        BORG_Algorithm_ms_shutdown();
        BORG_Problem_destroy(problem);
#else
        throw invalid_argument("This version of WaterPaths was not compiled "
                               "with Borg.");
#endif

        return 0;
    }
}
