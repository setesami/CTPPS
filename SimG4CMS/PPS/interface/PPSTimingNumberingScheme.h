#ifndef PPS_PPSTimingNumberingScheme_h
#define PPS_PPSTimingNumberingScheme_h

#include "SimG4CMS/PPS/interface/PPSTimingOrganization.h"

class PPSTimingNumberingScheme : public PPSTimingOrganization {

public:
  PPSTimingNumberingScheme(int i);
  ~PPSTimingNumberingScheme();
	 
//  virtual unsigned int GetUnitID(const G4Step* aStep) const ;

};

#endif  //PPS_PPSTimingNumberingScheme_h
