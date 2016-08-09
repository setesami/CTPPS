#ifndef ConfigurationTotemOpticsConfigurationBeamOpticsParamsESSource_h
#define ConfigurationTotemOpticsConfigurationBeamOpticsParamsESSource_h

#include <memory>
#include "boost/shared_ptr.hpp"


#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "TotemCondFormats/BeamOpticsParamsObjects/interface/BeamOpticsParams.h"
#include "TotemCondFormats/DataRecord/interface/BeamOpticsParamsRcd.h"


class BeamOpticsParamsESSource : public edm::ESProducer, public edm::EventSetupRecordIntervalFinder
{
  public:
  BeamOpticsParamsESSource(const edm::ParameterSet &);
  ~BeamOpticsParamsESSource();
  
  //      typedef edm::ESProducts<> ReturnType;
  
  virtual std::auto_ptr<BeamOpticsParams> produce(const BeamOpticsParamsRcd &);
  
  protected:
    virtual void setIntervalFor( const edm::eventsetup::EventSetupRecordKey&,
        const edm::IOVSyncValue&, edm::ValidityInterval& );

  private:
    edm::ParameterSet conf_;
};



#endif
