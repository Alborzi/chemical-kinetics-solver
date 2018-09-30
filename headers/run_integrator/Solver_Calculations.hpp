/*
 * Calculation_Functions.h
 *
 *  Created on: 20 Dec 2012
 *      Author: pmdcm
 */

#ifndef CALCULATION_FUNCTIONS_
#define CALCULATION_FUNCTIONS_



// The ODE RHS functions are split - regular initial value & pressure vessel (reservoir)
void ODE_RHS_Liquid_Phase(int*, double*, double*, double*);
void ODE_RHS_Gas_Phase(int*, double*, double*, double*);
void ODE_RHS_Gas_Liquid_Phase(int*, double*, double*, double*);
void ODE_RHS_Pressure_Vessel(int*, double*, double*, double*);






/*
 * Calculating Rate Constants
 */

void Calculate_Rate_Constant(
		vector< double >& ,
		vector< double >& ,
		double ,
		const vector< ReactionParameter >& ,
		//const vector< vector< double > >& ,
		const vector< CalculatedThermodynamics >& ,
		const vector< TrackSpecies >& ,
		const vector< double >&
);


//vector< double >
void CalculateReactionRates(
		vector< double >& ,
		const vector< double >& ,
		vector< double > ,
		vector< double > ,
		const vector< TrackSpecies >& ,
		const vector< TrackSpecies >&
);



void Evaluate_Thermodynamic_Parameters(
		vector<CalculatedThermodynamics> & ,
		const vector<ThermodynamicData> & ,
		double
);


vector< double > SpeciesLossRate(
		int,
		const vector< double > &,
		const vector< TrackSpecies > &
);


#endif /* CALCULATION_FUNCTIONS_ */