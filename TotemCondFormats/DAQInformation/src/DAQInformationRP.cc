/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com)
*    
* $$RCSfile: DAQInformationRP.cc,v $: $
* $Revision: 4564 $
* $Date: 2011-07-29 13:48:25 +0200 (Pt, 29 lip 2011) $
*
****************************************************************************/

#include "FWCore/Utilities/interface/typelookup.h"

#include "TotemCondFormats/DAQInformation/interface/DAQInformationRP.h"

void DAQInformationRP::Reset()
{
  readoutPositionToId.clear();
  readoutIdToRegisters.clear();
  coincidencePositionToId.clear();
  coincidenceIdToRegisters.clear();
}


TYPELOOKUP_DATA_REG(DAQInformationRP);

