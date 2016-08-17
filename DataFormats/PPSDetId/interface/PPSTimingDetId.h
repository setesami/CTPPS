/****************************************************************************
 *
 * This is a part of CT-PPS offline software.
 * Author: 
 *        Seyed Mohsen Etesami
 *
 *    
 * $$RCSfile: PPSTimingDetId.h,v $: $
 * $Revision: 1.9.2.6 $
 * $Date: 2015/05/27 9:50:56 $
 *
 ****************************************************************************/

#ifndef PPSTimingDetId_h
#define PPSTimingDetId_h


#include "DataFormats/DetId/interface/DetId.h"
#include <FWCore/Utilities/interface/Exception.h>

#include <iosfwd>
#include <iostream>
#include <string>

/**
 *\brief Roman Pot detector ID.
 *
 * There are 3 types of IDs used in CMSSW in the context of RP.
 * \li "class ID" - this class TotRPDetId, a daughter of DetId
 * \li "raw ID" - unsigned int, the result of rawId() method
 * \li "decimal or symbolic ID" - 4 decimal digit unsigned int, |arm|station|RP|det|
 *
 * The structure of the raw ID is the following (based on the concept of the DetId)
 * Bit 24 = Arm: 1=z>0 0=z<0
 * Bits [22:23] Station
 * Bits [19:21] Roman Pot number
 * Bits [15:18] Si det. number
 *
 * The advantage of the symbolic ID is that it is easily readable and that it can address most of the elements int the RP subdetector system:
 * chip ID = |arm|station|RP|det|VFAT|, ie. 5-digit decimal number (possibly with leading zeros)\n
 * detector ID = |arm|station|RP|det|\n
 * Roman Pot ID =  |arm|station|RP|\n
 * station ID =   |arm|station|\n
 * arm ID =     |arm|\n
 * where
 * \li arm = 0 (left, i.e. z < 0), 1 (right)
 * \li station = 0 (147m), 1 (180m), 2 (220m)
 * \li RP = 0 - 5; 0+1 vertical pots (lower |z|), 2+3 horizontal pots, 4+5 vertical pots (higher |z|)
 * \li det = 0 - 9 (0th det has the lowest |z|)
 * \li VFAT = 0 - 4
 *
 * Moreover there is an official naming scheme (EDMS 906715). It is supported by the ...Name() methods.
**/





/**
 *  PPS  The structure of the raw ID is the following (based on the concept of the DetId)
 *      [28:31] 4 bits Sub-detectors
 *      [25:27] 2 bits  PPStracking =1 PPSTiming=2
 * Bit  [24]    1 bit  Arm  1=z>0 0=z<0
 * Bits [22:23] 2 bits Station 0=210m 1=215m(timing) 2=220m
 * Bits [19:21] 3 bits Roman Pot number 0= only RP
 * Not apply this bit Bit 18= Timing detector Set 0= first set 1=second set(removed not sure if it is compatible with tracker   )
 * Bits [13:18] 6 bits lbar det. number if  sets of Lbars detectors spearatly 0,1,2,..39
 *   Raw means the id included all 32 bit with main sub-detectors and sub-sub-dtectros included
**/
class PPSTimingDetId : public DetId
{  
 public:
  PPSTimingDetId();
  
  /// Construct from a raw id. It is required that the Detector part of
  /// id is Totem and the SubDet part is RP, otherwise an exception is thrown.
  explicit PPSTimingDetId(uint32_t id);
  
  /// Construct from fully qualified identifier.
  PPSTimingDetId(unsigned int Arm, unsigned int Station,
	     unsigned int RomanPot, unsigned int Detector);
      
  
  inline int Arm() const
    {
      return int((id_>>startArmBit) & 0x1);
    }
  inline int Station() const
    {
      return int((id_>>startStationBit) & 0x3);
    }
  inline int RomanPot() const
    {
      return int((id_>>startRPBit) & 0x7);
    }
  inline int Detector() const
    {
      return int((id_>>startDetBit) & 0x3f); //decimal equ=63
    }
  int RPCopyNumber() const {return RomanPot() + 10*Station() + 100*Arm();}

  bool IsStripsCoordinateUDirection() const {return Detector()%2;}
  bool IsStripsCoordinateVDirection() const {return !IsStripsCoordinateUDirection();}

  /// is Detector u-detector?
  /// expect symbolic/decimal ID
  static bool IsStripsCoordinateUDirection(int Detector) { return Detector%2; }
    
  inline unsigned int DetectorDecId() const
    {
      return Detector()+RomanPot()*10+Station()*100+Arm()*1000;
    }

  //-------------------------------- static members ---------------------------------------

