//
// Created by bernardo on 4/12/18.
//

#include <cmath>
#include <algorithm>
#include "LevelDebtServiceBond.h"
#include "../../Utils/Utils.h"

LevelDebtServiceBond::LevelDebtServiceBond(const int id, const double cost_of_capital, const int n_payments,
                                           const double interest_rate, vector<int> pay_on_weeks,
                                           bool begin_repayment_at_issuance) :
        Bond(id, cost_of_capital, n_payments, pay_on_weeks, interest_rate, LEVEL_DEBT_SERVICE, begin_repayment_at_issuance) {
}

LevelDebtServiceBond::~LevelDebtServiceBond() = default;

/**
 * Calculates debt service payment for a given week.
 * @param week
 * @return debt service payment for that week, or 0 if no payment is due.
 */
double LevelDebtServiceBond::getDebtService(int week) {
    /// If there are still payments to be made, repayment has begun,
    /// and this is a payment week, issue payment.
    if (n_payments_made < n_payments &&
            week > week_issued + begin_repayment_after_n_years
                                 * WEEKS_IN_YEAR - 1 &&
            std::find(pay_on_weeks.begin(), pay_on_weeks.end(),
                      Utils::weekOfTheYear(week))
            != pay_on_weeks.end()) {

        n_payments_made++;
        return level_debt_service_payment;
    } else {
        return 0.;
    }
}

double LevelDebtServiceBond::getNetPresentValueAtIssuance(
        double infra_discount_rate, int week) const {
    // Calculate NPV at time of first payment
    double periodic_discount_rate = infra_discount_rate / pay_on_weeks.size();
    double npv_at_first_payment_date =
            level_debt_service_payment
            * (1. - pow(1. + periodic_discount_rate, -n_payments)) / periodic_discount_rate;
    
    // Calculate NPV at issuance date
    double npv = npv_at_first_payment_date
            / pow(1. + infra_discount_rate, begin_repayment_after_n_years);

    return npv;
}

void LevelDebtServiceBond::issueBond(int week, int construction_time,
                                     double bond_term_multiplier,
                                     double bond_interest_rate_multiplier) {
    Bond::issueBond(week, construction_time, bond_term_multiplier,
                    bond_interest_rate_multiplier);
    // Calculate the capital recovery factor for the bond.
    double capital_recovery_factor_num = interest_rate * pow(1. + interest_rate, n_payments);
    double capital_recovery_factor_den = pow(1. + interest_rate, n_payments) - 1.;

    // Calculate the level debt service payment value
    level_debt_service_payment = cost_of_capital * (capital_recovery_factor_num / capital_recovery_factor_den);
}

// ADDED ON 2025-05-01 BY LILLIAN LAU
double LevelDebtServiceBond::getPresentValueDebtService(int week, double infra_discount_rate) {
    // NOTE: pay_on_weeks is a vector with the weeks of each CALENDAR YEAR when payments are due
    // to designate how many times (and when) per year debt service is due. default is a
    // length-one vector with value 0 (single annual payment in first (0th) week of year)
    /// If there are still payments to be made, repayment has begun,
    /// and this is a payment week, issue payment.
    if ((n_payments_made < n_payments) &&
        (week > week_issued + begin_repayment_after_n_years * WEEKS_IN_YEAR - 1) &&
        (std::find(pay_on_weeks.begin(), pay_on_weeks.end(),
                  Utils::weekOfTheYear(week)) != pay_on_weeks.end())) {

        return level_debt_service_payment /
               pow(1. + infra_discount_rate, round((week - week_issued)/WEEKS_IN_YEAR_ROUND - 1));
    } else {
        return 0.;
    }
}