/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*  Jan Kašpar (jan.kaspar@gmail.com) 
*    
* $Id: PrintDAQMapping.cc 4868 2011-08-26 13:35:56Z pkaczmar $
* $Revision: 4868 $
* $Date: 2011-08-26 15:35:56 +0200 (Fri, 26 Aug 2011) $
*
****************************************************************************/

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "TotemCondFormats/DAQInformation/interface/AnalysisMask.h"
#include "TotemCondFormats/DAQInformation/interface/DAQMapping.h"
#include "TotemCondFormats/DataRecord/interface/TotemDAQMappingRecord.h"

/**
 *\brief Prints the DAQ mapping loaded by DAQMappingSourceXML.
 **/
class PrintDAQMapping : public edm::EDAnalyzer
{
  public:
    PrintDAQMapping(const edm::ParameterSet &ps) {}
    ~PrintDAQMapping() {}

  private:
    virtual void beginRun(edm::Run const&, edm::EventSetup const&);
    virtual void analyze(const edm::Event &e, const edm::EventSetup &es) {}
    virtual void endJob() {}
};

using namespace std;
using namespace edm;

//----------------------------------------------------------------------------------------------------

void PrintDAQMapping::beginRun(edm::Run const&, edm::EventSetup const& es)
{
  // get mapping
  ESHandle<DAQMapping> mapping;
  es.get<TotemDAQMappingRecord>().get(mapping);

  // get analysis mask to mask channels
  ESHandle<AnalysisMask> analysisMask;
  es.get<TotemDAQMappingRecord>().get(analysisMask);

  for (DAQMapping::maptype::const_iterator it = mapping->VFATMapping.begin();
      it != mapping->VFATMapping.end(); ++it) {
    cout << it->first << " -> " << it->second << endl;
  }
}

//----------------------------------------------------------------------------------------------------

DEFINE_FWK_MODULE(PrintDAQMapping);

