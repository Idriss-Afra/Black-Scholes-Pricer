#pragma once
#include <vector>
#include "BlackScholesModel.h"
#include "MultiAssetBSModel.h"
#include "Option.h"

using namespace std;

/*
	The Header file of the class "MonteCarlo".
*/

class MonteCarlo {
private :
	double nbSimulations; // Number of Simulations. Default : 20 000.
	double nbSteps; // Number of Time steps. This attribute is only needed for path-dependent Options. Default : 1.
	vector<double> timeSteps; // The time steps grid. This attribute is only needed for path-dependent Options.
public :
	MonteCarlo(double nb_simulations = 20000, double time_steps = 1);
	void setNbSimulations(double nbSimuls) { nbSimulations = nbSimuls; };
	double getNbSimulations() { return nbSimulations; };
	void setNbSteps(double steps) { nbSteps = steps; };
	double getNbSteps() { return nbSteps; };
	void setTimeSteps(Option* opt); // The "setTimeSteps" method calls the Option contract, and returns an equivalent time grid used for path simulations.
	vector<double> getBSPath(BlackScholesModel* bs_model, Option* opt); // This method calls the BS model and the Option contract, and returns a simulated path of the spot price.
	vector<double> getBSPath(MultiAssetBSModel* bs_model, Option* opt); // This method calls the Multi-Asset BS model and the Option contract, and returns a simulated path of the spot price.
	double price(BlackScholesModel* bs_model, Option* opt); // This method calls the BS model and the Option contract, and returns the equivalent BS Monte-Carlo price.
	double price(MultiAssetBSModel* bs_model, Option* opt); // This method calls the Multi-Asset BS model and the Option contract, and returns the equivalent BS Monte-Carlo price.
};
