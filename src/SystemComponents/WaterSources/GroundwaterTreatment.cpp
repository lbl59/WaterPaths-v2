//
// Created by Lillian Lau on 08-05-2025.
//

#include <numeric>
#include "GroundwaterTreatment.h"

/**
 * Copy assignment operator
 */
GroundwaterTreatment::~GroundwaterTreatment() {}

/**
 * Reservoir mass balance. Gets releases from upstream reservoirs, demands from
 * connected utilities, and
 * combines them with its catchments inflows.
 * @param week
 * @param upstream_source_inflow
 * @param demand_outflow
 */
void GroundwaterTreatment::applyContinuity(int week, double upstream_source_inflow,
                             double wastewater_inflow,
                             vector<double> &demand_outflow) {

    pump_rate = 0; // Reset pump rate for each week
}

void GroundwaterTreatment::setOnline() {
    WaterSource::setOnline();

    /// start empty and gradually fill as inflows start coming in.
    available_volume = NONE;
}
