/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#include "interface/BHModel.h"
#include "interface/Math.h"
#include "interface/Constants.h"

using namespace Elegent;

//----------------------------------------------------------------------------------------------------

BHModel::BHModel()
{
	fullLabel.name = "Block et al."; shortLabel.name = "block";
}

//----------------------------------------------------------------------------------------------------

void BHModel::Configure()
{
	// set labels
	fullLabel.variant = ""; shortLabel.variant = "";
	fullLabel.version = "Phys. Rept. 436 (2006) 71-215"; shortLabel.version = "06";
	fullLabel.mode = ""; shortLabel.mode = "";
}

//----------------------------------------------------------------------------------------------------

void BHModel::Init()
{
	// parameters from [2]: Table 5, N_g from above Eq. (463), a's and b's from A.1.2

	// common parameters
	m0 = 0.6;
	s0 = 9.53;
	al_s = 0.5;
	Sigma_gg = 9. * cnts->pi * al_s*al_s / m0/m0;

	// sigma_gg
	Cp_gg = 0.00103;
	epsilon = 0.05;

	// TODO: which is correct?
	// [1] : Ng = (6.-epsilon)*(5.-epsilon)*(4.-epsilon)*(3.-epsilon)*(2.-epsilon)*(1.-epsilon) /5./4./3./2. / 2.;	// = 2.649
	// [2] : Ng = 3./2. * (5.-epsilon)*(4.-epsilon)*(3.-epsilon)*(2.-epsilon)*(1.-epsilon) /5./4./3./2.;	// = 1.335
	// Value from [2] seems wrong
	Ng = (6.-epsilon)*(5.-epsilon)*(4.-epsilon)*(3.-epsilon)*(2.-epsilon)*(1.-epsilon) /5./4./3./2. / 2.;
		
	a.push_back(-41.1);
	a.push_back(-487.5);
	a.push_back(-600.);
	a.push_back(600.);
	a.push_back(487.5);
	a.push_back(41.1);

	b.push_back(-9.);
	b.push_back(-225.);
	b.push_back(-900.);
	b.push_back(-900.);
	b.push_back(-225.);
	b.push_back(-9.);

	// sigma_qg
	C_qg_log = 0.166;

	// sigma_qq
	C = 5.36;
	C_even_regge = 29.7;

	// sigma_odd
	C_odd = 10.3;
	
	// mu's
	mu_gg = 0.73;
	mu_odd = 0.53;
	mu_qq = 0.89;

	// integration settings
	upper_bound = 50.;
	precision = 1E-12;

	// precompute mu_qg
	mu_qg = sqrt(mu_qq*mu_gg);
		
	// precompute sigma_gg, Eq. (B5) in [1]: below the Cp_gg stands for C'_gg
	sigma_gg = Cp_gg * Sigma_gg * Ng*Ng * Sum(cnts->s);

	// precompute sigma_qq, Eq. (B9) in [1]
	sigma_qq = Sigma_gg * (C + C_even_regge * m0/sqrt(cnts->s) * TComplex::Exp(i * cnts->pi / 4.));

	// precompute sigma_qg, Eq. (B10) in [1]
	sigma_qg = Sigma_gg * C_qg_log * TComplex(log(cnts->s/s0), -cnts->pi/2.);
	
	// precompute sigma_odd, Eq. (B12) in [1] - the factor in front of W(b, mu_odd)
	// plus additional factor (-i) to match the normalization used in chi_without_i
	sigma_odd = -i * C_odd * Sigma_gg * m0 / cnts->sqrt_s * TComplex::Exp(i * cnts->pi / 4.);
}

//----------------------------------------------------------------------------------------------------

