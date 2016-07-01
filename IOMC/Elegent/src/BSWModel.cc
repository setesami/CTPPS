/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#include "interface/BSWModel.h"
#include "interface/Constants.h"
#include "interface/Math.h"

using namespace std;
using namespace Elegent;

//----------------------------------------------------------------------------------------------------

BSWModel::BSWModel()
{
	fullLabel.name = "Bourrely et al."; shortLabel.name = "bourrely";

	highAccuracy = true;
}

//----------------------------------------------------------------------------------------------------

void BSWModel::Configure(BSWModel::ModeType _mode, bool _presampled)
{
	mode = _mode;
	presampled = _presampled;

	// set labels
	fullLabel.variant = ""; shortLabel.variant = "";
	fullLabel.version = "Eur. Phys. J. C28 (2003) 97-105"; shortLabel.version = "03";
	if (mode == mPomReg)
		{ fullLabel.mode = "full"; shortLabel.mode = "full"; }
	if (mode == mPom)
		{ fullLabel.mode = "Pomeron"; shortLabel.mode = "pom"; }
	if (mode == mReg)
		{ fullLabel.mode = "Reggeon"; shortLabel.mode = "reg"; }
}

//----------------------------------------------------------------------------------------------------

void BSWModel::Init()
{
	// Eq. (6) in [3]
	c = 0.167;
	cp = 0.748;

	// Table 1 in [3]
	m1 = 0.577; m1sq = m1*m1;	// 0.333
	m2 = 1.719; m2sq = m2*m2;	// 2.955
	f = 6.971;
	a = 1.858;	asq = a*a;		// 3.452
	 
	// Table 2 in [3], b's given in text of section 3 in [3]
	A2.Init(	-24.269,	0.,		0.357,	1.,		+1);
	omega.Init( -167.329,	0.,		0.323,	0.795,	-1);
	rho.Init(	124.919,	8.54,	0.320,	1.,		-1);
	
	if (highAccuracy)
	{
		upper_bound_t = -500.; precision_t = 1E-15;
		upper_bound_b = 50.; precision_b = 1E-12;
	} else {
		upper_bound_t = -200.; precision_t = 1E-5;
		upper_bound_b = 30.; precision_b = 1E-5;
	}

	regge_fac(1., 0.);
	
	// save value of S0(0)
	S00 = S0(0.);
	if (presampled)
		BuildSample(25001);
}

//----------------------------------------------------------------------------------------------------

void BSWModel::Print() const
{
	printf(">> BSWModel::Print\n");
	printf("\t%s\n", CompileFullLabel().c_str());
	printf("\tmode = %u\n", mode);
	printf("\tc=%.3f, c'=%.3f, m1=%.3f, m2=%.3f, f=%.3f, a=%.3f\n", c, cp, m1, m2, f, a);
	printf("\tA2   : C=%.3f, b=%.3f, alpha=%.3f, aplha'=%.3f\n", A2.C, A2.b, A2.a, A2.ap);
	printf("\tomega: C=%.3f, b=%.3f, alpha=%.3f, aplha'=%.3f\n", omega.C, omega.b, omega.a, omega.ap);
	printf("\trho  : C=%.3f, b=%.3f, alpha=%.3f, aplha'=%.3f\n", rho.C, rho.b, rho.a, rho.ap);

	printf("\n");

	printf("\tpresampled = %u\n", presampled);
	if (presampled)
		printf("\t\tsample size = %u, db = %.1E\n", data_N, data_db);
	
	printf("\n");

	printf("\tintegration parameters:\n");
	printf("\t\tt: upper bound = %.1E, precision = %.1E\n", upper_bound_t, precision_t);
	printf("\t\tb: upper bound = %.1E, precision = %.1E\n", upper_bound_b, precision_b);
}

//----------------------------------------------------------------------------------------------------

double BSWModel::Ft(double t) const
{
	double G = 1. / (1. - t/m1sq) / (1. - t/m2sq);
	return f*G*G*	(asq + t) / (asq - t);
}

//----------------------------------------------------------------------------------------------------

TComplex BSWModel::Rt(Trajectory tr, double t) const
{
	/// s0 = 1 GeV^2

	double alpha = tr.a + tr.ap * t;
	return tr.C * exp(tr.b * t + cnts->ln_s * alpha) * (1. + tr.sig * TComplex::Exp(-i*cnts->pi*alpha));
}

//----------------------------------------------------------------------------------------------------

TComplex BSWModel::R0t(double t) const
{
	return Rt(A2, t) + Rt(omega, t) + Rt(rho, t);
}

//----------------------------------------------------------------------------------------------------

