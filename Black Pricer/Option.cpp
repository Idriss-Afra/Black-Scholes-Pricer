#include <string>
#include <iostream>
#include "Option.h"
#include <algorithm>

using namespace std;

/*
	The Source file of the class "Option".
*/

void Option::setPhi(double flavor) { 

	if (flavor != 1 && flavor != -1) {
		cout << "Flavor input must be equal to 1 for Calls and -1 for Puts." << endl;
		exit(-1);
	}

	phi = flavor; 
};

VanillaOption::VanillaOption(double strike, double maturity, int flavor) {

	/* The Vanilla Options constructor. */

	setStrike(strike);
	setMaturity(maturity);
	setPhi(flavor);
}

double VanillaOption::payoff(vector<double> path) {

	/* The Vanilla Options PayOff. */

	double S_T = path[path.size() - 1]; 
	return phi * (S_T - K) > 0 ? phi * (S_T - K) : 0;
}

DigitalOption::DigitalOption(double strike, double maturity, int flavor) {
	
	/* The Digital Options constructor. */

	setStrike(strike);
	setMaturity(maturity);
	setPhi(flavor);
}

double DigitalOption::payoff(vector<double> path) {

	/* The Digital Options PayOff. */

	double S_T = path[path.size() - 1];
	return phi * (S_T - K) > 0 ? 1 : 0;
}

BarrierOption::BarrierOption(double strike, double barrier, double maturity, int flavor, string barrierType) {
	
	/* The Barrier Options constructor. */

	if (flavor == 1 && barrier < strike) {
		cout << "The barrier level must be greater than the strike to benefit from the Barrier Call Option." << endl;
		exit(-1);
	}
	if (flavor == -1 && barrier > strike) {
		cout << "The barrier level must be smaller than the strike to benefit from the Barrier Put Option." << endl;
		exit(-1);
	}
	
	setStrike(strike);
	setMaturity(maturity);
	setPhi(flavor);
	setBarrier(barrier);
	type = barrierType;
}

double BarrierOption::payoff(vector<double> path) {

	/* The Barrier Options PayOff. */

	double S_T = path[path.size() - 1];

	// Remove the spaces from the string type and switch it to upper cases
	type.erase(remove_if(type.begin(), type.end(), isspace), type.end());
	for (char& c : type) c = toupper(c);

	if (type == "UPOUT" && phi == 1) {
		return ((S_T - K) > 0) && ((B - S_T) > 0) ? (S_T - K) : 0;
	}
	else if (type == "UPIN" && phi == 1) {
		return (S_T - B) > 0 ? (S_T - K) : 0;
	}
	else if (type == "DOWNOUT" && phi == -1) {
		return ((K - S_T) > 0) && ((S_T - B) > 0) ? (K - S_T) : 0;
	}
	else if (type == "DOWNIN" && phi == -1) {
		return (B - S_T) > 0 ? (K - S_T) : 0;
	}
	else {
		cout << "Unknow Barrier Option Type. The possible types are : \"Up Out\" and \"Up In\" for Calls, and \"Down Out\" and \"Down In\" for Puts." << endl;
		exit(-1);
	}

}

AsianOption::AsianOption(double strike, double maturity, int flavor, double frequency) {
	
	/* The Asian Options constructor. */

	setStrike(strike);
	setMaturity(maturity);
	setPhi(flavor);
	setFreq(frequency);
}

double AsianOption::payoff(vector<double> path) {
	
	/* The argument "path" contains the underlying fixings to be included in the average computation. */

	double avg_S = 0; 
	for (double s : path)
		avg_S += s / path.size();
	return phi * (avg_S - K) > 0 ? phi * (avg_S - K) : 0;
}

BasketOption::BasketOption(double strike, double maturity, int flavor, double d) {
	
	/* The Basket Options constructor. */

	setStrike(strike);
	setMaturity(maturity);
	setPhi(flavor);
	setSize(d);
}

double BasketOption::payoff(vector<double> path) {
	
	/* The argument "path" contains the underlyings spot prices at maturity. */

	double basket = 0;
	for (double s : path)
		basket += s / path.size();
	return phi * (basket - K) > 0 ? phi * (basket - K) : 0;
}

SpreadOption::SpreadOption(double strike, double maturity, int flavor) {
	
	/* The Spread Options constructor. Size defaulted to 2. */

	setStrike(strike);
	setMaturity(maturity);
	setPhi(flavor);
	setSize(2);
}

double SpreadOption::payoff(vector<double> path) {
	
	/* The argument "path" contains the two underlyings spot prices at maturity. */

	double spread = path[0] - path[1];
	return phi * (spread - K) > 0 ? phi * (spread - K) : 0;
}
