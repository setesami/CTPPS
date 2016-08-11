// system include files
#include <iostream>
#include <iomanip>
#include <cmath>

// user include files
#include "SimG4Core/Notification/interface/BeginOfJob.h"
#include "SimG4Core/Notification/interface/BeginOfRun.h"
#include "SimG4Core/Notification/interface/BeginOfEvent.h"
#include "SimG4Core/Notification/interface/BeginOfTrack.h"
#include "SimG4Core/Notification/interface/EndOfEvent.h"
#include "SimG4Core/Notification/interface/EndOfTrack.h"
#include "SimG4Core/Notification/interface/TrackInformation.h"
 
#include "SimG4CMS/TotemRP/interface/TotemRP.h"
#include "SimG4CMS/TotemRP/interface/Totem_RP_G4Hit.h"
#include "SimG4CMS/TotemRP/interface/Totem_RP_G4HitCollection.h"

//#include "SimG4CMS/TotemRP/interface/RPDebugEvent.h"
#include "DataFormats/TotemRPDetId/interface/TotRPDetId.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4HCofThisEvent.hh"
#include "CLHEP/Units/GlobalSystemOfUnits.h"
#include "G4Event.hh"
#include "G4PrimaryVertex.hh"

#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4ParticleTable.hh"
#include "G4ThreeVector.hh"
#include "TString.h"

//#include <cassert>

TotemRP::TotemRP(const edm::ParameterSet &p)
 : primary_proton_id_code(-1),  
 particle_leaving_220_right_station_id_code(-2),
 particle_leaving_220_left_station_id_code(-6),
 particle_leaving_147_right_station_id_code(-7),
 particle_leaving_147_left_station_id_code(-8),
 particle_entering_RP_id_code(-3), 
 particle_leaving_RP_id_code(-4), 
 particle_leaving_front_wall_of_RP_id_code(-9),
 primary_proton_inelastic_event_in_RP_station(-5), 
 particle_entering_station_id_code(-10)//, 
 //histos(0), tuplesManager(0)
{
  edm::ParameterSet m_Anal = p.getParameter<edm::ParameterSet>("TotemRP");
  verbosity_ = m_Anal.getParameter<bool>("Verbosity");
  fileName = m_Anal.getParameter<std::string>("FileName");
  //RP_debugfileName = m_Anal.getParameter<std::string>("RPDebugFileName");
  
  nomeFile = m_Anal.getParameter<std::string>("FileNameOLD");
  names = m_Anal.getParameter<std::vector<std::string> >("Names");
  
  edm::LogInfo("TotemRP") << "TotemRP:: Initialised as observer of "
           << "begin of job, begin/end events and of G4step";
  edm::LogInfo("HcalSim") << "TotemRP:===>>>  Book user"
        << " Histograms and Root tree";
  
  histos = new TotemRPHisto(nomeFile);
  event_no = -1;
  edm::LogInfo("TotemRP") << "CREATO TotemRPHisto OBJECT " <<std::endl;
  InitializePhysicalDetMap();
}

TotemRP::~TotemRP()
{
  delete histos;
}

void TotemRP::update(const BeginOfJob * job) {
  // Ntuples
  tuplesManager.reset(new TotemRPHistoManager(fileName));
}

//=================================================================== per EVENT

//some usefull functions
void TotemRP::Indent(int indent)
{
  for(int i=0; i<indent; i++)
  {
    edm::LogInfo("TotemRP")<<" ";
  }
}

void TotemRP::PrintParticleTreeNode(G4PrimaryParticle* particle, int indent)
{
  while(particle!=0)
  {
    Indent(indent);
    
    if(particle->GetG4code())
      edm::LogInfo("TotemRP")<<particle->GetG4code()->GetParticleName();
    
    edm::LogInfo("TotemRP")<<" "<<particle->GetPDGcode()
        <<" "<<particle->GetMomentum()<<" "
        <<particle->GetProperTime()<<", "<<IsGoodForTrack(particle)<<std::endl;
    PrintParticleTreeNode(particle->GetDaughter(), indent+2);
    particle = particle->GetNext();
  }
}

void TotemRP::PrintPrimaryVertex(G4PrimaryVertex* primaryVertex, int indent)
{
  if(primaryVertex!=0)
  {
    G4PrimaryParticle* primaryParticle = primaryVertex->GetPrimary();
    Indent(indent);
    edm::LogInfo("TotemRP")<<"Primary vertex: ("<<primaryVertex->GetX0()<<", "<<primaryVertex->GetY0()
        <<", "<<primaryVertex->GetZ0()<<"), "<<primaryVertex->GetT0()<<", "
        <<primaryVertex->GetWeight()<<std::endl;
    PrintParticleTreeNode(primaryParticle, indent+2);
  }
}

bool TotemRP::IsGoodForTrack(G4PrimaryParticle* pp)
{
  G4ParticleDefinition* pd = pp->GetG4code();
  
  if(!pd)
  {
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    pd = particleTable->FindParticle(pp->GetPDGcode());
  }

  if(!pd)
  {
    return false;
  }
  else if(!(pd->IsShortLived()))
  { 
    return true;
  }
// Following two lines should be removed if the user does not want to make shortlived 
// primary particle with proper decay table to be converted into a track.
  else if(pd->GetDecayTable())
  {
    return true;
  }
  return false;
}


//=============================================================================

