/**
 * @class WaterSource
 * @brief This class defines the main `WaterSource` object that provides basic representation of a general water source for a utility.
 * 
 * Created by bernardo on 1/22/17.
 */

#ifndef TRIANGLEMODEL_WATERSOURCE_H
#define TRIANGLEMODEL_WATERSOURCE_H

#include <string>
#include "../../Catchment.h"
#include "../../../Utils/Constants.h"
#include "../../Bonds/Base/Bond.h"

using namespace std;
using namespace Constants;

class WaterSource {
protected:
    /**
     * @brief Total available volume in the water source.
     */
    double available_volume = 0;
    /**
     * @brief Total outflow from the water source.
     */
    double total_outflow = 0;
    /**
     * @brief Total upstream inflow to the water source.
     */
    double upstream_source_inflow = 0;
    /**
     * @brief Total inflow from wastewater treatment plants.
     */
    double wastewater_inflow = 0;
    /**
     * @brief Total inflow from upstream catchments into the water source.
     */
    double upstream_catchment_inflow = 0;
    /**
     * @brief Total demand being drawn from the water source.
     */
    double total_demand = 0;
    /**
     * @brief The total demand added to the water source by policy.
     * FIXME: Definition is not clear.
     * REMOVED BY LILLIAN LAU ON 08-05-2025
     */
    //double policy_added_demand = 0;

    /**
     * @brief Permitting time required to develop the water source.
     */
    double permitting_time = NON_INITIALIZED;
    /**
     * @brief Minimum upstream environmental inflow required by the water source.
     */
    double upstream_min_env_inflow = 0;
    /**
     * @brief Total capacity of the water source.
     */
    double capacity = NON_INITIALIZED;
    /**
     * @brief Sequence of water sources that are connected to this water source.
     * FIXME: Needs to be clarified.
     */
    vector<int> built_in_sequence;
    /**
     * REMOVED BY LILLIAN LAU ON 08-05-2025
     */
    //vector<int> construction_prerequisites;
    /**
     * @brief Available allocated volumes of raw water supplying the water source.
     */
    vector<double> available_allocated_volumes;
    /**
     * @brief Allocated capacities of the water source to each utility.
     */
    vector<double> allocated_capacities;
    /**
     * @brief Allocated treatment capacities of the water source to each utility.
     */
    vector<double> allocated_treatment_capacities;
    /**
     * @brief Allocated treatment fractions of the water source to each utility.
     */
    vector<double> allocated_treatment_fractions;
    /**
     * @brief Allocated overall fractions of the water source to each utility.
     */
    vector<double> allocated_fractions;
    /**
     * @brief Utilities with allocations to the water source.
     */
    vector<double> supply_allocated_fractions;

    /**
     * @brief Utilities with allocations to the water source.
     */
    vector<int> utilities_with_allocations;
    /**
     * @brief Bonds associated with the water source.
     */
    vector<Bond *> bonds;
    /**
     * @brief A vector of all the catchments that supply the water source.
     */
    vector<Catchment> catchments;
    /**
     * @brief ID of the water quality pool, which is the water source used to ensure minimum 
     *     environmental flows downstream.
     */
    int wq_pool_id = NON_INITIALIZED;
    double total_allocated_fraction = NON_INITIALIZED;
    bool online;
    /**
     * @brief Minimum environmental outflow required by the water source.
     */
    double min_environmental_outflow = 0;
    /**
     * @brief Total volume evaporated from the water source.
     */
    double evaporated_volume = 0;
    /**
     * @brief Total treatment capacity of the water source.
     */
    double total_treatment_capacity = 0;
    /**
     * @brief The ID of the utility with the highest percent allocation from the water source.
     */
    int highest_alloc_id = NON_INITIALIZED;
    /**
     * @brief Seed to generate the random construction time.
     * FIXME: Needs to be clarified.
     */
    static int seed;
    /**
     * @brief This function handles the continuity calculations for a water source, ensuring proper inflow and outflow.
     * 
     * @param week The current week of the simulation.
     * @param upstream_source_inflow The inflow from upstream water sources.
     * @param wastewater_inflow The inflow from wastewater treatment plants.
     * @param demand_outflow A reference to a vector representing the water demand outflows to each utility.
     * 
     * @return void
     */
    virtual void applyContinuity(int week, double upstream_source_inflow,
                                     double wastewater_inflow,
                                     vector<double> &demand_outflow) = 0;
    /**
     * @brief This function bypasses continuity calculations, routing all inflows downstream without adjustments.
     *      Does not apply continuity to the water source, by instead just treats it as
     *      non existing, i.e. outflow = inflow + catchment_flow
     * 
     * @param week The current week of the simulation.
     * @param total_upstream_inflow The total inflow from upstream sources excluding water for the catchment 
     *      between both water sources.
     * 
     * @return void
     */
    void bypass(int week, double total_upstream_inflow);

public:
    /**
     * @brief The unique identifier of the water source.
     */
    const int id;
    /**
     * @brief The type of the water source.
     */
    const int source_type;
    /**
     * @brief The time required to build the water source.
     */
    const double construction_time;
    /**
     * @brief The name of the water source.
     */
    string name;
    
