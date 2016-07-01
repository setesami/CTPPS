/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#ifndef _elegent_jenkovszky_model_
#define _elegent_jenkovszky_model_

#include "Model.h"

namespace Elegent
{

/**
 * \brief The model of Jenkovszky et al.
 * References:
 *	[1] L. L. JENKOVSZKY, A. I. LENGYEL, and D. I. LONTKOVSKYI, Int. J. Mod. Phys. A 26 (2011) 4755. DOI: 10.1142/S0217751X11054760 
 **/
class JenkovszkyModel : public Model
{
	public:
		JenkovszkyModel();

		void Configure();
		virtual void Init();

		virtual void Print() const;

		virtual TComplex Amp(double t) const;
		virtual TComplex Prf(double b) const;

	protected:
		/// pomeron parameters
		double a_P, b_P, de_P, al1_P, ep_P, s_P;

		/// odderon parameters
		double a_O, b_O, de_O, al1_O, s_O;

		/// omega parameters
		double a_om, b_om, s_om, al0_om, al1_om;

		/// f parameters
		double a_f, b_f, s_f, al0_f, al1_f;
		
		/// integration parameters for profile-funcion calculation
		double precision_t, upper_bound_t;
		
		static TComplex Amp_J0(double *t, double *b, const void *obj);
};

} // namespace

#endif
