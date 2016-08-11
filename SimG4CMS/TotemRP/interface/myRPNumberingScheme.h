#ifndef TotemRP_myRPNumberingScheme_h
#define TotemRP_myRPNumberingScheme_h

#include "SimG4CMS/TotemRP/interface/myTotemRPOrganization.h"

class myRPNumberingScheme : public myTotemRPOrganization {

public:
  myRPNumberingScheme(int i);
  ~myRPNumberingScheme();
	 
//  virtual unsigned int GetUnitID(const G4Step* aStep) const ;

};

#endif  //TotemRP_myRPNumberingScheme_h
