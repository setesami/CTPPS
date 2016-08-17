/****************************************************************************
 * Author: Seyed Mohsen Etesami
 *	
 *	 
 *    
 * $$RCSfile: PPSTimingDetId.cc,v $: $
 * $Revision: 
 * $Date: 2015/05/20 $
 *
 ****************************************************************************/


#include "DataFormats/PPSDetId/interface/PPSTimingDetId.h"
#include "FWCore/Utilities/interface/Exception.h"

using namespace std;

// TOTEM =7, RP = 3
//CTPPS =8 , Timing=2

PPSTimingDetId::PPSTimingDetId():DetId(DetId::CTPPS,pps_timing_subdet_id)  //first constructor and defult
{}


PPSTimingDetId::PPSTimingDetId(uint32_t id):DetId(id)   //second constructor
{
  if (! Check(id))
    {
      throw cms::Exception("InvalidDetId") << "PPSTimingDetId ctor:"
					   << " det: " << det()
					   << " subdet: " << subdetId()
					   << " is not a valid PPS Timing id";  
    }
}


void PPSTimingDetId::init(unsigned int Arm, unsigned int Station,
		      unsigned int RomanPot, unsigned int Detector)
{
  if( Arm>=2 || Station>=3 || RomanPot>=1 || Detector>=40)   //in timing station=1 we have one RP for timing RomanPot>=6 change to RomanPot>=1
    {
      throw cms::Exception("InvalidDetId") << "PPSTimingDetId ctor:" 
					   << " Invalid parameters: " 
					   << " Arm "<<Arm
					   << " Station "<<Station
					   << " RomanPot "<<RomanPot
					   << " Detector "<<Detector
					   << std::endl;
    }

  uint32_t ok=0xfe000000;
  id_ &= ok;
  //0x3f= 3X8+15=39
  id_ |= ((Arm&0x1) << startArmBit);
  id_ |= ((Station&0x3) << startStationBit);
  id_ |= ((RomanPot&0x7) << startRPBit);
  id_ |= ((Detector&0x3f) << startDetBit);
}


PPSTimingDetId::PPSTimingDetId(unsigned int Arm, unsigned int Station,
			       unsigned int RomanPot, unsigned int Detector):       //third constructor with full id of all parts
  DetId(DetId::CTPPS,pps_timing_subdet_id)
{
  this->init(Arm,Station,RomanPot,Detector);
}


std::ostream& operator<<( std::ostream& os, const PPSTimingDetId& id )
{
  os << " Arm "<<id.Arm()
     << " Station "<<id.Station()
     << " RomanPot "<<id.RomanPot()
     << " Detector "<<id.Detector();

  return os;
}

//----------------------------------------------------------------------------------------------------

string PPSTimingDetId::SystemName(NameFlag flag)
{
  string name;
  if (flag == nFull) name = "rp";
  if (flag == nPath) name = "RP";

  return name;
}

//----------------------------------------------------------------------------------------------------

string PPSTimingDetId::ArmName(unsigned int id, NameFlag flag)
{
  string name;
  if (flag == nFull) name = SystemName(flag) + "_";
  if (flag == nPath) name = SystemName(flag) + "/sector ";

  if (id == 0) name += "45";
  if (id == 1) name += "56";

  return name;
}

//----------------------------------------------------------------------------------------------------

string PPSTimingDetId::StationName(unsigned int id, NameFlag flag)
{
  string name;
  if (flag == nFull) name = ArmName(id / 10, flag) + "_";
  if (flag == nPath) name = ArmName(id / 10, flag) + "/station ";

  if ((id % 10) == 0) name += "147";
  if ((id % 10) == 1) name += "215";   //needed for the station

 if ((id % 10) == 2) name += "220";

  return name;
}

//----------------------------------------------------------------------------------------------------

string PPSTimingDetId::RPName(unsigned int id, NameFlag flag)
{
  string name; 
  if (flag == nFull) name = StationName(id / 10, flag) + "_";
  if (flag == nPath) name = StationName(id / 10, flag) + "/";
  //FIXME need to be fixed
  if ((id % 10) == 0) name += "nr_tp";
  if ((id % 10) == 1) name += "nr_bt";
  if ((id % 10) == 2) name += "nr_hr";
  if ((id % 10) == 3) name += "fr_hr";
  if ((id % 10) == 4) name += "fr_tp";
  if ((id % 10) == 5) name += "fr_bt";
  return name;
}

//----------------------------------------------------------------------------------------------------

string PPSTimingDetId::PlaneName(unsigned int id, NameFlag flag)
{
  string name;
  if (flag == nFull) name = RPName(id / 10, flag) + "_";
  if (flag == nPath) name = RPName(id / 10, flag) + "/plane ";

  char buf[10];
  sprintf(buf, "%02u", (id % 10) + 1);

  return name + buf;
}

//----------------------------------------------------------------------------------------------------

string PPSTimingDetId::ChipName(unsigned int id, NameFlag flag)
{
  string name;
  if (flag == nFull) name = PlaneName(id / 10, flag) + "_";
  if (flag == nPath) name = PlaneName(id / 10, flag) + "/chip ";

  char buf[10];
  sprintf(buf, "%u", (id % 10) + 1);

  return name + buf;
}

//----------------------------------------------------------------------------------------------------

string PPSTimingDetId::StripName(unsigned int id, unsigned char strip, NameFlag flag)
{
  string name;
  if (flag == nFull) name = ChipName(id, flag) + "_";
  if (flag == nPath) name = ChipName(id, flag) + "/strip";

  char buf[10];
  sprintf(buf, "%u", strip);

  return name + buf;
}

//----------------------------------------------------------------------------------------------------

string PPSTimingDetId::OfficialName(ElementLevel level, unsigned int id, NameFlag flag, unsigned char strip)
{
  switch (level) {
    case lSystem: return SystemName(flag);
    case lArm: return ArmName(id, flag);
    case lStation: return StationName(id, flag);
    case lRP: return RPName(id, flag);
    case lPlane: return PlaneName(id, flag);
    case lChip: return ChipName(id, flag);
    case lStrip: return StripName(id, flag);
    default: return "";
  }
}


