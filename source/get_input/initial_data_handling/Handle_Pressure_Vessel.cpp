/*
 * Handle_PressureVessel.cpp
 *
 *  Created on: 19 Mar 2016
 *      Author: DetlevCM
 */

#include <MyHeaders.h>


/*
 * Input for the case of a pressure vessel
 */


void Handle_Pressure_Vessel(InitParam& InitialParameters, vector<string> Input, vector< string > Species)
{
	bool OxyGasSpeciesDefined = false;

	int i;
	for(i=0;i<(int)Input.size();i++)
	{
		vector< string > line_content;

		if(Test_If_Word_Found(Input[i],"Sample Size="))
		{
			line_content = Tokenise_String_To_String(Input[i],"=");

			// it is mL -> make into m^3
			InitialParameters.PressureVessel.SampleSize = strtod(line_content[1].c_str(),NULL)*1e-6;
			cout << "Sample Size: " << InitialParameters.PressureVessel.SampleSize << "\n";
			line_content.clear();
		}

		if(Test_If_Word_Found(Input[i],"Vessel Size="))
		{
			line_content = Tokenise_String_To_String(Input[i],"=");

			// it is mL -> make into m^3
			InitialParameters.PressureVessel.VesselSize = strtod(line_content[1].c_str(),NULL)*1e-6;
			cout << "Vessel Size: " << InitialParameters.PressureVessel.VesselSize << "\n";
			line_content.clear();
			// Calculated volume from estimate is 22.41mL - so let us assume 22.5mL, value is in m^3
		}

		if(Test_If_Word_Found(Input[i],"Initial Pressure="))
		{
			line_content = Tokenise_String_To_String(Input[i],"=");

			// kPa to Pa
			InitialParameters.PressureVessel.InitPressure = strtod(line_content[1].c_str(),NULL) * 1000;
			cout << "Initial Pressure: " << InitialParameters.PressureVessel.InitPressure << "\n";
			line_content.clear();
		}

		if(Test_If_Word_Found(Input[i],"Maximum Pressure="))
		{
			line_content = Tokenise_String_To_String(Input[i],"=");

			// kPa to Pa
			InitialParameters.PressureVessel.MaxPressure = strtod(line_content[1].c_str(),NULL) * 1000;
			cout << "Maximum Pressure: " << InitialParameters.PressureVessel.MaxPressure << "\n";
			line_content.clear();
		}

		if(Test_If_Word_Found(Input[i],"Gas Solubility="))
		{
			line_content = Tokenise_String_To_String(Input[i],"=");

			// kPa to Pa
			InitialParameters.PressureVessel.GasSolubility = strtod(line_content[1].c_str(),NULL);// * 1000;
			cout << "Gas Solubility: " << InitialParameters.PressureVessel.GasSolubility << "\n";
			line_content.clear();
		}


		if(Test_If_Word_Found(Input[i],"Gas Species="))
		{
			line_content = Tokenise_String_To_String(Input[i],"=");

			for(int i = 0;i<(int)Species.size();i++)
			{
				if(strcmp(line_content[1].c_str(),Species[i].c_str()) == 0){
					InitialParameters.PressureVessel.GasSpecies = i;
					OxyGasSpeciesDefined = true;
				}

			}

			cout << "Gas Species: " << Species[InitialParameters.PressureVessel.GasSpecies] << "\n";
			line_content.clear();
		}

		if(Test_If_Word_Found(Input[i],"PetroOxy Temperature Rise="))
		{
			line_content = Tokenise_String_To_String(Input[i],"=");

			// kPa to Pa
			InitialParameters.PressureVessel.TemperatureRise = strtod(line_content[1].c_str(),NULL) * 1000;
			cout << "Temperature Rise: " << InitialParameters.PressureVessel.TemperatureRise << "\n";
			line_content.clear();
		}


		// if all required data has been input, switch PetroOxy on
		// Need the Gas Species ID... - but gas species could be scpecies 0....


		if(InitialParameters.PressureVessel.SampleSize != 0 &&
				InitialParameters.PressureVessel.InitPressure != 0 &&
				InitialParameters.PressureVessel.MaxPressure != 0 &&
				OxyGasSpeciesDefined == true &&
				InitialParameters.PressureVessel.GasSolubility != 0
		){
			InitialParameters.PressureVessel.IsSet = true;
		}




		if(Test_If_Word_Found(Input[i],"Henry Law Diffusion Limit="))
		{
			line_content = Tokenise_String_To_String(Input[i],"=");

			InitialParameters.PressureVessel.HenryLawDiffusionLimit = strtod(line_content[1].c_str(),NULL);
			InitialParameters.PressureVessel.HenryLawDiffusionLimitSet = true;
			line_content.clear();
		}
	}

}