    /**
     * @brief This constructor initializes a basic `WaterSource` object with the given parameters.
     *      Used when the water source is built and operational.
     * 
     * @param name The name of the water source.
     * @param id The unique identifier for the water source.
     * @param catchments A vector of pointers to `Catchment` objects supplying this water source.
     * @param capacity The total storage capacity of the water source.
     * @param treatment_capacity The total treatment capacity of the water source.
     * @param source_type The type of water source (e.g., reservoir, river, etc.).
     * 
     * @return A new `WaterSource` object.
     */
    WaterSource(string name, const int id,
                const vector<Catchment *> &catchments,
                const double capacity, double treatment_capacity,
                    const int source_type);
    
    /**
     * @brief This constructor initializes a `WaterSource` object with pre-set allocation fractions and connected source details.
     *      Used when a water source is built and operational
     * 
     * @param name The name of the water source.
     * @param id The unique identifier for the water source.
     * @param catchments A vector of pointers to `Catchment` objects associated with this water source.
     * @param capacity The total storage capacity of the water source (in cubic meters).
     * @param treatment_capacity The total treatment capacity of the water source (in cubic meters per week).
     * @param connected_sources A vector of IDs representing sources connected to this water source.
     * @param source_type The type of water source (e.g., reservoir, river, etc.).
     * @param allocated_treatment_fractions A pointer to a vector of pre-set fractions of the total treatment capacity allocated to utilities.
     * @param allocated_fractions A pointer to a vector of pre-set fractions of the total storage capacity allocated to utilities.
     * @param utilities_with_allocations A pointer to a vector of IDs for utilities allocated to this water source.
     * 
     * @return A new `WaterSource` object.
     * 
     * @throws std::runtime_error if any issues arise in setting allocations (handled by `setAllocations`).
     */
    WaterSource(string name, const int id,
                const vector<Catchment *> &catchments,
                const double capacity, double treatment_capacity,
                vector<int> connected_sources,
                const int source_type,
                vector<double> allocated_treatment_fractions,
                vector<double> allocated_fractions,
                vector<int> utilities_with_allocations);

    /**
     * @brief This constructor initializes a `WaterSource` object with the provided parameters, including construction and permitting details.
     *      Used when the water source does not exist in the beginning of the simulation for water sources financed by only one bond.
     * 
     * @param name The name of the water source.
     * @param id The unique identifier for the water source.
     * @param catchments A vector of pointers to `Catchment` objects associated with this water source.
     * @param capacity The total storage capacity of the water source.
     * @param treatment_capacity The total treatment capacity of the water source.
     * @param connected_sources A vector of IDs representing sources connected to this water source.
     * @param source_type The type of water source (e.g., reservoir, river, etc.).
     * @param construction_time_range A vector containing the minimum and maximum construction time range.
     * @param permitting_period The permitting time required for the water source.
     * @param bond A reference to a `Bond` object associated with financing this water source.
     * 
     * @return A new `WaterSource` object.
     * 
     * @throws std::invalid_argument if permitting or construction time is NaN or negative  (checked by `checkForInputErrorsConstruction`).
     */
    WaterSource(string name, const int id,
                const vector<Catchment *> &catchments,
                const double capacity, double treatment_capacity, vector<int> connected_sources,
                const int source_type, const vector<double> construction_time_range,
                double permitting_period, Bond &bond);
    