void TotemRP::update(const BeginOfEvent * evt)
{
    // create tuple object
  tuples = new TotemRPHistoClass();
  //debug_event = new RPDebugEvent();

  int iev = (*evt)()->GetEventID();
  
  if(verbosity_)
  {
    LogDebug("TotemRP") << "TotemRP: Begin of event = " << iev;
    
    edm::LogInfo("TotemRP") << " Begin event !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! event " << (*evt)()->GetEventID() << std::endl;
  }
  
  // access to the G4 hit collections 
  G4HCofThisEvent* allHC = (*evt)()->GetHCofThisEvent();
  int HCRPid = G4SDManager::GetSDMpointer()->GetCollectionID("TotemHitsRP");
  Totem_RP_G4HitCollection* theHC_RP = (Totem_RP_G4HitCollection*) allHC->GetHC(HCRPid); 
  LogDebug("ForwardSim") << "TotemTestGem :: Hit Collection for TotemHitsRP of ID " << HCRPid << " is obtained at " << theHC_RP;
  G4int nentriesRP = 0;
  if (HCRPid >= 0 && theHC_RP > 0) {
     nentriesRP = theHC_RP->entries(); 
     if(verbosity_)
     {
        edm::LogInfo("TotemRP")<<nentriesRP<<"  entries in the collection"<<std::endl;
     }
  }

  int PrimNum = (*evt)()->GetNumberOfPrimaryVertex();
  int evtnum = (*evt)()->GetEventID();
  event_no = evtnum;

  for(int i=0; i<PrimNum; i++)
  {
    G4PrimaryVertex* PrimVert = (*evt)()->GetPrimaryVertex(i);
//    edm::LogInfo("TotemRP")<<std::endl<<"Vx="<<PrimVert->GetX0()<<"  Vy="<<PrimVert->GetY0()<<"  Vz="<<PrimVert->GetZ0()<<std::endl;
    int num_of_particles = PrimVert->GetNumberOfParticle();
    for(int j=0; j<num_of_particles; j++)
    {
      G4PrimaryParticle* primary = PrimVert->GetPrimary(j); 
      if(primary->GetPDGcode()==2212)
      {
        histos->set_EVT(evtnum);
        histos->set_UID(primary_proton_id_code);   //for primary particles
        histos->set_Ptype(primary->GetPDGcode());
        histos->set_TID(0);
        histos->set_PID(0);
        histos->set_ELoss(0);
        histos->set_PABS(primary->GetMomentum().mag()/GeV);
        histos->set_p_x(primary->GetPx()/GeV);
        histos->set_p_y(primary->GetPy()/GeV);
        histos->set_p_z(primary->GetPz()/GeV);
        histos->set_VX(PrimVert->GetX0()/mm);
        histos->set_VY(PrimVert->GetY0()/mm);
        histos->set_VZ(PrimVert->GetZ0()/mm);
        
        histos->set_X(0);
        histos->set_Y(0);
        histos->set_Z(0);
        histos->set_Loc_X(0);
        histos->set_Loc_Y(0);
        histos->set_Loc_Z(0);
        histos->set_X_Exit(0);
        histos->set_Y_Exit(0);
        histos->set_Z_Exit(0);
        histos->set_Loc_X_Exit(0);
        histos->set_Loc_Y_Exit(0);
        histos->set_Loc_Z_Exit(0);
        
        histos->fillNtuple();
      }
    }
  }
  
  for(int i=0; i<PrimNum; i++)
  {
    G4PrimaryVertex* PrimVert = (*evt)()->GetPrimaryVertex(i);
    if(verbosity_)
      PrintPrimaryVertex(PrimVert, 0);
  }


}

//=================================================================== each STEP

bool TotemRP::IsThroughPlane(double z1, double z2, double z0)
{
  return (z1<=z0 && z0<=z2) || (z2<=z0 && z0<=z1);
}


