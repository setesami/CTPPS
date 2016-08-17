///////////////////////////////////////////////////////////////////////////////
// File: PPS_Timing_SD.cc
// Date: 26.05.2015
//Author: Seyed Mohsen Etesami
// Description: Sensitive Detector class for PPS Timing Detectors
// Modifications: 
///////////////////////////////////////////////////////////////////////////////
#include "SimG4CMS/PPS/interface/PPS_Timing_SD.h"
#include "SimG4CMS/PPS/interface/PPSTimingNumberingScheme.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "SimG4Core/Notification/interface/TrackInformation.h"
#include "SimG4Core/Notification/interface/G4TrackToParticleID.h"
//#include "SimG4Core/Geometry/interface/SDCatalog.h"
#include "SimG4Core/Geometry/interface/SensitiveDetectorCatalog.h"
#include "SimG4Core/Physics/interface/G4ProcessTypeEnumerator.h"

#include "SimG4Core/SensitiveDetector/interface/SensitiveTkDetector.h"

#include "SimDataFormats/TrackingHit/interface/UpdatablePSimHit.h"
#include "SimDataFormats/SimHitMaker/interface/TrackingSlaveSD.h"

//#include "Geometry/Vector/interface/LocalPoint.h"
//#include "Geometry/Vector/interface/LocalVector.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalVector.h"
#include "G4LogicalBorderSurface.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
//


#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

//pps added
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "SimG4Core/Notification/interface/TrackInformation.h"

#include <iostream>
#include <vector>
#include <string>


PPS_Timing_SD::PPS_Timing_SD(std::string name, const DDCompactView & cpv, const SensitiveDetectorCatalog & clg,
     edm::ParameterSet const & p, const SimTrackManager* manager) : 
  SensitiveTkDetector(name, cpv, clg, p), 
  numberingScheme(0),  
  name(name), hcID(-1), theHC(0), currentHit(0), theTrack(0), currentPV(0), 
  unitID(0),  preStepPoint(0), postStepPoint(0), eventno(0)
{


PPS_Timing_SD::initRun();



  collectionName.insert(name);
std::cout<<"collectionName[0]: "<<collectionName[0]<<std::endl;
  
  edm::ParameterSet m_Anal = p.getParameter<edm::ParameterSet>("PPS_Timing_SD");
  verbosity_ = m_Anal.getParameter<int>("Verbosity");
  
// LogDebug("TotemRP")
  std::cout
    << "*******************************************************\n"
    << "*                                                     *\n"
    << "* Constructing a PPS_Timing_SD  with name " << name << "\n"
    << "*                                                     *\n"
    << "*******************************************************" << std::endl;


  slave  = new TrackingSlaveSD(name);


//PPS_Timing_MaterialProperties::DumpSurfaceInfo();
 

 //
  // Now attach the right detectors (LogicalVolumes) to me
  //
  
  //std::vector<std::string> lvNames = SensitiveDetectorCatalog::instance()->logicalNames(name);
  std::vector<std::string> lvNames = clg.logicalNames(name);
  this->Register();
  for (std::vector<std::string>::iterator it=lvNames.begin();  it !=lvNames.end(); it++)
  {

    std::cout<<"name: "<<name<<std::endl;
    std::cout<<"lvNames: "<<*it<<std::endl;
    this->AssignSD(*it);
    edm::LogInfo("PP_Timing_SD") << "PP_Timing_SD : Assigns SD to LV " << (*it);
  }

  

  if (name == "PPSTimingHits")  
{

    numberingScheme = dynamic_cast<PPSTimingVDetectorOrganization*>(new PPSTimingNumberingScheme(3));
  }
  else 
  {
    edm::LogWarning("PP_Timing_SD") << "PP_Timing_SD: ReadoutName not supported\n";
  }
  
  edm::LogInfo("PP_Timing_SD") << "PP_Timing_SD: Instantiation completed";

}


PPS_Timing_SD::~PPS_Timing_SD()
{ 
  if (slave)
    delete slave; 
  if (numberingScheme)
    delete numberingScheme;
}

void PPS_Timing_SD::Initialize(G4HCofThisEvent * HCE) {
    LogDebug("PP_Timing_SD") << "PP_Timing_SD : Initialize called for " << name;

    std::cout << "PPS_Timing_SD: Initialize called for:   " << name << std::endl << std::endl << std::endl;
    theHC = new PPS_Timing_G4HitCollection(name, collectionName[0]);
    G4SDManager::GetSDMpointer()->AddNewCollection(name, collectionName[0]);

    if (hcID < 0)
        hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    HCE->AddHitsCollection(hcID, theHC);
    theTrackSwitchVecF.push_back(-1);
    theTrackSwitchVecS.push_back(-1);

}