    /**
     * @brief This constructor initializes a `WaterSource` object with comprehensive parameters, including multiple bonds, construction, and permitting details.
     *      Used when the water source does not exist in the beginning of the simulation for water sources financed by a series of bonds (shared water source).
     * @param name The name of the water source.
     * @param id The unique identifier for the water source.
     * @param catchments A vector of pointers to `Catchment` objects associated with this water source.
     * @param capacity The total storage capacity of the water source.
     * @param treatment_capacity The total treatment capacity of the water source.
     * @param connected_sources A vector of IDs representing sources connected to this water source.
     * @param source_type The type of water source (e.g., reservoir, river, etc.).
     * @param construction_time_range A vector containing the minimum and maximum construction time range.
     * @param permitting_period The permitting time required for the water source.
     * @param bonds A vector of pointers to `Bond` objects associated with financing this water source.
     * 
     * @return A new `WaterSource` object.
     * 
     * @throws std::invalid_argument if permitting or construction time is NaN or negative  (checked by `checkForInputErrorsConstruction`).
     */
    WaterSource(string name, const int id,
                const vector<Catchment *> &catchments, const double capacity,
                double treatment_capacity, vector<int> connected_sources, const int source_type,
                const vector<double> construction_time_range, double permitting_period, vector<Bond *> bonds);

    /**
     * @brief This constructor initializes a `WaterSource` object with detailed allocation settings, construction time, and bond financing.
     *      Used when a non-shared water source with multiple stages does not exist in the beginning of the simulation 
     *      but has a built-in sequence of stages (i.e. low/high).
     * 
     * @param name The name of the water source.
     * @param id The unique identifier for the water source.
     * @param catchments A vector of pointers to `Catchment` objects associated with this water source.
     * @param capacity The total storage capacity of the water source (in cubic meters).
     * @param treatment_capacity The total treatment capacity of the water source (in cubic meters per week).
     * @param built_in_sequence A vector of IDs representing the construction sequence for this water source.
     * @param source_type The type of water source (e.g., reservoir, river, etc.).
     * @param allocated_treatment_fractions A pointer to a vector of fractions of total treatment capacity allocated to utilities.
     * @param allocated_fractions A pointer to a vector of fractions of total storage capacity allocated to utilities.
     * @param utilities_with_allocations A pointer to a vector of IDs for utilities allocated to this water source.
     * @param construction_time_range A vector containing the minimum and maximum construction time range (in years).
     * @param permitting_period The permitting time required for the water source (in years).
     * @param bond A reference to a `Bond` object representing financing details for this water source.
     * 
     * @return A new `WaterSource` object.
     * 
     * @throws std::invalid_argument if permitting or construction time is NaN or negative  (checked by `checkForInputErrorsConstruction`).
     */
    WaterSource(string name, const int id,
                const vector<Catchment *> &catchments,
                const double capacity, double treatment_capacity,
                vector<int> built_in_sequence,
                const int source_type,
                vector<double> allocated_treatment_fractions,
                vector<double> allocated_fractions,
                vector<int> utilities_with_allocations,
                const vector<double> construction_time_range, double permitting_period, Bond &bond);
    
    /**
     * @brief Copy constructor.
     * This constructor creates a copy of an existing `WaterSource` object.
     * 
     * @param water_source The `WaterSource` object to be copied.
     * 
     * @return A new `WaterSource` object that is a copy of the provided `water_source`.
     */
    WaterSource(const WaterSource &water_source);

    /**
     * @brief Destructor for the `WaterSource` class. 
     *
     * This destructor cleans up dynamically allocated memory for the `Bond` objects associated with the water source.
     * 
     * @param None
     * 
     * @return void
     */
    virtual ~WaterSource();

    /**
     * @brief Assignment operator for the `WaterSource` class.
     *
     * This operator assigns the values of one `WaterSource` object to another, making a deep copy of the relevant data.
     * 
     * @param water_source The `WaterSource` object whose data is being assigned.
     * 
     * @return A reference to the current `WaterSource` object after the assignment.
     */
    WaterSource &operator=(const WaterSource &water_source);
    
