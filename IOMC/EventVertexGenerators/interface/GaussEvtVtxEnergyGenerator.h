#ifndef IOMC_GaussEvtVtxEnergyGenerator_H
#define IOMC_GaussEvtVtxEnergyGenerator_H

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"

class GaussEvtVtxGenerator : public edm::EDProducer{
public:
    GaussEvtVtxGenerator(const edm::ParameterSet &p);
    virtual ~GaussEvtVtxGenerator();
    virtual void produce( edm::Event&, const edm::EventSetup&);

private:
    double fSigmaX, fSigmaY, fSigmaZ;
    double fMeanX,  fMeanY,  fMeanZ;
    double fTimeOffset;
    edm::InputTag sourceLabel;

};

#endif
