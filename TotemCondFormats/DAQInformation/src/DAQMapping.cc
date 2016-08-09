/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*   Maciej Wróbel (wroblisko@gmail.com)
*    
* $Id: DAQMapping.cc 4564 2011-07-29 11:48:25Z jkaspar $
* $Revision: 4564 $
* $Date: 2011-07-29 13:48:25 +0200 (Pt, 29 lip 2011) $
*
****************************************************************************/

#include "FWCore/Utilities/interface/typelookup.h"

#include "TotemCondFormats/DAQInformation/interface/DAQMapping.h"

using namespace std;

std::ostream& operator << (std::ostream& s, const VFATInfo &vi)
{
  if (vi.type == VFATInfo::data)
    s << "type=data, ";
  else
    s << "type=  CC, ";

  s << vi.symbolicID << ", hw id=0x" << hex << vi.hwID << dec;

  return s;
}

//----------------------------------------------------------------------------------------------------

void DAQMapping::Insert(const Totem::FramePosition &fp, const VFATInfo &vi)
{
  maptype::iterator it = VFATMapping.find(fp);  
  if (it != VFATMapping.end()) {
    cerr << "WARNING in DAQMapping::Insert > Overwriting entry at " << fp << ". Previous: " << endl 
      << "    " << VFATMapping[fp] << "," << endl << "  new: " << endl << "    " << vi << ". " << endl;
  }

  VFATMapping[fp] = vi;
}

TYPELOOKUP_DATA_REG(DAQMapping);