void TotemRP::FillIfLeavesRP220Station(const G4Step * aStep)
{
  const G4StepPoint* thePreStepPoint = aStep->GetPreStepPoint();
  const G4StepPoint* thePostStepPoint = aStep->GetPostStepPoint();
  const G4ThreeVector & hitPoint = thePreStepPoint->GetPosition();
  double x = hitPoint.x();
  double y = hitPoint.y();
  double z = hitPoint.z();
  
  const double pipe_inner_radius = 80*mm; //mm
  
  if( thePostStepPoint && thePostStepPoint->GetPhysicalVolume() 
      && aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName()== "DDDWorld"
      && ( (x*x+y*y)<=pipe_inner_radius*pipe_inner_radius) )
  {
    if(thePreStepPoint->GetPhysicalVolume()->GetName()== "RP_220_Right_Station_Vacuum_5")
    {
      //edm::LogInfo("TotemRP")<<"!!read"<<std::endl;
      //std::cout<<"220 right station left"<<std::endl;
      const G4Track *theTrack = aStep->GetTrack();
      
      histos->set_EVT(event_no);
      histos->set_UID(particle_leaving_220_right_station_id_code);   //for particles going from RP station 220m
      histos->set_Ptype( theTrack->GetDefinition()->GetPDGEncoding() );
      if(IsPrimary(theTrack))
        histos->set_PID(0);
      else histos->set_PID(theTrack->GetParentID());
      histos->set_ELoss(0);
      histos->set_PABS(thePreStepPoint->GetMomentum().mag()/GeV);
      histos->set_p_x(thePreStepPoint->GetMomentum().x()/GeV);
      histos->set_p_y(thePreStepPoint->GetMomentum().y()/GeV);
      histos->set_p_z(thePreStepPoint->GetMomentum().z()/GeV);
      histos->set_VX(theTrack->GetVertexPosition().x()/mm);
      histos->set_VY(theTrack->GetVertexPosition().y()/mm);
      histos->set_VZ(theTrack->GetVertexPosition().z()/mm);
      
      histos->set_X(x);
      histos->set_Y(y);
      histos->set_Z(z);
      histos->set_Loc_X(0);
      histos->set_Loc_Y(0);
      histos->set_Loc_Z(0);
      histos->set_X_Exit(0);
      histos->set_Y_Exit(0);
      histos->set_Z_Exit(0);
      histos->set_Loc_X_Exit(0);
      histos->set_Loc_Y_Exit(0);
      histos->set_Loc_Z_Exit(0);
      
      histos->fillNtuple();
  //    edm::LogInfo("TotemRP")<<"leaving!!!! "<<theTrack->GetDefinition()->GetParticleName()<<std::endl;
    }
    else if(thePreStepPoint->GetPhysicalVolume()->GetName()== "RP_220_Left_Station_Vacuum_5")
    {
      //edm::LogInfo("TotemRP")<<"!!read"<<std::endl;
      //std::cout<<"220 left station left"<<std::endl;
      const G4Track *theTrack = aStep->GetTrack();
      
      histos->set_EVT(event_no);
      histos->set_UID(particle_leaving_220_left_station_id_code);   //for particles going from RP station 220m
      histos->set_Ptype( theTrack->GetDefinition()->GetPDGEncoding() );
      if(IsPrimary(theTrack))
        histos->set_PID(0);
      else histos->set_PID(theTrack->GetParentID());
      histos->set_ELoss(0);
      histos->set_PABS(thePreStepPoint->GetMomentum().mag()/GeV);
      histos->set_p_x(thePreStepPoint->GetMomentum().x()/GeV);
      histos->set_p_y(thePreStepPoint->GetMomentum().y()/GeV);
      histos->set_p_z(thePreStepPoint->GetMomentum().z()/GeV);
      histos->set_VX(theTrack->GetVertexPosition().x()/mm);
      histos->set_VY(theTrack->GetVertexPosition().y()/mm);
      histos->set_VZ(theTrack->GetVertexPosition().z()/mm);
      
      histos->set_X(x);
      histos->set_Y(y);
      histos->set_Z(z);
      histos->set_Loc_X(0);
      histos->set_Loc_Y(0);
      histos->set_Loc_Z(0);
      histos->set_X_Exit(0);
      histos->set_Y_Exit(0);
      histos->set_Z_Exit(0);
      histos->set_Loc_X_Exit(0);
      histos->set_Loc_Y_Exit(0);
      histos->set_Loc_Z_Exit(0);
      
      histos->fillNtuple();
  //    edm::LogInfo("TotemRP")<<"leaving!!!! "<<theTrack->GetDefinition()->GetParticleName()<<std::endl;
    }
  }
  else if( thePostStepPoint && thePostStepPoint->GetPhysicalVolume() 
      && aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName()== "Beam_150_220_L"
      && ( (x*x+y*y)<=pipe_inner_radius*pipe_inner_radius) && 
      thePreStepPoint->GetPhysicalVolume()->GetName()== "RP_147_Left_Station_Vacuum_5")
  {
    //edm::LogInfo("TotemRP")<<"!!read"<<std::endl;
    //std::cout<<"150 left station left"<<std::endl;
    const G4Track *theTrack = aStep->GetTrack();
    
    histos->set_EVT(event_no);
    histos->set_UID(particle_leaving_147_left_station_id_code);   //for particles going from RP station 220m
    histos->set_Ptype( theTrack->GetDefinition()->GetPDGEncoding() );
    if(IsPrimary(theTrack))
      histos->set_PID(0);
    else histos->set_PID(theTrack->GetParentID());
    histos->set_ELoss(0);
    histos->set_PABS(thePreStepPoint->GetMomentum().mag()/GeV);
    histos->set_p_x(thePreStepPoint->GetMomentum().x()/GeV);
    histos->set_p_y(thePreStepPoint->GetMomentum().y()/GeV);
    histos->set_p_z(thePreStepPoint->GetMomentum().z()/GeV);
    histos->set_VX(theTrack->GetVertexPosition().x()/mm);
    histos->set_VY(theTrack->GetVertexPosition().y()/mm);
    histos->set_VZ(theTrack->GetVertexPosition().z()/mm);
    
    histos->set_X(x);
    histos->set_Y(y);
    histos->set_Z(z);
    histos->set_Loc_X(0);
    histos->set_Loc_Y(0);
    histos->set_Loc_Z(0);
    histos->set_X_Exit(0);
    histos->set_Y_Exit(0);
    histos->set_Z_Exit(0);
    histos->set_Loc_X_Exit(0);
    histos->set_Loc_Y_Exit(0);
    histos->set_Loc_Z_Exit(0);
    
    histos->fillNtuple();
//    edm::LogInfo("TotemRP")<<"leaving!!!! "<<theTrack->GetDefinition()->GetParticleName()<<std::endl;
  }
  else if( thePostStepPoint && thePostStepPoint->GetPhysicalVolume() 
      && aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName()== "Beam_150_220_R"
      && ( (x*x+y*y)<=pipe_inner_radius*pipe_inner_radius) && 
      thePreStepPoint->GetPhysicalVolume()->GetName()== "RP_147_Right_Station_Vacuum_5")
  {
    //edm::LogInfo("TotemRP")<<"!!read"<<std::endl;
    //std::cout<<"150 right station left"<<std::endl;
    const G4Track *theTrack = aStep->GetTrack();
    
    histos->set_EVT(event_no);
    histos->set_UID(particle_leaving_147_right_station_id_code);   //for particles going from RP station 220m
    histos->set_Ptype( theTrack->GetDefinition()->GetPDGEncoding() );
    if(IsPrimary(theTrack))
      histos->set_PID(0);
    else histos->set_PID(theTrack->GetParentID());
    histos->set_ELoss(0);
    histos->set_PABS(thePreStepPoint->GetMomentum().mag()/GeV);
    histos->set_p_x(thePreStepPoint->GetMomentum().x()/GeV);
    histos->set_p_y(thePreStepPoint->GetMomentum().y()/GeV);
    histos->set_p_z(thePreStepPoint->GetMomentum().z()/GeV);
    histos->set_VX(theTrack->GetVertexPosition().x()/mm);
    histos->set_VY(theTrack->GetVertexPosition().y()/mm);
    histos->set_VZ(theTrack->GetVertexPosition().z()/mm);
    
    histos->set_X(x);
    histos->set_Y(y);
    histos->set_Z(z);
    histos->set_Loc_X(0);
    histos->set_Loc_Y(0);
    histos->set_Loc_Z(0);
    histos->set_X_Exit(0);
    histos->set_Y_Exit(0);
    histos->set_Z_Exit(0);
    histos->set_Loc_X_Exit(0);
    histos->set_Loc_Y_Exit(0);
    histos->set_Loc_Z_Exit(0);
    
    histos->fillNtuple();
//    edm::LogInfo("TotemRP")<<"leaving!!!! "<<theTrack->GetDefinition()->GetParticleName()<<std::endl;
  }
}


