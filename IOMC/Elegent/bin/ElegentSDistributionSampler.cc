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
	printf("USAGE: ElegentSDistributionSampler [option] [option] ...\n");
	printf("OPTIONS:\n");
	printf("\t-h\t\t\tprint this help and exit\n");
	printf("\t-pp\t\t\tselect proton-proton interactions\n");
	printf("\t-app\t\t\tselect antiproton-proton interactions\n");
	printf("\t-N <number>\t\tnumber of sampling points\n");
	printf("\t-Wmin <value>\t\tlower bound of sqrt(s) (in GeV) for sampling\n");
	printf("\t-Wmax <value>\t\tupper bound of sqrt(s) (in GeV) for sampling\n");
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

		model = mf.MakeInstance(tag, false);

		if (model == NULL)
			return 3;

		// force no presampling for model of Bourrely et al.
		if (model->shortLabel.name.compare("bourrely") == 0)
		{
			BSWModel *bswm = (BSWModel *) model;
			bswm->presampled = false;
			bswm->highAccuracy = false;
		}

		models.push_back(model);

		printf("\n>> model with tag `%s':\n", tag.c_str());

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
	Constants::ParticleMode pMode = Constants::mPP;

	unsigned int N = 1001;
	double W_min = 5E0, W_max = 1E5;

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

		if (strcmp(argv[i], "-Wmax") == 0)
		{
			if (argc-1 > i)
				W_max = atof(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-Wmin") == 0)
		{
			if (argc-1 > i)
				W_min = atof(argv[++i]);
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

	if (W_max <= W_min)
	{
		printf("ERROR: W_max (%.3E) <= W_min (%.3E)\n", W_max, W_min);
		stop = true;
	}

	if (stop)
	{
		PrintUsage();
		return 1;
	}

	// print input
	printf(">> ElegentSDistributionSampler > input:\n");
	printf("\tparticle mode %u\n", pMode);

	printf("\tsamples = %u, W_min, = %.2E, W_max = %.2E\n", N, W_min, W_max);

	printf("\tmodels = %s\n", hadronicModelsString.c_str());
	printf("\toutput = %s\n", outputFileName.c_str());

	// initialise constants instance with dummy values
	Constants::Init(1., pMode);

	// inialise models
	vector<Model *> models;
	if (InitModels(hadronicModelsString, models) != 0)
		return 3;

	// prepare output
	TFile *outF = new TFile(outputFileName.c_str(), "recreate");

	// build the s-distributions
	for (unsigned int mi = 0; mi < models.size(); mi++)
	{
		model = models[mi];
	
		string fullLabel = model->CompileFullLabel();
		//printf(">> %s\n", fullLabel.c_str());

		gDirectory = outF->mkdir(model->CompileShortLabel().c_str());

		TGraph *g_si_tot = new TGraph(); g_si_tot->SetName("si_tot"); g_si_tot->SetTitle(fullLabel.c_str());
		TGraph *g_rho = new TGraph(); g_rho->SetName("rho"); g_rho->SetTitle(fullLabel.c_str());
		TGraph *g_B0 = new TGraph(); g_B0->SetName("B0"); g_B0->SetTitle(fullLabel.c_str());

		double f = exp( log(W_max / W_min) / (N - 1) );
		double W = W_min;
		for (unsigned int pi = 0; pi < N; pi++, W *= f)
		{
			cnts->Configure(W, pMode);

			model->Init();

			double ep = 1E-5;
			TComplex amp0 = model->Amp(0.);
			TComplex amp_ep = model->Amp(-ep);

			double si_tot = 4.*cnts->pi*cnts->sq_hbarc/cnts->p_cms/cnts->sqrt_s * amp0.Im();
			double rho = (amp0.Im() != 0.) ? amp0.Re() / amp0.Im() : 0.;
			double B0 = ( log(amp0.Rho2()) - log(amp_ep.Rho2()) ) / ep;

			// fill graphs
			g_si_tot->SetPoint(pi, W, si_tot);
			g_rho->SetPoint(pi, W, rho);
			g_B0->SetPoint(pi, W, B0);
		}

		g_si_tot->Write();
		g_rho->Write();
		g_B0->Write();
	}

	delete outF;
	return 0;
}
