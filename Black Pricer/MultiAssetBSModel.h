#pragma once
#include "Option.h"
#include <cmath>
#include <stdexcept>
#include <utility>
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
	size_t d; // The Underlyings basket size.
	vector<vector<double>> def_pos_corr; // Definite Positive Correlation Matrix.
	vector<vector<double>> cholesky_corr; // Lower Triangular Matrix : Output of the Cholesky Decomposition Algorithm.
		void setSize(size_t size) {
		if (size == 0)
			throw std::invalid_argument("Size must be positive.");
		d = size;
	};
	void setRate(double rate) {
		if (!std::isfinite(rate))
			throw std::invalid_argument("Rate must be finite.");
		r = rate;
	};
	void setVol(vector<double> vol) {
		if (vol.size() != d)
			throw std::invalid_argument("Volatility dimension must equal model size.");
		for (double value : vol) {
			if (!std::isfinite(value) || value <= 0.0)
				throw std::invalid_argument("Volatility must be finite and positive.");
		}
		sigma = std::move(vol);
	};
	void setSpot(vector<double> spot) {
		if (spot.size() != d)
			throw std::invalid_argument("Spot dimension must equal model size.");
		for (double value : spot) {
			if (!std::isfinite(value) || value <= 0.0)
				throw std::invalid_argument("Spot must be finite and positive.");
		}
		S = std::move(spot);
	};
	void setCorr(vector<vector<double>> correlations) { def_pos_corr = correlations; };
	void validateCorrelationMatrix(const vector<vector<double>>& corr) const;
	void setCholeskyCorr(vector<vector<double>> correlations) { cholesky_corr = correlations; };
	void CholeskyAlgo(const vector<vector<double>>& correlations); // Cholesky Decomposition Algorithm.

public :
	size_t getSize() { return d; };
	double getRate() { return r; };
	vector<double> getVol() { return sigma; };
	vector<double> getSpot() { return S; };
	vector<vector<double>> getCorr() { return def_pos_corr; };
	vector<vector<double>> getCholeskyCorr() { return cholesky_corr; };
	vector<double> simulation(const vector<double>& prev_S, double dt, const vector<double>& rnd_normal); // The simulation method is called in the "MonteCarlo" class.
	virtual double price(Option* opt) = 0; // The BS price is a pure virtual method.
	virtual ~MultiAssetBSModel() = default;
};


class BlackBasket : public MultiAssetBSModel {
public :
	BlackBasket(double rate, vector<double> spot, vector<double> vol, vector<vector<double>> corr_matrix);
	double price(Option* opt) override;
};

class BlackSpread : public MultiAssetBSModel {
public:
	BlackSpread(double rate, vector<double> spot, vector<double> vol, vector<vector<double>> corr_matrix);
	double price(Option* opt) override;
};
