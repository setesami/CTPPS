/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#ifndef _elegent_model_
#define _elegent_model_

#include "TComplex.h"

#include <string>

namespace Elegent
{

/**
 * \brief The base class for hadronic models of (anti)proton-proton elastic scattering.
 **/
class Model
{
	public:
		/// collection of strings that describe model instance
		struct Label
		{
			std::string name, variant, version, mode;
		};

		/// full label (e.g. for figure legend)
		Label fullLabel;	

		/// short label (e.g. for object names in ROOT files)
		Label shortLabel;

		/// compiles a human readable string from fullLabel
		std::string CompileFullLabel() const;

		/// compiles a human readable string from shortLabel
		std::string CompileShortLabel() const;

		Model() {}

		virtual ~Model() {}

		/// TODO
		virtual void Init() =0;

		/// prints model info
		virtual void Print() const	=0;

		///\brief amplitude, t in GeV^-2, t < 0
		/// Normalisation is such that
		///   dsigma/dt = (\hbar c)^2 * \pi / (s p^2) * |Amp(t)|^2
		/// Differential cross-section can be obtained as
		///   Constants::sig_fac * |Amp(t)|^2
		virtual TComplex Amp(double t) const =0;

		///\brief profile function, b in fm
		/// Normalisation is such that
		///   Amp(t) = 2 p \sqrt{s} \int db b Prf() J_0(b \sqrt{-t})
		virtual TComplex Prf(double b) const =0;
};

/// current model
extern Model *model;

} // namespace

#endif
