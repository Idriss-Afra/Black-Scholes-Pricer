# Black-Scholes Pricer

A C++ option-pricing project that compares closed-form or approximate pricing methods with Monte Carlo simulation under a Black-Scholes framework.

The project supports single-asset, path-dependent, and multi-asset products. Monte Carlo results include the estimated price, standard error, and a 95% confidence interval.

## Supported Products and Pricing Methods

| Product | Formula or approximation | Status |
|---|---|---|
| Vanilla call and put | Black-Scholes formula | Exact under the model |
| Cash-or-nothing digital call and put | Black-Scholes digital formula | Exact under the model |
| Terminal up/down in/out options | Static decomposition using vanilla and digital options | Exact for the implemented terminal barrier condition |
| Discrete arithmetic Asian call and put | Lognormal moment matching | Approximation |
| Equal-weight arithmetic basket call and put | Lognormal moment matching | Approximation |
| Spread call and put | Kirk/Margrabe-style method | Approximation |

The terminal barrier condition is evaluated at maturity only. It is not a continuously or discretely monitored path barrier.

## Monte Carlo Engine

The Monte Carlo implementation provides:

- Risk-neutral geometric Brownian motion simulation
- Single-asset and correlated multi-asset paths
- Cholesky decomposition for correlated normal variables
- A reusable `std::mt19937_64` random-number generator
- A configurable deterministic seed for reproducible results
- Sample standard errors and 95% confidence intervals
- Separate simulation grids for arithmetic Asian fixing dates

The implementation validates contract parameters, model parameters, vector dimensions, model/product compatibility, and correlation matrices. A correlation matrix must have valid dimensions, finite entries in `[-1, 1]`, a unit diagonal, symmetry, and strict positive definiteness. Invalid matrices are rejected before simulation; they are not automatically repaired.

## Model Assumptions

- Constant risk-free rate
- Constant volatility for each underlying
- No dividend yield
- Geometric Brownian motion under the risk-neutral measure
- Constant correlations for multi-asset products
- Positive spot prices, strikes, maturities, and volatilities
- Rates, volatilities, and correlations expressed as decimals
- Maturities expressed in years

## Repository Structure

```text
Black-Scholes-Pricer/
|-- README.md
|-- Black Pricer.sln
`-- Black Pricer/
    |-- Black Pricer.vcxproj
    |-- main.cpp
    |-- Option.h
    |-- Option.cpp
    |-- BlackScholesModel.h
    |-- BlackScholesModel.cpp
    |-- MultiAssetBSModel.h
    |-- MultiAssetBSModel.cpp
    |-- MonteCarlo.h
    `-- MonteCarlo.cpp
```

The main components are:

- `Option`: contract definitions and payoff implementations
- `BlackScholesModel`: single-asset simulation and pricing methods
- `MultiAssetBSModel`: basket/spread pricing and correlated simulation
- `MonteCarlo`: simulation, discounting, standard errors, and confidence intervals
- `main.cpp`: sample parameters and comparisons for every supported product

## How to Use

### Requirements

- A C++17-compatible compiler
- Visual Studio with the **Desktop development with C++** workload, or another compiler such as GCC or Clang

### Visual Studio

1. Clone or download the repository.
2. Open `Black Pricer.sln`.
3. Select an `x64` configuration such as `Release` or `Debug`.
4. Build the solution.
5. Run without debugging with `Ctrl+F5`.

### Command-Line Compilation

From the repository root, enter the source directory:

```bash
cd "Black Pricer"
```

Compile all implementation files:

```bash
g++ -std=c++17 -O2 main.cpp Option.cpp BlackScholesModel.cpp MultiAssetBSModel.cpp MonteCarlo.cpp -o black_pricer
```

Run on Linux or macOS:

```bash
./black_pricer
```

Run on Windows:

```powershell
.\black_pricer.exe
```

With Clang, replace `g++` with `clang++`.

### Changing the Inputs

The sample model and contract parameters are defined in `main.cpp`. They include:

- Spot prices
- Strikes and maturities
- Risk-free rate
- Volatilities
- Correlation matrices
- Number of Monte Carlo simulations
- Number of Asian simulation steps
- Random-number seed

### Program Output

For every product, the program displays:

- Formula or approximation price
- Monte Carlo price
- Monte Carlo standard error
- 95% Monte Carlo confidence interval

### Sample Results

The following results use the parameters in `main.cpp`, with 100,000 simulations for terminal products, 30,000 simulations for Asian products, and random seed 42.

| Product | Method | Formula/Approx. | Monte Carlo | 95% MC interval |
|---|---|---:|---:|---:|
| Vanilla Call | Exact | 11.9769 | 12.0840 | [11.9527, 12.2153] |
| Digital Call | Exact | 0.420418 | 0.420577 | [0.417648, 0.423505] |
| Terminal Up-and-Out Call | Exact | 5.22156 | 5.22018 | [5.16067, 5.27969] |
| Asian Call | Approximation | 7.31931 | 7.28902 | [7.14265, 7.43539] |
| Basket Call | Approximation | 9.70608 | 9.52939 | [9.44391, 9.61488] |
| Spread Call | Approximation | 15.2123 | 15.4233 | [15.2440, 15.6027] |

The confidence interval measures Monte Carlo sampling uncertainty only. Approximation prices are not required to lie inside it.

## Interpreting the Results

Monte Carlo is a finite-sample numerical estimator. Its standard error decreases proportionally to `1 / sqrt(N)`, where `N` is the number of simulations.

For vanilla, digital, and the implemented terminal-barrier products, the pricing formulas are exact under the model. Over repeated runs with different random seeds, a correctly constructed 95% confidence interval should contain the corresponding true model value approximately 95% of the time.

The Asian, basket, and spread formulas are approximations. Their values are therefore not required to lie inside the Monte Carlo confidence interval. A difference outside the interval can represent systematic formula-approximation error rather than a Monte Carlo defect.

Because the default random seed is fixed, unchanged inputs reproduce the same Monte Carlo results. Change the seed to obtain a different sample.

## Author

**Idriss Afra**
