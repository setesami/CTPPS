#ifndef _PPS_TrackerNumberingScheme_h
#define _PPS_TrackerNumberingScheme_h 1
// -*- C++ -*-
//
// Package:     PPS
// Class  :     PPSTrackerNumberingScheme
//
/**\class PPSTrackerNumberingScheme PPSTrackerNumberingScheme.h SimG4CMS/Forward/interface/PPSTrackerNumberingScheme.h
 
 Description: This class manages the UnitID that labels PPS sensitive
              volumes
 
 Usage:
    Used in PPSSD to get unique ID of sensitive detector element
 
*/
//
// Original Author: 
//         Created:  Tue May 16 10:14:34 CEST 2006
//
 
// system include files

// user include files

#include "SimG4CMS/PPS/interface/PPSTrackerOrganization.h"

class PPSTrackerNumberingScheme : public PPSTrackerOrganization {

public:

  // ---------- Constructor and destructor -----------------
  PPSTrackerNumberingScheme();
  ~PPSTrackerNumberingScheme();
	 
  //  virtual uint32_t GetUnitID(const G4Step* aStep) const ;

};

#endif
