/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*  Jan Kašpar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/RandomNumberGenerator.h"
#include "CLHEP/Random/RandomEngine.h"

#include "IOMC/Elegent/interface/Generator.h"

#include "TRandom2.h"

/**
 *\brief Monte-Carlo generator of (anti)proton-pron elastic collisions
 *  A CMSSW wrapper for Elegent::Generator, for details see
 *    http://elegent.hepforge.org/
 **/
class ElegentSource : public edm::EDProducer
{
  public:
	ElegentSource(const edm::ParameterSet &);
    virtual ~ElegentSource();
    virtual void beginJob();

  protected:
    unsigned int verbosity;

    Elegent::Generator* generator;

    virtual void produce(edm::Event&, const edm::EventSetup&);
  private:
    const UInt_t DEFAULT_SEED = 65539;
};



using namespace HepMC;
using namespace std;
using namespace edm;

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

ElegentSource::ElegentSource(const ParameterSet& pSet) :
  verbosity(pSet.getUntrackedParameter<unsigned int>("verbosity", 1))
{
  string fileName = pSet.getParameter<string>("fileName");
  if (fileName[0] != '/' && fileName[0] != '.')
  {
    char *cmsswPath = getenv("CMSSW_BASE");
    if (cmsswPath)
      fileName = string(cmsswPath) + string("/src/") + fileName;
  }

  generator = new Elegent::Generator(
    fileName,
    pSet.getParameter<string>("model") + "/cumulative cross-section",
    pSet.getParameter<double>("t_min"),
    pSet.getParameter<double>("t_max"),
    verbosity
  );

  produces<HepMCProduct>();
}

//----------------------------------------------------------------------------------------------------

ElegentSource::~ElegentSource()
{
  delete generator;
}

//----------------------------------------------------------------------------------------------------

void ElegentSource::beginJob()
{
  if (generator->Init() != 0)
    throw cms::Exception("ElegentSource::beginJob") << "Can not initialise Elegent::Generator.";
}

//----------------------------------------------------------------------------------------------------

void ElegentSource::produce(edm::Event &e, const edm::EventSetup &es)
{
  // initialize random seed
  if(gRandom->GetSeed() == DEFAULT_SEED) {
    Service<RandomNumberGenerator> rng;
    CLHEP::HepRandomEngine &rndEng = rng->getEngine(e.streamID());
    unsigned int seed = rndEng.getSeed();
    gRandom->SetSeed(seed);
    if (verbosity > 0)
      printf(">> ElegentSource > seed = %u\n", seed);
  }

  // create event structure 
  GenEvent* gEv = new GenEvent();
  gEv->set_event_number(e.id().event());

  // run generator
  generator->Generate(gEv);
    
  // store generator event to the FW event
  auto_ptr<HepMCProduct> output(new HepMCProduct());
  output->addHepMCData(gEv);
  e.put(output);
}

DEFINE_FWK_MODULE(ElegentSource);
