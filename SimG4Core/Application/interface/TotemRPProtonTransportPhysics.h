#ifndef SimG4Core_TotemRPProtonTransportPhysics_H
#define SimG4Core_TotemRPProtonTransportPhysics_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "G4VPhysicsConstructor.hh"

class TotemRPProtonTransportPhysics : public G4VPhysicsConstructor {
public:
    TotemRPProtonTransportPhysics(std::string name, const edm::ParameterSet &p);

    virtual ~TotemRPProtonTransportPhysics();

    virtual void ConstructParticle();

    virtual void ConstructProcess();

private:
    edm::ParameterSet theParSet;

};

#endif

