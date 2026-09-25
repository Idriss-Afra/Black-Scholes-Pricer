#include "MultiAssetBSModel.h"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <stdexcept>

using namespace std;

/*
	The Source file of the class "MultiAssetBSModel".
*/

double std_normal_cum_func(double x) {

	/* Standard Normal Distribution Cumulative function. */

	return 0.5 + 0.5 * erf(x / pow(2, 0.5));
}

void MultiAssetBSModel::validateCorrelationMatrix(const vector<vector<double>>& corr) const
{
    constexpr double tol = 1e-12;

    if (d <= 0 || corr.size() != static_cast<size_t>(d))
        throw invalid_argument("Correlation matrix has invalid dimensions.");

    for (int i = 0; i < d; ++i) {
        if (corr[i].size() != static_cast<size_t>(d))
            throw invalid_argument("Correlation matrix must be square.");
	}

	for (int i = 0; i < d; ++i) {
		for (int j = 0; j < d; ++j) {
			if (!isfinite(corr[i][j]) || abs(corr[i][j]) > 1.0 + tol)
				throw invalid_argument("Correlation entries must be finite and lie in [-1, 1].");
		}

		if (abs(corr[i][i] - 1.0) > tol)
            throw invalid_argument("Correlation diagonal must equal one.");

        for (int j = i + 1; j < d; ++j) {
			if (abs(corr[i][j] - corr[j][i]) > tol)
                throw invalid_argument("Correlation matrix must be symmetric.");
        }
    }
}

void MultiAssetBSModel::CholeskyAlgo(const vector<vector<double>>& corr)
{
    constexpr double tol = 1e-12;
    vector<vector<double>> L(d, vector<double>(d, 0.0));

	validateCorrelationMatrix(corr);

    for (int i = 0; i < d; ++i) {
        for (int j = 0; j <= i; ++j) {
            
			double sum = 0.0;
            for (int k = 0; k < j; ++k)
                sum += L[i][k] * L[j][k];

            if (i == j) {
                const double pivot = corr[i][i] - sum;
                if (pivot <= tol)
                    throw invalid_argument("Correlation matrix is not strictly positive definite.");
                L[i][j] = sqrt(pivot);
            } else {
                L[i][j] = (corr[i][j] - sum) / L[j][j];
            }
        }
    }
	setCorr(corr);
	setCholeskyCorr(L);
}

double sum_product(const vector<double>& vector_1, const vector<double>& vector_2, size_t end) {
	/* Helper method */
	
	double result = 0;
	for (int k = 0; k < end; ++k)
		result += vector_1[k] * vector_2[k];
	return result;
}

vector<double> MultiAssetBSModel::simulation(const vector<double>& prev_S, double dt, const vector<double>& rnd_normal) {
	/*
		Spot price simulation between t and t + dt under the BS model. 
		The correlations are handled by the Cholesky Decomposition output. 
	*/
	if (prev_S.size() != d || rnd_normal.size() != d)
		throw invalid_argument("Simulation vectors must match model size.");
	if (!isfinite(dt) || dt < 0.0)
		throw invalid_argument("Time step must be finite and non-negative.");

	vector<double> next_S(d);
	for (size_t i = 0; i < d; ++i)
		next_S[i] = prev_S[i] * exp((r - sigma[i] * sigma[i] / 2.0) * dt
			+ sigma[i] * sqrt(dt) * sum_product(cholesky_corr[i], rnd_normal, d));
	
	return next_S;
}

BlackBasket::BlackBasket(double rate, vector<double> spot, vector<double> vol, vector<vector<double>> correlations) {
	
	/* BS Basket constructor. */

	if (spot.size() != vol.size())
		throw invalid_argument("Spot and volatility dimensions must match.");

	setSize(spot.size());
	setSpot(spot);
	setRate(rate);
	setVol(vol);
	CholeskyAlgo(correlations);
}

double BlackBasket::price(Option* opt) {
	
	/* BS Basket price : BS formula based on the moments matching method of the basket. */

	auto* basket = dynamic_cast<BasketOption*>(opt);
	if (basket == nullptr)
		throw invalid_argument("BlackBasket requires a BasketOption.");

	double T = basket->getMaturity();
	double K = basket->getStrike();

	double beta1, beta2;
	double m1 = 0;
	double m2 = 0;

	for (int i = 0; i < d; ++i) {
		beta1 = S[i] * exp(r * T) / d;
		m1 += beta1;
		for (int j = 0; j < d; ++j) {
			beta2 = S[j] * exp(r * T) / d;
			m2 += beta1 * beta2 * exp(sigma[i] * sigma[j] * def_pos_corr[i][j] * T);
		}
	}

	double df = exp(-r * T);
	double d1 = (log(m1 / K) + log(m2 / pow(m1, 2)) / 2) / pow(log(m2 / pow(m1, 2)), 0.5);
	double d2 = d1 - pow(log(m2 / pow(m1, 2)), 0.5);
	int phi = basket->getPhi();
	return df * (phi * m1 * std_normal_cum_func(phi * d1) - phi * K * std_normal_cum_func(phi * d2));
}

BlackSpread::BlackSpread(double rate, vector<double> spot, vector<double> vol, vector<vector<double>> correlations) {
	
	/* BS Spread constructor. */

	if (spot.size() != 2 || vol.size() != 2 || correlations.size() != 2)
		throw invalid_argument("Spot, volatility and correlation dimensions must reflect two underlyings.");	

	setSize(2);
	setSpot(spot);
	setRate(rate);
	setVol(vol);
	CholeskyAlgo(correlations);
}

double BlackSpread::price(Option* opt) {
	
	/* BS Spread price : BS formula based on the Kirk�s approximation & Margrabe's method. */

	auto* spread = dynamic_cast<SpreadOption*>(opt);
	if (spread == nullptr)
		throw invalid_argument("BlackSpread requires a SpreadOption.");

	double T = spread->getMaturity();
	double K = spread->getStrike();
	double df = exp(-r * T);
	double corr = def_pos_corr[0][1];
	double S1_adj = S[1] + K * df;
	double vol1_adj = sigma[1] * S[1] / S1_adj;
	double vol = pow(pow(sigma[0], 2) + pow(vol1_adj, 2) - 2 * sigma[0] * vol1_adj * corr, 0.5);
	
	double d1 = (log(S[0] / S1_adj) + pow(vol, 2) * T / 2) / (vol * pow(T, 0.5));
	double d2 = d1 - vol * pow(T, 0.5);
	int phi = spread->getPhi();
	return phi * S[0] * std_normal_cum_func(phi * d1) - phi * S1_adj *std_normal_cum_func(phi * d2);
}