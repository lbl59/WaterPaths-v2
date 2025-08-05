//
// Created by bernardoct on 5/3/17.
//

#ifndef TRIANGLEMODEL_GROUNDWATERWELL_H
#define TRIANGLEMODEL_GROUNDWATERWELL_H

#include "Base/WaterSource.h"

class GroundWaterWell : public WaterSource {
private:
    double pump_rate = NON_INITIALIZED;

public:

    GroundwaterWell(
            string name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries &evaporation_series,
            DataSeries &storage_area_curve, double max_diversion);

    GroundwaterWell(const GroundwaterWell &groundwaterwell, const double max_diversion);

    GroundwaterWell &operator=(const GroundwaterWell &groundwaterwell);

    ~GroundwaterWell();

    void applyContinuity(
            int week, double upstream_source_inflow, double wastewater_inflow,
            vector<double> &demand_outflow) override;

    void setOnline() override;
};


#endif //TRIANGLEMODEL_GROUNDWATERWELL_H
