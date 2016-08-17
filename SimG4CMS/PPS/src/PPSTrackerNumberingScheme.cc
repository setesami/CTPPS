// -*- C++ -*-
//
// Package:     PPS
// Class  :     PPSTrackerNumberingScheme
//
// Implementation:
//     <Notes on implementation>
//
// Original Author: 
//         Created:  Tue May 16 10:14:34 CEST 2006
//

// system include files

// user include files
#include "SimG4CMS/PPS/interface/PPSTrackerNumberingScheme.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//
// constructors and destructor
//
PPSTrackerNumberingScheme::PPSTrackerNumberingScheme() {

  edm::LogInfo("PPSSim") << " Creating PPSTrackerNumberingScheme";
//  SetCurrentDetectorPosition();
}

PPSTrackerNumberingScheme::~PPSTrackerNumberingScheme() {
  edm::LogInfo("PPSSim") << " Deleting PPSTrackerNumberingScheme";
}
