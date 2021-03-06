#ifndef RPCChamberQuality_H
#define RPCChamberQuality_H

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include <FWCore/Framework/interface/EDAnalyzer.h>
#include <FWCore/Framework/interface/ESHandle.h>
#include <FWCore/Framework/interface/MakerMacros.h>
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DQMServices/Core/interface/MonitorElement.h"
//#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "DQM/RPCMonitorClient/interface/RPCClient.h"

#include <memory>
#include <string>
#include <map>

class DQMStore;

class RPCChamberQuality:public edm::EDAnalyzer {
public:

  RPCChamberQuality(const edm::ParameterSet& ps);
  virtual ~RPCChamberQuality();


  void beginJob();
  void beginRun(const edm::Run& r, const edm::EventSetup& c);
  void beginLuminosityBlock(edm::LuminosityBlock const& lumiSeg, edm::EventSetup const& context) ;
  void analyze(const edm::Event& iEvent, const edm::EventSetup& c);
  void endLuminosityBlock(edm::LuminosityBlock const& lumiSeg, edm::EventSetup const& c);
  void endRun(const edm::Run& r, const edm::EventSetup& c);
  
 private:


  enum chamberQualityState { GoodState= 1 , OffState =2, NoisyStripState= 3, NoisyRollState= 4 , PartiallyDeadState=5 , DeadState=6,BadShapeState=7 };


  void performeClientOperation(std::string , int , MonitorElement *);
  
  std::string prefixDir_;
  static const std::string xLabels_[7];
  static const std::string regions_[3];

  int prescaleFactor_;
  int numberOfDisks_;

  //  bool init_;
  DQMStore* dbe_;

  int minEvents;
  int numLumBlock_;
};

#endif
