/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#ifndef _elegent_bh_model_
#define _elegent_bh_model_

#include "Model.h"

namespace Elegent
{

/**
 * \brief Block-Halzen model of p-p and p-anti p elastic scattering.
 * References:
 *	[1] BLOCK, M. M., GREGORES, E. M., HALZEN, F. and PANCHERI, G., Phys. Rev. D60 (1999) 054024
 *	[2] BLOCK, M. M., Phys. Rept. 436 (2006) 71-215
 **/
class BHModel : public Model
{
	public:
		BHModel();
		
		void Configure();
		virtual void Init();
		virtual void Print() const;
		virtual TComplex Amp(double t) const;
		virtual TComplex Prf(double b) const;

	protected:
		/// common parameters
		double m0, s0, al_s, Sigma_gg; 

		/// parameters for sigma_gg
		double Cp_gg, epsilon, Ng; 
		std::vector<double> a, b;
		
		/// parameters for sigma_gg
		double C_qg_log; 
		
		/// parameters for sigma_qg
		double C, C_even_regge; 
		
		/// parameters for sigma_qq
		double C_odd; 

		/// effective areas
		double mu_gg, mu_qg, mu_qq, mu_odd;

		/// the integral cross-sections
		TComplex sigma_gg, sigma_qq, sigma_qg, sigma_odd;
		
		/// integration parameters
		double precision, upper_bound;

		/// profile defined by Eq. (B2) in [1]
		double W(double b, double mi) const;

		/// sum in Eq. (B5) in [1]
		TComplex Sum(double s) const;

		/// the full eikonal: Eq. (1) without the leading factor i and Eq. (12)
		TComplex chi_without_i(double b) const;

		TComplex prf0(double b) const;
		static TComplex prf0_J0(double *y, double *t, const void *obj);
};

} // namespace

#endif