    /**
     * @brief Less-than operator for comparing two `WaterSource` objects by their `id`.
     *
     * This operator compares the `id` of the current `WaterSource` object with that of another `WaterSource` object to determine their relative order.
     * 
     * @param other A pointer to the `WaterSource` object to be compared with the current object.
     * 
     * @return `true` if the `id` of the current `WaterSource` object is less than the `id` of the `other` object, `false` otherwise.
     */
    bool operator<(const WaterSource *other);

    /**
     * @brief Greater-than operator for comparing two `WaterSource` objects by their `id`.
     * 
     * This operator compares the `id` of the current `WaterSource` object with that of another `WaterSource` object to determine their relative order.
     * 
     * @param other A pointer to the `WaterSource` object to be compared with the current object.
     * 
     * @return `true` if the `id` of the current `WaterSource` object is greater than the `id` of the `other` object, `false` otherwise.
     */
    bool operator>(const WaterSource *other);
    
    /**
     * @brief Equality operator for comparing two `WaterSource` objects by their `id`.
     * 
     * This operator checks if the `id` of the current `WaterSource` object is equal to the `id` of another `WaterSource` object.
     * 
     * @param other A pointer to the `WaterSource` object to be compared with the current object.
     * 
     * @return `true` if the `id` of the current `WaterSource` object is equal to the `id` of the `other` object, `false` otherwise.
     */
    bool operator==(const WaterSource *other);

    /**
     * @brief Manages the continuity of a water source by either applying continuity or bypassing it.
     * 
     * This function checks if the water source is online and, depending on the status, either applies continuity or bypasses the process.
     * 
     * @param week The current week of the simulation or process.
     * @param upstream_source_inflow The inflow of water from the upstream source.
     * @param wastewater_inflow The inflow of wastewater to the water source.
     * @param demand_outflow A reference to a vector that will store the demand outflow values.
     * 
     * @return void
     */
    void continuityWaterSource(int week, double &upstream_source_inflow,
                               double &wastewater_inflow,
                               vector<double> &demand_outflow);
    
    /**
     * @brief Adds treatment capacity to the water source.
     * 
     * This function increases the total treatment capacity of the water source by a specified amount.
     * This is a virtual function that can be overridden by subclasses.
     * 
     * @param added_treatment_capacity The additional treatment capacity to be added to the water source.
     * @param utility_id The ID of the utility requesting the capacity addition (UNUSED).
     * 
     * @return void
     */
    virtual void addTreatmentCapacity(const double added_treatment_capacity, int utility_id);
    
    /**
     * @brief Removes water from the water source and updates demand.
     * 
     * This function decreases the available volume of water in the source by the specified volume and increases the total demand and policy-added demand accordingly.
     * This is a virtual function that can be overridden by subclasses.
     * 
     * @param allocation_id The ID of the allocation from which the water is being removed (UNUSED).
     * @param volume The volume of water to be removed from the water source.
     * 
     * @return void
     */
    virtual void removeWater(int allocation_id, double volume);
    
    /**
     * @brief Adds capacity to the water source.
     * 
     * This function increases the capacity of the water source by a specified amount.
     * This is a virtual function that can be overridden by subclasses.
     * 
     * @param capacity The additional capacity to be added to the water source.
     * @param utility_id The ID of the utility requesting the capacity addition (UNUSED).
     * 
     * @return void
     */
    virtual void addCapacity(double capacity);

    /**
     * @brief Sets the water source to be online.
     * 
     * This is a virtual function that can be overridden by subclasses.
     * 
     * This function changes the status of the water source to online, enabling its operations.
     * 
     * @return void
     */
    virtual void setOnline();

    // virtual bool skipConstruction(int utility_id) const; // exists in 2021 version but not in 2020

