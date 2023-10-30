#include <vector>
#include <iostream>
#include <random>
#include "MonteCarlo.h"

using namespace std;

/*
	The Source file of the class "MonteCarlo".
*/

double norm_variable(double mean = 0, double stddev = 1) {
	
	/* Normal distribution generator based on the Mersenne Twister algorithm. */

	// Seed for the random number generator
	std::random_device rd;
	std::mt19937 gen(rd());

	// Create a normal distribution object
	std::normal_distribution<double> normal_dist(mean, stddev);

	return normal_dist(gen);
}

MonteCarlo::MonteCarlo(double nb_simulations, double time_steps) { 
	
	/* MonteCarlo class constructor. */

	nbSimulations = nb_simulations;
	nbSteps = time_steps;

}

void MonteCarlo::setTimeSteps(Option* opt) {
	/*
		"setTimeSteps" method calls the Option contract, and returns an equivalent time grid used for path simulations.
		Time steps grid is only needed for path-dependent Options, in our case : Arithmetic Asian Options.
		For these latter, the method includes the fixing dates needed to compute the average spot price. 
	*/
	vector<double> grid = vector<double>(1, 0);

	if (opt->getType() == "Asian") {
		double T = opt->getMaturity();
		double freq = opt->getFreq();
		for (int s = 1; s <= nbSteps; s++)
			grid.push_back(s * (T / nbSteps)); // Dates based on the number of steps 
		for (int f = 1; f < freq; f++)
			grid.push_back(f * (T / freq)); // Dates based on the Asian fixing frequency 

		sort(grid.begin(), grid.end()); // Sort the time grid

		// Compute the time steps :
		vector<double>::iterator it_prev = grid.begin();
		vector<double>::iterator it = grid.begin();
		it++;
		while (it != grid.end()) {
			timeSteps.push_back(*it - *it_prev);
			it++;
			it_prev++;
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
		double freq = opt->getFreq();
		vector<double> fixings;
		double prev_S;
		double t = 0;

		for (int i = 0; i < timeSteps.size(); ++i) {
			prev_S = path.back();
			if (timeSteps[i] != 0) {
				path.push_back(bs_model->simulation(prev_S, timeSteps[i], norm_variable()));
				t += timeSteps[i];
				if (floor(freq * t) == freq * t)
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
		path.push_back(bs_model->simulation(path[0], T, norm_variable()));
		return path;
	} 
}

double MonteCarlo::price(BlackScholesModel* bs_model, Option* opt) {
	
	/* Black-Scholes Monte-Carlo price. */

	MonteCarlo::setTimeSteps(opt); // Set the time steps grid once for all 
	double T = opt->getMaturity();
	double df = exp(-bs_model->getRate() * T);
	double price = 0;
	for (int i = 0; i < nbSimulations; i++) 
		price += df * opt->payoff(getBSPath(bs_model, opt)) / nbSimulations;
	return price;
}


vector<double> MonteCarlo::getBSPath(MultiAssetBSModel* bs_model, Option* opt) {
	/*
		"getBSPath" method calls the Multi-Asset BS model and the Option contract, and returns a simulated path of the spot price.
		The simulation on every time step is handled by the BS model.
		Basket and Spread Options : Directly simulate the spot price at maturity S_T.
		Multi-Asset BS needs a vector of independent standard normal variables.
	*/
	double n = bs_model->getSize();
	double T = opt->getMaturity();
	vector<double> normal_vector;

	for (int i = 0; i < n; i++)
		normal_vector.push_back(norm_variable());
	
	return bs_model->simulation(bs_model->getSpot(), T, normal_vector);
}

double MonteCarlo::price(MultiAssetBSModel* bs_model, Option* opt) {
	
	/* Multi-Asset Black-Scholes Monte-Carlo price. */

	double T = opt->getMaturity();
	double df = exp(-bs_model->getRate() * T);
	double price = 0;
	for (int i = 0; i < nbSimulations; i++)
		price += df * opt->payoff(getBSPath(bs_model, opt)) / nbSimulations;
	return price;
}
