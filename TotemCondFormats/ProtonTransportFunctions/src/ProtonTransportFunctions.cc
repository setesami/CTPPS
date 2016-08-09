/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors:
*   Jan Kašpar (jan.kaspar@gmail.com)
*
* $$RCSfile: ProtonTransportFunctions.cc,v $: $
* $Revision: 1.1.2.1 $
* $Date: 2009/10/30 18:08:45 $
*
****************************************************************************/


#include "TotemCondFormats/ProtonTransportFunctions/interface/ProtonTransportFunctions.h"
#include "TotemCondFormats/BeamOpticsParamsObjects/interface/BeamOpticsParams.h"
#include "FWCore/Utilities/interface/typelookup.h"
#include "SimG4Core/TotemRPProtonTransportParametrization/interface/LHCOpticsApproximator.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "TFile.h"

using namespace std;

ProtonTransportFunctions::ProtonTransportFunctions()
{
//  printf(">> ProtonTransportFunctions::ProtonTransportFunctions\n");
}

//----------------------------------------------------------------------------------------------------

ProtonTransportFunctions::~ProtonTransportFunctions()
{
//  printf(">> ProtonTransportFunctions::~ProtonTransportFunctions\n");
}

/*
//----------------------------------------------------------------------------------------------------

LHCOpticsApproximator* ProtonTransportFunctions::GetFunction(const std::string &name) const
{
  for (vector<LHCOpticsApproximator *>::const_iterator it = functions.begin(); it != functions.end(); ++it)
    if (!name.compare((*it)->GetName())) return (*it);

  throw cms::Exception("ProtonTransportFunctions::GetFunction") << "Optical function `" << name << "' has not been found." << endl;
  return NULL;
}


//----------------------------------------------------------------------------------------------------
*/
void ProtonTransportFunctions::InitFunction(unsigned int RPId, LHCOpticsApproximator *of)
{
  functionMap[RPId].ideal = of;
  functionMap[RPId].real = new LHCOpticsApproximator(*of);

  //printf(">> ProtonTransportFunctions::InitFunction > ideal = %p, real = %p\n", functionMap[RPId].ideal, functionMap[RPId].real);
}

//----------------------------------------------------------------------------------------------------

LHCOpticsApproximator* ProtonTransportFunctions::GetFunction(unsigned int RPId) const
{
  map<unsigned int, FunctionPair>::const_iterator it = functionMap.find(RPId);
  if (it != functionMap.end()) return it->second.real;

  throw cms::Exception("ProtonTransportFunctions::GetFunction") << "RP Id " << RPId << " has not been found." << endl;
}


TYPELOOKUP_DATA_REG(ProtonTransportFunctions);
