#include <vector>
#include <iostream>
#include "MonteCarlo.h"

using namespace std;

namespace {

void validateBSInputs(BlackScholesModel* model, Option* opt) {
	if (model == nullptr || opt == nullptr)
		throw invalid_argument("Monte Carlo model and option must not be null.");

	bool valid_combination =
		(dynamic_cast<BlackVanilla*>(model) != nullptr && dynamic_cast<VanillaOption*>(opt) != nullptr) ||
		(dynamic_cast<BlackDigital*>(model) != nullptr && dynamic_cast<DigitalOption*>(opt) != nullptr) ||
		(dynamic_cast<BlackBarrier*>(model) != nullptr && dynamic_cast<BarrierOption*>(opt) != nullptr) ||
		(dynamic_cast<BlackAsian*>(model) != nullptr && dynamic_cast<AsianOption*>(opt) != nullptr);

	if (!valid_combination)
		throw invalid_argument("Monte Carlo model and option types do not match.");
}

void validateMultiAssetInputs(MultiAssetBSModel* model, Option* opt) {
	if (model == nullptr || opt == nullptr)
		throw invalid_argument("Monte Carlo model and option must not be null.");

	bool valid_combination =
		(dynamic_cast<BlackBasket*>(model) != nullptr && dynamic_cast<BasketOption*>(opt) != nullptr) ||
		(dynamic_cast<BlackSpread*>(model) != nullptr && dynamic_cast<SpreadOption*>(opt) != nullptr);

	if (!valid_combination)
		throw invalid_argument("Monte Carlo model and option types do not match.");
}

}

/*
	The Source file of the class "MonteCarlo".
*/

MonteCarlo::MonteCarlo(size_t nb_simulations, size_t nb_steps, uint64_t rng_seed) { 
	
	/* MonteCarlo class constructor. */

	setNbSimulations(nb_simulations);
	setNbSteps(nb_steps);
	seed = rng_seed;
}

void MonteCarlo::setTimeSteps(Option* opt) {
	/*
		"setTimeSteps" method calls the Option contract, and returns an equivalent time grid used for path simulations.
		Time steps grid is only needed for path-dependent Options, in our case : Arithmetic Asian Options.
		For these latter, the method includes the fixing dates needed to compute the average spot price. 
	*/
	vector<double> grid = vector<double>(1, 0);
	timeSteps.clear();
	if (opt->getType() == "Asian") {
		double T = opt->getMaturity();
		size_t freq = opt->getFreq();
		for (int s = 1; s <= nbSteps; ++s)
			grid.push_back(s * (T / nbSteps)); // Dates based on the number of steps 
		for (int f = 1; f < freq; ++f)
			grid.push_back(f * (T / freq)); // Dates based on the Asian fixing frequency 

		sort(grid.begin(), grid.end()); // Sort the time grid

		// Compute the time steps :
		vector<double>::iterator it_prev = grid.begin();
		vector<double>::iterator it = grid.begin();
		++it;
		while (it != grid.end()) {
			timeSteps.push_back(*it - *it_prev);
			++it;
			++it_prev;
		}
	}
}

vector<double> MonteCarlo::getBSPath(BlackScholesModel* bs_model, Option* opt) {
	/*
		"getBSPath" method calls the BS model and the Option contract, and returns a simulated path of the spot price.
		The simulation on every time step is handled by the BS model.
	*/

	// Init : S_0
	vector<double> path = vector<double>(1, bs_model->getSpot());

	if (opt->getType() == "Asian") {
		/*
			Asian Options : Path-dependent simulation on the time steps grid.
			Returned path : Fixings needed to compute the average [F_1, F_2, ..., F_n].
		*/
		size_t freq = opt->getFreq();
		double T = opt->getMaturity();
		vector<double> fixings;
		double prev_S;
		double t = 0;
		for (int i = 0; i < timeSteps.size(); ++i) {
			prev_S = path.back();
			if (timeSteps[i] != 0) {
				path.push_back(bs_model->simulation(prev_S, timeSteps[i], normalDraw()));
				t += timeSteps[i];
				double x = freq * t / T;
				if (fabs(x - round(x)) < 1e-9)
					fixings.push_back(path.back());
			}
		}

		return fixings;
	} 
	else {
		/* 
			Non-Path Dependent Options : Directly simulate the spot price at maturity S_T.
			Returned path : [S_0, S_T]
		*/
		double T = opt->getMaturity();
		path.push_back(bs_model->simulation(path[0], T, normalDraw()));
		return path;
	} 
}

MonteCarloResult MonteCarlo::price(BlackScholesModel* bs_model, Option* opt) {
	
	/* Black-Scholes Monte-Carlo price. */

	validateBSInputs(bs_model, opt);

	generator.seed(seed);
	normal.reset();
	MonteCarlo::setTimeSteps(opt); // Set the time steps grid once for all 
	double T = opt->getMaturity();
	double df = exp(-bs_model->getRate() * T);
	vector<double> values;
	double sum = 0.0;
	for (int i = 0; i < nbSimulations; ++i) {
		const double value = df * opt->payoff(getBSPath(bs_model, opt));
		values.push_back(value);
		sum += value;
	}
	const double mean = sum / static_cast<double>(nbSimulations);
	double squaredDeviations = 0.0;
	for (double value : values)
		squaredDeviations += (value - mean) * (value - mean);
	const double variance = squaredDeviations / static_cast<double>(nbSimulations - 1);
	const double se = sqrt(variance / static_cast<double>(nbSimulations));
	return {mean, se, mean - 1.96 * se, mean + 1.96 * se};
}


vector<double> MonteCarlo::getBSPath(MultiAssetBSModel* bs_model, Option* opt) {
	/*
		"getBSPath" method calls the Multi-Asset BS model and the Option contract, and returns a simulated path of the spot price.
		The simulation on every time step is handled by the BS model.
		Basket and Spread Options : Directly simulate the spot price at maturity S_T.
		Multi-Asset BS needs a vector of independent standard normal variables.
	*/
	size_t n = bs_model->getSize();
	double T = opt->getMaturity();
	vector<double> normal_vector;

	for (int i = 0; i < n; ++i)
		normal_vector.push_back(normalDraw());
	
	return bs_model->simulation(bs_model->getSpot(), T, normal_vector);
}

MonteCarloResult MonteCarlo::price(MultiAssetBSModel* bs_model, Option* opt) {
	
	/* Multi-Asset Black-Scholes Monte-Carlo price. */

	validateMultiAssetInputs(bs_model, opt);

	generator.seed(seed);
	normal.reset();
	double T = opt->getMaturity();
	double df = exp(-bs_model->getRate() * T);
	vector<double> values;
	double sum = 0.0;
	for (int i = 0; i < nbSimulations; ++i) {
		const double value = df * opt->payoff(getBSPath(bs_model, opt));
		values.push_back(value);
		sum += value;
	}
	const double mean = sum / static_cast<double>(nbSimulations);
	double squaredDeviations = 0.0;
	for (double value : values)
		squaredDeviations += (value - mean) * (value - mean);
	const double variance = squaredDeviations / static_cast<double>(nbSimulations - 1);
	const double se = sqrt(variance / static_cast<double>(nbSimulations));
	return {mean, se, mean - 1.96 * se, mean + 1.96 * se};
}
