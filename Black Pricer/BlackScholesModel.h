#pragma once
#include "Option.h"
#include <cmath>
#include <stdexcept>
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
	void setRate(double rate) {
		if (!std::isfinite(rate))
			throw std::invalid_argument("Rate must be finite.");
		r = rate;
	};
	double getRate() { return r; };
	void setVol(double vol) {
		if (!std::isfinite(vol) || vol <= 0.0)
			throw std::invalid_argument("Volatility must be finite and positive.");
		sigma = vol;
	};
	double getVol() { return sigma; };
	void setSpot(double spot) {
		if (!std::isfinite(spot) || spot <= 0.0)
			throw std::invalid_argument("Spot must be finite and positive.");
		S = spot;
	};
	double getSpot() { return S; };
	double simulation(double prev_S, double dt, double rnd_normal); // The simulation method is called in the "MonteCarlo" class.
	virtual double price(Option* opt) = 0; // The BS price is a pure virtual method.
	virtual ~BlackScholesModel() = default;
};

class BlackVanilla : public BlackScholesModel {
public :
	BlackVanilla(double rate, double spot, double vol);
	double price(Option* opt) override;
};

class BlackDigital : public BlackScholesModel {
public:
	BlackDigital(double rate, double spot, double vol);
	double price(Option* opt) override;
};

class BlackBarrier : public BlackScholesModel {
public:
	BlackBarrier(double rate, double spot, double vol);
	double price(Option* opt) override;
};

class BlackAsian : public BlackScholesModel {
public:
	BlackAsian(double rate, double spot, double vol);
	double price(Option* opt) override;
};

