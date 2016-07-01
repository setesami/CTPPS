/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#include "interface/Generator.h"

#include "HepMC/GenEvent.h"

#include <cmath>

#include "TGraph.h"
#include "TFile.h"
#include "TClass.h"
#include "TRandom2.h"


using namespace std;
using namespace HepMC;
using namespace Elegent;

//----------------------------------------------------------------------------------------------------

Generator::Generator(const string &_file, const string &_path, double _t_min, double _t_max, unsigned int _verbosity) :
	fileName(_file), modelPath(_path), t_min(_t_min), t_max(_t_max), verbosity(_verbosity)
{
}

//----------------------------------------------------------------------------------------------------

unsigned int Generator::Init()
{
	// try to open the file
	TFile file(fileName.c_str());
	if (file.IsZombie())
	{
		printf("ERROR in Elegent::Generator::Init > File `%s' can not be loaded.\n", fileName.c_str());
		return 1;
	}

	// get cms energy
	TGraph *data = (TGraph *) file.Get("data");
	if (!data)
	{
		printf("ERROR in Elegent::Generator::Init > File `%s' does not contain `data' object.\n", fileName.c_str());
		return 2;
	}
	double dummy;
	data->GetPoint(0, dummy, E_cms);

	// try to load sigma_int graph
	TObject *o = file.Get(modelPath.c_str());
	if (!o || !o->IsA()->InheritsFrom("TGraph"))
	{
		file.ls(); 
		printf("ERROR in Elegent::Generator::Init > Model `%s' not found.\n", modelPath.c_str());
		return 3;
	}
	TGraph *sigma_int = (TGraph*) o;

	// prepare normalized inverse c.d.f
	int i_min = 0, i_max = sigma_int->GetN() - 1;	// default index range
	for (int i = 0; i < sigma_int->GetN(); i++)
	{
		double x, y;
		sigma_int->GetPoint(i, x, y);
		if (x < t_min)
		i_min = i;
		if (x > t_max)
		{
			i_max = i;
			break;
		}
	}
	
	double p_min = 0., p_max = 0.;
	double t_min_real = 0., t_max_real = 0.;
	sigma_int->GetPoint(i_min, t_min_real, p_min);
	sigma_int->GetPoint(i_max, t_max_real, p_max);

	if (verbosity > 0)
	{
		printf(">> Elegent::Generator > inverse cdf:\n\tfile: %s\n\tmodel: %s\n", fileName.c_str(), modelPath.c_str());
		printf("\trequested |t| range: %.2E to %.2E GeV^2\n", t_min, t_max);
		printf("\tpoints loaded: %i\n", i_max - i_min + 1);
		printf("\t\tfirst point: idx = %i, |t| = %.2E GeV^2, p = %.2E mb\n", i_min, t_min_real, p_min);
		printf("\t\tlast point: idx = %i, |t| = %.2E GeV^2, p = %.2E mb\n", i_max, t_max_real, p_max);
		printf("\t|t| range: %.2E to %.2E GeV^2\n", t_min_real, t_max_real);
		printf("\tcorresponding cross-section: %.2E mb\n", p_max - p_min);
	}

	if (i_min >= i_max)
	{
		printf("ERROR in Elegent::Generator::Init > Wrong (t_min, t_max) region (%.2E, %.2E) or empty intersection with CDF domain.\n", t_min, t_max);
		return 4;
	}

	icdf = new TGraph();
	for (int i = i_min; i <= i_max; i++)
	{
		double x, y;
		sigma_int->GetPoint(i, x, y);
		icdf->SetPoint(icdf->GetN(), (y - p_min) / (p_max - p_min), x);
	}

	// precompute kinematics
	double m = 0.938; // GeV
	p_cms = sqrt(E_cms*E_cms - m*m);
	if (verbosity > 0)
	{
		printf("\n>> Elegent::Generator > proton kinematics in CM frame (in GeV)\n");
		printf("\tE = %.3f\n", E_cms);
		printf("\tp = %.3f\n", p_cms);
		printf("\tm = %.3f\n", m);
	}

	return 0;
}

//----------------------------------------------------------------------------------------------------

void Generator::GenerateBase(double rn1, double rn2, GenEvent* gEv)
{
	gEv->set_signal_process_id(Generator::ElasticScattering);

	// create vertex at t = 0, position 0;
	GenVertex* gVx = new GenVertex(FourVector(0., 0., 0., 0.));
	gEv->add_vertex(gVx);

	// generation
	double p = 0., t = 0., phi = 0., theta = 0.;
	p = rn1;
	t = icdf->Eval(p);			// |t|
	phi = rn2 * 2. * M_PI;

	theta = sqrt(t) / p_cms;

	// kinematics collison in CM frame
	double p_x = p_cms * sin(theta) * cos(phi);
	double p_y = p_cms * sin(theta) * sin(phi);
	double p_z = p_cms * cos(theta);

	if (verbosity > 5)
	{
		if (verbosity > 6)
			printf("prob = %.3f\n", p);
		printf("|t| = %.2E GeV^2, theta = %.2E\n", t, theta);
		printf("phi = %.2E, theta_x = %.2E, theta_y = %.2E\n", phi, theta*cos(phi), theta*sin(phi));
		printf("px = %.2E, py = %.2E, pz = %.2E\n", p_x, p_y, p_z);
	}

	// add initial and final particles to the vertex
	GenParticle* gPe; 
	gPe = new GenParticle(HepMC::FourVector(0., 0., p_cms, E_cms), PID, NullState);		gPe->suggest_barcode(1);	gVx->add_particle_in(gPe);
	gPe = new GenParticle(HepMC::FourVector(0., 0., -p_cms, E_cms), PID, NullState);	gPe->suggest_barcode(2);	gVx->add_particle_in(gPe);
	gPe = new GenParticle(HepMC::FourVector(p_x, p_y, p_z, E_cms), PID, FinalState);	gPe->suggest_barcode(3);	gVx->add_particle_out(gPe);
	gPe = new GenParticle(HepMC::FourVector(-p_x, -p_y, -p_z, E_cms), PID, FinalState);	gPe->suggest_barcode(4);	gVx->add_particle_out(gPe);
}

//----------------------------------------------------------------------------------------------------

void Generator::Generate(GenEvent* gEv)
{
	GenerateBase(gRandom->Rndm(), gRandom->Rndm(), gEv);
}
