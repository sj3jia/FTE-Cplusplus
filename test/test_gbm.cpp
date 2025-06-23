#include <cassert>
#include <cmath>
#include <iostream>
#include <optional>
#include <vector>

#include "gbm.h"

struct TestCase {
    std::string name;
    std::string csvFile;
    bool expectSuccess;
    std::optional<double> expectedMu;
    std::optional<double> expectedSigma;
};

void executeTestCase(const TestCase& test) {
    std::cout << "Testing " << test.name << "... ";
    try {
        // Calibrate parameters
        auto params = gbm::calibrate(test.csvFile);
        
        // Validations
        if (test.expectSuccess) {
            // Basic output validation
            assert(params.sigma >= 0 && !std::isnan(params.mu) && !std::isnan(params.sigma));
            
            // Check expected values if provided
            if (test.expectedMu.has_value()) {
                double muDiff = std::abs(params.mu - test.expectedMu.value()) / test.expectedMu.value();
                if (muDiff >= 1e-5) {
                    std::cout   << "!!!FAILED to match expected r!!! "
                                << "Calculated r=" << params.mu << "  Expected r=" << test.expectedMu.value() 
                                << " diff=" << muDiff << std::endl;
                    assert(false);
                }
            }
            if (test.expectedSigma.has_value()) {
                double sigmaDiff = std::abs(params.sigma - test.expectedSigma.value()) / test.expectedSigma.value();
                if (sigmaDiff >= 1e-5) {
                    std::cout << "!!!FAILED to match expected sigma!!! "
                              << "Calculated sigma=" << params.sigma << " Expected sigma=" << test.expectedSigma.value() 
                              << " diff=" << sigmaDiff << std::endl;
                    assert(false);
                }
            }
            
            std::cout << "PASS (r=" << params.mu << ", sigma=" << params.sigma << ")" << std::endl;
        } else {
            // Expected failure
            assert(false && "FAILED: Expect program to throw but test passed");
        }
    } catch (const std::exception&) {
        if (test.expectSuccess) {
            throw; // Re-throw if we expected success but got failure
        } else {
            // Expected failure
            std::cout << "PASS (correctly failed)" << std::endl;
        }
    }
}

int main() {
    std::cout << "=============================" << std::endl;
    std::cout << "Running Tests" << std::endl;
    std::cout << "=============================" << std::endl;
    
    std::vector<TestCase> testCases = {
        // Valid test cases (happy path)
        {"real AAPL calibration", "test/testData/happyPath/real_prices.csv", true, -0.124924, 0.177238},
        {"high volatility", "test/testData/happyPath/high_volatility.csv", true, 83.0763, 10.2155},
        {"minimal valid (2 points)", "test/testData/happyPath/minimal_valid.csv", true, 17.8084, 8.28217e-17},
        {"irregular intervals", "test/testData/happyPath/irregular_intervals.csv", true, 13.2853, 4.2159},
        {"zero volatility", "test/testData/happyPath/zero_volatility.csv", true, 0.0, 0.0},
        {"negative drift", "test/testData/happyPath/negative_drift.csv", true, -16.4463, 0.673919},
        {"large time gaps", "test/testData/happyPath/large_time_gaps.csv", true, 0.113601, 0.0974293},
        {"small changes (precision)", "test/testData/happyPath/small_changes.csv", true, 0.000364999, 2.09284e-05},
        
        // Error test cases (failure path)
        {"duplicate dates", "test/testData/failure/duplicate_dates.csv", false, {}, {}},
        {"single data point", "test/testData/failure/single_point.csv", false, {}, {}},
        {"negative prices", "test/testData/failure/negative_prices.csv", false, {}, {}},
        {"wrong timestamp order", "test/testData/failure/wrong_time_order.csv", false, {}, {}},
        {"invalid datetime format (MM/DD/YYYY)", "test/testData/failure/invalid_datetime_format.csv", false, {}, {}},
        {"random garbage content", "test/testData/failure/random_garbage.csv", false, {}, {}},
        {"invalid price (string)", "test/testData/failure/invalid_price_string.csv", false, {}, {}},
        {"invalid price (huge number)", "test/testData/failure/invalid_price_huge.csv", false, {}, {}},
        {"non-existent file", "test/testData/failure/does_not_exist.csv", false, {}, {}}
    };
    
    try {
        for (const auto& test : testCases) {
            executeTestCase(test);
        }
        std::cout << "=============================" << std::endl;
        std::cout << "            SUCCESS          " << std::endl;
        std::cout << "\n All " << testCases.size() << " tests passed!" << std::endl;
        std::cout << "=============================" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << "\n Test failed: " << e.what() << std::endl;
        return 1;
    }
}