    /**
     * @brief Retrieves the available allocated volume for a specific utility.
     * 
     * This function returns the available allocated volume for the given utility ID. In its current implementation,
     * it returns the total available volume, as the functionality to allocate volumes to different utilities has not been implemented.
     * This is a virtual function that can be overridden by subclasses.
     * 
     * @param utility_id The ID of the utility for which the available allocated volume is being requested (UNUSED).
     * 
     * @return The available allocated volume for the specified utility. 
     *         In the current implementation, this is equivalent to the total available volume.
     */
    virtual double getAvailableAllocatedVolume(int utility_id);

    /**
     * @brief Retrieves the minimum environmental outflow for the water source.
     * 
     * This function returns the minimum environmental outflow, which is a threshold value representing the 
     * minimum amount of water that must be released from the water source to maintain environmental standards.
     * 
     * @return The minimum environmental outflow of the water source.
     */
    double getMin_environmental_outflow() const;
    
    /**
     * @brief Sets the minimum environmental outflow for the water source.
     * 
     * This function sets the minimum environmental outflow, which represents the minimum amount of water 
     * that must be released from the water source to maintain ecological standards.
     * 
     * @param min_environmental_outflow The value to set for the minimum environmental outflow.
     * 
     * @return void
     */
    void setMin_environmental_outflow(double min_environmental_outflow);

    /**
     * @brief Retrieves the available volume of the water source.
     * 
     * This is a virtual function that can be overridden by subclasses.
     * 
     * This function returns the current available volume of the water source, which represents the amount of water 
     * that is currently available for allocation or usage.
     * 
     * @return The available volume of the water source.
     */
    double getAvailableVolume() const;

    const vector<int> &getConstructionPrerequisites() const;
    
    /**
     * @brief Sets the minimum environmental outflow for the water source.
     * 
     * This function sets the minimum environmental outflow, which represents the minimum amount of water 
     * that must be released from the water source to maintain ecological standards.
     * 
     * @param min_environmental_outflow The value to set for the minimum environmental outflow.
     * 
     * @return void
     */
    double getTotal_outflow() const;

    /**
     * @brief Checks if the water source is online.
     * 
     * This function returns a boolean indicating whether the water source is online and operational.
     * 
     * @return `true` if the water source is online, `false` otherwise.
     */
    bool isOnline() const;

    /**
     * @brief Sets the water source to its full capacity.
     * 
     * This is a virtual function that can be overridden by subclasses.
     * 
     * This function sets the available volume of the water source to its full capacity, indicating that the 
     * source is completely filled and no further volume can be added until some is used or allocated.
     * 
     * @return void
     */
    virtual void setFull();

    /**
     * @brief Sets the total outflow for the previous week.
     * 
     * This function updates the total outflow of the water source to the value provided for the previous week. 
     * This is used to track and record outflows from the water source on a weekly basis.
     * 
     * @param outflow_previous_week The outflow value for the previous week to set.
     * 
     * @return void
     */
    void setOutflow_previous_week(double outflow_previous_week);
    
    /**
     * @brief Retrieves the available supply capacity of the water source.
     * 
     * This is a virtual function that can be overridden by subclasses.
     * 
     * This function calculates and returns the available supply capacity of the water source, taking into account 
     * any allocations to the water quality pool (if initialized). If the water quality pool is not initialized, 
     * it returns the total capacity of the water source.
     * 
     * @return The available supply capacity of the water source.
     */
    virtual double getSupplyCapacity();

    /**
     * @brief Retrieves the upstream source inflow.
     * 
     * This function returns the current upstream inflow to the water source, which is the volume of water entering 
     * the source from upstream sources. This value is crucial for determining the availability of water for further 
     * allocation and usage.
     * 
     * @return The upstream source inflow in cubic units (e.g., cubic meters).
     */
    double getUpstream_source_inflow() const;
    
    /**
     * @brief Retrieves the total demand for the water source.
     * 
     * This function returns the total demand placed on the water source, which is the total volume of water 
     * required by various users or systems connected to the source.
     * 
     * @return The total demand from all utilities sharing the water source.
     */
    double getDemand() const;

    /**
     * @brief Retrieves the upstream catchment inflow.
     * 
     * This function returns the current inflow to the water source from upstream catchments. It represents the 
     * volume of water entering the source from the catchment areas upstream of the water source.
     * 
     * @return The upstream catchment inflow in cubic units (e.g., cubic meters).
     */
    double getUpstreamCatchmentInflow() const;

