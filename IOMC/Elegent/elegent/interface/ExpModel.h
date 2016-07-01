/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#ifndef _elegent_exp_model_
#define _elegent_exp_model_

#include "Model.h"

namespace Elegent
{

/**
 * \brief Exponential model of p-p and p-anti p elastic scattering.
 * amplitude(t) = a * exp( b1*t + b2*t*t + i*(p0 + p1*t) )
 * For reference pourposes only.
 **/
class ExpModel : public Model
{
	public:
		ExpModel();
		
		void Configure();
		virtual void Init();
		virtual void Print() const;
		virtual TComplex Amp(double t) const;
		virtual TComplex Prf(double b) const;

	public:
		double a, b1, b2, p0, p1;
};

} // namespace

#endif