void TotemRP::FillIfParticleEntersRP(const G4Step * aStep)
{
  const G4StepPoint* thePreStepPoint = aStep->GetPreStepPoint();
  const G4StepPoint* thePostStepPoint = aStep->GetPostStepPoint();
  
  if( thePostStepPoint && thePostStepPoint->GetPhysicalVolume() 
      && thePostStepPoint->GetPhysicalVolume()->GetName()== "RP_box_primary_vacuum"
      && (thePreStepPoint->GetPhysicalVolume()->GetName()== "RP_220_Right_Station_Vacuum_5"
      || thePreStepPoint->GetPhysicalVolume()->GetName()== "RP_220_Left_Station_Vacuum_5"
      || thePreStepPoint->GetPhysicalVolume()->GetName()== "RP_147_Right_Station_Vacuum_5"
      || thePreStepPoint->GetPhysicalVolume()->GetName()== "RP_147_Left_Station_Vacuum_5"))
  {
    const G4Track *theTrack = aStep->GetTrack();
    const G4ThreeVector & hitPoint = thePostStepPoint->GetPosition();
    double x = hitPoint.x();
    double y = hitPoint.y();
    double z = hitPoint.z();
    double rp_id = aStep->GetPostStepPoint()->GetPhysicalVolume()->GetCopyNo();
    
    histos->set_EVT(event_no);
    histos->set_UID(particle_entering_RP_id_code);   //for particles entering RP
    histos->set_Ptype(theTrack->GetDefinition()->GetPDGEncoding());
    if(IsPrimary(theTrack))
      histos->set_PID(0);
    else histos->set_PID(theTrack->GetParentID());
    histos->set_ELoss(rp_id);  //rp_id in eloss
    histos->set_PABS(thePostStepPoint->GetMomentum().mag()/GeV);
    histos->set_p_x(thePostStepPoint->GetMomentum().x()/GeV);
    histos->set_p_y(thePostStepPoint->GetMomentum().y()/GeV);
    histos->set_p_z(thePostStepPoint->GetMomentum().z()/GeV);
    histos->set_VX(theTrack->GetVertexPosition().x()/mm);
    histos->set_VY(theTrack->GetVertexPosition().y()/mm);
    histos->set_VZ(theTrack->GetVertexPosition().z()/mm);
    
    histos->set_X(x);
    histos->set_Y(y);
    histos->set_Z(z);
    histos->set_Loc_X(0);
    histos->set_Loc_Y(0);
    histos->set_Loc_Z(0);
    histos->set_X_Exit(0);
    histos->set_Y_Exit(0);
    histos->set_Z_Exit(0);
    histos->set_Loc_X_Exit(0);
    histos->set_Loc_Y_Exit(0);
    histos->set_Loc_Z_Exit(0);
    
    histos->fillNtuple();
//    edm::LogInfo("TotemRP")<<"entering RP!!!! "<<theTrack->GetDefinition()->GetParticleName()<<" "<<rp_id<<std::endl;
  }
}


