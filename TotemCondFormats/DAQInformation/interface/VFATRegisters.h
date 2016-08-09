/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*   Hubert Niewiadomski
*   Jan Kašpar
*    
* $Id: VFATRegisters.h 4564 2011-07-29 11:48:25Z jkaspar $
* $Revision: 4564 $
* $Date: 2011-07-29 13:48:25 +0200 (Pt, 29 lip 2011) $
*
****************************************************************************/


#ifndef _VFAT_Registers_
#define _VFAT_Registers_

#include <vector>

using namespace std;


/**
 *\brief Registers of VFAT2.
 **/
class VFATRegisters
{
  public:
    VFATRegisters(unsigned int id = 0);
    inline unsigned char GetSleepB() const {return (control0>>0)&(0x1);}
    inline void SetSleepB(unsigned char sleepb) {control0 = (control0 &(~(0x1<<0))) | ((sleepb & 0x1)<<0);}	//0-1
    inline unsigned char icompTrigMode() const {return (control0>>1)&(0x7);}
    inline void SetTrigMode(unsigned char trigmode) {control0 = (control0&(~(0x7<<1))) | ((trigmode & 0x7)<<1);}	//0-4
    inline unsigned char GetMSPolarity() const {return (control0>>4)&(0x1);}
    inline void SetMSPolarity(unsigned char mspolarity) {control0 = (control0&(~(0x1<<4))) | ((mspolarity & 0x1)<<4);}	//0-1
    inline unsigned char GetCalMode() const {return (control0>>6)&(0x3);}
    inline void SetCalMode(unsigned char calmode) {control0 = (control0&(~(0x3<<6))) | ((calmode & 0x3)<<6);}	//0-3
    
    inline unsigned char GetDACsel() const {return (control1>>0)&(0xf);}
    inline void SetDACsel(unsigned char dacsel) {control1 = (control1&(~(0xf<<0))) | ((dacsel & 0xf)<<0);}	//0-11
    inline unsigned char GetProbeMode() const {return (control1>>4)&(0x1);}
    inline void SetProbeMode(unsigned char probemode) {control1 = (control1&(~(0x1<<4))) | ((probemode & 0x1)<<4);}	//0-1
    inline unsigned char GetLVDSPowerSafe() const {return (control1>>5)&(0x1);}
    inline void SetLVDSPowerSafe(unsigned char vdspowersafe) {control1 = (control1&(~(0x1<<5))) | ((vdspowersafe & 0x1)<<5);}	//0-1
    inline unsigned char GetREHitCT() const {return (control1>>6)&(0x3);}
    inline void SetREHitCT(unsigned char rehitct) {control1 = (control1&(~(0x3<<6))) | ((rehitct & 0x3)<<6);}	//0-3
    
    inline unsigned char GetHitCountSel() const {return (control2>>0)&(0xf);}
    inline void SetHitCountSel(unsigned char hitcountsel) {control2 = (control2&(~(0xf<<0))) | ((hitcountsel & 0xf)<<0);}	//0-8
    inline unsigned char GetMSPulseLength() const {return (control2>>4)&(0x7);}
    inline void SetMSPulseLength(unsigned char mspulselength) {control2 = (control2&(~(0x7<<4))) | ((mspulselength & 0x7)<<4);}	//0-7
    inline unsigned char GetDigInSel() const {return (control2>>7)&(0x1);}
    inline void SetDigInSel(unsigned char diginsel) {control2 = (control2&(~(0x1<<7))) | ((diginsel & 0x1)<<7);}	//0-1
    
    inline unsigned char GetTrimDACrange() const {return (control3>>0)&(0x7);}
    inline void SetTrimDACrange(unsigned char trimdacrange) {control3 = (control3&(~(0x7<<0))) | ((trimdacrange & 0x7)<<0);}	//0-7
    inline unsigned char GetPbBG() const {return (control3>>3)&(0x1);}
    inline void SetPbBG(unsigned char pbbg) {control3 = (control3&(~(0x1<<3))) | ((pbbg & 0x1)<<3);}	//0-1
    inline unsigned char GetDFTestPAttern() const {return (control3>>4)&(0x1);}
    inline void SetDFTestPAttern(unsigned char dftestpattern) {control3 = (control3&(~(0x1<<4))) | ((dftestpattern & 0x1)<<4);}	//0-1
    
    //channels numbered from 0 to 127
    inline unsigned char GetChannelTrimDAC(unsigned int channel) const {return (chanreg[channel]>>0)&(0x1f);}
    inline void SetChannelTrimDAC(unsigned int channel, unsigned char trimdac) {chanreg[channel] = (chanreg[channel]&(~(0x1f<<0))) | ((trimdac & 0x1f)<<0);}	//0-31
    inline unsigned char GetChannelMask(unsigned int channel) const {return (chanreg[channel]>>5)&(0x1);}
    inline void SetChannelMask(unsigned int channel, unsigned char mask) {chanreg[channel] = (chanreg[channel]&(~(0x1<<5))) | ((mask & 0x1)<<5);}	//0-1
    inline unsigned char GetChannelCalChan(unsigned int channel) const {return (chanreg[channel]>>6)&(0x1);}
    inline void SetChannelCalChan(unsigned int channel, unsigned char calchan) {chanreg[channel] = (chanreg[channel]&(~(0x1<<6))) | ((calchan & 0x1)<<6);}
    
    inline unsigned int GetFullChipID() const {return (chipid0) | (chipid1<<8) |(chipid2<<16);}
    inline unsigned int GetDataChipID() const {return ((chipid0) | (chipid1<<8))&0xfff;}
    
