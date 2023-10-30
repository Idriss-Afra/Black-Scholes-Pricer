#include "BlackScholesModel.h"
#include <cmath>
#include <algorithm>
#include <numeric> 
#include <iostream>

using namespace std;

/*
	The Source file of the class "BlackScholesModel".
*/

double std_normal_cum(double x) {
	
	/* Standard Normal Distribution Cumulative function. */

	return 0.5 + 0.5 * erf(x / pow(2, 0.5));
}

double BlackScholesModel::simulation(double prev_S, double dt, double rnd_normal) {
	
	/* Spot price simulation between t and t + dt under the BS model. */

	return prev_S * exp((r - pow(sigma, 2) / 2) * dt + sigma * pow(dt, 0.5) * rnd_normal);

}

BlackVanilla::BlackVanilla(double rate, double spot, double vol) {
	
	/* BS Vanilla constructor. */

	setSpot(spot);
	setRate(rate);
	setVol(vol);
}

double BlackVanilla::price(Option* opt) {
	
	/* BS Vanilla price. */

	double T = opt->getMaturity();
	double K = opt->getStrike();
	double df = exp(-r * T);
	double fwd = S / df;
	double v2T = pow(sigma, 2) * T;
	double d1 = (log(fwd / K) + v2T / 2) / pow(v2T, 0.5);
	double d2 = d1 - pow(v2T, 0.5);
	double phi = opt->getPhi();
	return phi * S * std_normal_cum(phi * d1) - phi * K * df * std_normal_cum(phi * d2);
}

BlackDigital::BlackDigital(double rate, double spot, double vol) {
	
	/* BS Digital constructor. */

	setSpot(spot);
	setRate(rate);
	setVol(vol);
}

double BlackDigital::price(Option* opt) {
	
	/* BS Digital price. */

	double T = opt->getMaturity();
	double K = opt->getStrike();
	double df = exp(-r * T);
	double fwd = S / df;
	double v2T = pow(sigma, 2) * T;
	double d1 = (log(fwd / K) + v2T / 2) / pow(v2T, 0.5);
	double d2 = d1 - pow(v2T, 0.5);
	double phi = opt->getPhi();
	return df * std_normal_cum(phi * d2);
}

BlackBarrier::BlackBarrier(double rate, double spot, double vol) {
	
	/* BS Barrier constructor. */

	setSpot(spot);
	setRate(rate);
	setVol(vol);
}

double BlackBarrier::price(Option* opt) {
	
	/* BS Barrier price : Barrier Static Replication using Vanilla and Digital Options */

	double barrier = opt->getBarrier();
	double strike = opt->getStrike();
	double maturity = opt->getMaturity();
	double phi = opt->getPhi();
	string type = opt->getType();

	VanillaOption* vanilla_strike = new VanillaOption(strike, maturity, phi);
	VanillaOption* vanilla_barrier = new VanillaOption(barrier, maturity, phi);
	DigitalOption* digital_barrier = new DigitalOption(barrier, maturity, phi);

	BlackVanilla bs_vanilla(r, S, sigma);
	BlackDigital bs_digital(r, S, sigma);

	double vanilla_strike_price = bs_vanilla.price(vanilla_strike);
	double vanilla_barrier_price = bs_vanilla.price(vanilla_barrier);
	double digital_barrier_price = bs_digital.price(digital_barrier);

	double price_out = vanilla_strike_price - vanilla_barrier_price - phi * (barrier - strike) * digital_barrier_price;

	// Remove the spaces from the string type and switch it to upper cases
	type.erase(remove_if(type.begin(), type.end(), isspace), type.end());
	for (char& c : type) c = toupper(c);
	
	if ((type == "UPOUT" && phi == 1) || (type == "DOWNOUT" && phi == -1))
		return price_out;
	if ((type == "UPIN" && phi == 1) || (type == "DOWNIN" && phi == -1))
		return vanilla_strike_price - price_out;

	cout << "Unknow Barrier Option Type. The possible types are : \"Up Out\" and \"Up In\" for Calls, and \"Down Out\" and \"Down In\" for Puts." << endl;
	exit(-1);
}


BlackAsian::BlackAsian(double rate, double spot, double vol) {
	
	/* BS Asian constructor. */

	setSpot(spot);
	setRate(rate);
	setVol(vol);
}

double BlackAsian::price(Option* opt) {
	
	/* BS Asian price : BS formula based on the moments matching method of the arithmetic average. */

	double freq = opt->getFreq();
	double T = opt->getMaturity();
	double K = opt->getStrike();
	vector<double> betas;
	vector<double> e_v2T;

	for (int i = 1; i <= freq; i++) {
		betas.push_back(S * exp(r * i * T / freq) / freq);
		e_v2T.push_back(exp(pow(sigma, 2) * i * T / freq));
	}

	double m1 = accumulate(betas.begin(), betas.end(), 0.);
	double m2 = 0;
	double temp = 0;

	for (int i = 0; i < freq; i++) {
		for (int j = i; j < freq; j++)
			temp += 2 * betas[j];
		m2 += betas[i] * e_v2T[i] * (temp - betas[i]);
		temp = 0;
	}

	double df = exp(-r * T);
	double d1 = (log(m1 / K) + log(m2 / pow(m1, 2)) / 2) / pow(log(m2 / pow(m1, 2)), 0.5);
	double d2 = d1 - pow(log(m2 / pow(m1, 2)), 0.5);
	double phi = opt->getPhi();
	return df * (phi * m1 * std_normal_cum(phi * d1) - phi * K * std_normal_cum(phi * d2));
}
