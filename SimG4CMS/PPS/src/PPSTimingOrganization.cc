

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "SimG4CMS/PPS/interface/PPSTimingOrganization.h"
#include "DataFormats/PPSDetId/interface/PPSTimingDetId.h"

#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"
#include "G4Step.hh"

#include <iostream>

//******************************************************************** Constructor and destructor
PPSTimingOrganization :: ~PPSTimingOrganization()
{
}


uint32_t PPSTimingOrganization::GetUnitID(const G4Step* aStep) const
{
  return const_cast<PPSTimingOrganization *>(this)->GetUnitID(aStep);
}


uint32_t PPSTimingOrganization::GetUnitID(const G4Step* aStep)
{
  G4VPhysicalVolume* physVol;
  unsigned int arm = 0;
  unsigned int station = 0;
  unsigned int roman_pot = 0;
  unsigned int detector = 0;
  
  const G4VTouchable* touch = aStep->GetPreStepPoint()->GetTouchable();

  for(int ii = 0; ii < touch->GetHistoryDepth(); ii++ )
  {
    physVol = touch->GetVolume(ii);
    if(physVol->GetName() == "PhotoDetector_Window"||physVol->GetName() == "QLbar")
//    if(physVol->GetName() == "Lbar")

    {
      detector = physVol->GetCopyNo();
    }
   
 else if(physVol->GetName() == "PPS_Timing_Cylinder_Primary_Vacuum") // The physical volume just after Station

    {
      int cpy_no = physVol->GetCopyNo();
      arm = (cpy_no/100)%10;
      station = (cpy_no/10)%10;
      roman_pot = cpy_no%10;

    }
     
/////////////////////////////////////////////////////////
//   Allowed PPS Timing Copy Numbers and the RPs
//   147_Left copynum=0,1,2,3,4,5    147_Right copynum=100,101,102,103,104,105
//   215_Left copynum=10    215_Right copynum=110
//   220_Left copynum=20,21,22,23,24,25    220_Right copynum=120,121,122,123,124,125
////////////////////////////////////////////////////////  
  

#ifdef SCRIVI
    edm::LogInfo("PPS_Timing") << "physVol=" << physVol->GetName()<< ", level=" << ii
	  << ", physVol->GetCopyNo()=" << physVol->GetCopyNo()<< endl;
#endif
  }
   
//std::cout<<"arm: "<<arm<<" station: "<<station<<" romanpot: "<<roman_pot<<" detector: "<<detector<<std::endl;
//std::cout<<"Raw Det ID: "<<PPSTimingDetId(arm, station, roman_pot, detector).rawId()<<std::endl;
  return PPSTimingDetId(arm, station, roman_pot, detector).rawId();
}
