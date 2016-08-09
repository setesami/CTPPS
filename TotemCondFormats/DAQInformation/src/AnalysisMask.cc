/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*   Maciej Wróbel (wroblisko@gmail.com)
*    
* $Id: AnalysisMask.cc 4564 2011-07-29 11:48:25Z jkaspar $
* $Revision: 4564 $
* $Date: 2011-07-29 13:48:25 +0200 (Pt, 29 lip 2011) $
*
****************************************************************************/

#include "FWCore/Utilities/interface/typelookup.h"

#include "TotemCondFormats/DAQInformation/interface/AnalysisMask.h"


void AnalysisMask::Insert(const TotemSymbID &sid, const VFATAnalysisMask &vam)
{
  // TODO
  analysisMask[sid] = vam;
}

TYPELOOKUP_DATA_REG(AnalysisMask);

