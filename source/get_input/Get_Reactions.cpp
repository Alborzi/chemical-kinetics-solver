#include <MyHeaders.h>

/* The Function in this file deals with obtaining the reactions mechanism
 * data from the text input in a format that may be processed by the
 * program. The reactants, products and reaction parameters are read into
 * a number of 2D arrays (laid out as reaction ID vs species ID which are
 * output as a 3D array, structured as follows:
 * 1: reactant coefficients
 * 2: product coefficients
 * 3: reaction parameters
 * If extras functionality is to be added, this will make use of the third layer.
 * One could add a fourth layer, though it is questionable whether this is at
 * this point. The layout will scale if extra layers are desired.  */

// Some of the repetitive code in here should be stuck into a function... maybe limited just to the scope of this file
// The species search/comparison should be global

// http://stackoverflow.com/questions/7677007/passing-vector-to-function-c
// Pass vector by reference as input needs not be changed



struct SpeciesWithCoefficient
{
	int SpeciesID;
	double coefficient;
	bool ismatched;// = false;
};

SpeciesWithCoefficient Return_Species_With_Coefficient(string  , const vector< string > );


vector< SingleReactionData > Get_Reactions(
		string filename,
		const vector< string >& Species
){

	/* Struct Reaction Matrix
	 * 1) Vector Ractants
	 * 2) Vector Reactants
	 * 3) boolean reversible, doubles for A, n, Ea
	 */

	vector<string> Reactions_List; // vector to hold the list of reactions for processing

	vector< SingleReactionData > Reaction_Matrix;

	ifstream Mechanism_Data;
	Mechanism_Data.open (filename.c_str());

	vector< int > SchemeUnits(2); // identifiers for units of A and Ea
	vector< int > mapping;

	int begin_flag = 0, end_flag = 0; // treat as Boolean, true/false

	size_t found;

	int Number_Species = (int)Species.size();

	/*
	 * New implementation:
	 * 1: read the entire mechanism into a vector (this needs more RAM)
	 * 2: process the reactions into the mechanism for the solver
	 */


	if (Mechanism_Data.is_open())
	{
		string line;
		while (Mechanism_Data.good())
		{
			getline (Mechanism_Data,line);

			if(begin_flag && !end_flag)
			{
				found = line.find("END"); // need to check for end in loop for 4 line blocks
				if (found!=string::npos && begin_flag)
				{
					end_flag = 1;
				}

				if(!end_flag && !line.empty() && line.compare(0,1,"!") != 0) // Abort if end reached or initial line is comment
				{
					Reactions_List.push_back(line);
				}
			}

			// Moving reaction data check to end of function avoids "REAC" being read in as a name
			if(Test_If_Word_Found(line,"REAC") && !begin_flag) // only test once... once we found the reactions, its done
			{
				begin_flag=1;
				SchemeUnits = Set_Mechanism_Units(line);
			}
		}
		Mechanism_Data.close();
	}

	for(int j=0;j<(int)Reactions_List.size();j++)
	{
		string line = Reactions_List[j];
		vector< double > ReactantData; // Reactant Information
		ReactantData.resize(Number_Species);
		vector< double > ProductData; // Product Information
		ProductData.resize(Number_Species);
		vector< double > ReactionData; // Reaction parameters such as Arrhenius parameters and whether irreversible or not
		ReactionData.resize(4); // A, n, Ea and whether reversible or irreversible

		// Reaction is marked a duplicate
		if(line.compare(0,1,"!") != 0  && line.compare(0,1,"/") != 0 && line.compare(0,3,"DUP") == 0)
		{
			int position = (int) Reaction_Matrix.size() - 1;
			Reaction_Matrix[position].IsDuplicate=true;
		}
		// content in here - check if line does not start with a comment, ! or / or DUP
		else if(line.compare(0,1,"!") != 0  && line.compare(0,1,"/") != 0 && line.compare(0,3,"DUP") != 0)
		{
			// Split by = or => sign
			vector< string > SplitLineIn;
			vector< string > RemoveComments;

			RemoveComments = Tokenise_String_To_String(line , "!" );
			line = RemoveComments[0];
			RemoveComments.clear();


			// only continue if the string is not empty or only whitespace or only tab
			if(!line.empty() && line.find_first_not_of("\t ") != string::npos)
			{
				// Determine if the reaction is irreversible. We assume it is reversible and then correct.
				// irreversible is indicated using => or ->
				bool is_reversible = true;
				if(Test_If_Word_Found(line,"=>") || Test_If_Word_Found(line,"->"))
				{
					is_reversible = false;
					// assume the user may write "<=>" or "<=>" which he shouldn't...
					if(Test_If_Word_Found(line,"<=>") || Test_If_Word_Found(line,"<->"))
					{
						is_reversible = true;
					}
				}


				SplitLineIn = Tokenise_String_To_String(line, "<=>");

				vector< string > SplitLineLeft;
				vector< string > SplitLineRight;

				// SplitLineLeft Contains all inputs on the left hand side
				SplitLineLeft = Tokenise_String_To_String(SplitLineIn[0],"\t +");
				// SplitLineRight Contains all inputs on the right hand side -> last 3 are parameters
				SplitLineRight = Tokenise_String_To_String(SplitLineIn[1],"\t +");


				int SplitLineLeftSize, SplitLineRightSize;
				SplitLineLeftSize = (int)SplitLineLeft.size();
				SplitLineRightSize = (int)SplitLineRight.size() - 3; //(last 3 entries are Arrhenius Parameters

				//cout << SplitLineLeftSize << " " << SplitLineRightSize << "\n";

				for(int i = 0;i<SplitLineLeftSize;i++) // process all entries on left hand side
				{
					SpeciesWithCoefficient SpeciesAndCoefficient;
					SpeciesAndCoefficient = Return_Species_With_Coefficient(SplitLineLeft[i], Species);

					if(SpeciesAndCoefficient.ismatched)
					{
						ReactantData[SpeciesAndCoefficient.SpeciesID] =
								ReactantData[SpeciesAndCoefficient.SpeciesID] - SpeciesAndCoefficient.coefficient;
					}
				}



				for(int i = 0;i<SplitLineRightSize;i++) // process all entries on right hand side
				{

					SpeciesWithCoefficient SpeciesAndCoefficient;
					SpeciesAndCoefficient = Return_Species_With_Coefficient(SplitLineRight[i], Species);

					if(SpeciesAndCoefficient.ismatched)
					{
						ProductData[SpeciesAndCoefficient.SpeciesID] =
								ProductData[SpeciesAndCoefficient.SpeciesID] + SpeciesAndCoefficient.coefficient;
					}
				}



				/* In a next step we need to handle the Arrhenius parameters
						For simplicity, these get attached to the end of the reaction matrix
						i.e as columns Species +1,+2,+3
				 */


				// Tokenize line, then take last 3 positions - easiest to work on whole line
				vector< string > SplitLine;

				SplitLine = Tokenise_String_To_String(line,"\t ");
				int SplitLineSize = (int)SplitLine.size();

				if(SchemeUnits[0] == 0) // A is molecules / cm^3
				{
					// For molecules / cm^3
					//ReactionData[0]=(strtod(SplitLine[SplitLineSize - 3].c_str(),NULL)); // This is A

					// for moles / dm^3
					//*
					double order = 0;
					for(int i=0;i<(int)ReactantData.size();i++)
					{
						order = order + ReactantData[i];
					}
					order = fabs(order) - 1; // make sure it is positive

					double step = (strtod(SplitLine[SplitLineSize - 3].c_str(),NULL));
					// 6.0221e+23 <- molecules per L
					double scale = (6.0221e+23); // convert to molecules / cm^(-3)
					step = step * pow(scale,(order));

					//ReactionData[0] = step * 1000;
					ReactionData[0] = step / 1000;
					//*/
				}
				else
				{
					if(SchemeUnits[0] == 1) // A is in moles / cm^3
					{
						// for molecules / cm^3
						/*
								// get the reaction order
								double order = 0;
								for(i=0;i<(int)ReactantData.size();i++)
								{
									order = order + ReactantData[i];
								}
								order = abs(order) - 1; // make sure it is positive

								double step = (strtod(SplitLine[SplitLineSize - 3].c_str(),NULL));
								double scale = 1 /(6.0221e+23); // convert to molecules / cm^(-3)
								step = step * pow(scale,(order));
								ReactionData[0] = step;
								//*/

						// For working in moles/dm^3
						ReactionData[0]=(strtod(SplitLine[SplitLineSize - 3].c_str(),NULL));
						ReactionData[0] = ReactionData[0]/1000; // per liter
					}
				}

				ReactionData[1]=(strtod(SplitLine[SplitLineSize - 2].c_str(),NULL)); // This is n
				double step = (strtod(SplitLine[SplitLineSize - 1].c_str(),NULL));
				//cout << step << "\n";

				if(SchemeUnits[1] == 0) // Ea is in Kelvin
				{
					ReactionData[2] = step;
				}
				else if(SchemeUnits[1] == 1) // Ea is in kcal/mol
				{
					ReactionData[2] = step*1.98709e-3; // sure input gets converted
				}

				else if(SchemeUnits[1] == 2) // Ea is in cal/mol
				{
					ReactionData[2] = step*1.98709; // sure input gets converted
				}

				else if(SchemeUnits[1] == 3) // Ea is in kJ/mol
				{
					ReactionData[2] = step/8.3144621e-3; // sure input gets converted
				}

				else if(SchemeUnits[1] == 4) // Ea is in J/mol
				{
					ReactionData[2] = step/8.3144621; // sure input gets converted
				}

				cout << SchemeUnits[1] << " " << ReactionData[2] << "\n";

				// Make New Input
				SingleReactionData temp;

				// check for Third Body Indicator:
				temp.ThirdBodyType = 0; // no third body, default
				if(Test_If_Word_Found(line, "+M")) // first type, set parameter
				{
					temp.ThirdBodyType = 1;
				}
				if(Test_If_Word_Found(line, "+(M)")) // apparently not the first, but the second type
				{
					temp.ThirdBodyType = 2;
				}
				/*/ if there is a thrid body reaction, we need to read in the parameters...
				if(temp.ThirdBodyType == 1)
				{
				//

				}
				else if{temp.ThirdBodyType == 2}
				{
				// LOW/TROE parameters

				//*/


				temp.Reactants = ReactantData;
				temp.Products = ProductData;
				temp.paramA = ReactionData[0];
				temp.paramN = ReactionData[1];
				temp.paramEa = ReactionData[2];
				temp.Reversible = is_reversible;
				temp.IsDuplicate = false; // default, not a duplicate

				Reaction_Matrix.push_back(temp);

				//cout << temp.paramA << " " << temp.paramN << " " << temp.paramEa << "\n";

				ReactantData.clear();
				ProductData.clear();
				ReactionData.clear();

			}
			// Format per Reaction: First Coefficient of Species, negative for Reactant, Positive for Product
			// followed by pre-exponential constant A, n (from T^n), then Activation Energy Ea and
			// finally a flag to denote a reversible reaction (0) or irreversible reaction (1)
		}
	}


	return Reaction_Matrix;
}







