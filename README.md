# Black-Scholes Pricer

A quantitative finance repository focused on pricing **simple** and **exotic options** under the **Black framework** using both **analytical formulas** and **Monte Carlo methods**.

This project implements pricing frameworks for **vanilla**, **digital**, **European barrier**, **arithmetic Asian**, **spread**, and **basket options** in a **C++** environment.

---

## Repository Structure

```text
Black-Scholes-Pricer/
├── Black Pricer/
│   ├── Black Pricer.vcxproj
│   ├── Black Pricer.vcxproj.filters
│   ├── BlackScholesModel.cpp
│   ├── BlackScholesModel.h
│   ├── MonteCarlo.cpp
│   ├── MonteCarlo.h
│   ├── MultiAssetBSModel.cpp
│   ├── MultiAssetBSModel.h
│   ├── Option.cpp
│   ├── Option.h
│   └── main.cpp
├── Black Pricer.sln
└── Black-Scholes Model Analytics.pdf
```

---

## Project Overview

This project focuses on pricing **simple** and **exotic options** under the **Black framework**.

For single-asset products, the code implements analytical pricing classes for **vanilla options**, **digital options**, **European barrier options**, and **arithmetic Asian options**. The analytical barrier pricing is handled through **static replication** with vanilla and digital options, while the Asian pricing relies on a **moment-matching approximation** for the arithmetic average. ([BlackScholesModel.h][1])

For multi-asset products, the project also supports **basket** and **spread options** through a dedicated multi-asset Black framework based on spot vectors, volatility vectors, and a correlation matrix. The code includes a **Cholesky decomposition** step and a routine to enforce a **positive-definite correlation matrix** for simulation purposes. ([MultiAssetBSModel.h][2])

In parallel, the project implements a **Monte Carlo engine** for both single-asset and multi-asset models. ([MonteCarlo.h][4])

The main program compares **Monte Carlo prices** against **analytical prices** across all supported product types, including path-dependent contracts such as arithmetic Asians. ([main.cpp][3])

---

## Documentation

The repository also includes:

* `Black-Scholes Model Analytics.pdf`

This document complements the C++ implementation with the theoretical pricing framework.

---

## Best use case

Use this project when working with **equity derivatives** and building a **C++ Black pricing framework** for **plain-vanilla**, **exotic**, and **multi-asset option valuation**.

---

## How to Use

This project is structured as a **Visual Studio C++ solution**, with a top-level `.sln` file and a `.vcxproj` project file targeting the **v143 toolset**.

### Visual Studio (recommended on Windows)

1. Open `Black Pricer.sln` in **Visual Studio**.
2. Select a build configuration such as **Debug x64** or **Release x64**.
3. Build and run the project.
4. The executable will run `main.cpp`, which already contains sample pricing tests for all supported products.

### Standard C++ compilation

Because the source files rely on standard C++ headers and are split across `.cpp` / `.h` files, the project can also be compiled manually with a C++ compiler by building all source files together from the `Black Pricer/` folder. A typical command is:

```bash
g++ -std=c++17 -O2 *.cpp -o black_pricer
./black_pricer
```

The entry point is `main.cpp`, which instantiates the models, defines sample contracts, and prints **Monte Carlo** and **analytical** prices to the console.

---

## Author

**Idriss Afra**

[1]: https://raw.githubusercontent.com/Idriss-Afra/Black-Scholes-Pricer/main/Black%20Pricer/BlackScholesModel.h "raw.githubusercontent.com"
[2]: https://raw.githubusercontent.com/Idriss-Afra/Black-Scholes-Pricer/main/Black%20Pricer/MultiAssetBSModel.h "raw.githubusercontent.com"
[3]: https://raw.githubusercontent.com/Idriss-Afra/Black-Scholes-Pricer/main/Black%20Pricer/main.cpp "raw.githubusercontent.com"
[4]: https://raw.githubusercontent.com/Idriss-Afra/Black-Scholes-Pricer/main/Black%20Pricer/MonteCarlo.h "raw.githubusercontent.com"
