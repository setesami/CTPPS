#ifndef TotemRP_TotemRP_H
#define TotemRP_TotemRP_H

#include "SimG4CMS/TotemRP/interface/Totem_RP_G4Hit.h"
#include "SimG4CMS/TotemRP/interface/Totem_RP_G4HitCollection.h"
#include "SimG4CMS/TotemRP/interface/TotemRPHisto.h"
#include "SimG4CMS/TotemRP/interface/TotemRPHistoClass.h"
#include "SimG4CMS/TotemRP/interface/TotemRPHistoManager.h"

//#include "SimG4CMS/TotemRP/interface/RPDebugEvent.h"
//#include "SimG4CMS/TotemRP/interface/RPDebugEventManager.h"

#include "SimG4Core/Notification/interface/Observer.h"
#include "SimG4Core/Notification/interface/BeginOfJob.h"
#include "SimG4Core/Notification/interface/BeginOfEvent.h"
#include "SimG4Core/Notification/interface/BeginOfTrack.h"
#include "SimG4Core/Notification/interface/EndOfEvent.h"
#include "SimG4Core/Notification/interface/EndOfTrack.h"
#include "SimG4Core/Notification/interface/TrackInformation.h"
#include "SimG4Core/Watcher/interface/SimWatcher.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"

// system include files
#include <iostream>
#include <memory>
#include <vector>
#include <string>
//#include "hash_map"
#include <map>

class G4Step;
class BeginOfEvent;
class BeginOfJob;
class BeginOfTrack;
class EndOfEvent;
class EndOfTrack;


//struct eqstr
//{
//  bool operator()(const char* s1, const char* s2) const
//  {
//    return strcmp(s1, s2) == 0;
//  }
//};

//class BeginOfRun;
//class EndOfRun;
//class BeginOfEvent;
//class EndOfEvent;
//class BeginOfTrack;
//class EndOfTrack;
//class G4Step;


class TotemRP : public SimWatcher,
    public Observer<const BeginOfJob *>,
    public Observer<const BeginOfEvent *>, 
    public Observer<const EndOfEvent *>, 
    public Observer<const G4Step *>,
    public Observer<const EndOfTrack *>, 
    public Observer<const BeginOfTrack *>
{
 public:
  TotemRP(const edm::ParameterSet &p);
  virtual ~TotemRP();
  
 private:
  void update(const BeginOfEvent * evt);
  void update(const EndOfEvent * evt);
  void update(const G4Step * step);
  void update(const EndOfTrack * end_of_track);
  void update(const BeginOfTrack * beg_of_track);
  void update(const BeginOfJob * job);
  
  inline bool IsThroughPlane(double z1, double z2, double z0);
  void FillIfLeavesRP220Station(const G4Step * step);
  void KillParticlesBetweenStations(const G4Step * step);
  void FillIfParticleEntersRP(const G4Step * step);
  void FillIfParticleLeavesRP(const G4Step * step);
  void FillIfParticleLeavesFrontWallOfRP(const G4Step * aStep);
  void InitializePhysicalDetMap();
  void Indent(int indent);
  void PrintParticleTreeNode(G4PrimaryParticle* particle, int indent);
  void PrintPrimaryVertex(G4PrimaryVertex* primaryVertex, int indent);
  bool IsGoodForTrack(G4PrimaryParticle* pp);
  bool IsPrimary(const G4Track * track);


 private:
  const int primary_proton_id_code;
  const int particle_leaving_220_right_station_id_code;
  const int particle_leaving_220_left_station_id_code;
  const int particle_leaving_147_right_station_id_code;
  const int particle_leaving_147_left_station_id_code;
  const int particle_entering_RP_id_code;
  const int particle_leaving_RP_id_code;
  const int particle_leaving_front_wall_of_RP_id_code;
  const int primary_proton_inelastic_event_in_RP_station;
  const int particle_entering_station_id_code;
  
  TotemRPHisto * histos;
  int event_no;
  //hash_map<const char*, int, hash<const char*>, eqstr> PhysicalDetMap;
  std::map<std::string, int> PhysicalDetMap;
  
  //Keep parameters to instantiate TotemRPHistoManager later
  std::string                             fileName;
  std::vector<std::string>                names;

  // Private Tuples
  std::string RP_debugfileName;
  std::auto_ptr<TotemRPHistoManager>    tuplesManager;
  TotemRPHistoClass *                   tuples;
  
  //Debug RP hits
  //std::auto_ptr<RPDebugEventManager> debugManager;
  //RPDebugEvent *debug_event;

  std::string nomeFile;
  bool verbosity_;
};

#endif  //TotemRP_TotemRP_H
