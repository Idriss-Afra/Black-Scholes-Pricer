#pragma once
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

/*
	The Header file of the class "Option".
	The "Option" is an abstract class from which we derive different Option flavors : Vanillas, Arithmetic Asians, Baskets, and Spreads Options.
*/

class Option {
private :
	string type;
protected :
	double K; // The Strike.
	double T; // The Maturity date.
	int phi; // Phi is equal to +1 for Call Options, and -1 for Put Options.
	size_t freq = 1; // The frequency is necessary to define Asian Options. It is defaulted to 1 for the other flavors.
	size_t size = 1; // The size is necessary to define Multi-Asset Options. It is defaulted to 1 for the other flavors.
	double B = 0.0; // The barrier level is necessary to define Barrier Options.
public :
	void setMaturity(double maturity) {
		if (!std::isfinite(maturity) || maturity <= 0.0)
			throw std::invalid_argument("Maturity must be finite and positive.");
		T = maturity;
	};
	double getMaturity() { return T; };
	void setStrike(double strike) {
		if (!std::isfinite(strike) || strike <= 0.0)
			throw std::invalid_argument("Strike must be finite and positive.");
		K = strike;
	};
	double getStrike() { return K; };
	void setPhi(int flavor) {
		if (flavor != 1 && flavor != -1)
			throw std::invalid_argument("Flavor input must be equal to 1 for Calls and -1 for Puts.");
		phi = flavor;
	};
	int getPhi() { return phi; };
	void setSize(size_t s) {
		if (s == 0)
			throw std::invalid_argument("Size must be positive.");
		size = s;
	};
	size_t getSize() { return size; };
	void setFreq(size_t f) {
		if (f == 0)
			throw std::invalid_argument("Frequency must be positive.");
		freq = f;
	};
	size_t getFreq() { return freq; };
	void setBarrier(double barrier) {
		if (!std::isfinite(barrier) || barrier <= 0.0)
			throw std::invalid_argument("Barrier must be finite and positive.");
		B = barrier;
	};
	double getBarrier() { return B; };
	virtual string getType() { return type; };
	virtual double payoff(const vector<double>& path) = 0; // The PayOff script is a pure virtual method.
	virtual ~Option() = default;
};

class VanillaOption : public Option {
private :
	string type = "Vanilla";
public :
	VanillaOption(double strike, double maturity, int flavor);
	string getType() { return type; };
	double payoff(const vector<double>& path);
};

class DigitalOption : public Option {
private:
	string type = "Digital";
public:
	DigitalOption(double strike, double maturity, int flavor);
	string getType() { return type; };
	double payoff(const vector<double>& path);
};

class BarrierOption : public Option {
private:
	string type = "Barrier";
public:
	BarrierOption(double strike, double barrier, double maturity, int flavor, string barrierType);
	string getType() { return type; };
	double payoff(const vector<double>& path);
};

class AsianOption : public Option {
private:
	string type = "Asian";
public:
	AsianOption(double strike, double maturity, int flavor, size_t freq);
	string getType() { return type; };
	double payoff(const vector<double>& path);
};

class BasketOption : public Option {
private:
	string type = "Basket";
public:
	BasketOption(double strike, double maturity, int flavor, size_t d);
	string getType() { return type; };
	double payoff(const vector<double>& path);
};

class SpreadOption : public Option {
private:
	string type = "Spread";
public:
	SpreadOption(double strike, double maturity, int flavor);
	string getType() { return type; };
	double payoff(const vector<double>& path);
};