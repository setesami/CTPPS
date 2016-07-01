/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#include <string>
#include <vector>
#include <map>

#include "interface/Constants.h"
#include "interface/ModelFactory.h"

#include "TFile.h"
#include "TGraph.h"

using namespace Elegent;
using namespace std;


//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: ElegentBDistributionSampler [option] [option] ...\n");
	printf("OPTIONS:\n");
	printf("\t-h\t\t\tprint this help and exit\n");
	printf("\t-energy <W>\t\tset collision energy (i.e. sqrt(s)), in GeV\n");
	printf("\t-pp\t\t\tselect proton-proton interactions\n");
	printf("\t-app\t\t\tselect antiproton-proton interactions\n");
	printf("\t-N <number>\t\tnumber of sampling points\n");
	printf("\t-bmin <value>\t\tlower bound of b (in fm) for sampling\n");
	printf("\t-bmax <value>\t\tupper bound of b (in fm) for sampling\n");
	printf("\t-models <string>\tsemicolon-separated list of model tags\n");
	printf("\t-l, -model-list\t\tprint list of available model tags and exit\n");
	printf("\t-output <filename>\toutput file name\n");
}

//----------------------------------------------------------------------------------------------------

void PrintModelList()
{
	Constants::Init(1., Constants::mPP);
	ModelFactory mf;
	mf.PrintList();
}

//----------------------------------------------------------------------------------------------------

int InitModels(const string& hadronicModelsString, vector<Model *> &models)
{
	ModelFactory mf;

	size_t p_curr = 0;
	while (true)
	{
		size_t p_next = hadronicModelsString.find(';', p_curr);
		string tag = (p_next != string::npos) ? hadronicModelsString.substr(p_curr, p_next - p_curr) : hadronicModelsString.substr(p_curr);

		model = mf.MakeInstance(tag);

		if (model == NULL)
			return 3;

		models.push_back(model);

		printf("\n>> model with tag `%s':\n", tag.c_str());
		model->Print();

		if (p_next == string::npos)
			break;
		else
			p_curr = p_next + 1;
	}

	return 0;
}

//----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	// defaults
	double W = 0.;
	Constants::ParticleMode pMode = Constants::mPP;

	unsigned int N = 1001;
	double b_min = 0., b_max = 10.;

	string hadronicModelsString = "";

	string outputFileName = "";

	// process command line
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-h") == 0)
		{
			PrintUsage();
			return 0;
		}
		
		if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "-model-list") == 0)
		{
			PrintModelList();
			return 0;
		}

		if (strcmp(argv[i], "-energy") == 0)
		{
			if (argc-1 > i)
				W = atof(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-pp") == 0)
		{
			pMode = Constants::mPP;
			continue;
		}

		if (strcmp(argv[i], "-app") == 0)
		{
			pMode = Constants::mAPP;
			continue;
		}

		if (strcmp(argv[i], "-N") == 0)
		{
			if (argc-1 > i)
				N = atoi(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-bmax") == 0)
		{
			if (argc-1 > i)
				b_max = atof(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-bmin") == 0)
		{
			if (argc-1 > i)
				b_min = atof(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-models") == 0)
		{
			if (argc-1 > i)
				hadronicModelsString = argv[++i];
			continue;
		}


		if (strcmp(argv[i], "-output") == 0)
		{
			if (argc-1 > i)
				outputFileName = argv[++i];
			continue;
		}

		printf("ERROR: unrecognised parameter `%s'.\n", argv[i]);
		PrintUsage();
		return 10;
	}

	// test input
	bool stop = false;
	if (W == 0.)
	{
		printf("ERROR: collision energy not specified.\n");
		stop = true;
	}

	if (outputFileName.empty())
	{
		printf("ERROR: output not specified.\n");
		stop = true;
	}

	if (N < 2)
	{
		printf("ERROR: N (%u) < 2\n", N);
		stop = true;
	}

	if (b_max <= b_min)
	{
		printf("ERROR: b_max (%.3E) <= b_min (%.3E)\n", b_max, b_min);
		stop = true;
	}

	if (stop)
	{
		PrintUsage();
		return 1;
	}

	// print input
	printf(">> ElegentBDistributionSampler > input:\n");
	printf("\tsqrt s = %E\n", W);
	printf("\tparticle mode %u\n", pMode);

	printf("\tsamples = %u, b_min, = %.2E, b_max = %.2E\n", N, b_min, b_max);

	printf("\tmodels = %s\n", hadronicModelsString.c_str());
	printf("\toutput = %s\n", outputFileName.c_str());

	// initialise constants etc.
	Constants::Init(W, pMode);
	cnts->Print();

	// inialise models
	vector<Model *> models;
	if (InitModels(hadronicModelsString, models) != 0)
		return 3;

	// prepare output
	TFile *outF = new TFile(outputFileName.c_str(), "recreate");

	// a trick to save E, since of->WriteObject(&E, "cmsEnergy") doesn't work
	TGraph *data = new TGraph();
	data->SetName("data");
	data->SetPoint(0, 0., W/2.);
	data->Write();

	// build the profile-function plots
	for (unsigned int mi = 0; mi < models.size(); mi++)
	{
		model = models[mi];
	
		gDirectory = outF->mkdir(model->CompileShortLabel().c_str());

		TGraph *g_prf_re = new TGraph(); g_prf_re->SetName("prf_re"); g_prf_re->SetTitle(model->CompileFullLabel().c_str());
		TGraph *g_prf_im = new TGraph(); g_prf_im->SetName("prf_im"); g_prf_im->SetTitle(model->CompileFullLabel().c_str());

		double db = (b_max - b_min) / (N - 1);
		double b = b_min;
		for (unsigned int pi = 0; pi < N; pi++, b += db)
		{
			TComplex prf = model->Prf(b);
			g_prf_re->SetPoint(pi, b, prf.Re());
			g_prf_im->SetPoint(pi, b, prf.Im());
		}

		g_prf_re->Write();
		g_prf_im->Write();
	}

	delete outF;
	return 0;
}
