#ifndef _PPSSD_h
#define _PPSSD_h
// -*- C++ -*-
//
// Package:     Forward
// Class  :     PPSSD
//
/**\class PPSSD PPSSD.h SimG4CMS/Forward/interface/PPSSD.h
 
 Description: Stores hits of PPS in appropriate  container
 
 Usage:
    Used in sensitive detector builder 
 
*/
//
// Original Author: 
//         Created:  Tue May 16 10:14:34 CEST 2006
//
 
// system include files

// user include files

#include "SimG4Core/Notification/interface/Observer.h"
#include "SimG4Core/SensitiveDetector/interface/SensitiveTkDetector.h"
#include "SimG4Core/Notification/interface/BeginOfEvent.h"
#include "SimG4Core/Notification/interface/EndOfEvent.h"

#include "SimG4CMS/PPS/interface/PPSG4Hit.h"
#include "SimG4CMS/PPS/interface/PPSG4HitCollection.h"
#include "SimG4CMS/PPS/interface/PPSVDetectorOrganization.h"
#include "SimG4Core/Application/interface/SimTrackManager.h"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
 
#include <string>

class TrackingSlaveSD;

class PPSSD : public SensitiveTkDetector,
		public Observer<const BeginOfEvent*>,
		public Observer<const EndOfEvent*> {

public:

  /*
   * std::string,
   * DDCompactView const&,
   * SensitiveDetectorCatalog&,
   * edm::ParameterSet const&,
   * SimTrackManager const*
   */
  PPSSD(std::string name,
        DDCompactView const &cpv,
        const SensitiveDetectorCatalog &clg,
        edm::ParameterSet const &p,
        SimTrackManager const *manager);
  virtual ~PPSSD();
  
  virtual bool   ProcessHits(G4Step *,G4TouchableHistory *);
  virtual uint32_t setDetUnitId(G4Step*);

  virtual void   Initialize(G4HCofThisEvent * HCE);
  virtual void   EndOfEvent(G4HCofThisEvent * eventHC);
  virtual void   clear();
  virtual void   DrawAll();
  virtual void   PrintAll();

  void fillHits(edm::PSimHitContainer&, std::string use);

private:

  void           update(const BeginOfEvent *);
  void           update(const ::EndOfEvent *);
  virtual void   clearHits();

private:

  G4ThreeVector  SetToLocal(const G4ThreeVector& globalPoint);
  void           GetStepInfo(G4Step* aStep);
  bool           HitExists();
  void           CreateNewHit();
  void           CreateNewHitEvo();
  G4ThreeVector  PosizioEvo(const G4ThreeVector&,double ,double ,double, double,int&);
  void           UpdateHit();
  void           StoreHit(PPSG4Hit*);
  void           ResetForNewPrimary();
  void           Summarize();

private:

  TrackingSlaveSD*            slave;
  PPSVDetectorOrganization* numberingScheme;

  // Data relative to primary particle (the one which triggers a shower)
  // These data are common to all Hits of a given shower.
  // One shower is made of several hits which differ by the
  // unit ID (cristal/fiber/scintillator) and the Time slice ID.

  G4ThreeVector               entrancePoint;
  float                       incidentEnergy;
  G4int                       primID  ; //@@ ID of the primary particle.

  std::string                 name;
  G4int                       hcID;
  PPSG4HitCollection*       theHC; 
  const SimTrackManager*      theManager;

  int                         tsID; 
  PPSG4Hit*                 currentHit;
  G4Track*                    theTrack;
  G4VPhysicalVolume*          currentPV;
  uint32_t                    unitID, previousUnitID;
  int                         primaryID, tSliceID;  
  double                      tSlice;

  G4StepPoint*                preStepPoint; 
  G4StepPoint*                postStepPoint; 
  float                       edeposit;
  G4ThreeVector               hitPoint;

  G4ThreeVector               Posizio;
  G4ThreeVector               theEntryPoint;
  G4ThreeVector               theExitPoint;
  float                       Pabs;
  float                       Tof;
  float                       Eloss;	
  short                       ParticleType; 

  float                       ThetaAtEntry;
  float                       PhiAtEntry;

  int                         ParentId;
  float                       Vx,Vy,Vz;

  int                         eventno;
};

#endif