    /**
     * @brief Compares two water sources based on their ID.
     * 
     * This function compares two instances of `WaterSource` based on their `id` attribute. It returns `true` if 
     * the ID of the left-hand side water source (`lhs`) is less than that of the right-hand side (`rhs`).
     * This can be useful for sorting or organizing water sources by their ID.
     * 
     * @param lhs Pointer to the first `WaterSource` object.
     * @param rhs Pointer to the second `WaterSource` object.
     * 
     * @return `true` if `lhs->id` is less than `rhs->id`, otherwise `false`.
     */
    static bool compare(WaterSource *lhs, WaterSource *rhs);

    /**
     * @brief Sets the timeseries corresponding to the realization of the water source based on random factors.
     * 
     * This function updates the water source's properties, including catchment inflow, permitting time, and 
     * construction cost overruns based on the given realization index (`r`) and randomization factors (`rdm_factors`). 
     * The function applies the random factors to the catchments, permitting time, and bonds associated with the water source.
     * This function is a virtual function that can be overridden by subclasses.
     * 
     * @param r The realization index, which is used to apply different realizations of the water source.
     * @param rdm_factors A vector of DU factors, where each entry corresponds to a specific property of the water source.
     * 
     * @return void
     */
    virtual void setRealization(unsigned long r, const vector<double> &rdm_factors);

    /**
     * @brief Retrieves the allocated capacity of the water source.
     * 
     * This function returns the total capacity of the water source, which can be allocated to a utility. 
     * The function does not currently account for any specific allocation per utility, returning the full capacity.
     * This function is a virtual function that can be overridden by subclasses.
     * 
     * @param utility_id The ID of the utility requesting the allocated capacity. (Currently not used in the function.)
     * 
     * @return The total capacity of the water source, represented as a double.
     */
    virtual double getAllocatedCapacity(int utility_id);

    /**
     * @brief Retrieves the allocated fraction of the water source for a specific utility.
     * 
     * This function returns the fraction of the water source's capacity allocated to a utility. 
     * In its current implementation, the function returns a fixed value of 1.0, indicating full allocation to the utility.
     * This function is a virtual function that can be overridden by subclasses.
     * 
     * @param utility_id The ID of the utility requesting the allocated fraction. (Currently not used in the function.)
     * 
     * @return The fraction of the total water source capacity allocated to the utility, represented as a double (always 1.0).
     */
    virtual double getAllocatedFraction(int utility_id);

    /**
     * @brief Retrieves the total evaporated volume of the water source.
     * 
     * This function returns the total volume of water that has evaporated from the water source.
     * The value represents the accumulated evaporated volume for the water source over time.
     * 
     * @return The total evaporated volume, represented as a double.
     */
    double getEvaporated_volume() const;

    /**
     * @brief Retrieves the allocated treatment capacity of the water source.
     * 
     * This function returns the total treatment capacity allocated to the water source.
     * The value represents the total treatment capacity available for use by the water source.
     * This function is a virtual function that can be overridden by subclasses.
     * 
     * @param utility_id The utility identifier for which the treatment capacity is requested (UNUSED).
     * 
     * @return The allocated treatment capacity, represented as a double.
     */
    virtual double getAllocatedTreatmentCapacity(int utility_id) const;

    /**
     * @brief Retrieves the total treatment capacity of the water source.
     * 
     * This function returns the total treatment capacity allocated to the water source.
     * The value represents the overall treatment capacity available for use by the water source.
     * 
     * @param utility_id The utility identifier for which the total treatment capacity is requested.
     * 
     * @return The total treatment capacity, represented as a double.
     */
    double getTotal_treatment_capacity() const;
    
