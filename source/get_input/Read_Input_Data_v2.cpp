#include <MyHeaders.h>

// http://stackoverflow.com/questions/7677007/passing-vector-to-function-c
// Pass vector by reference as input needs not be changed
void Read_Input_Data_v2(
		string filename,
		const vector< string >& Species,
		InitParam& SetupParam,
		vector< InitSpecies >& SetupSpecies
)
{

	// Now we need to deal with the structure of the input file provided by the user
	// It is safe to assume that all entries will be separated by a space or tab
	// therefore we should be able to just tokenize the input and read it in

	ifstream Input_Data;
	Input_Data.open (filename.c_str());

	string line1;

	int Number_Fixed_Positions = 11;

	vector< vector< double > > temp_input(Number_Fixed_Positions); // 11 fixed positions, plus extra entries via push_back for all species
	int i;

	vector< double > LineIn(2); // 2 parameters is most I expect
	// initialize the storage array
	for(i=0;i<Number_Fixed_Positions;i++)
	{
		temp_input[i] = LineIn;
	}
	LineIn.clear();

	int Number_Species = Species.size();
	bool OxyGasSpeciesDefined = false;

	if (Input_Data.is_open())
	{

		// Set defaults

		SetupParam.irrev = false; // shoud be bool
		SetupParam.PrintReacRates = false;
		SetupParam.nrspec = 0;
		SetupParam.rtol = 1.e-5;
		SetupParam.atol = 1.e-25;
		SetupParam.threshold = 1.e-10;
		SetupParam.temperature = 500;
		SetupParam.hm = 1.e-12;
		SetupParam.h = 1.e-7;
		SetupParam.MechanismAnalysis.MaximumRates = false;
		SetupParam.MechanismAnalysis.StreamRatesAnalysis = false;
		SetupParam.MechanismAnalysis.RatesSpeciesAllAnalysis = false;
		SetupParam.MechanismAnalysis.RatesAnalysisAtTime = false;
		SetupParam.MechanismAnalysis.RatesAnalysisAtTimeData.resize(0);
		SetupParam.ReduceReactions = 0;
		SetupParam.UseStiffSolver = true;
		SetupParam.EnforceStability = false;

		// Mapping setup
		SetupParam.UseNewLumping = false;
		SetupParam.UseFastLumping = true; // old method wasn't wrong but inefficient and slow

		/* Code extension for Gas Phase */
		SetupParam.GasPhase = false; // default not gas phase
		SetupParam.GasPhaseVolume = 0; // 1 L = 0.001 m^3
		SetupParam.GasPhasePressure = 0; // Pa, normal pressure

		/* Modification to deal with PetroOxy in main Input file */

		/* Data is:
		 * 0) Sample Size <- m^3
		 * p - 1) Initial Pressure
		 * T - 2) Target Temperature
		 * pmax - 3) Maximum Pressure
		 * 4) O2 space in PetroOxy <- m^3
		 * 5) Gas Species
		 * 6) mol of gas species at 25 degree celsius
		 * 7) O2 derived pressure
		 * 8) Vapour Pressure solvent component
		 * 9) solubility of gas at 298K, mol/L (C)
		 * 10) "k" as Henry's Law Constant, k = P_gas/C //  Pa mol / L
		 */

		SetupParam.PressureVessel.IsSet = false;
		SetupParam.PressureVessel.SampleSize = 0;
		SetupParam.PressureVessel.InitPressure = 0;
		SetupParam.PressureVessel.MaxPressure = 0;
		SetupParam.PressureVessel.GasSpecies = 0;
		SetupParam.PressureVessel.GasSolubility = 0;
		SetupParam.PressureVessel.TemperatureRise = 0;

		SetupParam.PressureVessel.HenryLawDiffusionLimitSet = false;
		SetupParam.PressureVessel.HenryLawDiffusionLimit = 0;


		SetupParam.StoichiometryMatrixForOpt = false;


		SetupParam.Jacobian = false;

		SetupParam.ConstantConcentration = false;

		int i;
		char * cstr, *p;
		string str;
		size_t found;


		// while the file is open
		while (Input_Data.good())
		{
			getline (Input_Data,line1); // Output Control
			//printf("Line In: %s \n\n",line1.c_str());

			if(line1.compare(0,1,"!") != 0 && !line1.empty()) // line is not a comment and not empty. Whitespaces on lines may be an issue
			{

				// dispose of any comments that may be in the file after entries
				str = line1;//a String - For some reason cannot pass line1 directly...
				cstr = new char [str.size()+1];
				strcpy (cstr, str.c_str());

				vector< string > RemoveComments;

				found = line1.find("!");
				if (found!=string::npos)
				{
					p=strtok (cstr,"!");
					RemoveComments.push_back(p);
					line1 = RemoveComments[0];
					delete[] cstr;
					RemoveComments.clear();
					// make the program believe it has read in the string directly,
					// i.e rest line1 to the new string without the comments
					str = line1;//a String - For some reason cannot pass line1 directly...
					cstr = new char [str.size()+1];
					strcpy (cstr, str.c_str());
				}


				// Search for the keyword and Store the input

				found = line1.find("Jacobian");
				if (found!=string::npos)
				{
					SetupParam.Jacobian = true;
					line1.clear();
					delete[] cstr;
				}

				found = line1.find("IRREV");
				if (found!=string::npos)
				{
					SetupParam.irrev = true;
					line1.clear();
					delete[] cstr;
					//printf("Test - Scheme will be made irreversible. \n");
				}

				found = line1.find("PrintReac");
				if (found!=string::npos)
				{
					SetupParam.PrintReacRates = true;
					line1.clear();
					delete[] cstr;
				}

				found = line1.find("Print Rates");
				if (found!=string::npos)
				{
					SetupParam.PrintReacRates = true;
					line1.clear();
					delete[] cstr;
				}

				found = line1.find("Force Stability");
				if (found!=string::npos)
				{
					SetupParam.EnforceStability = true;
					line1.clear();
					delete[] cstr;
				}


				found = line1.find("Use New Lumping");
				if (found!=string::npos)
				{
					SetupParam.UseNewLumping = true;
					line1.clear();
					delete[] cstr;
				}

				found = line1.find("Use Slow New Lumping");
				if (found!=string::npos)
				{
					SetupParam.UseNewLumping = true;
					SetupParam.UseFastLumping = false;
					line1.clear();
					delete[] cstr;
				}

				found = line1.find("Use Slow Lumping");
				if (found!=string::npos)
				{
					SetupParam.UseFastLumping = false;
					line1.clear();
					delete[] cstr;
				}

				found = line1.find("Stoichiometry Matrix For Opt");
				if (found!=string::npos)
				{
					SetupParam.StoichiometryMatrixForOpt = true;
					line1.clear();
					delete[] cstr;
				}

				found = line1.find("RatesMaxAnalysis");
				if (found!=string::npos)
				{
					SetupParam.MechanismAnalysis.MaximumRates = true;
					line1.clear();
					delete[] cstr;
				}

				found = line1.find("StreamRatesAnalysis");
				if (found!=string::npos)
				{
					SetupParam.MechanismAnalysis.StreamRatesAnalysis = true;
					line1.clear();
					delete[] cstr;
				}

				found = line1.find("RatesSpeciesAllAnalysis");
				if (found!=string::npos)
				{
					SetupParam.MechanismAnalysis.RatesSpeciesAllAnalysis = true;
					line1.clear();
					delete[] cstr;
				}

				found = line1.find("Use General Solver");
				if (found!=string::npos)
				{
					SetupParam.UseStiffSolver = false;
					line1.clear();
					delete[] cstr;
				}

				found = line1.find("ReduceReactions");
				if (found!=string::npos)
				{
					LineIn.clear(); // make sure storage array is empty
					LineIn.push_back(1); // default value, 1K

					p=strtok (cstr," \t"); // break at space or tab
					p=strtok(NULL," \t"); // break again as first is the keyword

					if(p!=NULL){ // only read remainder is something is left
						LineIn[0] = strtod(p,NULL);
						p=strtok(NULL," \t");
					}
					line1.clear();
					delete[] cstr;
					SetupParam.ReduceReactions = LineIn[0];
				}


				found = line1.find("Temperature");
				if (found!=string::npos)
				{

					//make sure it isn't PetroOxy Temperature Rise...
					found = line1.find("PetroOxy");
					if (found==string::npos)
					{
						LineIn.clear(); // make sure storage array is empty
						LineIn.push_back(1); // default value, 1K

						p=strtok (cstr," \t"); // break at space or tab
						p=strtok(NULL," \t"); // break again as first is the keyword

						if(p!=NULL){ // only read remainder is something is left
							LineIn[0] = strtod(p,NULL);
							p=strtok(NULL," \t");
						}
						line1.clear();
						delete[] cstr;
						SetupParam.temperature = LineIn[0];
					}
				}


				found = line1.find("RatesAnalysisAtTime");
				if (found!=string::npos)
				{
					SetupParam.MechanismAnalysis.RatesAnalysisAtTime = true; // user wants rates at specified times

					LineIn.clear(); // make sure storage array is empty
					LineIn.push_back(1); // default value, 1K

					p=strtok (cstr," \t"); // break at space or tab
					p=strtok(NULL," \t"); // break again as first is the keyword

					while(p!=NULL){ // only read remainder is something is left
						SetupParam.MechanismAnalysis.RatesAnalysisAtTimeData.push_back(strtod(p,NULL));
						p=strtok(NULL," \t");
					}
					line1.clear();
					delete[] cstr;
					//SetupParam.RatesAnalysisAtTimeData = LineIn;
				}


				found = line1.find("hm");
				if (found!=string::npos)
				{
					LineIn.clear(); // make sure storage array is empty
					LineIn.push_back(1); // default value, 1K

					p=strtok (cstr," \t"); // break at space or tab
					p=strtok(NULL," \t"); // break again as first is the keyword

					if(p!=NULL){ // only read remainder is something is left
						LineIn[0] = strtod(p,NULL);
						p=strtok(NULL," \t");
					}
					line1.clear();
					delete[] cstr;
					SetupParam.hm = LineIn[0];
				}

				found = line1.find("initialh");
				if (found!=string::npos)
				{
					LineIn.clear(); // make sure storage array is empty
					LineIn.push_back(1); // default value, 1K

					p=strtok (cstr," \t"); // break at space or tab
					p=strtok(NULL," \t"); // break again as first is the keyword

					if(p!=NULL){ // only read remainder is something is left
						LineIn[0] = strtod(p,NULL);
						p=strtok(NULL," \t");
					}
					line1.clear();
					delete[] cstr;
					SetupParam.h = LineIn[0];
				}



				found = line1.find("Tolerance");
				if (found!=string::npos)
				{
					LineIn.clear(); // make sure storage array is empty

					p=strtok (cstr," \t"); // break at space or tab
					p=strtok(NULL," \t"); // break again as first is the keyword

					while(p!=NULL){ // only read remainder is something is left
						LineIn.push_back(strtod(p,NULL));
						p=strtok(NULL," \t");
					}
					line1.clear();
					delete[] cstr;
					SetupParam.rtol = LineIn[0];
					SetupParam.threshold = LineIn[1];
				}

				found = line1.find("Threshold");
				if (found!=string::npos)
				{
					LineIn.clear(); // make sure storage array is empty

					p=strtok (cstr," \t"); // break at space or tab
					p=strtok(NULL," \t"); // break again as first is the keyword

					while(p!=NULL){ // only read remainder is something is left
						LineIn.push_back(strtod(p,NULL));
						p=strtok(NULL," \t");
					}
					line1.clear();
					delete[] cstr;
					SetupParam.threshold = LineIn[0];
				}

				found = line1.find("RTOL");
				if (found!=string::npos)
				{
					LineIn.clear(); // make sure storage array is empty

					p=strtok (cstr," \t"); // break at space or tab
					p=strtok(NULL," \t"); // break again as first is the keyword

					while(p!=NULL){ // only read remainder is something is left
						LineIn.push_back(strtod(p,NULL));
						p=strtok(NULL," \t");
					}
					line1.clear();
					delete[] cstr;
					SetupParam.rtol = LineIn[0];
				}


				found = line1.find("EndTime");
				if (found!=string::npos)
				{
					LineIn.clear(); // make sure storage array is empty

					p=strtok (cstr," \t"); // break at space or tab
					p=strtok(NULL," \t"); // break again as first is the keyword

					while(p!=NULL){ // only read remainder is something is left
						LineIn.push_back(strtod(p,NULL));
						p=strtok(NULL," \t");
					}
					line1.clear();
					delete[] cstr;

					// allows users to provide multiple time points
					SetupParam.TimeEnd.push_back(LineIn[0]);
					SetupParam.TimeStep.push_back(LineIn[1]);
				}


				//*
				// This will work, but I don't think it is the best implementation
				found = line1.find("ConstantConcentration");
				if (found!=string::npos)
				{
					p=strtok (cstr," \t"); // break at space or tab
					p=strtok(NULL," \t"); // break again as first is the keyword

					//cout << "does this work?\n";
					for(i=0;i<Number_Species;i++){
						if(strcmp(p,Species[i].c_str()) == 0)
						{
							// allows users to provide multiple time points
							SetupParam.ConstantSpecies.push_back(i);
							//cout << "Just checking " << i << "\n";
						}
					}
					SetupParam.ConstantConcentration = true;
					line1.clear();
					delete[] cstr;
				}
				//*/


				/*
				 * Gas Phase Code Extension
				 */

				found = line1.find("GasPhasePressure"); // We Assume kPa
				if (found!=string::npos)
				{
					LineIn.clear(); // make sure storage array is empty

					p=strtok (cstr," \t"); // break at space or tab
					p=strtok(NULL," \t"); // break again as first is the keyword

					while(p!=NULL){ // only read remainder is something is left
						LineIn.push_back(strtod(p,NULL));
						p=strtok(NULL," \t");
					}
					line1.clear();
					delete[] cstr;
					SetupParam.GasPhasePressure = LineIn[0]*1000;
					//SetupParam.GasPhase = true; // activate Gas Phase Correction for User
				}

				found = line1.find("GasPhaseVolume"); // We Assume Litres
				if (found!=string::npos)
				{
					LineIn.clear(); // make sure storage array is empty

					p=strtok (cstr," \t"); // break at space or tab
					p=strtok(NULL," \t"); // break again as first is the keyword

					while(p!=NULL){ // only read remainder is something is left
						LineIn.push_back(strtod(p,NULL));
						p=strtok(NULL," \t");
					}
					line1.clear();
					delete[] cstr;
					SetupParam.GasPhaseVolume = LineIn[0]/1000; // Convert to m^3
					//SetupParam.GasPhase = true; // activate Gas Phase Correction for User
				}


				// if not in use, value will be zero - if the user has entered both, activate
				if(SetupParam.GasPhasePressure != 0 && SetupParam.GasPhaseVolume != 0){
					SetupParam.GasPhase = true; // activate Gas Phase Correction for User
				}
				/*
				 * End Gas Phase Code Extension
				 */




				/*
				 * PetroOxy Data Input
				 */

				vector<string> Readout;

				if(string::npos!=line1.find("PetroOxy Solvent Sample="))
				{
					char * cstr, *p;
					string str = line1;
					cstr = new char [str.size()+1];
					strcpy (cstr, str.c_str());

					p=strtok (cstr,"=");
					while (p!=NULL)
					{
						Readout.push_back(p);
						p=strtok(NULL,"<>");
					}
					delete[] cstr;
					delete[] p;

					// it is mL -> make into m^3
					SetupParam.PressureVessel.SampleSize = strtod(Readout[1].c_str(),NULL)*1e-6;
					//PetroOxyData[0] = strtod(Readout[1].c_str(),NULL)*1e-6;
					Readout.clear();

					// Calculated volume from estimate is 22.41mL - so let us assume 22.5mL, value is in m^3
					//PetroOxyData[4] = 22.5*1e-6 - PetroOxyData[0];
				}


				if(string::npos!=line1.find("PetroOxy Initial Pressure="))
				{
					char * cstr, *p;
					string str = line1;
					cstr = new char [str.size()+1];
					strcpy (cstr, str.c_str());

					p=strtok (cstr,"=");
					while (p!=NULL)
					{
						Readout.push_back(p);
						p=strtok(NULL,"<>");
					}
					delete[] cstr;
					delete[] p;

					// kPa to Pa
					SetupParam.PressureVessel.InitPressure = strtod(Readout[1].c_str(),NULL) * 1000;
					//PetroOxyData[1] = strtod(Readout[1].c_str(),NULL) * 1000;
					Readout.clear();
				}

				if(string::npos!=line1.find("PetroOxy Maximum Pressure="))
				{
					char * cstr, *p;
					string str = line1;
					cstr = new char [str.size()+1];
					strcpy (cstr, str.c_str());

					p=strtok (cstr,"=");
					while (p!=NULL)
					{
						Readout.push_back(p);
						p=strtok(NULL,"<>");
					}
					delete[] cstr;
					delete[] p;

					// kPa to Pa
					SetupParam.PressureVessel.MaxPressure = strtod(Readout[1].c_str(),NULL) * 1000;
					//PetroOxyData[3] = strtod(Readout[1].c_str(),NULL) * 1000;
					Readout.clear();
				}

				if(string::npos!=line1.find("PetroOxy Gas Solubility="))
				{
					char * cstr, *p;
					string str = line1;
					cstr = new char [str.size()+1];
					strcpy (cstr, str.c_str());

					p=strtok (cstr,"=");
					while (p!=NULL)
					{
						Readout.push_back(p);
						p=strtok(NULL,"<>");
					}
					delete[] cstr;
					delete[] p;

					// kPa to Pa
					SetupParam.PressureVessel.GasSolubility = strtod(Readout[1].c_str(),NULL);// * 1000;
					//PetroOxyData[9] = strtod(Readout[1].c_str(),NULL);// * 1000;
					Readout.clear();
				}


				if(string::npos!=line1.find("PetroOxy Gas Species="))
				{
					char * cstr, *p;
					string str = line1;
					cstr = new char [str.size()+1];
					strcpy (cstr, str.c_str());

					p=strtok (cstr,"=");
					while (p!=NULL)
					{
						Readout.push_back(p);
						p=strtok(NULL,"<>");
					}
					delete[] cstr;
					delete[] p;

					for(int i = 0;i<(int)Species.size();i++)
					{
						if(strcmp(Readout[1].c_str(),Species[i].c_str()) == 0){
							SetupParam.PressureVessel.GasSpecies = i;
							OxyGasSpeciesDefined = true;
						}

					}

					Readout.clear();
				}

				if(string::npos!=line1.find("PetroOxy Temperature Rise="))
				{
					char * cstr, *p;
					string str = line1;
					cstr = new char [str.size()+1];
					strcpy (cstr, str.c_str());

					p=strtok (cstr,"=");
					while (p!=NULL)
					{
						Readout.push_back(p);
						p=strtok(NULL,"<>");
					}
					delete[] cstr;
					delete[] p;

					// kPa to Pa
					SetupParam.PressureVessel.TemperatureRise = strtod(Readout[1].c_str(),NULL) * 1000;
					//PetroOxyData[3] = strtod(Readout[1].c_str(),NULL) * 1000;
					Readout.clear();
				}


				// if all required data has been input, switch PetroOxy on
				// Need the Gas Species ID... - but gas species could be scpecies 0....


				if(SetupParam.PressureVessel.SampleSize != 0 &&
						SetupParam.PressureVessel.InitPressure != 0 &&
						SetupParam.PressureVessel.MaxPressure != 0 &&
						OxyGasSpeciesDefined == true &&
						SetupParam.PressureVessel.GasSolubility != 0
				){
					SetupParam.PressureVessel.IsSet = true;
				}




				if(string::npos!=line1.find("Henry Law Diffusion Limit="))
				{
					char * cstr, *p;
					string str = line1;
					cstr = new char [str.size()+1];
					strcpy (cstr, str.c_str());

					p=strtok (cstr,"=");
					while (p!=NULL)
					{
						Readout.push_back(p);
						p=strtok(NULL,"<>");
					}
					delete[] cstr;
					delete[] p;

					SetupParam.PressureVessel.HenryLawDiffusionLimit = strtod(Readout[1].c_str(),NULL);
					SetupParam.PressureVessel.HenryLawDiffusionLimitSet = true;
					Readout.clear();

				}



				/*
				 * End PetroOxy Data Input
				 */





				InitSpecies SingleSpeciesInput;

				// last option... the line contains a species...

				// check the string still contains data
				if(!line1.empty()){
					//LineIn.clear();
					//LineIn.resize(2);
					p=strtok (cstr," \t"); // p contains the species name

					for(i=0;i<Number_Species;i++){
						string Comparator = Species[i];
						if(strcmp(p,Comparator.c_str()) == 0)
						{
							SingleSpeciesInput.SpecID = i;
							// Obviously assigning only makes sense if I have a species name
							p=strtok(NULL," \t"); // p contains the concentration
							SingleSpeciesInput.SpecConc = strtod(p,NULL);
							SetupSpecies.push_back(SingleSpeciesInput);
						}
					}

					delete[] cstr;
					line1.clear();
				}
			}
		}
	}

	Input_Data.close();



	// Check that special requirements are met
	if(SetupParam.MechanismAnalysis.MaximumRates && !SetupParam.irrev)
	{
		SetupParam.irrev = true;
		cout << "Rates Analysis requires an irreversible scheme - method set. \n";
	}

	if(SetupParam.MechanismAnalysis.RatesSpeciesAllAnalysis)
	{
		SetupParam.irrev = true;
		cout << "Rates Analysis requires an irreversible scheme - method set. \n";
	}

	if(SetupParam.MechanismAnalysis.StreamRatesAnalysis && !SetupParam.irrev)
	{
		SetupParam.irrev = true;
		cout << "Rates Analysis requires an irreversible scheme - method set. \n";
	}

	if(SetupParam.MechanismAnalysis.RatesAnalysisAtTime && !SetupParam.irrev)
	{
		SetupParam.irrev = true;
		cout << "Rates Analysis At Times requires an irreversible scheme - method set. \n";
	}

	if(SetupParam.ReduceReactions != 0 && !SetupParam.irrev)
	{
		SetupParam.irrev = true;
		cout << "Rates Based Mechanism Reduction requires an irreversible scheme - method set. \n";
	}
}
