#pragma once
#include "Option.h"
#include <vector>

/*
	The Header file of the class "MultiAssetBSModel".
	The "MultiAssetBSModel" is an abstract class from which we derive different Multi-Asset BS methods : BS for Baskets, and BS for Spread Options.
*/

class MultiAssetBSModel {
protected :
	double r; // ZC Rate.
	vector<double> sigma; // The Underlyings Volatilities.
	vector<double> S; // The Underlyings Spot Prices.
	double d; // The Underlyings basket size.
	vector<vector<double>> def_pos_corr; // Definite Positive Correlation Matrix.
	vector<vector<double>> cholesky_corr; // Lower Triangular Matrix : Output of the Cholesky Decomposition Algorithm.
public:
	void setSize(double size) { d = size; };
	double getSize() { return d; };
	void setRate(double rate) { r = rate; };
	double getRate() { return r; };
	void setVol(vector<double> vol) { sigma = vol; };
	vector<double> getVol() { return sigma; };
	void setSpot(vector<double> spot) { S = spot; };
	vector<double> getSpot() { return S; };
	void setCorr(vector<vector<double>> correlations) { def_pos_corr = correlations; };
	vector<vector<double>> getCorr() { return def_pos_corr; };
	void setCholeskyCorr(vector<vector<double>> correlations) { cholesky_corr = correlations; };
	vector<vector<double>> getCholeskyCorr() { return cholesky_corr; };
	void CholeskyAlgo(vector<vector<double>> correlations); // Cholesky Decomposition Algorithm.
	void makeCorrDefPos(vector<vector<double>> correlations); // The "makeCorrDefPos" method ensures that the correlation matrix is Definite Positive.
	vector<double> simulation(vector<double> prev_S, double dt, vector<double> rnd_normal); // The simulation method is called in the "MonteCarlo" class.
	virtual double price(Option* opt) = 0; // The BS price is a pure virtual method.
};


class BlackBasket : public MultiAssetBSModel {

public :
	BlackBasket(double rate, double size, vector<double> spot, vector<double> vol, vector<vector<double>> corr_matrix);
	double price(Option* opt);
};

class BlackSpread : public MultiAssetBSModel {

public:
	BlackSpread(double rate, vector<double> spot, vector<double> vol, vector<vector<double>> corr_matrix);
	double price(Option* opt);
};
