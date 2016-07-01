/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#ifndef _elegent_constants_
#define _elegent_constants_

#include "TComplex.h"

namespace Elegent
{

/**
 *\brief Set of constants used in Elegent calculations.
 **/
struct Constants
{
	// physics constants
	static double alpha;					///< fine structure constant
	static double proton_mass;				///< GeV
	static double neutron_mass;				///< GeV
	static double hbarc;					///< GeV * fm
	static double sq_hbarc;					///< GeV^2 * mbarn
											///< sigma/mbarn = sigma/GeV^-2 * sq_hbarc
	static double M;						///< abbreviation for proton mass in GeV
	static double M_sq;						///< proton mass squared, GeV^2
	static double kappa;					///< the anomalous magnetic moment of proton
	
	// mathematics constants
	static double pi;						///< pi
	static double gamma;					///< Euler's constant

	// physics data
	double sqrt_s;							///< sqrt_s / GeV
	double s;								///< s / GeV^2
	double ln_s;							///< ln(s / GeV^2)
	double p_cms;							///< particle CMS impuls
	double sig_fac;							///< d sig/dt = sig_fac * |A|^2
	double t_min;							///< negative
	
	enum ParticleMode {mPP, mAPP} pMode; 	///< particle mode
 
	/// \param W = sqrt(s)
	Constants(double W = 0., ParticleMode mode = mPP)
	{
		Configure(W, mode);
	}
	
	void Configure(double W, ParticleMode mode);

	/// \brief initilize new instance of Constants and save its pointer to the `cnts' global variable
	/// \param W = sqrt(s)
	static void Init(double W, ParticleMode mode);

	/// print the actual values
	void Print();
};


extern TComplex i;
extern Constants *cnts;

} // namespace

#endif
