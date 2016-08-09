/****************************************************************************
*
* This is a part of TOTEM ofFileline software.
* Authors:
*   Jan Kašpar (jan.kaspar@gmail.com)
*
* $$RCSfile: ProtonTransportFunctionsESSource.cc,v $: $
* $Revision: 1.1.2.4 $
* $Date: 2009/11/16 16:54:05 $
*
****************************************************************************/

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/SourceFactory.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"

#include <CLHEP/Vector/TwoVector.h>

#include "TotemCondFormats/DataRecord/interface/BeamOpticsParamsRcd.h"
#include "TotemCondFormats/DataRecord/interface/ProtonTransportRcd.h"
#include "TotemCondFormats/BeamOpticsParamsObjects/interface/BeamOpticsParams.h"
#include "TotemCondFormats/ProtonTransportFunctions/interface/ProtonTransportFunctions.h"
#include "SimG4Core/TotemRPProtonTransportParametrization/interface/LHCOpticsApproximator.h"
#include "Geometry/TotemRecords/interface/RealGeometryRecord.h"
#include "Geometry/TotemRPGeometryBuilder/interface/TotemRPGeometry.h"

#include "TFile.h"
#include "TKey.h"

#define DEBUG 0


class ProtonTransportFunctionsESSource : public edm::ESProducer
{
  public:
    ProtonTransportFunctionsESSource(const edm::ParameterSet &);
  	~ProtonTransportFunctionsESSource();
  
	virtual std::auto_ptr<ProtonTransportFunctions> produce(const ProtonTransportRcd &);
  
  private:
    std::string opticsFile;
    bool maySymmetrize;
    unsigned int verbosity;

    TFile* ofFile;
    ProtonTransportFunctions data;
    bool idealFunctionsLoaded;

    void LoadIdealFunctions(const ProtonTransportRcd &ptRcd);
};

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

using namespace std;
using namespace edm;

ProtonTransportFunctionsESSource::ProtonTransportFunctionsESSource(const edm::ParameterSet& conf) : 
  opticsFile(conf.getParameter<string>("opticsFile")),
  maySymmetrize(conf.getParameter<bool>("maySymmetrize")),
  verbosity(conf.getUntrackedParameter<unsigned int>("verbosity", 1)),
  ofFile(NULL),
  idealFunctionsLoaded(false)
{
#if DEBUG > 1
  printf(">> ProtonTransportFunctionsESSource::ProtonTransportFunctionsESSource\n");
#endif

  setWhatProduced(this);
}

//----------------------------------------------------------------------------------------------------

ProtonTransportFunctionsESSource::~ProtonTransportFunctionsESSource()
{ 
  // this automatically frees the ideal functions
  if (ofFile)
    delete ofFile;
}

//----------------------------------------------------------------------------------------------------