// Helper function to simplify code
// splits Species Coefficient from Species Name
SpeciesWithCoefficient Return_Species_With_Coefficient(
		string InputToSplit,
		const vector< string > Species
)
{
	int i,j;
	int Number_Species = (int)Species.size();
	SpeciesWithCoefficient SpeciesAndCoefficient;
	SpeciesAndCoefficient.ismatched = false; // default, defined here not globally

	// isalpha returns true if entry is upper or lower case letter
	i = 0;

	// work through every character of the string checking whether it is alphabetic
	// if the character is alphabetic, we have reached the species name
	// if it is not alphabetic we have the coefficient
	while(!isalpha(InputToSplit[i]) && i < (int) InputToSplit.size())
	{
		i++;
	}
	// i now contains the count of non-alphabetical characters in front of the species

	// In the next step we need to process the species name (remainder of the string)
	// and add it to the appropriate location in the array

	string species_name;
	double species_coefficient = 0;
	species_name = InputToSplit.substr(i,InputToSplit.size()-i);
	species_coefficient = strtod(InputToSplit.c_str(),NULL);

	j = 0;
	while(j<Number_Species && !SpeciesAndCoefficient.ismatched) // just loop until the species is found
	{
		string Comparator = Species[j];

		// Find the appropriate species and its position
		if(strcmp(species_name.c_str(),Comparator.c_str()) == 0){
			if(species_coefficient == 0)
			{
				species_coefficient = 1; // Deal with implicit coefficient of 1
			}
			SpeciesAndCoefficient.SpeciesID = j;
			SpeciesAndCoefficient.coefficient = species_coefficient;
			SpeciesAndCoefficient.ismatched = true;
		}

		j = j + 1;
	}

	return SpeciesAndCoefficient;
}

