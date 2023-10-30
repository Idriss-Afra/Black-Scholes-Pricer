#pragma once
#include "Option.h"
#include <vector>

/*
	The Header file of the class "BlackScholesModel".
	The "BlackScholesModel" is an abstract class from which we derive different BS methods : BS for Vanillas, Digitals, European Barriers, and Arithmetic Asians.
*/

class BlackScholesModel {
protected :
	double r; // ZC Rate.
	double sigma; // The underlying Volatility.
	double S; // The Underlying Spot Price.
public :
	void setRate(double rate) { r = rate; };
	double getRate() { return r; };
	void setVol(double vol) { sigma = vol; };
	double getVol() { return sigma; };
	void setSpot(double spot) { S = spot; };
	double getSpot() { return S; };
	double simulation(double prev_S, double dt, double rnd_normal); // The simulation method is called in the "MonteCarlo" class.
	virtual double price(Option* opt) = 0; // The BS price is a pure virtual method.
};

class BlackVanilla : public BlackScholesModel {
public :
	BlackVanilla(double rate, double spot, double vol);
	double price(Option* opt);
};

class BlackDigital : public BlackScholesModel {
public:
	BlackDigital(double rate, double spot, double vol);
	double price(Option* opt);
};

class BlackBarrier : public BlackScholesModel {
public:
	BlackBarrier(double rate, double spot, double vol);
	double price(Option* opt);
};

class BlackAsian : public BlackScholesModel {
public:
	BlackAsian(double rate, double spot, double vol);
	double price(Option* opt);
};