void TotemRP::FillIfParticleLeavesFrontWallOfRP(const G4Step * aStep)
{
  const G4StepPoint* thePreStepPoint = aStep->GetPreStepPoint();
  const G4StepPoint* thePostStepPoint = aStep->GetPostStepPoint();
  
  if( thePostStepPoint && thePostStepPoint->GetPhysicalVolume() 
      && thePostStepPoint->GetPhysicalVolume()->GetName()== "RP_box_secondary_vacuum"
      && thePreStepPoint->GetPhysicalVolume()->GetName()== "RP_front_wall_6"
      && thePreStepPoint->GetPhysicalVolume()->GetCopyNo()==0)
  {
    const G4Track *theTrack = aStep->GetTrack();
    const G4ThreeVector & hitPoint = thePostStepPoint->GetPosition();
    double x = hitPoint.x();
    double y = hitPoint.y();
    double z = hitPoint.z();

    const G4VTouchable* touchable = theTrack->GetTouchable();
    const G4VPhysicalVolume *ph_vol = touchable->GetVolume(1);
    double rp_id = ph_vol->GetCopyNo();
//    edm::LogInfo("TotemRP")<<"rp id:"<<rp_id<<" name:"<<theTrack->GetDefinition()->GetParticleName()<<std::endl;

    histos->set_EVT(event_no);
    histos->set_UID(particle_leaving_front_wall_of_RP_id_code);   //for particles leaving the front wall of RP
    histos->set_Ptype(theTrack->GetDefinition()->GetPDGEncoding());
    if(IsPrimary(theTrack))
      histos->set_PID(0);
    else histos->set_PID(theTrack->GetParentID());
    histos->set_ELoss(rp_id);  //rp_id in eloss
    histos->set_PABS(thePostStepPoint->GetMomentum().mag()/GeV);
    histos->set_p_x(thePostStepPoint->GetMomentum().x()/GeV);
    histos->set_p_y(thePostStepPoint->GetMomentum().y()/GeV);
    histos->set_p_z(thePostStepPoint->GetMomentum().z()/GeV);
    histos->set_VX(theTrack->GetVertexPosition().x()/mm);
    histos->set_VY(theTrack->GetVertexPosition().y()/mm);
    histos->set_VZ(theTrack->GetVertexPosition().z()/mm);
    
    histos->set_X(x);
    histos->set_Y(y);
    histos->set_Z(z);
    histos->set_Loc_X(0);
    histos->set_Loc_Y(0);
    histos->set_Loc_Z(0);
    histos->set_X_Exit(0);
    histos->set_Y_Exit(0);
    histos->set_Z_Exit(0);
    histos->set_Loc_X_Exit(0);
    histos->set_Loc_Y_Exit(0);
    histos->set_Loc_Z_Exit(0);
    
    histos->fillNtuple();
//    edm::LogInfo("TotemRP")<<"leaving front wall of RP!!!! "<<theTrack->GetDefinition()->GetParticleName()<<" "<<rp_id<<std::endl;
  }
}


void TotemRP::FillIfParticleLeavesRP(const G4Step * aStep)
{
  const G4StepPoint* thePreStepPoint = aStep->GetPreStepPoint();
  const G4StepPoint* thePostStepPoint = aStep->GetPostStepPoint();
  
  if( thePostStepPoint && thePostStepPoint->GetPhysicalVolume() 
      && thePreStepPoint->GetPhysicalVolume()->GetName()== "RP_box_primary_vacuum"
      && (thePostStepPoint->GetPhysicalVolume()->GetName()== "RP_220_Right_Station_Vacuum_5"
      || thePostStepPoint->GetPhysicalVolume()->GetName()== "RP_220_Left_Station_Vacuum_5"
      || thePostStepPoint->GetPhysicalVolume()->GetName()== "RP_147_Right_Station_Vacuum_5"
      || thePostStepPoint->GetPhysicalVolume()->GetName()== "RP_147_Left_Station_Vacuum_5"))
  {
    const G4Track *theTrack = aStep->GetTrack();
    const G4ThreeVector & hitPoint = thePreStepPoint->GetPosition();
    double x = hitPoint.x();
    double y = hitPoint.y();
    double z = hitPoint.z();
    double rp_id = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
    
    histos->set_EVT(event_no);
    histos->set_UID(particle_leaving_RP_id_code);   //for particles going from RP station 220m
    histos->set_Ptype( theTrack->GetDefinition()->GetPDGEncoding() );
    if(IsPrimary(theTrack))
      histos->set_PID(0);
    else histos->set_PID(theTrack->GetParentID());
    histos->set_ELoss(rp_id);  //rp_id in eloss
    histos->set_PABS(thePreStepPoint->GetMomentum().mag()/GeV);
    histos->set_p_x(thePreStepPoint->GetMomentum().x()/GeV);
    histos->set_p_y(thePreStepPoint->GetMomentum().y()/GeV);
    histos->set_p_z(thePreStepPoint->GetMomentum().z()/GeV);
    histos->set_VX(theTrack->GetVertexPosition().x()/mm);
    histos->set_VY(theTrack->GetVertexPosition().y()/mm);
    histos->set_VZ(theTrack->GetVertexPosition().z()/mm);
    
    histos->set_X(x);
    histos->set_Y(y);
    histos->set_Z(z);
    histos->set_Loc_X(0);
    histos->set_Loc_Y(0);
    histos->set_Loc_Z(0);
    histos->set_X_Exit(0);
    histos->set_Y_Exit(0);
    histos->set_Z_Exit(0);
    histos->set_Loc_X_Exit(0);
    histos->set_Loc_Y_Exit(0);
    histos->set_Loc_Z_Exit(0);
    
    histos->fillNtuple();
//    edm::LogInfo("TotemRP")<<"leaving RP!!!! "<<theTrack->GetDefinition()->GetParticleName()<<" "<<rp_id<<std::endl;
  }
}


