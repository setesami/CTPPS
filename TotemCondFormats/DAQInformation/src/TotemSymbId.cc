/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@cern.ch)
*    
* $Id: TotemSymbId.h 4981 2011-09-05 13:13:55Z pkaczmar $
* $Revision: 4981 $
* $Date: 2011-09-05 15:13:55 +0200 (Mon, 05 Sep 2011) $
*
****************************************************************************/

#include "TotemCondFormats/DAQInformation/interface/TotemSymbId.h"

std::ostream& operator << (std::ostream& s, const TotemSymbID &sid)
{
  switch (sid.subSystem) {
    case TotemSymbID::RP:
      s << "sub-system=RP, ";
      break;
    case TotemSymbID::T1:
      s << "sub-system=T1, ";
      break;
    case TotemSymbID::T2:
      s << "sub-system=T2, ";
      break;
  }

  s << "symb. id=" << sid.symbolicID;

  return s;
}

