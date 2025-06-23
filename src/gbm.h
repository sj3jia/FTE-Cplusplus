#ifndef GBM_H
#define GBM_H

#include <chrono>
#include <string>
#include <vector>

namespace gbm {

    // Price point with date and price
    struct PricePoint {
        std::chrono::year_month_day date;
        double price;
    };

    // Output structure
    struct ModelParams {
        double mu;
        double sigma;
    };

    // Calibrate parameters from price curve
    // @param csvInput: path to csv file with price points
    // @return: calibrated parameters (mu, sigma)
    // @throws: std::runtime_error if the csv file is invalid
    ModelParams calibrate(const std::string& csvInput);

}

#endif
