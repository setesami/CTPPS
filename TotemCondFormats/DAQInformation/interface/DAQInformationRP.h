/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com)
*    
* $$RCSfile: DAQInformationRP.h,v $: $
* $Revision: 4564 $
* $Date: 2011-07-29 13:48:25 +0200 (Pt, 29 lip 2011) $
*
****************************************************************************/

#ifndef _DAQInformationRP_h_
#define _DAQInformationRP_h_


#include "TotemRawDataLibrary/DataFormats/interface/FramePosition.h"
#include "TotemCondFormats/DAQInformation/interface/VFATRegisters.h"

#include <map>

using namespace Totem;


/**
 *\brief Container for RP related DAQ data (mappings etc.).
 */
class DAQInformationRP
{
  public:
    /// 5 digit decimal integer
    typedef unsigned int VFATID;

    /// 3 digit decimal integer
    typedef unsigned int RPID;

    /// map: DAQ channel --> VFAT Id, for readout VFATs
    std::map<FramePosition, VFATID> readoutPositionToId;
    
    /// map: VFAT Id --> VFAT registers, for readout VFATs
    std::map<VFATID, VFATRegisters> readoutIdToRegisters;

    /// map: DAQ channel --> RP Id, here the RP Id identifies the trigger VFAT of that RP
    std::map<FramePosition, RPID> coincidencePositionToId;

    /// map: RP Id --> VFAT registers, here the RP Id identifies the trigger VFAT of that RP
    std::map<RPID, VFATRegisters> coincidenceIdToRegisters;

    void Reset();
};


#endif
