#include "MultiAssetBSModel.h"
#include <cmath>
#include <algorithm>
#include <numeric> 

using namespace std;

double std_normal_cum_func(double x) {

	/* Standard Normal Distribution Cumulative function. */

	return 0.5 + 0.5 * erf(x / pow(2, 0.5));
}

void MultiAssetBSModel::makeCorrDefPos(vector<vector<double>> correlations) {
	/*
		"makeCorrDefPos" method ensures that the correlation matrix is Definite Positive.
		It caps the correlations with 1 and floors them with - 1 / (size - 1).
	*/
	double eps = 0.0001;
	double cap = 1 - eps;
	double floor = -1 / (d - 1) + eps;

	for (int i = 0; i < d; i++) {
		for (int j = i + 1; j < d; j++) {
			correlations[i][j] = max(min(correlations[i][j], cap), floor);
			correlations[j][i] = correlations[i][j];
		}
	}
	setCorr(correlations);
}

double sum_squared(vector<double> vector, int end) {
	double result = 0;
	for (int k = 0; k < end; k++)
		result += pow(vector[k], 2);
	return result;
}

double sum_product(vector<double> vector_1, vector<double> vector_2, int end) {
	double result = 0;
	for (int k = 0; k < end; k++)
		result += vector_1[k] * vector_2[k];
	return result;
}

void MultiAssetBSModel::CholeskyAlgo(vector<vector<double>> correlations) {
	
	/* Cholesky Decomposition Algorithm. */

	makeCorrDefPos(correlations);
	vector<vector<double>> corrTriangInf = vector<vector<double>>(d , vector<double>(d, 0));
	
	for (int j = 0; j < d; j++)
		for (int i = j; i < d; i++) {
			if (i == j)
				corrTriangInf[j][j] = pow(def_pos_corr[j][j] - sum_squared(corrTriangInf[i], j), 0.5);
			else 
				corrTriangInf[i][j] = (def_pos_corr[i][j] - sum_product(corrTriangInf[i], corrTriangInf[j], j)) / corrTriangInf[j][j];
		}
	
	setCholeskyCorr(corrTriangInf);
}

vector<double> MultiAssetBSModel::simulation(vector<double> prev_S, double dt, vector<double> rnd_normal) {
	/*
		Spot price simulation between t and t + dt under the BS model. 
		The correlations are handled by the Cholesky Decomposition output. 
	*/
	vector<double> next_S;
	for (int i = 0; i < d; i++) 
		next_S.push_back(prev_S[i] * exp((r - pow(sigma[i], 2) / 2) * dt + sigma[i] * pow(dt, 0.5) * sum_product(cholesky_corr[i], rnd_normal, d)));
	
	return next_S;
}

BlackBasket::BlackBasket(double rate, double size, vector<double> spot, vector<double> vol, vector<vector<double>> correlations) {
	
	/* BS Basket constructor. */

	setSize(size);
	setSpot(spot);
	setRate(rate);
	setVol(vol);
	CholeskyAlgo(correlations);
}

double BlackBasket::price(Option* opt) {
	
	/* BS Basket price : BS formula based on the moments matching method of the basket. */

	double T = opt->getMaturity();
	double K = opt->getStrike();

	double beta1, beta2;
	double m1 = 0;
	double m2 = 0;

	for (int i = 0; i < d; i++) {
		beta1 = S[i] * exp(r * T) / d;
		m1 += beta1;
		for (int j = 0; j < d; j++) {
			beta2 = S[j] * exp(r * T) / d;
			m2 += beta1 * beta2 * exp(sigma[i] * sigma[j] * def_pos_corr[i][j] * T);
		}
	}

	double df = exp(-r * T);
	double d1 = (log(m1 / K) + log(m2 / pow(m1, 2)) / 2) / pow(log(m2 / pow(m1, 2)), 0.5);
	double d2 = d1 - pow(log(m2 / pow(m1, 2)), 0.5);
	double phi = opt->getPhi();
	return df * (phi * m1 * std_normal_cum_func(phi * d1) - phi * K * std_normal_cum_func(phi * d2));
}

BlackSpread::BlackSpread(double rate, vector<double> spot, vector<double> vol, vector<vector<double>> correlations) {
	
	/* BS Spread constructor. */

	setSize(2);
	setSpot(spot);
	setRate(rate);
	setVol(vol);
	CholeskyAlgo(correlations);
}

double BlackSpread::price(Option* opt) {
	
	/* BS Spread price : BS formula based on the Margrabe's method. */

	double T = opt->getMaturity();
	double K = opt->getStrike();
	double df = exp(-r * T);
	double corr = def_pos_corr[0][1];
	double S1_adj = S[1] + K;
	double vol1_adj = sigma[1] * S[1] / (S[1] + K * df);
	double vol = pow(pow(sigma[0], 2) + pow(vol1_adj, 2) - 2 * sigma[0] * vol1_adj * corr, 0.5);
	
	double d1 = (log(S[0] / S1_adj) + pow(vol, 2) * T / 2) / (vol * pow(T, 0.5));
	double d2 = d1 - vol * pow(T, 0.5);
	double phi = opt->getPhi();
	return phi * S[0] * std_normal_cum_func(phi * d1) - phi * S1_adj *std_normal_cum_func(phi * d2);
}