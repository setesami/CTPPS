//
// Joanna Weng 08.2005
// Physics process for Gflash parameterisation
// modified by Soon Yung Jun, Dongwook Jang
// V.Ivanchenko rename the class, cleanup, and move
//              to SimG4Core/Application - 2012/08/14

#include "SimG4Core/Application/interface/TotemRPProtonTransportPhysics.h"
#include "SimG4Core/Application/interface/TotemRPProtonTransportSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "G4FastSimulationManagerProcess.hh"
#include "G4ProcessManager.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4ShortLivedConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4RegionStore.hh"
#include "G4SystemOfUnits.hh"


G4ThreadLocal G4FastSimulationManagerProcess* theFastSimulationManagerProcess = 0;
G4ThreadLocal TotemRPProtonTransportSetup* proton_transport_setup = 0;

TotemRPProtonTransportPhysics::TotemRPProtonTransportPhysics(std::string name, const edm::ParameterSet &p)
        : G4VPhysicsConstructor(name), theParSet(p) {
}

TotemRPProtonTransportPhysics::~TotemRPProtonTransportPhysics() {
}

void TotemRPProtonTransportPhysics::ConstructParticle() {
    edm::LogInfo("TotemRPProtonTransportPhysics") << "ConstructParticle";
    G4LeptonConstructor pLeptonConstructor;
    pLeptonConstructor.ConstructParticle();

    G4MesonConstructor pMesonConstructor;
    pMesonConstructor.ConstructParticle();

    G4BaryonConstructor pBaryonConstructor;
    pBaryonConstructor.ConstructParticle();

    G4ShortLivedConstructor pShortLivedConstructor;
    pShortLivedConstructor.ConstructParticle();

    G4IonConstructor pConstructor;
    pConstructor.ConstructParticle();
}

void TotemRPProtonTransportPhysics::ConstructProcess() {
    edm::LogInfo("TotemRPProtonTransportPhysics") << "ConstructProcess";

    if (proton_transport_setup == 0)
        proton_transport_setup = new TotemRPProtonTransportSetup(theParSet);

    if (theFastSimulationManagerProcess == 0)
        theFastSimulationManagerProcess =
                new G4FastSimulationManagerProcess("TotemRPParameterisationProcess", fParameterisation);

    aParticleIterator->reset();
    while ((*aParticleIterator)()) {
        G4ParticleDefinition *particle = aParticleIterator->value();
        G4ProcessManager *pmanager = particle->GetProcessManager();
        pmanager->AddProcess(theFastSimulationManagerProcess, -1, 1, 1);
    }
}
