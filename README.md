# FTE-Cplusplus

A C++ utility for calibrating geometric Brownian motion (GBM) parameters: the drift (expected return, 'r') and volatility ('σ') from stock price time series data.

## Background and Overview

This tool implements a calibration algorithm for estimating annualized drift (μ) and volatility (σ) parameters in a stock price model that follows Geometric Brownian Motion (GBM). μ is a good estimate of r. 
It accepts a time series of stock closing prices with timestamps in the 'YYYY-MM-DD' format and handles irregular time intervals using weighted calculations.

### Model - Stock Price Model: Geometric Brownian Motion

The GBM model follows the stochastic differential equation:
```
dS_t = S_t * μ * dt + S_t * σ * dW_t
```

Where:
- `S_t` = Stock price at time t
- `μ` = Drift parameter (annualized return)
- `σ` = Volatility parameter (annualized volatility)
- `dW_t` = Brownian motion process

Using Ito's Lemma, the above SDE can be solved as:
``` 
S_t = S_0 * exp( ( μ -  σ²/2 )t + σW_t )
```
Equivalently, stock log returns are normally distributed:
``` 
ln( S_t / S_0) ~ N( ( μ -  σ²/2 )t, σ²t )
```

### Algorithm
Based on the above model, the calibration uses stock log returns and calculates:
- **Volatility**: `σ_hat = √(variance)`
- **Drift**:      `μ_hat = mean + σ²/2`

For stock price S(t) modeled as 
``` 
dS_t = S_t * r * dt + σ * dW_t
```
above **Volatility** `σ_hat` is a good estimate of constant `σ` and **Drift** `μ_hat` is a good estimate of `r` 


### Data 
This project assumes all stock prices are adjusted close prices, labeled with date. Calculation uses the actual calendar date convention (365 days). To handle variable time gaps, like weekends, holidays, or missing data points, delta t is calculated as (number of calendar days) / 365.

## Getting Started

### Requirements
- **C++17** or later
- **Standard library** (no external dependencies)
- **GNU Make** for building

### Build

To build, simply run the command below. This will build the main executable, build the test suite executable, and run the test suite.
```
make
```

To clean previous builds:
```
make clean
```

### Execute 
Run the command with your desired CSV file:
```bash
./build/gbm_calibrator <csv_filepath>
```

#### Examples

```bash
# Using sample data
./build/gbm_calibrator sample_prices.csv

# Using test data
./build/gbm_calibrator test/testData/happyPath/normal_prices.csv
```

#### Input Format

CSV file with date and price columns (no header):
```
2024-01-01,100.50
2024-01-02,102.25
2024-01-03,101.75
2024-01-04,103.50
```
**Note:** 
- Only the exact date format YYYY-MM-DD is accepted. Any other format will result in a parsing error.
- All prices must be positive.
- Dates must be in ascending order.
- A minimum of 2 data points is required.

**Output Example:**
```
Calibrated GBM Parameters:
r (drift): 0.0101998
σ (volatility): 0.0101477
```

## Testing

The project includes a test suite, which is compiled during `make`. You can run tests by: 

```bash
./build/test_gbm
```

**Test coverage:**

Test CSV files are stored in `test/testData/` with two categories:

- **Happy Path (`test/testData/happyPath/`)** - Valid test cases
- **Failure Path (`test/testData/failure/`)** - Error cases

## Potential Enhancements
1. Improve date system: 
   - Incorporate weekends and exchange holiday calendars in date counting
   - Switch to 252-day trading calendar convention
2. Data cleaning and processing to support more robust inputs:
   - Improve parsing to support more input date formats
   - Automatic sorting by dates
   - Handle erroneous data points by removing or autofilling
3. Other statistical methods for estimation:
   - Bootstraping to provide confidence interval
   - Weighted average with more power on recent data, e.f. EWMA