TComplex BSWModel::S0(double t) const
{
	// the s term
	TComplex term_s = pow(cnts->s, c) / pow(cnts->ln_s, cp);

	// the u term
	double u = 4.*cnts->proton_mass*cnts->proton_mass - cnts->s - t;
	TComplex Lnu = TComplex(log(fabs(u)), -cnts->pi); // ambiguity in the article: which sign in +-pi?
	double Lnu_rho2 = Lnu.Rho2();
	double Lnu_theta = atan2(Lnu.Im(), Lnu.Re());	 // atan2 results in (-pi, +pi)
	TComplex LnLnu = TComplex(0.5*log(Lnu_rho2), Lnu_theta);
	TComplex term_u = TComplex::Exp(c * Lnu) / TComplex::Exp(cp * LnLnu);

#ifdef DEBUG
	printf(">> BSWModel::S0\n");
	printf("\ts=%E, u=%E\n", cnts->s, u);
	printf("\tLn u=%E +i%E\n", Lnu.Re(), Lnu.Im());
	printf("\tLn Ln u=%E +i%E\n", LnLnu.Re(), LnLnu.Im());
	printf("\tterm_s: %E + i%E\n", term_s.Re(), term_s.Im());
	printf("\tterm_u: %E + i%E\n", term_u.Re(), term_u.Im());
#endif

	return term_s + term_u;
}

//----------------------------------------------------------------------------------------------------

TComplex BSWModel::Omega0t(double t) const
{
	// S00 = S0(0) instead of S0(t) is used here, valid for high s only !!

	switch (mode) {
		case mPomReg:
			return S00*Ft(t) + R0t(t) / cnts->s / regge_fac;
		case mPom:
			return S00*Ft(t);
		case mReg:
			return R0t(t);
	}
	
	return TComplex(0, 0);
}

//----------------------------------------------------------------------------------------------------

TComplex BSWModel::Omega0t_J0(double *t, double *b, const void *obj)
{
	return ((BSWModel *)obj)->Omega0t(t[0]) * TMath::BesselJ0(b[0] * sqrt(-t[0]));
}

//----------------------------------------------------------------------------------------------------

TComplex BSWModel::Omega0b(double b) const
{
	// the 1/2 factor is consequence of dt integration (instead of q dq)
	return 0.5 * CmplxInt(this, Omega0t_J0, upper_bound_t, 0., &b, precision_t);
}

//----------------------------------------------------------------------------------------------------

TComplex BSWModel::prf0(double b) const
{
	return 1. - TComplex::Exp( - Omega0b(b) );
}

//----------------------------------------------------------------------------------------------------

TComplex BSWModel::Prf(double b) const
{
	return prf0(b / cnts->hbarc) * i / 2.;
}

//----------------------------------------------------------------------------------------------------

TComplex BSWModel::prf0_J0(double *b, double *q, const void *obj)
{
	BSWModel *m = (BSWModel *) obj;

	if (m->presampled)
		return m->SampleEval(b[0]) * b[0] * TMath::BesselJ0(b[0] * q[0]);
	else
		return m->prf0(b[0]) * b[0] * TMath::BesselJ0(b[0] * q[0]);
}

//----------------------------------------------------------------------------------------------------

TComplex BSWModel::Amp(double t) const
{
	double q = sqrt(-t);
	
	return i * cnts->p_cms * cnts->sqrt_s * CmplxInt(this, prf0_J0, 0., upper_bound_b, &q, precision_b);
}

//----------------------------------------------------------------------------------------------------

void BSWModel::BuildSample(unsigned int samples)
{
	printf(">> BSWModel::BuildSample > Building %u samples...\n", samples);

	data_re.clear();
	data_re.reserve(samples);
	data_im.clear();
	data_im.reserve(samples);
#ifdef DEBUG
	data_b.clear();
	data_b.reserve(samples);
#endif

	double db = upper_bound_b / (samples - 1);
	data_db = db;
	data_N = samples;

	double b = 0.;
	for (unsigned int i = 0; i < samples; i++, b += db)
	{
		TComplex v = prf0(b);

#ifdef DEBUG
		//printf("v=%.5f: re=%E, im=%E\n", b, v.Re(), v.Im());
		data_b.push_back(b);
#endif

		data_re.push_back(v.Re());
		data_im.push_back(v.Im());
	}
}

//----------------------------------------------------------------------------------------------------

TComplex BSWModel::SampleEval(double b)
{
	unsigned int idx = (int)(b / data_db);
	
	if (idx + 1 > data_N - 1)
		return TComplex(0, 0);

	double f = b/data_db - idx;

	return TComplex(
		(data_re[idx+1] - data_re[idx])*f + data_re[idx],
		(data_im[idx+1] - data_im[idx])*f + data_im[idx]
	);
}