void TotemRP::KillParticlesBetweenStations(const G4Step * aStep)
{
  double pre_step_z = aStep->GetPreStepPoint()->GetPosition().z();
  double post_step_z = aStep->GetPostStepPoint()->GetPosition().z();
  
  double cut_plane_147_220_position = 183.5*m;
  double cut_plane_147_position = 100*m;
  if(IsThroughPlane(pre_step_z, post_step_z, cut_plane_147_220_position)
    || IsThroughPlane(pre_step_z, post_step_z, -cut_plane_147_220_position)
    || IsThroughPlane(pre_step_z, post_step_z, cut_plane_147_position)
    || IsThroughPlane(pre_step_z, post_step_z, -cut_plane_147_position))
  {
    aStep->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);
    //edm::LogInfo("TotemRP")<<"killed"<<std::endl;
  }
}

void TotemRP::update(const G4Step * aStep)
{
  const G4ThreeVector& pre_step_pos = aStep->GetPreStepPoint()->GetPosition();
  const G4ThreeVector& post_step_pos = aStep->GetPostStepPoint()->GetPosition();
  
  if(verbosity_ && aStep->GetTrack()->GetDefinition()->GetPDGEncoding()==2212)
  {
    edm::LogInfo("TotemRP") << aStep->GetTrack()->GetDefinition()->GetParticleName() << " "
         << "trackID:" << aStep->GetTrack()->GetTrackID()
         << " parentID:" << aStep->GetTrack()->GetParentID() << " "
         << aStep->GetPreStepPoint()->GetPosition()<<" "
         << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()<< " "
         << aStep->GetPostStepPoint()->GetPosition()<<" "
         <<"primary_vert="<<aStep->GetTrack()->GetVertexPosition()<<std::endl;
  }
  
  FillIfLeavesRP220Station(aStep);

//do not uncomment ever in cmssw with parameterization  
  //KillParticlesBetweenStations(aStep);

  FillIfParticleEntersRP(aStep);
  FillIfParticleLeavesRP(aStep);
  FillIfParticleLeavesFrontWallOfRP(aStep);
  
  if(verbosity_)
  {
    edm::LogInfo("TotemRP")<<"step info, "<<aStep->GetTrack()->GetTrackID()<<", pre_step:";
    if(aStep && aStep->GetPreStepPoint() && aStep->GetPreStepPoint()->GetPhysicalVolume())
      edm::LogInfo("TotemRP")<<aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()<<" "
      <<pre_step_pos<<" "<<aStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
    edm::LogInfo("TotemRP")<<", post_step:";
    if(aStep && aStep->GetPostStepPoint() && aStep->GetPostStepPoint()->GetPhysicalVolume())
      edm::LogInfo("TotemRP")<<aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName()<<" "
      <<post_step_pos<<" "<<aStep->GetPostStepPoint()->GetPhysicalVolume()->GetCopyNo();
    edm::LogInfo("TotemRP")<<std::endl;
  }
}

//================================================================ Begin of track



void TotemRP::update(const BeginOfTrack * beg_of_track)
{
  const G4Track *track = (*beg_of_track)();
  
  TrackInformation* info 
    = dynamic_cast<TrackInformation*>( (*beg_of_track)()->GetUserInformation() );
     
     
  if(track->GetDynamicParticle()->GetPDGcode()==2212
     && track->GetCreatorProcess() 
     && track->GetCreatorProcess()->GetProcessType()==fParameterisation
     && track->GetCreatorProcess()->GetProcessName()=="TotemRPParameterisationProcess"
     && info && info->isPrimary())
  {
    G4ThreeVector vert = track->GetVertexPosition();
    
    G4ThreeVector position = track->GetPosition();
    double pos_x = position.x()/mm;
    double pos_y = position.y()/mm;
    double pos_z = position.z()/mm;
    
    histos->set_EVT(event_no);
    histos->set_UID(particle_entering_station_id_code);   //for particles going from RP station 220m
    histos->set_Ptype(2212);

    histos->set_ELoss(0);
    histos->set_PABS(track->GetMomentum().mag()/GeV);
    histos->set_p_x(track->GetMomentum().x()/GeV);
    histos->set_p_y(track->GetMomentum().y()/GeV);
    histos->set_p_z(track->GetMomentum().z()/GeV);
    histos->set_VX(vert.x()/mm);
    histos->set_VY(vert.y()/mm);
    histos->set_VZ(vert.z()/mm);
    
    histos->set_X(pos_x);
    histos->set_Y(pos_y);
    histos->set_Z(pos_z);
    histos->set_Loc_X(0);
    histos->set_Loc_Y(0);
    histos->set_Loc_Z(0);
    histos->set_X_Exit(0);
    histos->set_Y_Exit(0);
    histos->set_Z_Exit(0);
    histos->set_Loc_X_Exit(0);
    histos->set_Loc_Y_Exit(0);
    histos->set_Loc_Z_Exit(0);
        
    histos->fillNtuple();
  }
}

//================================================================ End of TRACK

