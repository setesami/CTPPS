#ifndef PPSTimingVDetectorOrganization_h
#define PPSTimingVDetectorOrganization_h

#include "G4Step.hh"
#include <boost/cstdint.hpp>

class PPSTimingVDetectorOrganization {

public:
  PPSTimingVDetectorOrganization(){};
  virtual ~PPSTimingVDetectorOrganization(){};   
  virtual uint32_t GetUnitID(const  G4Step* aStep) const =0;
};      


#endif  //PPSTimingVDetectorOrganization_h
