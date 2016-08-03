/****************************************************************************
*
* This is a part of the TOTEM offline software.
* Authors:
*    Hubert Niewiadomski
*    Jan Kašpar (jan.kaspar@gmail.com)
*
* $$RCSfile: TotemRPProtonTransportModel.cc,v $: $
* $Revision: 1.5.2.3 $
* $Date: 2009/11/16 16:54:43 $
*
****************************************************************************/
#include "SimG4Core/Application/interface/TotemRPProtonTransportModel.h"

#include <iostream>
#include "CLHEP/Units/GlobalSystemOfUnits.h"
#include "CLHEP/Units/GlobalPhysicalConstants.h"
#include "G4ThreeVector.hh"
#include "G4AffineTransform.hh"
#include "G4DynamicParticle.hh"
#include "G4ParticleMomentum.hh"
#include "G4Proton.hh"
#include "G4Track.hh"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

using namespace std;

const double TotemRPProtonTransportModel::beampipe_aperture_radius = 0.040;  //in m

TotemRPProtonTransportModel::TotemRPProtonTransportModel(
        G4String ModelVolumeName, G4Envelope *envelope, const LHCOpticsApproximator &approx,
        double zin, double zout, bool verbosity) :
        G4VFastSimulationModel(ModelVolumeName, envelope),
        modelName_(ModelVolumeName),
        approximator_(approx),
        zin_(zin),
        zout_(zout),
        verbosity_(verbosity) {
    edm::LogInfo("TotemRPProtonTransportModel") << "TotemRPProtonTransportModel created for volume " << ModelVolumeName;

    if (ModelVolumeName == "Beam_IP_150_R")
        transport_region_ = BEAM_IP_150_R;
    else if (ModelVolumeName == "Beam_IP_150_L")
        transport_region_ = BEAM_IP_150_L;
    else
        transport_region_ = NOT_DEF;
}


void TotemRPProtonTransportModel::DoIt(const G4FastTrack &track, G4FastStep &step) {
    edm::LogInfo("TotemRPProtonTransportModel") << "step info: ";
    step.DumpInfo();

    step.Initialize(track);
    step.KillPrimaryTrack();

    G4ThreeVector in_loc_pos = track.GetPrimaryTrackLocalPosition();
    G4ThreeVector in_loc_mom = track.GetPrimaryTrackLocalMomentum();

    const G4AffineTransform *loc_glob_tran = track.GetInverseAffineTransformation();
    G4ThreeVector in_glob_pos = loc_glob_tran->TransformPoint(in_loc_pos);
    G4ThreeVector in_glob_mom = loc_glob_tran->TransformPoint(in_loc_mom);

    edm::LogInfo("TotemRPProtonTransportModel") <<
    "\tin_loc_pos:" << in_loc_pos <<
    "\n\tin_loc_mom:" << in_loc_mom <<
    "\n\tin_glob_pos:" << in_glob_pos <<
    "\n\tin_glob_mom:" << in_glob_mom;

    if (IsBeamPipeProton(track)) {
        edm::LogInfo("TotemRPProtonTransportModel") << "Is a BeamPipe Proton";
        if (!Transport(track, step)) {
            edm::LogInfo("TotemRPProtonTransportModel") << "Transport failed";
            KillParticleAndSecondaries(track, step);
        }
    }
    else {
        edm::LogInfo("TotemRPProtonTransportModel") << "Is not a BeamPipe Proton, it will be killed";
        KillParticleAndSecondaries(track, step);
    }
}


bool TotemRPProtonTransportModel::IsBeamPipeProton(const G4FastTrack &track) {
    G4ThreeVector pos = track.GetPrimaryTrackLocalPosition();
    G4ThreeVector dir = track.GetPrimaryTrackLocalDirection();

    //if is a proton
    if (track.GetPrimaryTrack()->GetDefinition()->GetPDGEncoding() != 2212)
        return false;

    //if inside of the beampipe
    if (pos.x() / m * pos.x() / m + pos.y() / m * pos.y() / m > beampipe_aperture_radius * beampipe_aperture_radius)
        return false;

    //if the momentum is in the correct direction
    if ((dir.z() <= 0 && (transport_region_ == BEAM_IP_150_R)) ||
        (dir.z() >= 0 && (transport_region_ == BEAM_IP_150_L)))
        return false;

    return true;
}


void TotemRPProtonTransportModel::KillParticleAndSecondaries(const G4FastTrack &fastTrack, G4FastStep &fastStep) {
    fastStep.KillPrimaryTrack();
}