    /**
     * @brief Sets the allocations for utilities in the water source.
     * 
     * This function takes in vectors of utilities, allocated fractions, and treatment fractions,
     * and sets the allocation for the water source. It validates that the fractions sum up to 1.0,
     * assigns the appropriate values to the allocation vectors, and checks that no treatment capacity 
     * is allocated to the water quality pool.
     * 
     * Initial set up of allocations with full reservoir in the beginning of the simulations. 
     * To be used in constructors only.
     * 
     * @param utilities_with_allocations A pointer to a vector of utility IDs that have allocations to the water source.
     * @param allocated_fractions A pointer to a vector of fractions of the water source's total capacity allocated for each utility.
     * @param allocated_treatment_fractions A pointer to a vector of fractions of the water source's treatment capacity allocated for each utility.
     * 
     * @return void
     * 
     * @throws invalid_argument if any of the following conditions are not met:
     *   - The size of `utilities_with_allocations` does not match the size of `allocated_fractions`.
     *   - The sum of `allocated_fractions` is less than or greater than 1.0.
     *   - A treatment capacity is allocated to the water quality pool.
     *   - The sizes of `utilities_with_allocations`, `allocated_fractions`, and `allocated_treatment_fractions` are inconsistent.
     */
    void setAllocations(
            vector<int> &utilities_with_allocations,
            vector<double> &allocated_fractions,
            vector<double> &allocated_treatment_fractions);

    /**
     * @brief Resets the allocations for utilities in the water source.
     * 
     * This function resets the allocation fractions and recalculates the corresponding capacities
     * and available volumes for each utility. The water quality pool allocation is handled by the 
     * `wq_pool_id`. The updated allocation fractions are applied to the existing allocation data.
     * This is a virtual function that can be overridden by subclasses.
     * 
     * @param new_allocated_fractions A pointer to a vector of new allocation fractions for each utility.
     * 
     * @return void
     */
    void resetAllocations(const vector<double> *new_allocated_fractions);

    /**
     * @brief Sets the available allocated volumes to each utility and available volume from the water source.
     * 
     * This function updates the available allocated volumes for each utility and the total available volume
     * for the water source. If `utilities_with_allocations` is not null, the allocated volumes will be updated
     * accordingly.
     * 
     * @param available_allocated_volumes A vector containing the available allocated volumes for each utility.
     * @param available_volume The total available volume of the water source.
     * 
     * @return void
     */
    void setAvailableAllocatedVolumes(
            vector<double> available_allocated_volumes, double available_volume);

    /**
     * @brief Returns the available allocated volumes for the water source.
     * 
     * This function retrieves the available allocated volumes for each utility that has an allocation
     * in the water source.
     * 
     * @return A vector of available allocated volumes for each utility.
     */
    vector<double> getAvailable_allocated_volumes() const;

    /**
     * @brief Returns the IDs of the utilities with allocations for the water source.
     * 
     * This function retrieves the list of utilities that have allocations in the water source.
     * 
     * @param None
     * 
     * @return A pointer to a vector of utility IDs that have allocations in the water source.
     */
    const vector<int> &getUtilitiesWithAllocations() const;

    /**
     * @brief Returns the wastewater inflow to the water source.
     * 
     * This function retrieves the amount of wastewater inflow currently associated with the water source.
     * 
     * @return The wastewater inflow as a double value.
     */
    double getWastewater_inflow() const;

    /**
     * @brief Returns the permitting period of the water source.
     * 
     * This function retrieves the permitting period for the water source, which defines the time required 
     * for permitting operations to be completed for the water source.
     * 
     * @return The permitting period as a double value.
     */
    double getPermitting_period() const;

    /**
     * @brief Returns the available supply volume of the water source.
     * 
     * This function calculates the available supply volume by subtracting the allocated volumes for the 
     * water quality pool from the total available volume, if the water quality pool is initialized.
     * If the water quality pool is not initialized, it simply returns the total available volume.
     * 
     * @return The available supply volume as a double value.
     */
    double getAvailableSupplyVolume() const;

    /**
     * @brief This function returns the allocated inflow for a given utility.
     *
     * This function checks if the water quality pool ID is initialized. If it is not, it returns the upstream catchment inflow. 
     * If it is initialized, the function returns the fraction of inflow from the upstream catchment and allocates it to the specified utility.
     *
     * @param utility_id The ID of the utility for which the allocated inflow is being calculated.
     * 
     * @return The allocated inflow for the specified utility.
     */
    double getAllocatedInflow(int utility_id) const;

