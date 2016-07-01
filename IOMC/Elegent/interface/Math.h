/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#ifndef _elegent_math_
#define _elegent_math_

#include <TComplex.h>

namespace Elegent
{

// ================================== INTEGRATION ROUTINES =================================================

/**
 *\brief Integrates real-valued function `fcn' between points `a' and `b'.
 * `obj' is passed as the third parameter to `fcn'
 * `params' is passed as the second parameter to `fcn'
 * the integration variable is the first element of the array in the first parameter of `fcn'
 * `epsilon' controls the precision of the integration
 *
 * The function has been adapted from ROOT's TF1::Integral.
 **/
double DoubleInt(const void *obj, double (*fcn)(double*, double*, const void*), double a, double b, double *params = NULL, double epsilon = 1E-9);

/**
 *\brief Integrates complex-valued function `fcn' between points `a' and `b'.
 * `obj' is passed as the third parameter to `fcn'
 * `params' is passed as the second parameter to `fcn'
 * the integration variable is the first element of the array in the first parameter of `fcn'
 * `epsilon' controls the precision of the integration
 *
 * The function has been adapted from ROOT's TF1::Integral.
 **/
TComplex CmplxInt(const void *obj, TComplex (*fcn)(double*, double*, const void *), double a, double b, double *params = NULL, double epsilon = 1E-9);

} // namespace

#endif
