#pragma once
#include <stdexcept>
#include <vector>
#include <random>
#include "BlackScholesModel.h"
#include "MultiAssetBSModel.h"
#include "Option.h"

using namespace std;

/*
	The Header file of the class "MonteCarlo".
*/

struct MonteCarloResult {
    double price;
    double standardError;
    double ciLower; // 95% confidence interval lower bound
    double ciUpper; // 95% confidence interval upper bound
};

class MonteCarlo {
private :
	size_t  nbSimulations; // Number of Simulations. Default : 20 000.
	size_t  nbSteps; // Number of Time steps. This attribute is only needed for path-dependent Options. Default : 1.
	vector<double> timeSteps;
	// Normal distribution generator based on the Mersenne Twister algorithm.
	uint64_t seed;
	mt19937_64 generator;
    normal_distribution<double> normal{0.0, 1.0};
    double normalDraw() {
        return normal(generator);
    }
	void setTimeSteps(Option* opt); // The "setTimeSteps" method calls the Option contract, and returns an equivalent time grid used for path simulations.
	vector<double> getBSPath(BlackScholesModel* bs_model, Option* opt); // This method calls the BS model and the Option contract, and returns a simulated path of the spot price.
	vector<double> getBSPath(MultiAssetBSModel* bs_model, Option* opt); // This method calls the Multi-Asset BS model and the Option contract, and returns a simulated path of the spot price.

public :
	MonteCarlo(size_t nb_simulations = 20000, size_t nb_steps = 1, uint64_t seed = 42);
	void setNbSimulations(size_t nb_simulations) {
		if (nb_simulations < 2)
			throw invalid_argument("At least two simulations are required.");
		nbSimulations = nb_simulations;
	};
	size_t getNbSimulations() { return nbSimulations; };
	void setNbSteps(size_t nb_steps) {
		if (nb_steps == 0)
			throw invalid_argument("Time steps must be positive.");
		nbSteps = nb_steps;
	};
	size_t getNbSteps() { return nbSteps; };
	MonteCarloResult price(BlackScholesModel* bs_model, Option* opt); // This method calls the BS model and the Option contract, and returns the equivalent BS Monte-Carlo price.
	MonteCarloResult price(MultiAssetBSModel* bs_model, Option* opt); // This method calls the Multi-Asset BS model and the Option contract, and returns the equivalent BS Monte-Carlo price.

};
