#include "SimG4CMS/PPS/interface/PPSTimingNumberingScheme.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <iostream>
// #define debug

PPSTimingNumberingScheme::PPSTimingNumberingScheme(int i)
{
  edm::LogInfo("PPS") << " Creating PPSTimingNumberingScheme" << std::endl;
}

PPSTimingNumberingScheme::~PPSTimingNumberingScheme()
{
  edm::LogInfo("PPSTiming") << " Deleting PPSTimingNumberingScheme" << std::endl;
}
