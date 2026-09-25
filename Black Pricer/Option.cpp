#include <string>
#include <iostream>
#include "Option.h"
#include <algorithm>
#include <stdexcept>
#include <cmath>

using namespace std;

/*
	The Source file of the class "Option".
*/

VanillaOption::VanillaOption(double strike, double maturity, int flavor) {

	/* The Vanilla Options constructor. */

	setStrike(strike);
	setMaturity(maturity);
	setPhi(flavor);
}

double VanillaOption::payoff(const vector<double>& path) {

	/* The Vanilla Options PayOff. */

	if (path.empty())
		throw invalid_argument("Vanilla payoff requires a non-empty path.");
	double S_T = path.back();
	if (!isfinite(S_T))
		throw invalid_argument("Vanilla payoff path must contain finite values.");
	return phi * (S_T - K) > 0 ? phi * (S_T - K) : 0;
}

DigitalOption::DigitalOption(double strike, double maturity, int flavor) {
	
	/* The Digital Options constructor. */

	setStrike(strike);
	setMaturity(maturity);
	setPhi(flavor);
}

double DigitalOption::payoff(const vector<double>& path) {

	/* The Digital Options PayOff. */

	if (path.empty())
		throw invalid_argument("Digital payoff requires a non-empty path.");
	double S_T = path.back();
	if (!isfinite(S_T))
		throw invalid_argument("Digital payoff path must contain finite values.");
	return phi * (S_T - K) > 0 ? 1 : 0;
}

BarrierOption::BarrierOption(double strike, double barrier, double maturity, int flavor, string barrierType) {
	
	/* The Barrier Options constructor. */

	setStrike(strike);
	setMaturity(maturity);
	setPhi(flavor);
	setBarrier(barrier);
	if (flavor == 1 && barrier < strike) 
		throw invalid_argument("The barrier level must be greater than the strike to benefit from the Barrier Call Option.");
	if (flavor == -1 && barrier > strike) 
		throw invalid_argument("The barrier level must be smaller than the strike to benefit from the Barrier Put Option.");

	// Remove the spaces from the string type and switch it to upper 
	type = barrierType;
	type.erase(remove_if(type.begin(), type.end(), [](unsigned char c) { return isspace(c) != 0; }), type.end());
	for (char& c : type) 
		c = (char)toupper((unsigned char)c);

}

double BarrierOption::payoff(const vector<double>& path) {

	/* The Barrier Options PayOff. */

	if (path.empty())
		throw invalid_argument("Barrier payoff requires a non-empty path.");
	double S_T = path.back();
	if (!isfinite(S_T))
		throw invalid_argument("Barrier payoff path must contain finite values.");

	if (type == "UPOUT" && phi == 1)
		return ((S_T - K) > 0) && ((B - S_T) > 0) ? (S_T - K) : 0;
	else if (type == "UPIN" && phi == 1)
		return (S_T - B) > 0 ? (S_T - K) : 0;
	else if (type == "DOWNOUT" && phi == -1)
		return ((K - S_T) > 0) && ((S_T - B) > 0) ? (K - S_T) : 0;
	else if (type == "DOWNIN" && phi == -1)
		return (B - S_T) > 0 ? (K - S_T) : 0;
	else 
	throw invalid_argument("Unknown Barrier Option Type. The possible types are : Up Out and Up In for Calls, and Down Out and Down In for Puts.");

}

AsianOption::AsianOption(double strike, double maturity, int flavor, size_t frequency) {
	
	/* The Asian Options constructor. */

	setStrike(strike);
	setMaturity(maturity);
	setPhi(flavor);
	setFreq(frequency);
}

double AsianOption::payoff(const vector<double>& path) {
	
	/* The argument "path" contains the underlying fixings to be included in the average computation. */

	if (path.size() != freq)
		throw invalid_argument("Asian payoff path size must equal the fixing frequency.");
	double avg_S = 0;
	for (double s : path)
		if (!isfinite(s))
			throw invalid_argument("Asian payoff path must contain finite values.");
	for (double s : path)
		avg_S += s / path.size();
	return phi * (avg_S - K) > 0 ? phi * (avg_S - K) : 0;
}

BasketOption::BasketOption(double strike, double maturity, int flavor, size_t d) {
	
	/* The Basket Options constructor. */

	setStrike(strike);
	setMaturity(maturity);
	setPhi(flavor);
	setSize(d);
}

double BasketOption::payoff(const vector<double>& path) {
	
	/* The argument "path" contains the underlyings spot prices at maturity. */

	if (path.size() != size)
		throw invalid_argument("Basket payoff path size must equal the basket size.");
	double basket = 0;
	for (double s : path)
		if (!isfinite(s))
			throw invalid_argument("Basket payoff path must contain finite values.");
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

double SpreadOption::payoff(const vector<double>& path) {
	
	/* The argument "path" contains the two underlyings spot prices at maturity. */

	if (path.size() != 2)
		throw invalid_argument("Spread payoff path must contain exactly two values.");
	if (!isfinite(path[0]) || !isfinite(path[1]))
		throw invalid_argument("Spread payoff path must contain finite values.");
	double spread = path[0] - path[1];
	return phi * (spread - K) > 0 ? phi * (spread - K) : 0;
}
