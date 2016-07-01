/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#ifndef _elegent_elegent_
#define _elegent_elegent_

#include <string>

class TGraph;

namespace HepMC {
	class GenEvent;
}

namespace Elegent
{

/**
 * \brief MC generator of proton-proton elastic scattering events.
 * All internal quantities are in GeV or in mm.
 **/
class Generator
{
	public:
		Generator(const std::string &_file, const std::string &_path, double _t_min, double _t_max, unsigned int _verbosity=1);
		~Generator() {}

		unsigned int Init();

		static const int PID = 2212;
		static const int ElasticScattering = 91;
		static const int FinalState = 1;
		static const int NullState = 0;

	protected:
		/// name of file containing the cumulative distribution function (CDF)
		std::string fileName;

		/// path of the (CDF) within the file
		std::string modelPath;

		/// |t| values in GeV^2, bounds for CDF
		double t_min, t_max;
		
		/// verbosity level (0 = no, 1 = normal, 2 = debug)
		unsigned int verbosity;

		/// [GeV] cms (one) proton energy
		double E_cms;

		/// [GeV] cms proton momentum
		double p_cms;

		/// graph with inverse c.d.f.
		TGraph *icdf;
	
	public:
		/// generates one event provided two random numbers with uniform distribution on (0, 1)
		void GenerateBase(double rn1, double rn2, HepMC::GenEvent* gE);

		/// generates one event, using ROOT random number generator TRandom2
		void Generate(HepMC::GenEvent* gE);
};

} // namespace

#endif