void BHModel::Print() const
{
	printf(">> BHModel::Print\n");
	printf("\t%s\n", CompileFullLabel().c_str());

	printf("\tcommon:\n");
	printf("\t\ts0 = %E\n", s0);
	printf("\t\tm0 = %E\n", m0);
	printf("\t\tSigma_gg = %E\n", Sigma_gg);

	printf("\tsigma_gg:\n");
	printf("\t\tCp_gg = %E\n", Cp_gg);
	printf("\t\tNg = %E\n", Ng);
	printf("\t\tepsilon = %E\n", epsilon);
	printf("\t\ta0 = %E, a1 = %E, a2 = %E, a3 = %E, a4 = %E, a5 = %E\n", a[0], a[1], a[2], a[3], a[4], a[5]);
	printf("\t\tb0 = %E, b1 = %E, b2 = %E, b3 = %E, b4 = %E, b5 = %E\n", b[0], b[1], b[2], b[3], b[4], b[5]);
	printf("\t\tsigma_gg: Re = %E, Im = %E\n", sigma_gg.Re(), sigma_gg.Im());

	printf("\tsigma_qg:\n");
	printf("\t\tC_qg_log = %E\n", C_qg_log);
	printf("\t\tsigma_qg: Re = %E, Im = %E\n", sigma_qg.Re(), sigma_qg.Im());

	printf("\tsigma_qq:\n");
	printf("\t\tC = %E\n", C);
	printf("\t\tC_even_regge = %E\n", C_even_regge);
	printf("\t\tsigma_qq: Re = %E, Im = %E\n", sigma_qq.Re(), sigma_qq.Im());

	printf("\tsigma_odd:\n");
	printf("\t\tC_odd = %E\n", C_odd);
	printf("\t\tsigma_odd: Re = %E, Im = %E\n", sigma_odd.Re(), sigma_odd.Im());
	
	printf("\tmu's:\n");
	printf("\t\tmu_gg = %E\n", mu_gg);
	printf("\t\tmu_qq = %E\n", mu_qq);
	printf("\t\tmu_qg = %E\n", mu_qg);
	printf("\t\tmu_odd = %E\n", mu_odd);
}

//----------------------------------------------------------------------------------------------------

TComplex BHModel::Sum(double s) const
{
	TComplex log_tau0 = log(m0*m0 / s) + i * cnts->pi / 2.;

	TComplex S = 0.;

	for (unsigned int ii = 0; ii <= 5; ii++)
	{
		double i = double(ii);
		double ime = i - epsilon;
		double f = b[ii] / ime;
		double t1 = (a[i] - f) / ime;
		TComplex v = t1 - TComplex::Exp(ime * log_tau0) * (t1 + f * log_tau0);
		S += v;
	}

	return S;
}

//----------------------------------------------------------------------------------------------------
double BHModel::W(double b, double mu) const
{
	// Eq. (B2) in [1]
	double mub = mu * b;

	// evaluates v = (mu b)^3 K_3(mu b); directly or in continuous limit
	double v = 0.;
	if (mub < 1E-10)
		v = 8.;
	else
		v = mub*mub*mub * TMath::BesselK(3, mub);
	return mu*mu * v / 96. / cnts->pi;
}

//----------------------------------------------------------------------------------------------------

TComplex BHModel::chi_without_i(double b) const
{
	// TODO: why the one half?

	// Eqs. (B1) without the leading i factor and Eq. (B12) in [1]
	double odd_sign = (cnts->pMode == cnts->mAPP) ? +1. : -1.;
	return (
			sigma_gg * W(b, mu_gg)
			+ sigma_qg * W(b, mu_qg)
			+ sigma_qq * W(b, mu_qq)
			+ odd_sign * sigma_odd * W(b, mu_odd)
		) / 2.;
}

//----------------------------------------------------------------------------------------------------

TComplex BHModel::prf0(double b) const
{
	return (1. - TComplex::Exp(-chi_without_i(b)));
}

//----------------------------------------------------------------------------------------------------

TComplex BHModel::Prf(double b) const
{
	return prf0(b / cnts->hbarc) * i / 2.;
}

//----------------------------------------------------------------------------------------------------

TComplex BHModel::prf0_J0(double *b, double *q, const void *obj)
{
	return b[0] * TMath::BesselJ0(b[0]*q[0]) * ((BHModel *)obj)->prf0(b[0]);
} 

//----------------------------------------------------------------------------------------------------

TComplex BHModel::Amp(double t) const
{
	double q = sqrt(-t);

	// from Eqs. (A11) and (A12)
	return	i * cnts->p_cms * cnts->sqrt_s * CmplxInt(this, prf0_J0, 0., upper_bound, &q, precision);
}
