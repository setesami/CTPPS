/****************************************************************************
*
* This is a part of the TOTEM offline software.
* Authors:
*    Hubert Niewiadomski
*    Jan Kašpar (jan.kaspar@gmail.com)
*
* $$RCSfile: TotemRPProtonTransportModel.h,v $: $
* $Revision: 1.1.1.1.6.2 $
* $Date: 2009/11/16 16:56:06 $
*
****************************************************************************/

#ifndef SimG4Core_Application_TotemRPProtonTransportModel_h_
#define SimG4Core_Application_TotemRPProtonTransportModel_h_

//#define G4V7

#include "G4VFastSimulationModel.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TwoVector.hh"
#include "G4RotationMatrix.hh"

#include "SimG4Core/TotemRPProtonTransportParametrization/interface/LHCOpticsApproximator.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

/**
 *\brief A proton transport model.
 **/
class TotemRPProtonTransportModel : public G4VFastSimulationModel {
public:
    enum type {
        BEAM_IP_150_R, BEAM_IP_150_L, NOT_DEF
    };

    /// Constructor
    TotemRPProtonTransportModel(G4String ModelVolumeName, G4Envelope *envelope,
                                const LHCOpticsApproximator &approx, double zin, double zout,
                                bool verbosity);

    //Overloaded methods doing the job
    virtual G4bool ModelTrigger(const G4FastTrack &) { return true; };

    virtual G4bool IsApplicable(const G4ParticleDefinition &) { return true; };

    virtual void DoIt(const G4FastTrack &, G4FastStep &);

private:
    G4String modelName_;
    LHCOpticsApproximator approximator_;
    type transport_region_;
    double zin_;    ///< in m
    double zout_;   ///< in m
    int verbosity_;
    const static double beampipe_aperture_radius;  ///< in m

    void KillParticleAndSecondaries(const G4FastTrack &fastTrack, G4FastStep &fastStep);

    bool IsBeamPipeProton(const G4FastTrack &track);

    bool Transport(const G4FastTrack &track, G4FastStep &step);

    G4ThreeVector Unsmear_z_position(const G4FastTrack &track);
};

#endif  //SimG4Core_Application_TotemRPProtonTransportModel_h_

