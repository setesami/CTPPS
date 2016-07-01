/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#ifndef _elegent_coulomb_
#define _elegent_coulomb_

#include "Model.h"

namespace Elegent
{

/**
 * \brief Coulomb hadron interference for elastic scattering.
 **/
class CoulombInterference
{
	public:
		/// the mode of coulomb interference
		enum CIMode
		{
			mPC,				///< pure electromagnetic amplitude (Born/OPE approximation) [default]
			mPH,				///< pure hadronic amplitude
			mWY,				///< WY formula
			mSWY,			 	///< simplified WY formula
			mKL				 	///< (corrected) KL formula (includes the one of Cahn)
		} mode;

		std::string GetModeString() const;

		/// form factor type
		enum FFType
		{
			ffNone,				///< form factor = 1
			ffDipole,			///< dipole form factor, in G_eff, G_E and G_M (G_M(0) = 1)
			ffHofstadter,	 	///< Hofstader et al.: Rev. Mod. Phys. 30 (1958)
			ffBorkowski,		///< Borkowski et al.: Nucl. Phys. B93 (1975)
			ffKelly,			///< Kelly: Phys. Rev. C70 (2004)
			ffArrington,		///< Arrington et al.: Phys. Rev C76 (2007)
			ffPuckett,			///< Puckett et al.: arXiv 1008.0855v1 [default]
			ffPuckettEl		 	///< only electric form-factor of Puckett et al.
		} ffType;

		CoulombInterference();

		double GetT()
			{ return T; }

		/// the size of the region around t=t' which is cut off from integration, see B_term method
		double tau;
		
		/// the upper bound of the integration in A_term and B_term is |t|+T
		double T;

		/// precision of the integration
		double precision;

		/// print the parameters
		void Print() const;

	protected:
		/// the integrand of I(t, t') integral
		static double I_integrand(double *phi, double *par, const void *obj);

		/// the integrand of the A term
		static double A_integrand(double *tt, double *t, const void *obj);

		/// the integrand of the B term
		static TComplex B_integrand(double *tt, double *par, const void *obj);
		
	public:
		/// A: $\int_{t_{min}}^0 \log(t'/t) * d/dt(FF^2(t'))$
		/// \par t in GeV^2, negative
		double A_term(double t) const;

		double I_integral(double t, double tp) const;
		
		/// B: ${1 / 2\pi} \int_{t_{min}}^0 [ F^N(t') / F^N(t) - 1] I(t, t')$
		/// \par t in GeV^2, negative
		TComplex B_term(double t) const;

		/// C: the correction for non-vanishing form factors at t_min
		/// $FF^2(t_{min} \log(t/t_{min}))$
		/// \par t in GeV^2, negative
		double C_term(double t) const;

		//-------------------- form factors --------------------
		
	public:
		std::string GetFFName() const;

		/// dipole form factor
		double FF_dipole(double t) const;
	
		/// eletric form factor
		/// normalized such FF_e(0) = 1, t negative
		double FF_e(double t) const;

		/// magnetic form factor
		/// normalized such FF_m(0) = 1, t negative
		double FF_m(double t) const;
		
		/// square of the effective form factor
		/// \par t in GeV^2, negative
		double FF_sq(double t) const;

		/// d/dt of the effective form factor square
		/// \par t in GeV^2, negative
		double FF_sq_prime(double t) const
		{
			double ep = 1E-5;
			return (FF_sq(t + ep) - FF_sq(t)) / ep;
		}

		//-------------------- interference phases --------------------

		/// full West-Yennie phase (with alpha factor)
		/// that is the $\alpha\Phi$ in the decomposition $F^{C+H} = F^C e^{i \alpha \Phi} +	F^H$
		/// \par t in GeV^2, negative
		TComplex Phi_WY(double t) const;
		
		/// simplified West-Yennie phase
		TComplex Phi_SWY(double t) const;

		/// Kundrat-Lokajicek phase (with alpha factor)
		/// that is the $\alpha\Phi$ in the decomposition $F^{C+H} = F^C + F^H * e^{i \alpha \Psi}$
		/// \par t in GeV^2, negative
		TComplex Psi_KL(double t) const;

		///\brief the interference phase WITH the alpha factor
		/// returns either $-\Phi$ or $\Psi$
		/// \par t in GeV^2, negative
		TComplex Phase(double t) const;

		//-------------------- amplitudes --------------------

		/// pure Coulomb amplitude (PC)
		/// \par t in GeV^2, negative
		TComplex Amp_pure(double t) const;	
		
		TComplex Amp_WY(double t) const;	
		TComplex Amp_SWY(double t) const;	
		TComplex Amp_KL(double t) const;	

		/// total Amplitude according to the choice in `mode'
		TComplex Amp(double t) const;

		//-------------------- standard quantities --------------------

		/// ratio (|KL|^2 - |WY|^2) / |KL|^2
		/// \par t in GeV^2, negative
		TComplex R(double t) const;
		
		/// for |t| < |cutoff|: (|KL|^2 - |WY|^2) / |KL|^2, otherwise (|KL|^2 - |PH|^2) / |KL|^2
		/// \par t in GeV^2, negative
		/// \par cutoff in GeV^2, negative
		TComplex R_with_cutoff(double t, double cutoff) const;

		/// ratio (|KL|^2 - |PH|^2 - |PC|^2) / |KL|^2
		/// \par t in GeV^2, negative
		TComplex Z(double t) const;
		
		/// ratio (|KL|^2 - |PH|^2) / |PH|^2
		/// \par t in GeV^2, negative
		TComplex C(double t) const;
};

extern CoulombInterference *coulomb;

} // namespace

#endif