void TotemRP::update(const EndOfTrack *end_of_track)
{
  const G4Track *track = (*end_of_track)();
  if(IsPrimary(track) && track->GetDefinition()->GetPDGEncoding()==2212 && 
      track->GetVolume()->GetName().substr(0, 3)=="RP_")
  {
    const G4VTouchable* touchable = track->GetTouchable();
    int hist_depth = touchable->GetHistoryDepth();
    double rp_id=-1;
    for(int i=0; i<hist_depth; ++i)
    {
      const G4VPhysicalVolume *ph_vol = touchable->GetVolume(i);
      if(ph_vol->GetName()=="RP_box_primary_vacuum")
      {
        rp_id = ph_vol->GetCopyNo();
        break;
      }
    }
    
    const G4ThreeVector & position = track->GetPosition();
    double x = position.x()/mm;
    double y = position.y()/mm;
    double z = position.z()/mm;
    
    histos->set_EVT(event_no);
    histos->set_UID(primary_proton_inelastic_event_in_RP_station);   //for particles going from RP station 220m
    histos->set_Ptype(2212);

    histos->set_ELoss(rp_id);  //rp_id in eloss
    histos->set_PABS(track->GetMomentum().mag()/GeV);
    histos->set_p_x(track->GetMomentum().x()/GeV);
    histos->set_p_y(track->GetMomentum().y()/GeV);
    histos->set_p_z(track->GetMomentum().z()/GeV);
    histos->set_VX(track->GetVertexPosition().x()/mm);
    histos->set_VY(track->GetVertexPosition().y()/mm);
    histos->set_VZ(track->GetVertexPosition().z()/mm);
    
    histos->set_X(x);
    histos->set_Y(y);
    histos->set_Z(z);
    histos->set_Loc_X(0);
    histos->set_Loc_Y(0);
    histos->set_Loc_Z(0);
    histos->set_X_Exit(0);
    histos->set_Y_Exit(0);
    histos->set_Z_Exit(0);
    histos->set_Loc_X_Exit(0);
    histos->set_Loc_Y_Exit(0);
    histos->set_Loc_Z_Exit(0);
    
    G4VPhysicalVolume* myVolume = track->GetVolume();
    int vol_copy_no = myVolume->GetCopyNo();
    int prim_vert_id = PhysicalDetMap[myVolume->GetName()];
    histos->set_prim_ver_id(prim_vert_id);
    histos->set_PID(vol_copy_no);
    
    histos->fillNtuple();
//    edm::LogInfo("TotemRP")<<"prim proton died "<<prim_vert_id<<std::endl;
  }
}


//================================================================ End of EVENT
void TotemRP::update(const EndOfEvent * evt)
{
  if(verbosity_)
    edm::LogInfo("TotemRP") << " Fill event " << (*evt)()->GetEventID() << std::endl;

  // access to the G4 hit collections 
  G4HCofThisEvent* allHC = (*evt)()->GetHCofThisEvent();
  
  int ihit = 0;
  for (unsigned int in=0; in<names.size(); in++)
  {
    int HCRPid = G4SDManager::GetSDMpointer()->GetCollectionID(names[in]);
    if(HCRPid == -1)
    {


    }
    else
    {
      Totem_RP_G4HitCollection* theHC_RP = (Totem_RP_G4HitCollection*) allHC->GetHC(HCRPid);
      if(verbosity_)
        LogDebug("TotemRP") << "TotemRP :: Hit Collection for " <<names[in]
           << " of ID " << HCRPid << " is obtained at " << theHC_RP;

      G4int nentriesRP = 0;
      nentriesRP = theHC_RP->entries();

      if(HCRPid >= 0 && theHC_RP > 0 && nentriesRP>0)
      {
        for(ihit = 0; ihit <nentriesRP; ihit++)
        {
          Totem_RP_G4Hit* aHit = (*theHC_RP)[ihit];

          int evtnum = (*evt)()->GetEventID();
          //edm::LogInfo("TotemRP")<<"event no: "<<evtnum<<std::endl;

          int UID = aHit->getUnitID();
          int Ptype = aHit->getParticleType();
          int TID = aHit->getTrackID();
          int PID = aHit->getParentId();
          double ELoss =  aHit->getEnergyLoss();
          double PABS =  aHit->getPabs();
          double x = aHit->getEntry().x();
          double y = aHit->getEntry().y();
          double z = aHit->getEntry().z();
          double lx = aHit->getLocalEntry().x();
          double ly = aHit->getLocalEntry().y();
          double lz = aHit->getLocalEntry().z();

          double x_ex = aHit->getExit().x();
          double y_ex = aHit->getExit().y();
          double z_ex = aHit->getExit().z();
          double lx_ex = aHit->getLocalExit().x();
          double ly_ex = aHit->getLocalExit().y();
          double lz_ex = aHit->getLocalExit().z();

          double vx = aHit->getVx();
          double vy = aHit->getVy();
          double vz = aHit->getVz();

          double p_x = aHit->get_p_x();
          double p_y = aHit->get_p_y();
          double p_z = aHit->get_p_z();

          int prim_vert_id = -1;

          if(PID!=0)
          {
            G4ThreeVector myPoint(vx, vy, vz);
            G4Navigator* theNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
            G4VPhysicalVolume* myVolume = theNavigator->LocateGlobalPointAndSetup(myPoint);
            prim_vert_id = PhysicalDetMap[myVolume->GetName()];
          }

          histos->set_EVT(evtnum);

          TotRPDetId det_id((uint32_t)UID);
          histos->set_UID(det_id.DetectorDecId());

          histos->set_Ptype(Ptype) ;
          histos->set_TID(TID) ;
          histos->set_PID(PID);
          histos->set_ELoss(ELoss) ;
          histos->set_PABS(PABS) ;
          histos->set_VX(vx);
          histos->set_VY(vy) ;
          histos->set_VZ(vz);

          histos->set_X(x);
          histos->set_Y(y);
          histos->set_Z(z);
          histos->set_Loc_X(lx);
          histos->set_Loc_Y(ly);
          histos->set_Loc_Z(lz);
          histos->set_X_Exit(x_ex);
          histos->set_Y_Exit(y_ex);
          histos->set_Z_Exit(z_ex);
          histos->set_Loc_X_Exit(lx_ex);
          histos->set_Loc_Y_Exit(ly_ex);
          histos->set_Loc_Z_Exit(lz_ex);
          histos->set_p_x(p_x);
          histos->set_p_y(p_y);
          histos->set_p_z(p_z);
          histos->set_prim_ver_id(prim_vert_id);
          histos->fillNtuple();

          tuples->fillHit(UID, Ptype, TID, PID, ELoss, PABS, p_x, p_y, p_z, vx, vy, vz,
          x, y, z, lx, ly, lz, x_ex, y_ex, z_ex, lx_ex, ly_ex, lz_ex, prim_vert_id);

    //  void fillHit(int UID_, int Ptype_, int TID_, int PID_, double ELoss_, double PABS_,
    //    double p_x_, double p_y_, double p_z_, double vx_, double vy_, double vz_, double x_,
    //    double y_, double z_, double lx_, double ly_, double lz_,
    //    double x_ex_, double y_ex_, double z_ex_, double lx_ex_, double ly_ex_, double lz_ex_
    //    int prim_vert_id_);
        }
      }
    }
  }
  tuplesManager->fillTree(tuples); // (no need to delete it...)
  //debugManager->fillTree(debug_event); // (no need to delete it...)
//  tuples = 0; // but avoid to reuse it...

  if(verbosity_)
    LogDebug("TotemRP") << "TotemRP:: --- after fillTree";
}