void PPS_Timing_SD::Print_Hit_Info()
{
//  LogDebug("PP_Timing_SD")
std::cout 
<< theTrack->GetDefinition()->GetParticleName()
       << " PPS_Timing_SD CreateNewHit for"
       << " PV "     << currentPV->GetName()
       << " PVid = " << currentPV->GetCopyNo()
       //<< " MVid = " << currentPV->GetMother()->GetCopyNo()
       << " Unit "   << unitID;
  //LogDebug("PP_Timing_SD") 
std::cout
<< " primary "    << primaryID
       << " time slice " << tSliceID 
       << " of energy " << theTrack->GetTotalEnergy()
       << " Eloss " << Eloss
       << " positions ";
       printf("(%10f,%10f,%10f)",preStepPoint->GetPosition().x(),preStepPoint->GetPosition().y(),preStepPoint->GetPosition().z());
       printf("(%10f,%10f,%10f)",postStepPoint->GetPosition().x(),postStepPoint->GetPosition().y(),postStepPoint->GetPosition().z());
//  LogDebug("PP_Timing_SD") 
std::cout
<< " positions " << "(" <<postStepPoint->GetPosition().x()<<","<<postStepPoint->GetPosition().y()<<","<<postStepPoint->GetPosition().z()<<")"
       << " For Track  " << theTrack->GetTrackID()
       << " which is a " << theTrack->GetDefinition()->GetParticleName()
       << " ParentID is " << theTrack->GetParentID()<<std::endl<<std::endl;
     
  if(theTrack->GetTrackID()==1)
  {
    LogDebug("PP_Timing_SD") << " primary particle ";
  }
  else
  {
    LogDebug("PP_Timing_SD") << " daughter of part. " << theTrack->GetParentID();
  }

  LogDebug("PP_Timing_SD")  << " and created by " ;
  
  if(theTrack->GetCreatorProcess()!=NULL)
    LogDebug("PP_Timing_SD") << theTrack->GetCreatorProcess()->GetProcessName() ;
  else 
    LogDebug("PP_Timing_SD") << "NO process";
    
  LogDebug("PP_Timing_SD") << std::endl;
}


G4bool PPS_Timing_SD::ProcessHits(G4Step * aStep, G4TouchableHistory * )
{
  if (aStep == NULL)
  {	    
      std::cout<<"PPS_TIMING : There is no hit to process"<<std::endl<<std::endl;
      return true;
  }
  else
  {
 


   GetStepInfo(aStep);
 




if(aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()=="PhotoDetector_Window")
 { ImportInfotoHit();    // added pps //in addtion to import info to hit it STORE hit as well
   //Print_Hit_Info();
   theTrack->SetTrackStatus(fStopAndKill);
 } 

   
    TrackSwitchF=-1;
    TrackSwitchS=-1;


 const G4VTouchable* thetouch = aStep->GetPreStepPoint()->GetTouchable();

if(aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()=="QLbar"&&theTrack->GetDefinition()!=G4OpticalPhoton::OpticalPhotonDefinition()&&thetouch->GetVolume(1)->GetName()=="PPS_Timing_Box_First")
 {
  for(unsigned int i=0;i<theTrackSwitchVecF.size();i++)
    {
      if(theTrackSwitchVecF[i]==theTrack->GetTrackID())
        TrackSwitchF=1;
    }
    
    if(TrackSwitchF!=1)
    {    
      theTrackSwitchVecF.push_back(theTrack->GetTrackID());
      ImportInfotoHit();
      if(theTrack->GetDefinition()->GetParticleName()=="proton")
        Print_Hit_Info();

    } 
    
 }

if(aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()=="QLbar"&&theTrack->GetDefinition()!= G4OpticalPhoton::OpticalPhotonDefinition()&&thetouch->GetVolume(1)->GetName()=="PPS_Timing_Box_Second")
 {
  for(unsigned int i=0;i<theTrackSwitchVecS.size();i++)
    {
      if(theTrackSwitchVecS[i]==theTrack->GetTrackID())
        TrackSwitchS=1;
    }

    if(TrackSwitchS!=1)
    {
      theTrackSwitchVecS.push_back(theTrack->GetTrackID());
      ImportInfotoHit();
      if(theTrack->GetDefinition()->GetParticleName()=="proton")
        Print_Hit_Info();  
    }

 }





   




      //LogDebug("PP_Timing_SD")<<"New hit created"<<std::endl;
	    return true;
	}
}


