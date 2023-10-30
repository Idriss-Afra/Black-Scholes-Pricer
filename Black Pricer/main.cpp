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
	MonteCarlo mc_path_dep(30000, 10); // Path-Dependent MC : Number of Simulation = 30 000 & Number of Time Steps = 10.
	
	cout << "*********************** Vanilla Call ***********************" << endl;
	Option* call_vanilla = new VanillaOption(105, 1, -1); 
	BlackVanilla* bs_vanilla = new BlackVanilla(rate, spot, vol);
	cout << "Monte Carlo Price : " << mc.price(bs_vanilla, call_vanilla) << endl;
	cout << "Analytical Price : " << bs_vanilla->price(call_vanilla) << endl;
	cout << "************************************************************" << endl;
	cout << endl;
	cout << "*********************** Vanilla Put ************************" << endl;
	Option* put_vanilla = new VanillaOption(95, 1, -1);
	cout << "Monte Carlo Price : " << mc.price(bs_vanilla, put_vanilla) << endl;
	cout << "Analytical Price : " << bs_vanilla->price(put_vanilla) << endl;
	cout << "************************************************************" << endl;
	cout << endl;

	cout << "*********************** Digital Call ***********************" << endl;
	Option* call_digital = new DigitalOption(105, 1, 1);
	BlackDigital* bs_digital = new BlackDigital(rate, spot, vol);
	cout << "Monte Carlo Price : " << mc.price(bs_digital, call_digital) << endl;
	cout << "Analytical Price : " << bs_digital->price(call_digital) << endl;
	cout << "************************************************************" << endl;
	cout << endl;
	cout << "*********************** Digital Put ************************" << endl;
	Option* put_digital = new DigitalOption(95, 1, -1);
	cout << "Monte Carlo Price : " << mc.price(bs_digital, put_digital) << endl;
	cout << "Analytical Price : " << bs_digital->price(put_digital) << endl;
	cout << "************************************************************" << endl;
	cout << endl;

	cout << "*********************** UP & OUT Call **********************" << endl;
	Option* call_upout = new BarrierOption(105, 145, 1, 1, "Up Out");
	BlackBarrier* bs_barrier = new BlackBarrier(rate, spot, vol);
	cout << "Monte Carlo Price : " << mc.price(bs_barrier, call_upout) << endl;
	cout << "Analytical Price : " << bs_barrier->price(call_upout) << endl;
	cout << "************************************************************" << endl;
	cout << endl;
	cout << "*********************** UP & IN Call ***********************" << endl;
	Option* call_upin = new BarrierOption(105, 145, 1, 1, "Up In");
	cout << "Monte Carlo Price : " << mc.price(bs_barrier, call_upin) << endl;
	cout << "Analytical Price : " << bs_barrier->price(call_upin) << endl;
	cout << "************************************************************" << endl;
	cout << endl;
	cout << "*********************** DONW & OUT Put *********************" << endl;
	Option* put_downout = new BarrierOption(105, 65, 1, -1, "Down Out");
	cout << "Monte Carlo Price : " << mc.price(bs_barrier, put_downout) << endl;
	cout << "Analytical Price : " << bs_barrier->price(put_downout) << endl;
	cout << "************************************************************" << endl;
	cout << endl;
	cout << "*********************** DOWN & IN Put **********************" << endl;
	Option* put_downin = new BarrierOption(105, 65, 1, -1, "Down In");
	cout << "Monte Carlo Price : " << mc.price(bs_barrier, put_downin) << endl;
	cout << "Analytical Price : " << bs_barrier->price(put_downin) << endl;
	cout << "************************************************************" << endl;
	cout << endl;

	cout << "*********************** Asian Call *************************" << endl;
	Option* call_asian = new AsianOption(105, 1, 1, 4);
	BlackAsian* bs_asian = new BlackAsian(rate, spot, vol);
	cout << "Monte Carlo Price : " << mc_path_dep.price(bs_asian, call_asian) << endl;
	cout << "Analytical Price : " << bs_asian->price(call_asian) << endl;
	cout << "************************************************************" << endl;
	cout << endl;
	cout << "*********************** Asian Put **************************" << endl;
	Option* put_asian = new AsianOption(95, 1, -1, 4);
	cout << "Monte Carlo Price : " << mc_path_dep.price(bs_asian, put_asian) << endl;
	cout << "Analytical Price : " << bs_asian->price(put_asian) << endl;
	cout << "************************************************************" << endl;
	cout << endl;

	double size = 3;
	vector<double> spots = { 100, 105 , 95};
	vector<double> vols = {0.35, 0.3, 0.4};
	vector<vector<double>> corr_matrix = { {1, -0.6, 0.3}, {-0.6, 1, -0.2}, {0.3, -0.2, 1} };

	cout << "*********************** Basket Call ************************" << endl;
	Option* call_basket = new BasketOption(100, 1, 1, size);
	MultiAssetBSModel* bs_basket = new BlackBasket(rate, size, spots, vols, corr_matrix);
	cout << "Monte Carlo Price : " << mc.price(bs_basket, call_basket) << endl;
	cout << "Analytical Price : " << bs_basket->price(call_basket) << endl;
	cout << "************************************************************" << endl;
	cout << endl;
	cout << "*********************** Basket Put *************************" << endl;
	Option* put_basket = new BasketOption(100, 1, -1, size);
	cout << "Monte Carlo Price : " << mc.price(bs_basket, put_basket) << endl;
	cout << "Analytical Price : " << bs_basket->price(put_basket) << endl;
	cout << "************************************************************" << endl;
	cout << endl;

	spots = { 105, 95};
	vols = { 0.4, 0.3 };
	corr_matrix = { {1, 0.3}, {0.3, 1} };

	cout << "*********************** Spread Call **************************" << endl;
	Option* call_spread = new SpreadOption(15, 1, 1);
	MultiAssetBSModel* bs_spread = new BlackSpread(rate, spots, vols, corr_matrix);
	cout << "Monte Carlo Price : " << mc.price(bs_spread, call_spread) << endl;
	cout << "Analytical Price : " << bs_spread->price(call_spread) << endl;
	cout << "**************************************************************" << endl;
	cout << endl;
	cout << "*********************** Spread Put ***************************" << endl;
	Option* put_spread = new SpreadOption(5, 1, -1);
	cout << "Monte Carlo Price : " << mc.price(bs_spread, put_spread) << endl;
	cout << "Analytical Price : " << bs_spread->price(put_spread) << endl;
	cout << "**************************************************************" << endl;
	cout << endl;

	return 0;
}