  static const unsigned int startArmBit = 24;
  static const unsigned int startStationBit = 22;
  static const unsigned int startRPBit = 19;
  static const unsigned int startDetBit = 13; 
  // static const unsigned int totem_rp_subdet_id = 3;
  static const unsigned int pps_timing_subdet_id = 2;

  /* /// returns true it the raw ID is a TOTEM RP  one */
  /* static bool Check(unsigned int raw) */
  /* { */
  /*   return (((raw >>DetId::kDetOffset) & 0xF) == DetId::Totem && */
  /*     ((raw >> DetId::kSubdetOffset) & 0x7) == totem_rp_subdet_id); */
  /* } */

 /// returns true if the raw ID is a PPS-timing one
  static bool Check(unsigned int raw)
  {
    return (((raw >>DetId::kDetOffset) & 0xF) == DetId::CTPPS &&
      ((raw >> DetId::kSubdetOffset) & 0x7) == pps_timing_subdet_id);
  }





  /// fast conversion Raw to Decimal ID
  static unsigned int RawToDecId(unsigned int raw)
    {
      return ((raw >> startArmBit) & 0x1) * 1000 + ((raw >> startStationBit) & 0x3) * 100 +
	((raw >> startRPBit) & 0x7) * 10 + ((raw >> startDetBit) & 0x3f);
    }

  /// fast conversion Decimal to Raw ID
  static unsigned int DecToRawId(unsigned int dec)
    {
      unsigned int i = (DetId::CTPPS << DetId::kDetOffset)
        | (pps_timing_subdet_id << DetId::kSubdetOffset);
      i &= 0xfe000000;
      i |= ((dec % 10) & 0x3f) << startDetBit; dec /= 10;
      i |= ((dec % 10) & 0x7) << startRPBit; dec /= 10;
      i |= ((dec % 10) & 0x3) << startStationBit; dec /= 10;
      i |= ((dec % 10) & 0x1) << startArmBit;
      return i;
    }

  /// returns ID of RP for given detector ID ''i''
  static unsigned int RPOfDet(unsigned int i) { return i / 10; }

  /// returns ID of station for given detector ID ''i''
  static unsigned int StOfDet(unsigned int i) { return i / 100; }

  /// returns ID of arm for given detector ID ''i''
  static unsigned int ArmOfDet(unsigned int i) { return i / 1000; }

  /// returns ID of station for given RP ID ''i''
  static unsigned int StOfRP(unsigned int i) { return i / 10; }

  /// returns ID of arm for given RP ID ''i''
  static unsigned int ArmOfRP(unsigned int i) { return i / 100; }

  /// returns ID of arm for given station ID ''i''
  static unsigned int ArmOfSt(unsigned int i) { return i / 10; }
     


  /// type of name returned by *Name functions
  enum NameFlag {nShort, nFull, nPath};

  /// level identifier in the RP hierarchy
  enum ElementLevel {lSystem, lArm, lStation, lRP, lPlane, lChip, lStrip};

  /// returns the name of the RP system
  static std::string SystemName(NameFlag flag = nFull);

  /// returns official name of an arm characterized by ''id''; if ''full'' is true, prefix rp_ added
  static std::string ArmName(unsigned int id, NameFlag flag = nFull);

  /// returns official name of a station characterized by ''id''; if ''full'' is true, name of arm is prefixed
  static std::string StationName(unsigned int id, NameFlag flag = nFull);
  //
  /// returns official name of a RP characterized by ''id''; if ''full'' is true, name of station is prefixed
  static std::string RPName(unsigned int id, NameFlag flag = nFull);
  
  /// returns official name of a plane characterized by ''id''; if ''full'' is true, name of RP is prefixed
  static std::string PlaneName(unsigned int id, NameFlag flag = nFull);
  
  /// returns official name of a chip characterized by ''id''; if ''full'' is true, name of plane is prefixed
  static std::string ChipName(unsigned int id, NameFlag flag = nFull);
  
  /// returns official name of a strip characterized by ''id'' (of chip) and strip number; if ''full'' is true, name of chip is prefixed
  static std::string StripName(unsigned int id, unsigned char strip, NameFlag flag = nFull);

  /// shortcut to use any of the *Name methods, given the ElementLevel
  static std::string OfficialName(ElementLevel level, unsigned int id, NameFlag flag = nFull, unsigned char strip = 0);


 private:
  inline void init(unsigned int Arm, unsigned int Station, unsigned int RomanPot, unsigned int Detector);
};

//std::ostream& operator<<(std::ostream& os, const TotRPDetId& id);
std::ostream& operator<<(std::ostream& os, const PPSTimingDetId& id);

#endif 