void PPS_Timing_SD::GetStepInfo(G4Step* aStep)
{
 

  theTrack = aStep->GetTrack();   

  preStepPoint = aStep->GetPreStepPoint(); 
  postStepPoint = aStep->GetPostStepPoint(); 
  hitPoint = preStepPoint->GetPosition();
  exitPoint = postStepPoint->GetPosition();
  currentPV = preStepPoint->GetPhysicalVolume();
  theLocalEntryPoint = SetToLocal(hitPoint);
  theLocalExitPoint = SetToLocal(exitPoint);

  tSlice = (postStepPoint->GetGlobalTime() )/nanosecond;
  tSliceID = (int) tSlice;
  unitID = setDetUnitId(aStep);

  if(verbosity_)
    LogDebug("PP_Timing_SD") << "UNITa " << unitID <<std::endl;

  primaryID = theTrack->GetTrackID();

  Pabs = (aStep->GetPreStepPoint()->GetMomentum().mag())/GeV;
  p_x = (aStep->GetPreStepPoint()->GetMomentum().x())/GeV;
  p_y = (aStep->GetPreStepPoint()->GetMomentum().y())/GeV;
  p_z = (aStep->GetPreStepPoint()->GetMomentum().z())/GeV;
  
  //pps change post to pre
  Tof = aStep->GetPreStepPoint()->GetGlobalTime()/nanosecond;  
  
  //pps comment  Eloss = aStep->GetTotalEnergyDeposit()/GeV;
  Eloss = (aStep->GetPreStepPoint()->GetTotalEnergy()/eV);  
  
  ParticleType = theTrack->GetDefinition()->GetPDGEncoding();

  //corrected phi and theta treatment
  G4ThreeVector gmd  = aStep->GetPreStepPoint()->GetMomentumDirection();
  // convert it to local frame
  G4ThreeVector lmd = ((G4TouchableHistory *)(aStep->GetPreStepPoint()->GetTouchable()))->GetHistory()->GetTopTransform().TransformAxis(gmd);
  Local3DPoint lnmd = ConvertToLocal3DPoint(lmd);
  ThetaAtEntry = lnmd.theta();
  PhiAtEntry = lnmd.phi();

  //ThetaAtEntry     = aStep->GetPreStepPoint()->GetPosition().theta()/deg;
  //PhiAtEntry       = aStep->GetPreStepPoint()->GetPosition().phi()/deg;

  //LogDebug("PP_Timing_SD") << "UUUUUUUNNNNNNNNNNIIIIIIIIIITTTTTTTTTTTTTIIIIDDDD " << 
  //numberingScheme->GetUnitID(aStep) << std::endl ;
 

 if(IsPrimary(theTrack))
    ParentId = 0;
  else ParentId = theTrack->GetParentID();
  
  Vx = theTrack->GetVertexPosition().x()/mm;
  Vy = theTrack->GetVertexPosition().y()/mm;
  Vz = theTrack->GetVertexPosition().z()/mm;
}


uint32_t PPS_Timing_SD::setDetUnitId(G4Step * aStep)
{ 
//  LogDebug("PP_Timing_SD")<< "ALL'INTERNO DI SETDETUNITID PER IL DETECTOR "<< name << std::endl;
//  LogDebug("PP_Timing_SD") << "PIANO " << numberingScheme->GetCurrentPlane()<<std::endl;
  return (numberingScheme == 0 ? 0 : numberingScheme->GetUnitID(aStep));
}


void PPS_Timing_SD::StoreHit(PPS_Timing_G4Hit* hit)
{
  if (hit == 0 )
  {
    if(verbosity_)
      LogDebug("PP_Timing_SD") << "PP_Timing_SD: hit to be stored is NULL !!" <<std::endl;
    return;
  }

  theHC->insert( hit );
}


