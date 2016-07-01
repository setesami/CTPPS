/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#include "interface/InterpolationModel.h"

#include "TGraph.h"

namespace Elegent
{

//#define DEBUG 1

//----------------------------------------------------------------------------------------------------

InterpolationModel::InterpolationModel(unsigned int _N, double _t_min, double _t_max) :
	N(_N), t_min(_t_min), t_max(_t_max), dt( (t_max - t_min) / (N-1) ), amp_data(N)
{
	fullLabel.name = "Interpolation"; shortLabel.name = "itpl";
}

//----------------------------------------------------------------------------------------------------

void InterpolationModel::Configure()
{
}

//----------------------------------------------------------------------------------------------------

InterpolationModel::~InterpolationModel()
{
}

//----------------------------------------------------------------------------------------------------

void InterpolationModel::Print() const
{
	printf(">> InterpolationModel::Print\n");
	printf("\tN = %u, t_min = %.3E, t_max = %.3E\n", N, t_min, t_max);
}

//----------------------------------------------------------------------------------------------------

TComplex InterpolationModel::Prf(double) const
{
	// this function is not planned to be used
	printf(">> InterpolationModel::Prf > not implemented.\n");
	return 0.;
}


} // namespace
