#ifndef GflashHadronShowerModel_H
#define GflashHadronShowerModel_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "G4VFastSimulationModel.hh"

#include "G4TouchableHandle.hh"
#include "G4Navigator.hh"
#include "G4Step.hh"

class GflashHadronShowerProfile;
class GflashPiKShowerProfile;
class GflashKaonPlusShowerProfile;
class GflashKaonMinusShowerProfile;
class GflashProtonShowerProfile;
class GflashAntiProtonShowerProfile;
class GflashHistogram;

class GflashHadronShowerModel : public G4VFastSimulationModel 
{
public:
  //-------------------------
  // Constructor, destructor
  //-------------------------
  GflashHadronShowerModel (G4String modelName, G4Region* envelope, edm::ParameterSet parSet);
  ~GflashHadronShowerModel ();

  //------------------------------------------------------------------------
  // Virtual methods that should be implemented for this hadron shower model
  //------------------------------------------------------------------------

  G4bool IsApplicable(const G4ParticleDefinition&);
  G4bool ModelTrigger(const G4FastTrack &);
  void DoIt(const G4FastTrack&, G4FastStep&);

private:
  G4bool isFirstInelasticInteraction(const G4FastTrack& fastTrack);
  G4bool excludeDetectorRegion(const G4FastTrack& fastTrack);
  void makeHits(const G4FastTrack& fastTrack);
  void updateGflashStep(G4ThreeVector position, G4double time);

private:  

  edm::ParameterSet theParSet;
  GflashHadronShowerProfile *theProfile;
  GflashPiKShowerProfile *thePiKProfile;
  GflashKaonPlusShowerProfile *theKaonPlusProfile;
  GflashKaonMinusShowerProfile *theKaonMinusProfile;
  GflashProtonShowerProfile *theProtonProfile;
  GflashAntiProtonShowerProfile *theAntiProtonProfile;

  G4Step *theGflashStep;
  G4Navigator *theGflashNavigator;
  G4TouchableHandle  theGflashTouchableHandle;

  //debugging histograms
  GflashHistogram* theHisto;

};

#endif