bool TotemRPProtonTransportModel::Transport(const G4FastTrack &track, G4FastStep &step) {
    edm::LogInfo("TotemRPProtonTransportModel") << "===== BEGIN Transport " << GetName() << "==================";

    // the global and local coordinates in case of the beam parametrized area are the same
    G4ThreeVector in_pos = Unsmear_z_position(track);
    G4ThreeVector in_mom = track.GetPrimaryTrackLocalMomentum();
    edm::LogInfo("TotemRPProtonTransportModel") << "input ->" <<
    " position: " << in_pos <<
    " momentum: " << in_mom;

    double in_position[3] = {in_pos.x() / meter, in_pos.y() / meter, zin_};
    double in_momentum[3] = {in_mom.x() / GeV, in_mom.y() / GeV, in_mom.z() / GeV};
    double out_position[3];
    double out_momentum[3];
    edm::LogInfo("TotemRPProtonTransportModel") << "before transport ->" <<
    " position: " << in_position[0] << ", " << in_position[1] << ", " << in_position[2] <<
    " momentum: " << in_momentum[0] << ", " << in_momentum[1] << ", " << in_momentum[2];

    bool tracked = approximator_.Transport_m_GeV(in_position, in_momentum,
                                                 out_position, out_momentum, true, zout_ - zin_);

    if (!tracked) {
        edm::LogInfo("TotemRPProtonTransportModel") << "Proton not tracked";
        edm::LogInfo("TotemRPProtonTransportModel") << "===== END Transport " << GetName() << "====================" <<
        endl;
        return false;
    }

    edm::LogInfo("TotemRPProtonTransportModel") << "after transport -> " <<
    "position: " << out_position[0] << ", " << out_position[1] << ", " << out_position[2] <<
    "momentum: " << out_momentum[0] << ", " << out_momentum[1] << ", " << out_momentum[2];

    if (out_position[0] * out_position[0] + out_position[1] * out_position[1] >
        beampipe_aperture_radius * beampipe_aperture_radius) {
        edm::LogInfo("TotemRPProtonTransportModel") << "Proton ouside beampipe";
        edm::LogInfo("TotemRPProtonTransportModel") << "===== END Transport " << GetName() << "====================";
        return false;
    }

    G4ThreeVector out_pos(out_position[0] * meter, out_position[1] * meter, out_position[2] * meter);
    G4ThreeVector out_mom(out_momentum[0] * GeV, out_momentum[1] * GeV, out_momentum[2] * GeV);
    edm::LogInfo("TotemRPProtonTransportModel") << "output -> " <<
    "position: " << out_pos << " momentum: " << out_mom << endl;

    // Adds one "secondary". First, user has to say how many secondaries will be created:
    step.SetNumberOfSecondaryTracks(1);

    // Build the secondary:
    G4ParticleMomentum direction(out_mom.unit());
    G4DynamicParticle dynamique(G4Proton::ProtonDefinition(),
                                direction,
                                track.GetPrimaryTrack()->GetKineticEnergy());
    step.CreateSecondaryTrack(dynamique, out_pos, track.GetPrimaryTrack()->GetGlobalTime());

    edm::LogInfo("TotemRPProtonTransportModel") << "Proton transported successfully";
    edm::LogInfo("TotemRPProtonTransportModel") << "===== END Transport " << GetName() << "====================";
    return true;
}


G4ThreeVector TotemRPProtonTransportModel::Unsmear_z_position(const G4FastTrack &track) {
    edm::LogInfo("TotemRPProtonTransportModel") << "Unsmear_z_position";

    G4ThreeVector registered_pos;
    if (transport_region_ == BEAM_IP_150_R || transport_region_ == BEAM_IP_150_L) {
        edm::LogInfo("TotemRPProtonTransportModel") << "BEAM_IP_150_R || BEAM_IP_150_L";
        registered_pos = track.GetPrimaryTrack()->GetVertexPosition();
    }
    else {
        edm::LogInfo("TotemRPProtonTransportModel") << "NOT A BEAM_IP_150_R || BEAM_IP_150_L";
        registered_pos = track.GetPrimaryTrackLocalPosition();
    }

    double dist = registered_pos.z() - zin_ * m;
    edm::LogInfo("TotemRPProtonTransportModel") << "z distance to normalize position: " << dist;

    G4ThreeVector dir = track.GetPrimaryTrackLocalDirection();
    dir = dir / dir.z();
    G4ThreeVector pos_for_param = registered_pos - dir * dist;

    return pos_for_param;
}

