#include "SimG4CMS/TotemRP/interface/myRPNumberingScheme.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <iostream>
// #define debug

myRPNumberingScheme::myRPNumberingScheme(int i)
{
  edm::LogInfo("TotemRP") << " Creating myRPNumberingScheme" << std::endl;
}

myRPNumberingScheme::~myRPNumberingScheme()
{
  edm::LogInfo("TotemRP") << " Deleting myRPNumberingScheme" << std::endl;
}
