/** \file

 */

#include <DataFormats/PPSDetId/interface/PPSTrackerDetId.h>
#include <FWCore/Utilities/interface/Exception.h>
#include "FWCore/MessageLogger/interface/MessageLogger.h"

// CTPPS =8, Tracker = 1

PPSTrackerDetId::PPSTrackerDetId():DetId(DetId::CTPPS,1),trind(0){}


PPSTrackerDetId::PPSTrackerDetId(uint32_t id):DetId(id),trind(0) {
  //  std::cout<<" constructor of the PPSTrackerDetId" <<std::endl;
  if (det()!=DetId::CTPPS || subdetId()!=1) {
    throw cms::Exception("InvalidDetId") << "PPSTrackerDetId ctor:"
					 << " det: " << det()
					 << " subdet: " << subdetId()
					 << " is not a valid PPSTracker id";  
  }
}



PPSTrackerDetId::PPSTrackerDetId(unsigned int Arm, unsigned int Station, unsigned int RP, unsigned int Plane):	      
  DetId(DetId::CTPPS,1),trind(0)
{
//  unsigned int d=0;
  this->init(Arm,Station,RP,Plane);
}

/*
PPSTrackerDetId::PPSTrackerDetId(unsigned int Arm, unsigned int Plane, unsigned int CSC, unsigned int Layer):	      
  DetId(DetId::Totem,1),trind(0)
{
  this->init(Arm,Plane,CSC,Layer);
}
*/

void
PPSTrackerDetId::init(unsigned int Arm, unsigned int Station, unsigned int RP, unsigned int Plane)
{
  if ( 
      (Arm != 0 && Arm !=1) || Station != 0 ||
      Plane > 5 ||
      RP > 3 || RP < 2
      ) {
    edm::LogInfo("PPSSim") <<Arm << " " << RP << " " << Plane;
    throw cms::Exception("InvalidDetId") << "PPSTrackerDetId ctor:" 
					 << " Invalid parameterss: " 
					 << " Arm "<<Arm
					 << " RP "<<RP
					 << " Plane "<<Plane
					 << std::endl;
  }

  uint32_t ok=0x82000000;
  id_ &= ok;
/*
  id_ |=  Plane   << 20    | 
    RP    << 23    |
    Arm  << 24 ;
*/
    id_ |= ((Arm&0x1) << startArmBit);
  id_ |= ((Station&0x3) << startStationBit);
  id_ |= ((RP&0x7) << startRPBit);
  id_ |= ((Plane&0x7) << startPlaneBit);
//  std::cout  << id_ << " " << Arm << " " << RP << " "<< Plane << std::endl; 

}

std::ostream& operator<<( std::ostream& os, const PPSTrackerDetId& id ){
  os <<  " Arm "<<id.Arm()
     << " RP "<<id.RP()
     << " Plane "<<id.Plane();

  return os;
}
