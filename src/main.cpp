#include <iostream>
#include <stdexcept>

#include "gbm.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Please provide one filename as argument" << std::endl;
        return 1;
    }
    
    try {
        std::string filename = argv[1];
        gbm::ModelParams params = gbm::calibrate(filename);

        // print result
        std::cout << " Calibrated parameters: " << std::endl;
        std::cout << "r (drift annualizedexpected return): " << params.mu << "\n";
        std::cout << "σ (annualized volatility): " << params.sigma << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 
