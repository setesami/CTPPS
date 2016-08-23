#include "IOMC/EventVertexGenerators/interface/GaussEvtVtxEnergyGenerator.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CLHEP/Random/RandGaussQ.h"
#include "CLHEP/Units/GlobalSystemOfUnits.h"
#include "CLHEP/Units/GlobalPhysicalConstants.h"
//#include "CLHEP/Vector/ThreeVector.h"
#include "HepMC/SimpleVector.h"
#include "FWCore/Framework/interface/Event.h"

GaussEvtVtxEnergyGenerator::GaussEvtVtxEnergyGenerator(const edm::ParameterSet & p )
        : sourceLabel(pset.getParameter<edm::InputTag>("src"))
{
    fMeanX =  p.getParameter<double>("MeanX")*cm;
    fMeanY =  p.getParameter<double>("MeanY")*cm;
    fMeanZ =  p.getParameter<double>("MeanZ")*cm;
    fSigmaX = p.getParameter<double>("SigmaX")*cm;
    fSigmaY = p.getParameter<double>("SigmaY")*cm;
    fSigmaZ = p.getParameter<double>("SigmaZ")*cm;
    fTimeOffset = p.getParameter<double>("TimeOffset")*ns*c_light;

    if (fSigmaX < 0) {
        throw cms::Exception("Configuration")
              << "Error in GaussEvtVtxGenerator: "
              << "Illegal resolution in X (SigmaX is negative)";
    }
    if (fSigmaY < 0) {
        throw cms::Exception("Configuration")
              << "Error in GaussEvtVtxGenerator: "
              << "Illegal resolution in Y (SigmaY is negative)";
    }
    if (fSigmaZ < 0) {
        throw cms::Exception("Configuration")
              << "Error in GaussEvtVtxGenerator: "
              << "Illegal resolution in Z (SigmaZ is negative)";
    }

    consumes<edm::HepMCProduct>(sourceLabel);
    produces<edm::HepMCProduct>();
}

GaussEvtVtxGenerator::~GaussEvtVtxGenerator()
{
}

void SmearingGenerator::produce(edm::Event& event, const edm::EventSetup& es)
{
    edm::Service<edm::RandomNumberGenerator> rng;
    CLHEP::HepRandomEngine* engine = &rng->getEngine(evt.streamID());

    Handle<HepMCProduct> HepUnsmearedMCEvt ;

    evt.getByLabel( sourceLabel, HepUnsmearedMCEvt );

    GenEvent *genevt = (GenEvent *) HepUnsmearedMCEvt->GetEvent();

    //ApplyBeamSmearing(genevt);
    //ApplyVertexSmearing(genevt);

    return;
}
