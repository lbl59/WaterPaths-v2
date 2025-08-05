//
// Created by bernardoct on 5/3/17.
//

#ifndef TRIANGLEMODEL_GROUNDWATERTREATMENT_H
#define TRIANGLEMODEL_GROUNDWATERTREATMENT_H

#include "Base/WaterSource.h"

class GroundWaterTreatment : public WaterSource {
private:
    double pump_rate = NON_INITIALIZED;
    double treatment_capacity = NON_INITIALIZED;
    double treatment_cost = NON_INITIALIZED;

public:

    GroundwaterTreatment(
            string name, const int id,
            const vector<Catchment *> &catchments, const double capacity,
            const double max_treatment_capacity,
            EvaporationSeries &evaporation_series,
            DataSeries &storage_area_curve, double max_diversion);

    GroundwaterTreatment(const Quarry &quarry, const double max_diversion);

    GroundwaterTreatment &operator=(const Quarry &quarry);

    ~GroundwaterTreatment();

    void applyContinuity(
            int week, double upstream_source_inflow, double wastewater_inflow,
            vector<double> &demand_outflow) override;

    void setOnline() override;
};


#endif //TRIANGLEMODEL_GROUNDWATERTREATMENT_H