void ProtonTransportFunctionsESSource::LoadIdealFunctions(const ProtonTransportRcd &ptRcd)
{
#if DEBUG > 1
  printf(">> ProtonTransportFunctionsESSource::LoadIdealFunctions\n");
#endif

  // determine file name
  if (opticsFile.empty()) {
    ESHandle<BeamOpticsParams> optPar;
    ptRcd.getRecord<BeamOpticsParamsRcd>().get(optPar);
    optPar->GetStdRecoGeometryFile(opticsFile);
  } else {
    char *cmsswPath = getenv("CMSSW_BASE");
    opticsFile = std::string(cmsswPath) + "/src/" + opticsFile;
  }

  // open file
  ofFile = new TFile(opticsFile.c_str());
  if (ofFile->IsZombie())
    throw cms::Exception("ProtonTransportFunctionsESSource::produce") << "File `" << opticsFile << "' cannot be opened." << endl;

  // allocate output
  auto_ptr<ProtonTransportFunctions> ptf(new ProtonTransportFunctions());

  // parse the geometry file
  TIter next(ofFile->GetListOfKeys());
  TKey *key;
  unsigned char fCount = 0;
  while ((key = (TKey *)next())) {
    if (strcmp(key->GetClassName(), "LHCOpticsApproximator")) continue;
    //printf(">> %s\n", key->GetName());

    // process name
    string full(key->GetName());
    string begin(full, 0, 15);

    if (begin.compare("ip5_to_station_")) continue;

    string sSt(full, 15, 3);
    unsigned int st = 0;
    if (!sSt.compare("150")) st = 0;
      else if (!sSt.compare("220")) st = 2;
        else continue;

    string sPos(full, 19, 1);
    unsigned int pos = 0;
    if (!sPos.compare("v")) pos = 0;
      else if (!sPos.compare("h")) pos = 1;
        else continue;

    unsigned int unit = atoi(full.substr(21, 1).c_str());
    if (unit != 1 && unit != 2) continue;

    unsigned int arm = 2 - atoi(full.substr(27, 1).c_str());  // to standard
    if (arm != 0 && arm != 1) continue;

    //printf("> st %i, pos %i, unit %i, arm %i\n", st, pos, unit, arm);

    // name OK, add the object
    LHCOpticsApproximator *optFun = (LHCOpticsApproximator *) key->ReadObj();
    fCount++;

    // update map RP->function
    unsigned int RPId = 100 * arm + 10 * st;
    //printf("> %i\n", RPId);
    if (unit == 1 && pos == 0) data.InitFunction(RPId + 0, optFun);
    if (unit == 1 && pos == 0) data.InitFunction(RPId + 1, optFun);
    if (unit == 1 && pos == 1) data.InitFunction(RPId + 2, optFun);
    if (unit == 2 && pos == 1) data.InitFunction(RPId + 3, optFun);
    if (unit == 2 && pos == 0) data.InitFunction(RPId + 4, optFun);
    if (unit == 2 && pos == 0) data.InitFunction(RPId + 5, optFun);

    // add symmetric links to the RP->functions map, if permitted
    if (!maySymmetrize)
      continue;
    string reflected(full);
    if (arm == 0)
      reflected.replace(27, 1, "1");
    else
      reflected.replace(27, 1, "2");
    if (ofFile->Get(reflected.c_str()))
      continue;
    arm = 1 - arm;
	RPId = 100 * arm + 10 * st;
    if (unit == 1 && pos == 0) data.InitFunction(RPId + 0, optFun);
    if (unit == 1 && pos == 0) data.InitFunction(RPId + 1, optFun);
    if (unit == 1 && pos == 1) data.InitFunction(RPId + 2, optFun);
    if (unit == 2 && pos == 1) data.InitFunction(RPId + 3, optFun);
    if (unit == 2 && pos == 0) data.InitFunction(RPId + 4, optFun);
    if (unit == 2 && pos == 0) data.InitFunction(RPId + 5, optFun);
  }


  if (verbosity)
    printf(">> ProtonTransportFunctionsESSource::LoadIdealFunctions : %u optical functions read, mapped to %lu RPs\n",
        fCount, data.functionMap.size());

  if (verbosity > 4) {
    printf(">> ProtonTransportFunctionsESSource::LoadIdealFunctions : map RPId --> optical function\n");
    for (map<unsigned int, ProtonTransportFunctions::FunctionPair>::const_iterator it = data.functionMap.begin(); it != data.functionMap.end(); ++it) {
      printf("\t%3u --> %s\n", it->first, it->second.ideal->GetName());
    }
  }

  idealFunctionsLoaded = true;
}

//----------------------------------------------------------------------------------------------------

std::auto_ptr<ProtonTransportFunctions> ProtonTransportFunctionsESSource::produce(const ProtonTransportRcd &ptRcd)
{
#if DEBUG > 1
  printf(">> ProtonTransportFunctionsESSource::produce\n");
#endif

  if (!idealFunctionsLoaded)
    LoadIdealFunctions(ptRcd);

  auto_ptr<ProtonTransportFunctions> ptf(new ProtonTransportFunctions(data));
  return ptf;
}

//----------------------------------------------------------------------------------------------------

DEFINE_FWK_EVENTSETUP_MODULE(ProtonTransportFunctionsESSource);

