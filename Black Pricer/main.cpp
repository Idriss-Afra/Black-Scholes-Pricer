#include <iostream>
#include "Option.h"
#include "BlackScholesModel.h"
#include "MultiAssetBSModel.h"
#include "MonteCarlo.h"

using namespace std;

int main() {
	double rate = 0.05;
	double vol = 0.3;
	double spot = 100;

	MonteCarlo mc(100000); // Number of Simulation = 100 000.
	MonteCarlo mc_path_dep(30000, 50); // Path-Dependent MC : Number of Simulation = 30 000 & Number of Time Steps = 50.
	
	cout << "*********************** Vanilla Call ***********************" << endl;
	VanillaOption call_vanilla(105, 1, 1); 
	BlackVanilla bs_vanilla(rate, spot, vol);
	cout << "Analytical Price  : " << bs_vanilla.price(&call_vanilla) << endl;
	MonteCarloResult result = mc.price(&bs_vanilla, &call_vanilla);
	cout << "Monte Carlo Price : " << result.price << endl;
	cout << "Standard Error    : " << result.standardError << endl;
	cout << "95% CI            : ["
		<< result.ciLower << ", "
		<< result.ciUpper << "]" << endl;
	cout << "************************************************************" << endl;
	cout << endl;
	cout << "*********************** Vanilla Put ************************" << endl;
	VanillaOption put_vanilla(95, 1, -1);
	cout << "Analytical Price  : " << bs_vanilla.price(&put_vanilla) << endl;
	result = mc.price(&bs_vanilla, &put_vanilla);
	cout << "Monte Carlo Price : " << result.price << endl;
	cout << "Standard Error    : " << result.standardError << endl;
	cout << "95% CI            : ["
		<< result.ciLower << ", "
		<< result.ciUpper << "]" << endl;
	cout << "************************************************************" << endl;
	cout << endl;

	cout << "*********************** Digital Call ***********************" << endl;
	DigitalOption call_digital(105, 1, 1);
	BlackDigital bs_digital(rate, spot, vol);
	cout << "Analytical Price  : " << bs_digital.price(&call_digital) << endl;
	result = mc.price(&bs_digital, &call_digital);
	cout << "Monte Carlo Price : " << result.price << endl;
	cout << "Standard Error    : " << result.standardError << endl;
	cout << "95% CI            : ["
		<< result.ciLower << ", "
		<< result.ciUpper << "]" << endl;
	cout << "************************************************************" << endl;
	cout << endl;
	cout << "*********************** Digital Put ************************" << endl;
	DigitalOption put_digital(95, 1, -1);
	cout << "Analytical Price  : " << bs_digital.price(&put_digital) << endl;
	result = mc.price(&bs_digital, &put_digital);
	cout << "Monte Carlo Price : " << result.price << endl;
	cout << "Standard Error    : " << result.standardError << endl;
	cout << "95% CI            : ["
		<< result.ciLower << ", "
		<< result.ciUpper << "]" << endl;
	cout << "************************************************************" << endl;
	cout << endl;

	cout << "*********************** UP & OUT Call **********************" << endl;
	BarrierOption call_upout(105, 145, 1, 1, "Up Out");
	BlackBarrier bs_barrier(rate, spot, vol);
	cout << "Analytical Price  : " << bs_barrier.price(&call_upout) << endl;
	result = mc.price(&bs_barrier, &call_upout);
	cout << "Monte Carlo Price : " << result.price << endl;
	cout << "Standard Error    : " << result.standardError << endl;
	cout << "95% CI            : ["
		<< result.ciLower << ", "
		<< result.ciUpper << "]" << endl;
	cout << "************************************************************" << endl;
	cout << endl;
	cout << "*********************** UP & IN Call ***********************" << endl;
	BarrierOption call_upin(105, 145, 1, 1, "Up In");
	cout << "Analytical Price  : " << bs_barrier.price(&call_upin) << endl;
	result = mc.price(&bs_barrier, &call_upin);
	cout << "Monte Carlo Price : " << result.price << endl;
	cout << "Standard Error    : " << result.standardError << endl;
	cout << "95% CI            : ["
		<< result.ciLower << ", "
		<< result.ciUpper << "]" << endl;
	cout << "************************************************************" << endl;
	cout << endl;
	cout << "*********************** DOWN & OUT Put *********************" << endl;
	BarrierOption put_downout(105, 65, 1, -1, "Down Out");
	cout << "Analytical Price  : " << bs_barrier.price(&put_downout) << endl;
	result = mc.price(&bs_barrier, &put_downout);
	cout << "Monte Carlo Price : " << result.price << endl;
	cout << "Standard Error    : " << result.standardError << endl;
	cout << "95% CI            : ["
		<< result.ciLower << ", "
		<< result.ciUpper << "]" << endl;
	cout << "************************************************************" << endl;
	cout << endl;
	cout << "*********************** DOWN & IN Put **********************" << endl;
	BarrierOption put_downin(105, 65, 1, -1, "Down In");
	cout << "Analytical Price  : " << bs_barrier.price(&put_downin) << endl;
	result = mc.price(&bs_barrier, &put_downin);
	cout << "Monte Carlo Price : " << result.price << endl;
	cout << "Standard Error    : " << result.standardError << endl;
	cout << "95% CI            : ["
		<< result.ciLower << ", "
		<< result.ciUpper << "]" << endl;
	cout << "************************************************************" << endl;
	cout << endl;

	cout << "*********************** Asian Call *************************" << endl;
	AsianOption call_asian(105, 1, 1, 4);
	BlackAsian bs_asian(rate, spot, vol);
	cout << "Analytical Price  : " << bs_asian.price(&call_asian) << endl;
	result = mc_path_dep.price(&bs_asian, &call_asian);
	cout << "Monte Carlo Price : " << result.price << endl;
	cout << "Standard Error    : " << result.standardError << endl;
	cout << "95% CI            : ["
		<< result.ciLower << ", "
		<< result.ciUpper << "]" << endl;
	cout << "************************************************************" << endl;
	cout << endl;
	cout << "*********************** Asian Put **************************" << endl;
	AsianOption put_asian(95, 1, -1, 4);
	cout << "Analytical Price  : " << bs_asian.price(&put_asian) << endl;
	result = mc_path_dep.price(&bs_asian, &put_asian);
	cout << "Monte Carlo Price : " << result.price << endl;
	cout << "Standard Error    : " << result.standardError << endl;
	cout << "95% CI            : ["
		<< result.ciLower << ", "
		<< result.ciUpper << "]" << endl;
	cout << "************************************************************" << endl;
	cout << endl;

	size_t size = 3;
	vector<double> spots = { 100, 105 , 95};
	vector<double> vols = {0.35, 0.3, 0.4};
	vector<vector<double>> corr_matrix = { {1, -0.6, 0.3}, {-0.6, 1, -0.2}, {0.3, -0.2, 1} };

	cout << "*********************** Basket Call ************************" << endl;
	BasketOption call_basket(100, 1, 1, size);
	BlackBasket bs_basket(rate, spots, vols, corr_matrix);
	cout << "Analytical Price : " << bs_basket.price(&call_basket) << endl;
	result = mc.price(&bs_basket, &call_basket);
	cout << "Monte Carlo Price : " << result.price << endl;
	cout << "Standard Error    : " << result.standardError << endl;
	cout << "95% CI            : ["
		<< result.ciLower << ", "
		<< result.ciUpper << "]" << endl;
	cout << "************************************************************" << endl;
	cout << endl;
	cout << "*********************** Basket Put *************************" << endl;
	BasketOption put_basket(100, 1, -1, size);
	cout << "Analytical Price : " << bs_basket.price(&put_basket) << endl;
	result = mc.price(&bs_basket, &put_basket);
	cout << "Monte Carlo Price : " << result.price << endl;
	cout << "Standard Error    : " << result.standardError << endl;
	cout << "95% CI            : ["
		<< result.ciLower << ", "
		<< result.ciUpper << "]" << endl;
	cout << "************************************************************" << endl;
	cout << endl;

	spots = { 105, 95};
	vols = { 0.4, 0.3 };
	corr_matrix = { {1, 0.3}, {0.3, 1} };

	cout << "*********************** Spread Call **************************" << endl;
	SpreadOption call_spread(15, 1, 1);
	BlackSpread bs_spread(rate, spots, vols, corr_matrix);
	cout << "Analytical Price : " << bs_spread.price(&call_spread) << endl;
	result = mc.price(&bs_spread, &call_spread);
	cout << "Monte Carlo Price : " << result.price << endl;
	cout << "Standard Error    : " << result.standardError << endl;
	cout << "95% CI            : ["
		<< result.ciLower << ", "
		<< result.ciUpper << "]" << endl;
	cout << "**************************************************************" << endl;
	cout << endl;
	cout << "*********************** Spread Put ***************************" << endl;
	SpreadOption put_spread(5, 1, -1);
	cout << "Analytical Price : " << bs_spread.price(&put_spread) << endl;
	result = mc.price(&bs_spread, &put_spread);
	cout << "Monte Carlo Price : " << result.price << endl;
	cout << "Standard Error    : " << result.standardError << endl;
	cout << "95% CI            : ["
		<< result.ciLower << ", "
		<< result.ciUpper << "]" << endl;
	cout << "**************************************************************" << endl;
	cout << endl;

	return 0;
}