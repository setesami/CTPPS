/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*   Maciej Wróbel (wroblisko@gmail.com)
*	Tuomas Tajakka
*   Jan Kašpar (jan.kaspar@cern.ch)
*    
* $Id: DAQMapping.h 4564 2011-07-29 11:48:25Z jkaspar $
* $Revision: 4564 $
* $Date: 2011-07-29 13:48:25 +0200 (Pt, 29 lip 2011) $
*
****************************************************************************/

#ifndef _DAQMapping_h_
#define _DAQMapping_h_

#include "TotemRawDataLibrary/DataFormats/interface/FramePosition.h"
#include "TotemCondFormats/DAQInformation/interface/TotemSymbId.h"

#include <map>


/**
 *\brief Contains mappind data related to a VFAT.
 */
class VFATInfo
{
  public:
    /// is data of coincidence-chip VFAT
    enum {data, CC} type;

    /// the symbolic id
    TotemSymbID symbolicID;

    /// the hardware ID (16 bit)
    unsigned int hwID;
    
    friend std::ostream& operator << (std::ostream& s, const VFATInfo &fp);
};



/**
 *\brief The mapping between FramePosition and VFATInfo.
 */
class DAQMapping
{
  public:
    typedef std::map<Totem::FramePosition, VFATInfo> maptype;
    maptype VFATMapping;
    
    void Insert(const Totem::FramePosition &fp, const VFATInfo &vi);
};

#endif