    /**
     * @brief This function returns the supply allocated fraction for a given utility.
     *
     * This function currently returns a constant value of `1.0` for any given utility ID, indicating that the entire supply is allocated to the utility. 
     * This is a virtual function that can be overridden by subclasses.
     *
     * @param utility_id The ID of the utility for which the supply allocated fraction is being calculated (UNSUED).
     * 
     * @return The supply allocated fraction (always 1.0).
     */
    virtual double getSupplyAllocatedFraction(int utility_id);

    /**
     * @brief This function returns the bond associated with a given utility ID.
     *
     * This function checks the size of the `bonds` collection. If the collection contains only one bond, it returns the first bond. 
     * Otherwise, it returns the bond corresponding to the specified `utility_id`.
     * This is a virtual function that can be overridden by subclasses.
     *
     * @param utility_id The ID of the utility for which the bond is being retrieved.
     * 
     * @return The bond associated with the specified utility ID.
     */
    Bond &getBond(int utility_id);

    /**
     * @brief This function checks for input errors related to the permitting and construction times for a water source.
     * 
     * @return Void
     * 
     * @throws std::invalid_argument if the permitting time or construction time is invalid (NaN or negative).
     */
    void checkForInputErrorsConstruction();

    /**
     * @brief This function generates a random construction time between the specified time bounds, converted to weeks.
     *
     * This function uses a random number generator to produce a random value within the range `[t0, tf]` (in years). 
     * The result is then scaled by the constant `WEEKS_IN_YEAR` to convert it into weeks. 
     * The function also incorporates a seeded random number generator based on either the `id` and `seed` or a fresh random device if `seed` is not properly initialized.
     *
     * @param t0 The lower bound of the construction time range (in years).
     * @param tf The upper bound of the construction time range (in years).
     * 
     * @return The random construction time (in weeks).
     */
    int randomConstructionTime(double t0, double tf);

    /**
     * @brief This function sets the seed value for the random number generator.
     *
     * This function assigns the provided seed value to the `seed` member of the `WaterSource` class. 
     * This seed is used to initialize the random number generator for operations that require randomness, such as generating random construction times.
     *
     * @param seed The value to set for the random number generator seed.
     * 
     * @return void
     */
    static void setSeed(int seed); 

    /**
     * @brief This function unsets the seed value for the random number generator.
     *
     * This function resets the `seed` member of the `WaterSource` class to a non-initialized state, effectively unsetting the seed. 
     * This means that any subsequent use of the random number generator will rely on a fresh random device or an unseeded state.
     *
     * @param void
     * 
     * @return void
     */
    static void unsetSeed();

    /**
     * @brief This function returns the construction time of the water source.
     *
     * This function simply returns the value of the `construction_time` member, which represents the total time required for construction, typically measured in weeks.
     * 
     * @return The construction time (in weeks).
     */
    const double getConstruction_time() const;

    /**
     * @brief This function returns the vector of allocated treatment capacities to relevant utilities.
     *
     * This function provides access to the `allocated_treatment_capacities` member, which contains the treatment capacities allocated to each utility. 
     * The returned vector holds the allocated capacities for all utilities that have allocations.
     * 
     * @return A vector containing the allocated treatment capacities for each utility.
     */
    const vector<double> &getAllocatedTreatmentCapacities() const;

    /**
     * @brief Get the vector of catchments associated with the water source.
     * 
     * @return const vector<Catchment>& 
     */
    const vector<Catchment> &getCatchments() const;
    
    /**
     * @brief Get the total treatment capacity of the water source.
     * 
     * @return double 
     */
    double getTotalTreatmentCapacity() const;

    /**
     * @brief Validates and adjusts allocation fractions for a water source to ensure they sum to 1.0.
     * 
     * @param total_allocated_fraction The sum of all allocated fractions.
     * @param utilities_with_allocations Vector of utility IDs that have allocations.
     * @param allocated_fractions VEctor of allocation fractions for each utility.
     */
    void checkAllocations(double total_allocated_fraction,
                          vector<int> &utilities_with_allocations,
                          vector<double> &allocated_fractions) const;
};


#endif //TRIANGLEMODEL_WATERSOURCE_H
