#pragma once
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
	double freq = 1; // The frequency is necessary to define Asian Options. It is defaulted to 1 for the other flavors.
	double size = 1; // The size is necessary to define Multi-Asset Options. It is defaulted to 1 for the other flavors.
	double B; // The barrier level is necessary to define Barrier Options.
public:
	void setMaturity(double maturity) { T = maturity; };
	double getMaturity() { return T; };
	void setStrike(double strike) { K = strike; };
	double getStrike() { return K; };
	void setPhi(double flavor);
	int getPhi() { return phi; };
	void setSize(double s) { size = s; };
	double getSize() { return size; };
	void setFreq(double f) { freq = f; };
	double getFreq() { return freq; };
	void setBarrier(double barrier) { B = barrier; };
	double getBarrier() { return B; };
	virtual string getType() { return type; };
	virtual double payoff(vector<double> path) = 0; // The PayOff script is a pure virtual method.
};

class VanillaOption : public Option {
private :
	string type = "Vanilla";
public :
	VanillaOption(double strike, double maturity, int flavor);
	string getType() { return type; };
	double payoff(vector<double> path);
};

class DigitalOption : public Option {
private:
	string type = "Digital";
public:
	DigitalOption(double strike, double maturity, int flavor);
	string getType() { return type; };
	double payoff(vector<double> path);
};

class BarrierOption : public Option {
private:
	string type = "Digital";
public:
	BarrierOption(double strike, double barrier, double maturity, int flavor, string barrierType);
	string getType() { return type; };
	double payoff(vector<double> path);
};

class AsianOption : public Option {
private:
	string type = "Asian";
public:
	AsianOption(double strike, double maturity, int flavor, double freq);
	string getType() { return type; };
	double payoff(vector<double> path);
};

class BasketOption : public Option {
private:
	string type = "Basket";
public:
	BasketOption(double strike, double maturity, int flavor, double d);
	string getType() { return type; };
	double payoff(vector<double> path);
};

class SpreadOption : public Option {
private:
	string type = "Spread";
public:
	SpreadOption(double strike, double maturity, int flavor);
	string getType() { return type; };
	double payoff(vector<double> path);
};