void PPS_Timing_SD::ImportInfotoHit()    // added pps
{
  currentHit = new PPS_Timing_G4Hit;
  currentHit->setTrackID(primaryID);
  currentHit->setTimeSlice(tSlice);

  currentHit->setUnitID(unitID);
  currentHit->setIncidentEnergy(incidentEnergy);

  currentHit->setPabs(Pabs);
  currentHit->setTof(Tof);
   currentHit->setEnergyLoss(Eloss);
  currentHit->setParticleType(ParticleType);
  currentHit->setThetaAtEntry(ThetaAtEntry);
  currentHit->setPhiAtEntry(PhiAtEntry);

  currentHit->setEntry(hitPoint);
  currentHit->setExit(exitPoint);

 currentHit->setLocalEntry(hitPoint);
 currentHit->setLocalExit(exitPoint);

//  currentHit->setLocalEntry(theLocalEntryPoint);
// currentHit->setLocalExit(theLocalExitPoint);

  currentHit->setParentId(ParentId);
  currentHit->setVx(Vx);
  currentHit->setVy(Vy);
  currentHit->setVz(Vz);
  
  currentHit->set_p_x(p_x);
  currentHit->set_p_y(p_y);
  currentHit->set_p_z(p_z);



  StoreHit(currentHit);
// LogDebug("PP_Timing_SD") << "STORED HIT IN: " << unitID << std::endl;
}	 


G4ThreeVector PPS_Timing_SD::SetToLocal(G4ThreeVector global)
{
  G4ThreeVector localPoint;
  const G4VTouchable* touch= preStepPoint->GetTouchable();
  localPoint = touch->GetHistory()->GetTopTransform().TransformPoint(global);
  
  return localPoint;
}
     

void PPS_Timing_SD::EndOfEvent(G4HCofThisEvent* )
{

    theTrackSwitchVecF.clear();
    theTrackSwitchVecS.clear();

  // here we loop over transient hits and make them persistent
  for (int j=0; j<theHC->entries() && j<15000; j++)
  {
    PPS_Timing_G4Hit* aHit = (*theHC)[j];
    //Local3DPoint theEntrance(aHit->getEntry().x(),aHit->getEntry().y(),aHit->getEntry().z());
    //Local3DPoint theExitPoint(aHit->getExit().x(),aHit->getExit().y(),aHit->getExit().z());
    
    Local3DPoint Entrata(aHit->getLocalEntry().x(),
       aHit->getLocalEntry().y(),
       aHit->getLocalEntry().z());
    Local3DPoint Uscita(aHit->getLocalExit().x(),
       aHit->getLocalExit().y(),
       aHit->getLocalExit().z());
    slave->processHits(PSimHit(Entrata,Uscita,
             aHit->getPabs(), aHit->getTof(),
             aHit->getEnergyLoss(), aHit->getParticleType(),
             aHit->getUnitID(), aHit->getTrackID(),
             aHit->getThetaAtEntry(),aHit->getPhiAtEntry()));
  }
  Summarize();
}
     

void PPS_Timing_SD::Summarize()
{
}


void PPS_Timing_SD::clear()
{
} 


void PPS_Timing_SD::DrawAll()
{
} 


void PPS_Timing_SD::PrintAll()
{
  LogDebug("PP_Timing_SD") << "PPS_Timing_SD: Collection " << theHC->GetName() << std::endl;
  theHC->PrintAllHits();
}

void PPS_Timing_SD::fillHits(edm::PSimHitContainer& c, std::string n) {
  if (slave->name() == n) c=slave->hits();
}


void PPS_Timing_SD::SetNumberingScheme(PPSTimingVDetectorOrganization* scheme)
{
  if (numberingScheme)
    delete numberingScheme;
  numberingScheme = scheme;
}

void PPS_Timing_SD::update(const BeginOfEvent * i){
  //  LogDebug("PP_Timing_SD") <<" Dispatched BeginOfEvent !"<<std::endl;
  clearHits();
  eventno = (*i)()->GetEventID();
std::cout<<"#######################Begin of Event number: "<<eventno<<std::endl<<std::endl;

}

void PPS_Timing_SD::update (const ::EndOfEvent*)
{
}


//pps added
void PPS_Timing_SD::clearTrack( G4Track * track){
    track->SetTrackStatus(fStopAndKill);   
}


void PPS_Timing_SD::clearHits(){
  slave->Initialize();
}

bool PPS_Timing_SD::IsPrimary(const G4Track * track)
{
  TrackInformation* info 
    = dynamic_cast<TrackInformation*>( track->GetUserInformation() );
  return info && info->isPrimary();
}



void PPS_Timing_SD::initRun(){
// construct your own material properties for setting refractionindex and so on
  // theMaterialProperties = new PPS_Timing_MaterialProperties(theMPDebug_);
   theMaterialProperties = new PPS_Timing_MaterialProperties(3);

}
  
