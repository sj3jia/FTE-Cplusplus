#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>

#include "gbm.h"

using std::runtime_error;

namespace gbm {

    // Constants
    const double DAYS_PER_YEAR = 365.0;

    static std::chrono::year_month_day parseDate(
        const std::string& dateStr
    ) {
        
        // Validation: Input date format should be YYYY-MM-DD
        std::tm tm = {};
        std::istringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        if (ss.fail() || !ss.eof()) {
            throw runtime_error("Invalid date format. Please follow: YYYY-MM-DD. Got: " + dateStr);
        }

        // Convert and construct chrono year month day
        std::chrono::year year{tm.tm_year + 1900};
        std::chrono::month month{static_cast<unsigned>(tm.tm_mon + 1)};
        std::chrono::day day{static_cast<unsigned>(tm.tm_mday)};
        std::chrono::year_month_day ymd{year, month, day};
        if (!ymd.ok()) {
            throw runtime_error("Invalid date: " + dateStr);
        }
        return ymd;
    }

    static std::vector<PricePoint> parseCSVOrThrowException(
        const std::string& filename
    ) {
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            throw runtime_error("Unable to open file: " + filename);
        }
        
        std::vector<PricePoint> priceCurve;
        std::string line;
        
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string dateStr, priceStr;
            
            if (std::getline(ss, dateStr, ',') && 
                std::getline(ss, priceStr)) {
                try {
                    auto date = parseDate(dateStr);
                    double price = std::stod(priceStr);
                    priceCurve.push_back({date, price});
                } catch (const std::exception& e) {
                    throw runtime_error("Invalid data in line: " + 
                                             line + " (" + e.what() + ")");
                }
            } else {
                throw runtime_error("Invalid CSV format in line: " + line);
            }
        }
        
        return priceCurve;
    }

    static void validatePriceCurve(
        const std::vector<PricePoint>& priceCurve
    ) {
        if (priceCurve.size() < 2) {
            throw std::invalid_argument(
                "Need at least 2 price observations");
        }
        
        if (!std::all_of(priceCurve.begin(), priceCurve.end(), 
                         [](const PricePoint& pp) { 
                             return pp.price > 0; 
                         })) {
            throw std::invalid_argument("All prices must be positive");
        }
        
        for (std::size_t i = 1; i < priceCurve.size(); ++i) {
            if (priceCurve[i].date <= priceCurve[i-1].date) {
                throw std::invalid_argument(
                    "Dates must be in ascending order");
            }
        }

    }

    static ModelParams calibrateFromPriceCurve(
        const std::vector<PricePoint>& priceCurve
    ) {
        
        std::vector<double> logReturns;
        std::vector<double> timeDeltas;
        double totalLogReturn = 0.0, totalTime = 0.0;
        
        for (std::size_t i = 1; i < priceCurve.size(); ++i) {
            double logReturn = std::log(priceCurve[i].price / 
                                       priceCurve[i-1].price);
            
            // Convert dates to days for calculation
            auto sys_days1 = std::chrono::sys_days{priceCurve[i-1].date};
            auto sys_days2 = std::chrono::sys_days{priceCurve[i].date};
            double days = (sys_days2 - sys_days1).count();
            double dt = days/DAYS_PER_YEAR;
                       
            logReturns.push_back(logReturn);
            timeDeltas.push_back(days/DAYS_PER_YEAR); // days/365
            totalLogReturn += logReturn;
            totalTime += dt;
        }

        // calculate mean return (annualized, w actual 365 days)
        double meanReturn = totalLogReturn / totalTime;

        // calculate variance (annualized, w actual 365 days)
        double sum_error_sq = 0.0;
        for (std::size_t i = 0; i < logReturns.size(); ++i) {
            double error = logReturns[i] - meanReturn * timeDeltas[i];
            sum_error_sq += error * error / timeDeltas[i];
        }
        double variance = sum_error_sq / logReturns.size();

        // Calibrate parameters
        double sigma = std::sqrt(variance);
        double mu = meanReturn + (sigma * sigma / 2.0);
        
        return {mu, sigma};
    }

    ModelParams calibrate(const std::string& csvInput) {
        // Parse CSV file input
        std::vector<PricePoint> priceCurve = parseCSVOrThrowException(csvInput);
        //  Validate Price Curve
        validatePriceCurve(priceCurve);
        // Calibrate parameters
        return calibrateFromPriceCurve(priceCurve);
    }

}
