/**************************************************
 * This file is a part of the Elegent package:
 * 	http://elegent.hepforge.org/
 *************************************************/

#include "interface/Math.h"

namespace Elegent
{

double DoubleInt(const void *obj, double (*fcn)(double*, double*, const void*), double a, double b, double *params, double epsilon)
{	
	const double Z1 = 1;
	const double HF = Z1/2;
	const double CST = 5*Z1/1000;
	
	double x[12] = { 0.96028985649753623,	0.79666647741362674,
										 0.52553240991632899,	0.18343464249564980,
										 0.98940093499164993,	0.94457502307323258,
										 0.86563120238783174,	0.75540440835500303,
										 0.61787624440264375,	0.45801677765722739,
										 0.28160355077925891,	0.09501250983763744};
	
	double w[12] = { 0.10122853629037626,	0.22238103445337447,
										 0.31370664587788729,	0.36268378337836198,
										 0.02715245941175409,	0.06225352393864789,
										 0.09515851168249278,	0.12462897125553387,
										 0.14959598881657673,	0.16915651939500254,
										 0.18260341504492359,	0.18945061045506850};
	
	double h, aconst, bb, aa, c1, c2, u, s8, s16, f1, f2;
	double xx[1];
	int i;
	
	h = 0;
	if (b == a) return h;
	
	aconst = CST/TMath::Abs(b-a);
	bb = a;
	
		CASE1:
	aa = bb;
	bb = b;
	
		CASE2:
	c1 = HF*(bb+aa);
	c2 = HF*(bb-aa);
	
	s8 = 0;
	for (i=0; i<4; i++) {
		u		 = c2*x[i];
		xx[0] = c1+u;
		f1		= (*fcn)(xx, params, obj);
		xx[0] = c1-u;
		f2		= (*fcn)(xx, params, obj);
		s8	 += w[i]*(f1 + f2);
		}
	
	s16 = 0;
	for (i=4; i<12; i++) {
		u		 = c2*x[i];
		xx[0] = c1+u;
		f1		= (*fcn)(xx, params, obj);
		xx[0] = c1-u;
		f2		= (*fcn)(xx, params, obj);
		s16	+= w[i]*(f1 + f2);
		}
	
	s16 = c2*s16;
	
	if (TMath::Abs(s16-c2*s8) <= epsilon*(1. + TMath::Abs(s16))) {
			h += s16;
			if(bb != b) goto CASE1;
		} else {
			bb = c1;
			if(1. + aconst*TMath::Abs(c2) != 1) goto CASE2;
			h = s8;	//this is a crude approximation (cernlib function returned 0 !)
			printf("WARNING in DoubleInt > Crude approximation.\n");
		}
		
	return h;
}

//----------------------------------------------------------------------------------------------------

TComplex CmplxInt(const void *obj, TComplex (*fcn)(double*, double*, const void *), double a, double b, double *params, double epsilon)
{	
	TComplex C;
	
	const double Z1 = 1;
	const double HF = Z1/2;
	const double CST = 5*Z1/1000;
	
	double x[12] = { 0.96028985649753623,	0.79666647741362674,
											 0.52553240991632899,	0.18343464249564980,
											 0.98940093499164993,	0.94457502307323258,
											 0.86563120238783174,	0.75540440835500303,
											 0.61787624440264375,	0.45801677765722739,
											 0.28160355077925891,	0.09501250983763744};
	
	double w[12] = { 0.10122853629037626,	0.22238103445337447,
											 0.31370664587788729,	0.36268378337836198,
											 0.02715245941175409,	0.06225352393864789,
											 0.09515851168249278,	0.12462897125553387,
											 0.14959598881657673,	0.16915651939500254,
											 0.18260341504492359,	0.18945061045506850};
	
	double aconst, bb, aa, c1, c2, u;
	double xx[1];
	int i;
	
	if (b == a) return TComplex(0., 0.);
	
	// REAL
	double Rh, Rs8, Rs16, Rf1, Rf2;
	
	Rh = 0;
	aconst = CST/TMath::Abs(b-a);
	bb = a;
		
		CASE1:
	aa = bb;
	bb = b;
		
		CASE2:
	c1 = HF*(bb+aa);
	c2 = HF*(bb-aa);
		
	Rs8 = 0;
	for (i=0; i<4; i++) {
		u = c2 * x[i];
		xx[0] = c1 + u;
		C = (*fcn)(xx, params, obj); Rf1 = C.Re();
		xx[0] = c1 - u;
		C = (*fcn)(xx, params, obj); Rf2 = C.Re();
		Rs8 += w[i]*(Rf1 + Rf2);
		}
		
	Rs16 = 0;
	for (i=4; i<12; i++) {
		u = c2 * x[i];
		xx[0] = c1 + u;
		C = (*fcn)(xx, params, obj); Rf1 = C.Re();
		xx[0] = c1 - u;
		C = (*fcn)(xx, params, obj); Rf2 = C.Re();
		Rs16 += w[i]*(Rf1 + Rf2);
		}
		
	Rs16 = c2*Rs16;
		
	if (TMath::Abs(Rs16 - c2*Rs8) <= epsilon*(1. + TMath::Abs(Rs16))) {
			Rh += Rs16;
			if(bb != b) goto CASE1;
		} else {
			bb = c1;
			if (1. + aconst*TMath::Abs(c2) != 1) goto CASE2;
			Rh = Rs8;	// this is a crude approximation (cernlib function returned 0 !)
			printf("WARNING in CmplxInt (real part) > Crude approximation.\n");
		}
	
	// IMAGINARY
	double Ih, Is8, Is16, If1, If2;
	
	Ih = 0;
	aconst = CST/TMath::Abs(b-a);
	bb = a;
		
		CASE1I:
	aa = bb;
	bb = b;
		
		CASE2I:
	c1 = HF*(bb+aa);
	c2 = HF*(bb-aa);
		
	Is8 = 0;
	for (i=0; i<4; i++) {
		u = c2 * x[i];
		xx[0] = c1 + u;
		C = (*fcn)(xx, params, obj); If1 = C.Im();
		xx[0] = c1 - u;
		C = (*fcn)(xx, params, obj); If2 = C.Im();
		Is8 += w[i]*(If1 + If2);
		}
		
	Is16 = 0;
	for (i=4; i<12; i++) {
		u = c2 * x[i];
		xx[0] = c1 + u;
		C = (*fcn)(xx, params, obj); If1 = C.Im();
		xx[0] = c1 - u;
		C = (*fcn)(xx, params, obj); If2 = C.Im();
		Is16 += w[i]*(If1 + If2);
		}
		
	Is16 = c2*Is16;
		
	if (TMath::Abs(Is16 - c2*Is8) <= epsilon*(1. + TMath::Abs(Is16))) {
			Ih += Is16;
			if(bb != b) goto CASE1I;
		} else {
			bb = c1;
			if (1. + aconst*TMath::Abs(c2) != 1) goto CASE2I;
			Ih = Is8;
			printf("WARNING in CmplxInt (imaginary part) > Crude approximation.\n");
		}
		
	// put it together 
	return TComplex(Rh, Ih);
}

} // namespace
