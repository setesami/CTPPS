/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*   Maciej Wróbel (wroblisko@gmail.com)
*   Jan Kašpar (jan.kaspar@cern.ch)
*    
* $Id: AnalysisMask.h 4564 2011-07-29 11:48:25Z jkaspar $
* $Revision: 4564 $
* $Date: 2011-07-29 13:48:25 +0200 (Pt, 29 lip 2011) $
*
****************************************************************************/

#ifndef _AnalysisMask_h_
#define _AnalysisMask_h_

#include "TotemCondFormats/DAQInformation/interface/TotemSymbId.h"

#include <set>
#include <map>

/**
 *\brief Contains data on masked channels of a VFAT.
 */
class VFATAnalysisMask
{
  public:
    VFATAnalysisMask() : fullMask(false) {}

    /// whether all channels of the VFAT shall be masked
    bool fullMask;

    /// list of channels to be masked
    std::set<unsigned char> maskedChannels;
};


/**
 *\brief Channel-mask mapping.
 **/
class AnalysisMask
{
  public:
    std::map<TotemSymbID, VFATAnalysisMask> analysisMask;

    void Insert(const TotemSymbID &sid, const VFATAnalysisMask &vam);
};

#endif