bool TotemRP::IsPrimary(const G4Track * track)
{
  TrackInformation* info 
    = dynamic_cast<TrackInformation*>( track->GetUserInformation() );
  return info && info->isPrimary();
}


void TotemRP::InitializePhysicalDetMap()
{
  PhysicalDetMap["Other"] = 0;
  PhysicalDetMap["RP_Silicon_Detector"] = 1;
  PhysicalDetMap["RP_Separ_Spacer"] = 2;
  PhysicalDetMap["RP_Separ_Frame_5"] = 3;
  PhysicalDetMap["RP_PCB"] = 4;
  PhysicalDetMap["RP_Left_Right_Wall"] = 5;
  PhysicalDetMap["RP_front_wall_6"] = 6;
  PhysicalDetMap["RP_Front_Frame_3"] = 7;
  PhysicalDetMap["RP_Device_Vert_Corp_3"] = 8;
  PhysicalDetMap["RP_Device_Hor_Corp_3"] = 9;
  PhysicalDetMap["RP_box_secondary_vacuum"] = 10;
  PhysicalDetMap["RP_box_primary_vacuum"] = 11;
  PhysicalDetMap["RP_bottom_wall_6"] = 12;
  PhysicalDetMap["RP_bottom_foil"] = 13;
  PhysicalDetMap["RP_220_Right_Station_Vacuum_5"] = 14;
  PhysicalDetMap["RP_220_Right_Station_Tube_5"] = 15;
  PhysicalDetMap["RP_220_Right_Station_Tube_4"] = 16;
  PhysicalDetMap["RP_220_Right_Station_Tube_3"] = 17;
  PhysicalDetMap["RP_220_Right_Station_Tube_2"] = 18;
  PhysicalDetMap["RP_220_Right_Station_Tube_1"] = 19;
  PhysicalDetMap["RP_220_Left_Station_Vacuum_5"] = 20;
  PhysicalDetMap["RP_220_Left_Station_Tube_5"] = 21;
  PhysicalDetMap["RP_220_Left_Station_Tube_4"] = 22;
  PhysicalDetMap["RP_220_Left_Station_Tube_3"] = 23;
  PhysicalDetMap["RP_220_Left_Station_Tube_2"] = 24;
  PhysicalDetMap["RP_220_Left_Station_Tube_1"] = 25;
  PhysicalDetMap["RP_147_Right_Station_Vacuum_5"] = 26;
  PhysicalDetMap["RP_147_Right_Station_Tube_5"] = 27;
  PhysicalDetMap["RP_147_Right_Station_Tube_4"] = 28;
  PhysicalDetMap["RP_147_Right_Station_Tube_3"] = 29;
  PhysicalDetMap["RP_147_Right_Station_Tube_2"] = 30;
  PhysicalDetMap["RP_147_Right_Station_Tube_1"] = 31;
  PhysicalDetMap["RP_147_Left_Station_Vacuum_5"] = 32;
  PhysicalDetMap["RP_147_Left_Station_Tube_5"] = 33;
  PhysicalDetMap["RP_147_Left_Station_Tube_4"] = 34;
  PhysicalDetMap["RP_147_Left_Station_Tube_3"] = 35;
  PhysicalDetMap["RP_147_Left_Station_Tube_2"] = 36;
  PhysicalDetMap["RP_147_Left_Station_Tube_1"] = 37;
}
