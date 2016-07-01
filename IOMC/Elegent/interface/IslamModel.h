/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#ifndef _elegent_islam_model_
#define _elegent_islam_model_

#include "Model.h"

namespace Elegent
{


/**
 * \brief Islam model of p-p and p-anti p elastic scattering.
 * References:
 *	[1] ISLAM, M. M., FEARNLEY, T. and GUILLAUD, J. P., Nuovo Cim. A81 (1984) 737
 *	[2] ISLAM, M. M., INNOCENTE V., FEARNLEY T. and SANGUIETTI, G., Europhys. Lett. 4 (1987) 189-196
 *	[3] ISLAM, M. M., LUDDY, R. J. and PROKUDIN, A. V., Phys. Lett. B605 (2005) 115-122
 *	[4] ISLAM, M. M., LUDDY, R. J. and PROKUDIN, A. V., Int. J. Mod. Phys. A21 (2006) 1-42
 *	[5] ISLAM, M. M., KASPAR, J. and LUDDY, R. J., Mod. Phys. Lett. A24 (2009) 485-496
 **/
class IslamModel : public Model
{
	public:
		/// variant of the model
		enum VariantType
		{
			vHP,	///< hard Pomeron
			vLxG	///< low-x gluons
		} variant;

		/// mode of the model
		enum ModeType
		{
			mDiff,		///< diffraction amplitude
			mCore,		///< core amplitude
			mQuark,		///< quark-quark amplitude
			mDiffCore,	///< diffraction and core amplitude
			mFull		///< diffraction, core and quark-quark amplitude
		} mode;

		IslamModel();

		void Configure(VariantType _v, ModeType _m);

		virtual void Init();

		static TComplex CEF(double a, double b, double c);

		void SetUnitarizationOrders(int qq, int cgc)
		{
			qqMaxOrder = qq;
			cgcMaxOrder = cgc;
		}

		virtual void Print() const;

		virtual TComplex Amp(double t) const;
		virtual TComplex Prf(double b) const;
		
	protected:
		/// diffraction variables
		TComplex R, a, Diff_fac_profile, Diff_fac;
		
		/// hard scattering variables
		TComplex Abs_fac;
	
		/// core scattering variables
		double beta, m_omega_sq, Core_fac;
		
		/// quark confinement parameters
		double m0sq;
	
		/// "hard pomeron" scattering variables
		double r0, omega;
		TComplex Quark_fac;
		TComplex Quark_const;
		int qqMaxOrder;
			
		/// "low-x gluons" scattering
		double lambda, m_c;
		TComplex cgc_fac;
		int cgcMaxOrder;
	
		/// integration variables
		double precision, precision_t, upper_bound, upper_bound_t;

		/// diffraction amplitude
		TComplex T_diff(double t) const;
		TComplex GammaD(double b) const;
		static TComplex GammaD_J0(double *b, double *t, const void *obj);
		
		/// core amplitude
		TComplex T_core(double t) const;
		double F_sq(double t) const;

		/// quark-quark amplitude
		TComplex T_quark(double t) const;
		double I_integral(double qt, double al) const;
		static double F_cal_integ(double *x, double *qt, const void *obj);
		double F_cal(int n, double qt, double om, double m0sq) const;
		
		/// quark-quark amplitude: hard-pomeron variant
		static double T_hp_integ(double *, double *, const void *);
		TComplex T_hp_n(int n, double t) const;
		TComplex T_hp(double t) const;

		/// quark-quark amplitude: low-x gluons variant
		static double T_lxg_integ(double *, double *, const void *);
		TComplex T_lxg_n(int n, double t) const;
		TComplex T_lxg(double t) const;

		/// profile funcion methods
		static TComplex Amp_J0(double *t, double *b, const void *obj);
};

} // namespace

#endif
