/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#include "interface/JenkovszkyModel.h"
#include "interface/Constants.h"
#include "interface/Math.h"

using namespace Elegent;

JenkovszkyModel::JenkovszkyModel()
{
	fullLabel.name = "Jenkovszky et al."; shortLabel.name = "jenkovszky";
}

//----------------------------------------------------------------------------------------------------

void JenkovszkyModel::Configure()
{
	// set labels
	fullLabel.variant = ""; shortLabel.variant = "";
	fullLabel.version = "Int. J. Mod. Phys. A 26 (2011) 4755"; shortLabel.version = "11";
	fullLabel.mode = ""; shortLabel.mode = "";
}

//----------------------------------------------------------------------------------------------------

void JenkovszkyModel::Init()
{
	// fit parameters from Table 3 (corresponding to trajectory (8)),
	// reggeon trajectories given under Eq. (4) in [1]

	a_P = 261.966;
	b_P = 8.40969;	// GeV^-2
	de_P = 0.0504675;
	al1_P = 0.436108;
	ep_P = 0.0152887;
	s_P = 100.;	// GeV^2
	
	a_O = 0.0875234;
	b_O = 14.226;
	de_O = 0.172615;
	al1_O = 0.0434551;
	s_O = 100.;	// GeV^2
	
	a_om = 8.21009;
	b_om = 23.81201;	// GeV^-2
	s_om = 1.;	// GeV^2
	al0_om = 0.43;
	al1_om = 0.93;
	
	a_f = -12.6303;
	b_f = 4.35673;	// GeV^-2
	s_f = 1.;	// GeV^2
	al0_f = 0.70;
	al1_f = 0.84;

	precision_t = 1E-4;
	upper_bound_t = -50.;
}

//----------------------------------------------------------------------------------------------------

void JenkovszkyModel::Print() const
{
	printf(">> JenkovszkyModel::Print\n");
	printf("\t%s\n", CompileFullLabel().c_str());
	printf("\tpomeron:\n\t\ta_P=%.3E, b_P=%.3E, de_P=%.3E, al1_P=%.3E, ep_P=%.3E, s_P=%.3E\n", a_P, b_P, de_P, al1_P, ep_P, s_P);
	printf("\todderon:\n\t\ta_O=%.3E, b_O=%.3E, de_O=%.3E, al1_O=%.3E, s_O=%.3E\n", a_O, b_O, de_O, al1_O, s_O);
	printf("\tomega:\n\t\ta_om=%.3E, b_om=%.3E, s_om=%.3E, al0_om=%.3E, al1_om=%.3E\n", a_om, b_om, s_om, al0_om, al1_om);
	printf("\tf:\n\t\ta_f=%.3E, b_f=%.3E, s_f=%.3E, al0_f=%.3E, al1_f=%.3E\n", a_f, b_f, s_f, al0_f, al1_f);
	printf("\tupper_bound_t=%.3E, precision_t=%.3E\n", upper_bound_t, precision_t);
}

//----------------------------------------------------------------------------------------------------

TComplex JenkovszkyModel::Amp(double t) const
{
	double s_eff = 0.;

	// pomeron
	s_eff = cnts->s / s_P;
	double al_P = 1. + de_P + al1_P*t; // TR.1
	TComplex r_1_sq = b_P + log(s_eff) - i*M_PI/2.;
	TComplex r_2_sq = log(s_eff) - i*M_PI/2.;
	TComplex A_P = i * a_P/b_P * s_eff * (
		r_1_sq * TComplex::Exp(r_1_sq * (al_P - 1.))
		- ep_P * r_2_sq * TComplex::Exp(r_2_sq * (al_P - 1.))
	);

	// odderon
	s_eff = cnts->s / s_O;
	double al_O = 1. + de_O + al1_O*t; // TR.1
	TComplex rO_1_sq = b_O + log(s_eff) - i*M_PI/2.;
	// NB: wrong sign in Eq (8) in arXiv: 1105.1202v1
	TComplex A_O = - a_O/b_O * s_eff * (
		rO_1_sq * TComplex::Exp(rO_1_sq * (al_O - 1.))
	);

	if (cnts->pMode == cnts->mPP)
		A_O = -A_O;
		
	// omega
	s_eff = cnts->s / s_om;
	double al_om = al0_om + al1_om*t;
	TComplex A_om = a_om * TComplex::Exp(-i*M_PI/2. * al_om + b_om*t) * pow(s_eff, al_om);
		
		if (cnts->pMode == cnts->mPP)
			A_om = -A_om;
	
	// f
	s_eff = cnts->s / s_f;
	double al_f = al0_f + al1_f*t;
	TComplex A_f = a_f * TComplex::Exp(-i*M_PI/2. * al_f + b_f*t) * pow(s_eff, al_f);

	//printf("%E | %E, %E | %E, %E\n", t, A_P.Re(), A_P.Im(), A_O.Re(), A_O.Im());

	return cnts->p_cms/cnts->sqrt_s * (A_P + A_O + A_om + A_f);
}

//----------------------------------------------------------------------------------------------------

TComplex JenkovszkyModel::Amp_J0(double *t, double *b, const void *obj)
{
	/// t[0] ... t in GeV^2
	/// b[0] ... impact parameter in fm
	return ((JenkovszkyModel *)obj)->Amp(t[0]) * TMath::BesselJ0(b[0] * sqrt(-t[0]));
}

//----------------------------------------------------------------------------------------------------

TComplex JenkovszkyModel::Prf(double b_fm) const
{
	double b = b_fm / cnts->hbarc;	// b in GeV^-1
	return CmplxInt(this, Amp_J0, upper_bound_t, 0., &b, precision_t) / 4. / cnts->p_cms / cnts->sqrt_s;
}