    unsigned char GetCalphaseBitNumber() const;
    void SetCalphaseBitNumber(unsigned char calphasenumber_);
    
    
    //registers directly accessible
    inline unsigned int GetFecSlot() const {return fecSlot;}
    inline void SetFecSlot(unsigned int fecSlot_) {fecSlot=fecSlot_;}
    inline unsigned int GetFecHardwareId() const {return fecHardwareId;}
    inline void SetFecHardwareId(unsigned int fecHardwareId_) {fecHardwareId=fecHardwareId_;}
    inline unsigned int GetRingSlot() const {return ringSlot;}
    inline void SetRingSlot(unsigned int ringSlot_) {ringSlot=ringSlot_;}
    inline unsigned int GetCcuAddress() const {return ccuAddress;}
    inline void SetCcuAddress(unsigned int ccuAddress_) {ccuAddress=ccuAddress_;}
    inline unsigned int GetI2cChannel() const {return i2cChannel;}
    inline void SetI2cChannel(unsigned int i2cChannel_) {i2cChannel=i2cChannel_;}
    inline unsigned int GetI2cAddress() const {return i2cAddress;}
    inline void SetI2cAddress(unsigned int i2cAddress_) {i2cAddress=i2cAddress_;}
    
    inline unsigned char GetControl0() const {return control0;}
    inline void SetControl0(unsigned char control0_) {control0=control0_;}
    inline unsigned char GetControl1() const {return control1;}
    inline void SetControl1(unsigned char control1_) {control1=control1_;}
    inline unsigned char GetControl2() const {return control2;}
    inline void SetControl2(unsigned char control2_) {control2=control2_;}
    inline unsigned char GetControl3() const {return control3;}
    inline void SetControl3(unsigned char control3_) {control3=control3_;}
    
    inline unsigned char GetIpreampin() const {return ipreampin;}
    inline void SetIpreampin(unsigned char ipreampin_) {ipreampin=ipreampin_;}
    inline unsigned char GetIpreampfeed() const {return ipreampfeed;}
    inline void SetIpreampfeed(unsigned char ipreampfeed_) {ipreampfeed=ipreampfeed_;}
    inline unsigned char GetIpreampout() const {return ipreampout;}
    inline void SetIpreampout(unsigned char ipreampout_) {ipreampout=ipreampout_;}
    inline unsigned char GetIshaper() const {return ishaper;}
    inline void SetIshaper(unsigned char ishaper_) {ishaper=ishaper_;}
    inline unsigned char GetIshaperfeed() const {return ishaperfeed;}
    inline void SetIshaperfeed(unsigned char ishaperfeed_) {ishaperfeed=ishaperfeed_;}
    inline unsigned char GetIcomp() const {return icomp;}
    inline void SetIcomp(unsigned char icomp_) {icomp=icomp_;}
    
    inline unsigned char GetChipid0() const {return chipid0;}
    inline void SetChipid0(unsigned char chipid0_) {chipid0=chipid0_;}
    inline unsigned char GetChipid1() const {return chipid1;}
    inline void SetChipid1(unsigned char chipid1_) {chipid1=chipid1_;}
    inline unsigned char GetChipid2() const {return chipid2;}
    inline void SetChipid2(unsigned char chipid2_) {chipid2=chipid2_;}
    
    inline unsigned char GetUpset() const {return upset;}
    inline void SetUpset(unsigned char upset_) {upset=upset_;}
    inline unsigned char GetHitcount0() const {return hitcount0;}
    inline void SetHitcount0(unsigned char hitcount0_) {hitcount0=hitcount0_;}
    inline unsigned char GetHitcount1() const {return hitcount1;}
    inline void SetHitcount1(unsigned char hitcount1_) {hitcount1=hitcount1_;}
    inline unsigned char GetHitcount2() const {return hitcount2;}
    inline void SetHitcount2(unsigned char hitcount2_) {hitcount2=hitcount2_;}
    inline unsigned char GetLat() const {return lat;}
    inline void SetLat(unsigned char lat_) {lat=lat_;}
    inline unsigned char GetVcal() const {return vcal;}
    inline void SetVcal(unsigned char vcal_) {vcal=vcal_;}
    inline unsigned char GetVthreshold1() const {return vthreshold1;}
    inline void SetVthreshold1(unsigned char vthreshold1_) {vthreshold1=vthreshold1_;}
    inline unsigned char GetVthreshold2() const {return vthreshold2;}
    inline void SetVthreshold2(unsigned char vthreshold2_) {vthreshold2=vthreshold2_;}
    inline unsigned char GetCalphase() const {return calphase;}
    inline void SetCalphase(unsigned char calphase_) {calphase=calphase_;}
    
    //channels numbered from 0 to 127
    inline void SetChanReg(unsigned i, unsigned char chanregval_) {chanreg[i]=chanregval_;}
    inline unsigned char GetChanReg(unsigned i) const {return chanreg[i];}
    
  private:
    unsigned int fecSlot;
    unsigned int fecHardwareId;
    unsigned int ringSlot;
    unsigned int ccuAddress;
    unsigned int i2cChannel;
    unsigned int i2cAddress;
    unsigned char control0;
    unsigned char control1;
    unsigned char control2;
    unsigned char control3;
    unsigned char ipreampin;
    unsigned char ipreampfeed;
    unsigned char ipreampout;
    unsigned char ishaper;
    unsigned char ishaperfeed;
    unsigned char icomp;
    unsigned char chipid0;
    unsigned char chipid1;
    unsigned char chipid2;
    unsigned char upset;
    unsigned char hitcount0;
    unsigned char hitcount1;
    unsigned char hitcount2;
    unsigned char lat;
    typedef vector<unsigned char> ChanRegType;
    ChanRegType chanreg;
    unsigned char vcal;
    unsigned char vthreshold1;
    unsigned char vthreshold2;
    unsigned char calphase;
};

#endif

