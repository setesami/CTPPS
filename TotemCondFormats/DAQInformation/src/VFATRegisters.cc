/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*   Hubert Niewiadomski
*    
* $Id: VFATRegisters.cc 4564 2011-07-29 11:48:25Z jkaspar $
* $Revision: 4564 $
* $Date: 2011-07-29 13:48:25 +0200 (Pt, 29 lip 2011) $
*
****************************************************************************/


#include "TotemCondFormats/DAQInformation/interface/VFATRegisters.h"

using namespace std;

VFATRegisters::VFATRegisters(unsigned int id)
{
  chipid0 = id & 0xff;
  chipid1 = (id >> 8) & 0xff;
  chipid2 = (id >> 16) & 0xff;

  fecSlot=0;
  fecHardwareId=0;
  ringSlot=0;
  ccuAddress=0;
  i2cChannel=0;
  i2cAddress=0;
  control0=0;
  control1=0;
  control2=0;
  control3=0;
  ipreampin=168;
  ipreampfeed=80;
  ipreampout=150;
  ishaper=150;
  ishaperfeed=100;
  icomp=120;
  upset=0;
  hitcount0=0;
  hitcount1=0;
  hitcount2=0;
  lat=127;
  vcal=100;
  vthreshold1=0;
  vthreshold2=26;
  calphase=0;

  chanreg.resize(128);
  for (unsigned int i=0; i<128; ++i)
    chanreg[i]=0;
}



unsigned char VFATRegisters::GetCalphaseBitNumber() const
{
  unsigned int n=calphase;
  unsigned int i=0;
  
  if(n==0)
    return n;
    
  while(n)
  {
    ++i;
    n=n>>1;
  }
  return i-1;
}



void VFATRegisters::SetCalphaseBitNumber(unsigned char calphasenumber_)
{
  calphase=(0x1<<calphasenumber_);
}
