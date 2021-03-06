/*
 * \file EEBeamHodoTask.cc
 *
 * $Date: 2010/03/27 20:08:00 $
 * $Revision: 1.29 $
 * \author G. Della Ricca
 * \author G. Franzoni
 *
 */

#include <iostream>
#include <fstream>

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DQMServices/Core/interface/MonitorElement.h"

#include "DQMServices/Core/interface/DQMStore.h"

#include "DataFormats/EcalRawData/interface/EcalRawDataCollections.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "TBDataFormats/EcalTBObjects/interface/EcalTBCollections.h"

#include "DQM/EcalCommon/interface/Numbers.h"

#include "DQM/EcalEndcapMonitorTasks/interface/EEBeamHodoTask.h"

EEBeamHodoTask::EEBeamHodoTask(const edm::ParameterSet& ps){

  init_ = false;

  dqmStore_ = edm::Service<DQMStore>().operator->();

  prefixME_ = ps.getUntrackedParameter<std::string>("prefixME", "");

  enableCleanup_ = ps.getUntrackedParameter<bool>("enableCleanup", false);

  mergeRuns_ = ps.getUntrackedParameter<bool>("mergeRuns", false);

  EcalTBEventHeader_ = ps.getParameter<edm::InputTag>("EcalTBEventHeader");
  EcalRawDataCollection_ = ps.getParameter<edm::InputTag>("EcalRawDataCollection");
  EcalUncalibratedRecHitCollection_ = ps.getParameter<edm::InputTag>("EcalUncalibratedRecHitCollection");
  EcalTBTDCRawInfo_ = ps.getParameter<edm::InputTag>("EcalTBTDCRawInfo");
  EcalTBHodoscopeRawInfo_ = ps.getParameter<edm::InputTag>("EcalTBHodoscopeRawInfo");
  EcalTBTDCRecInfo_ = ps.getParameter<edm::InputTag>("EcalTBTDCRecInfo");
  EcalTBHodoscopeRecInfo_ = ps.getParameter<edm::InputTag>("EcalTBHodoscopeRecInfo");

  tableIsMoving_ = false;
  cryInBeam_ =0;
  previousCryInBeam_ = -99999;

  // to be filled all the time
  for (int i=0; i<4; i++) {
    meHodoOcc_[i] =0;
    meHodoRaw_[i] =0;
  }
  meTDCRec_      =0;

  // filled only when: the table does not move
  meHodoPosRecX_    =0;
  meHodoPosRecY_    =0;
  meHodoPosRecXY_ =0;
  meHodoSloXRec_  =0;
  meHodoSloYRec_  =0;
  meHodoQuaXRec_ =0;
  meHodoQuaYRec_ =0;
  meHodoPosXMinusCaloPosXVsCry_   =0;
  meHodoPosYMinusCaloPosYVsCry_   =0;
  meTDCTimeMinusCaloTimeVsCry_ =0;
  meMissingCollections_        =0;

  meEvsXRecProf_     =0;
  meEvsYRecProf_     =0;
  meEvsXRecHis_     =0;
  meEvsYRecHis_     =0;

  //                       and matrix 5x5 available
  meCaloVsHodoXPos_ =0;
  meCaloVsHodoYPos_ =0;
  meCaloVsTDCTime_    =0;

}

EEBeamHodoTask::~EEBeamHodoTask(){

}

void EEBeamHodoTask::beginJob(void){

  ievt_  = 0;

  LV1_ = 0;
  cryInBeamCounter_ =0;
  resetNow_                =false;

  if ( dqmStore_ ) {
    dqmStore_->setCurrentFolder(prefixME_ + "/EEBeamHodoTask");
    dqmStore_->rmdir(prefixME_ + "/EEBeamHodoTask");
  }

}

void EEBeamHodoTask::beginRun(const edm::Run& r, const edm::EventSetup& c) {

  Numbers::initGeometry(c, false);

  if ( ! mergeRuns_ ) this->reset();

}

void EEBeamHodoTask::endRun(const edm::Run& r, const edm::EventSetup& c) {

}

void EEBeamHodoTask::reset(void) {

  for (int i=0; i<4; i++) {
    if ( meHodoOcc_[i] ) meHodoOcc_[i]->Reset();
    if ( meHodoRaw_[i] ) meHodoRaw_[i]->Reset();
  }

  if ( meHodoPosRecX_ ) meHodoPosRecX_->Reset();
  if ( meHodoPosRecY_ ) meHodoPosRecY_->Reset();
  if ( meHodoPosRecXY_ ) meHodoPosRecXY_->Reset();
  if ( meHodoSloXRec_ ) meHodoSloXRec_->Reset();
  if ( meHodoSloYRec_ ) meHodoSloYRec_->Reset();
  if ( meHodoQuaXRec_ ) meHodoQuaXRec_->Reset();
  if ( meHodoQuaYRec_ ) meHodoQuaYRec_->Reset();
  if ( meTDCRec_ ) meTDCRec_->Reset();
  if ( meEvsXRecProf_ ) meEvsXRecProf_->Reset();
  if ( meEvsYRecProf_ ) meEvsYRecProf_->Reset();
  if ( meEvsXRecHis_ ) meEvsXRecHis_->Reset();
  if ( meEvsYRecHis_ ) meEvsYRecHis_->Reset();
  if ( meCaloVsHodoXPos_ ) meCaloVsHodoXPos_->Reset();
  if ( meCaloVsHodoYPos_ ) meCaloVsHodoYPos_->Reset();
  if ( meCaloVsTDCTime_ ) meCaloVsTDCTime_->Reset();
  if ( meHodoPosXMinusCaloPosXVsCry_  ) meHodoPosXMinusCaloPosXVsCry_->Reset();
  if ( meHodoPosYMinusCaloPosYVsCry_  ) meHodoPosYMinusCaloPosYVsCry_->Reset();
  if ( meTDCTimeMinusCaloTimeVsCry_  ) meTDCTimeMinusCaloTimeVsCry_->Reset();
  if ( meMissingCollections_  ) meMissingCollections_->Reset();

}

void EEBeamHodoTask::setup(void){

  init_ = true;

  smId =1;

  char histo[200];

  if ( dqmStore_ ) {
    dqmStore_->setCurrentFolder(prefixME_ + "/EEBeamHodoTask");

    // following ME (type I):
    //  *** do not need to be ever reset
    //  *** can be filled regardless of the moving/notMoving status of the table

    for (int i=0; i<4; i++) {
      sprintf(histo, "EEBHT occup %s %02d", Numbers::sEE(smId).c_str(), i+1);
      meHodoOcc_[i] = dqmStore_->book1D(histo, histo, 30, 0., 30.);
      meHodoOcc_[i]->setAxisTitle("hits per event", 1);
      sprintf(histo, "EEBHT raw %s %02d", Numbers::sEE(smId).c_str(), i+1);
      meHodoRaw_[i] = dqmStore_->book1D(histo, histo, 64, 0., 64.);
      meHodoRaw_[i]->setAxisTitle("hodo fiber number", 1);
    }

    sprintf(histo, "EEBHT PosX rec %s", Numbers::sEE(smId).c_str());
    meHodoPosRecX_ = dqmStore_->book1D(histo, histo, 100, -20, 20);
    meHodoPosRecX_->setAxisTitle("reconstructed position    (mm)", 1);

    sprintf(histo, "EEBHT PosY rec %s", Numbers::sEE(smId).c_str());
    meHodoPosRecY_ = dqmStore_->book1D(histo, histo, 100, -20, 20);
    meHodoPosRecY_->setAxisTitle("reconstructed position    (mm)", 1);

    sprintf(histo, "EEBHT PosYX rec %s", Numbers::sEE(smId).c_str());
    meHodoPosRecXY_ = dqmStore_->book2D(histo, histo, 100, -20, 20,100, -20, 20);
    meHodoPosRecXY_->setAxisTitle("reconstructed X position    (mm)", 1);
    meHodoPosRecXY_->setAxisTitle("reconstructed Y position    (mm)", 2);

    sprintf(histo, "EEBHT SloX %s", Numbers::sEE(smId).c_str());
    meHodoSloXRec_ = dqmStore_->book1D(histo, histo, 50, -0.005, 0.005);
    meHodoSloXRec_->setAxisTitle("reconstructed track slope", 1);

    sprintf(histo, "EEBHT SloY %s", Numbers::sEE(smId).c_str());
    meHodoSloYRec_ = dqmStore_->book1D(histo, histo, 50, -0.005, 0.005);
    meHodoSloYRec_->setAxisTitle("reconstructed track slope", 1);

    sprintf(histo, "EEBHT QualX %s", Numbers::sEE(smId).c_str());
    meHodoQuaXRec_ = dqmStore_->book1D(histo, histo, 50, 0, 5);
    meHodoQuaXRec_->setAxisTitle("track fit quality", 1);

    sprintf(histo, "EEBHT QualY %s", Numbers::sEE(smId).c_str());
    meHodoQuaYRec_ = dqmStore_->book1D(histo, histo, 50, 0, 5);
    meHodoQuaYRec_->setAxisTitle("track fit quality", 1);

    sprintf(histo, "EEBHT TDC rec %s", Numbers::sEE(smId).c_str());
    meTDCRec_  = dqmStore_->book1D(histo, histo, 25, 0, 1);
    meTDCRec_->setAxisTitle("offset", 1);

    sprintf(histo, "EEBHT Hodo-Calo X vs Cry %s", Numbers::sEE(smId).c_str());
    meHodoPosXMinusCaloPosXVsCry_  = dqmStore_->book1D(histo, histo, 50, 0, 50);
    meHodoPosXMinusCaloPosXVsCry_->setAxisTitle("scan step number", 1);
    meHodoPosXMinusCaloPosXVsCry_->setAxisTitle("PosX_{hodo} - PosX_{calo}    (mm)", 2);

    sprintf(histo, "EEBHT Hodo-Calo Y vs Cry %s", Numbers::sEE(smId).c_str());
    meHodoPosYMinusCaloPosYVsCry_  = dqmStore_->book1D(histo, histo, 50, 0, 50);
    meHodoPosYMinusCaloPosYVsCry_->setAxisTitle("scan step number", 1);
    meHodoPosYMinusCaloPosYVsCry_->setAxisTitle("PosY_{hodo} - PosY_{calo}    (mm)", 2);

    sprintf(histo, "EEBHT TDC-Calo vs Cry %s", Numbers::sEE(smId).c_str());
    meTDCTimeMinusCaloTimeVsCry_  = dqmStore_->book1D(histo, histo, 50, 0, 50);
    meTDCTimeMinusCaloTimeVsCry_->setAxisTitle("scan step number", 1);
    meTDCTimeMinusCaloTimeVsCry_->setAxisTitle("Time_{TDC} - Time_{calo}    (sample)", 2);

    sprintf(histo, "EEBHT Missing Collections %s", Numbers::sEE(smId).c_str());
    meMissingCollections_ = dqmStore_->book1D(histo, histo, 7, 0, 7);
    meMissingCollections_->setAxisTitle("missing collection", 1);

    // following ME (type II):
    //  *** can be filled only when table is **not** moving
    //  *** need to be reset once table goes from 'moving'->notMoving

    sprintf(histo, "EEBHT prof E1 vs X %s", Numbers::sEE(smId).c_str());
    meEvsXRecProf_    = dqmStore_-> bookProfile(histo, histo, 100, -20, 20, 500, 0, 5000, "s");
    meEvsXRecProf_->setAxisTitle("PosX    (mm)", 1);
    meEvsXRecProf_->setAxisTitle("E1 (ADC)", 2);

    sprintf(histo, "EEBHT prof E1 vs Y %s", Numbers::sEE(smId).c_str());
    meEvsYRecProf_    = dqmStore_-> bookProfile(histo, histo, 100, -20, 20, 500, 0, 5000, "s");
    meEvsYRecProf_->setAxisTitle("PosY    (mm)", 1);
    meEvsYRecProf_->setAxisTitle("E1 (ADC)", 2);

    sprintf(histo, "EEBHT his E1 vs X %s", Numbers::sEE(smId).c_str());
    meEvsXRecHis_    = dqmStore_-> book2D(histo, histo, 100, -20, 20, 500, 0, 5000);
    meEvsXRecHis_->setAxisTitle("PosX    (mm)", 1);
    meEvsXRecHis_->setAxisTitle("E1 (ADC)", 2);

    sprintf(histo, "EEBHT his E1 vs Y %s", Numbers::sEE(smId).c_str());
    meEvsYRecHis_    = dqmStore_-> book2D(histo, histo, 100, -20, 20, 500, 0, 5000);
    meEvsYRecHis_->setAxisTitle("PosY    (mm)", 1);
    meEvsYRecHis_->setAxisTitle("E1 (ADC)", 2);

    sprintf(histo, "EEBHT PosX Hodo-Calo %s", Numbers::sEE(smId).c_str());
    meCaloVsHodoXPos_   = dqmStore_->book1D(histo, histo, 40, -20, 20);
    meCaloVsHodoXPos_->setAxisTitle("PosX_{hodo} - PosX_{calo}     (mm)", 1);

    sprintf(histo, "EEBHT PosY Hodo-Calo %s", Numbers::sEE(smId).c_str());
    meCaloVsHodoYPos_   = dqmStore_->book1D(histo, histo, 40, -20, 20);
    meCaloVsHodoYPos_->setAxisTitle("PosY_{hodo} - PosY_{calo}     (mm)", 1);

    sprintf(histo, "EEBHT TimeMax TDC-Calo %s", Numbers::sEE(smId).c_str());
    meCaloVsTDCTime_  = dqmStore_->book1D(histo, histo, 100, -1, 1);//tentative
    meCaloVsTDCTime_->setAxisTitle("Time_{TDC} - Time_{calo} (samples)", 1);

  }

}

void EEBeamHodoTask::cleanup(void){

  if ( ! init_ ) return;

  if ( dqmStore_ ) {
    dqmStore_->setCurrentFolder(prefixME_ + "/EEBeamHodoTask");

    for (int i=0; i<4; i++) {
      if ( meHodoOcc_[i] ) dqmStore_->removeElement( meHodoOcc_[i]->getName() );
      meHodoOcc_[i] = 0;
      if ( meHodoRaw_[i] ) dqmStore_->removeElement( meHodoRaw_[i]->getName() );
      meHodoRaw_[i] = 0;
    }

    if ( meHodoPosRecX_ ) dqmStore_->removeElement( meHodoPosRecX_->getName() );
    meHodoPosRecX_ = 0;
    if ( meHodoPosRecY_ ) dqmStore_->removeElement( meHodoPosRecY_->getName() );
    meHodoPosRecY_ = 0;
    if ( meHodoPosRecXY_ ) dqmStore_->removeElement( meHodoPosRecXY_->getName() );
    meHodoPosRecXY_ = 0;
    if ( meHodoSloXRec_ ) dqmStore_->removeElement( meHodoSloXRec_->getName() );
    meHodoSloXRec_ = 0;
    if ( meHodoSloYRec_ ) dqmStore_->removeElement( meHodoSloYRec_->getName() );
    meHodoSloYRec_ = 0;
    if ( meHodoQuaXRec_ ) dqmStore_->removeElement( meHodoQuaXRec_->getName() );
    meHodoQuaXRec_ = 0;
    if ( meHodoQuaYRec_ ) dqmStore_->removeElement( meHodoQuaYRec_->getName() );
    meHodoQuaYRec_ = 0;
    if ( meTDCRec_ ) dqmStore_->removeElement( meTDCRec_->getName() );
    meTDCRec_ = 0;
    if ( meEvsXRecProf_ ) dqmStore_->removeElement( meEvsXRecProf_->getName() );
    meEvsXRecProf_ = 0;
    if ( meEvsYRecProf_ ) dqmStore_->removeElement( meEvsYRecProf_->getName() );
    meEvsYRecProf_ = 0;
    if ( meEvsXRecHis_ ) dqmStore_->removeElement( meEvsXRecHis_->getName() );
    meEvsXRecHis_ = 0;
    if ( meEvsYRecHis_ ) dqmStore_->removeElement( meEvsYRecHis_->getName() );
    meEvsYRecHis_ = 0;
    if ( meCaloVsHodoXPos_ ) dqmStore_->removeElement( meCaloVsHodoXPos_->getName() );
    meCaloVsHodoXPos_ = 0;
    if ( meCaloVsHodoYPos_ ) dqmStore_->removeElement( meCaloVsHodoYPos_->getName() );
    meCaloVsHodoYPos_ = 0;
    if ( meCaloVsTDCTime_ ) dqmStore_->removeElement( meCaloVsTDCTime_->getName() );
    meCaloVsTDCTime_ = 0;
    if ( meHodoPosXMinusCaloPosXVsCry_  ) dqmStore_->removeElement( meHodoPosXMinusCaloPosXVsCry_->getName() );
    meHodoPosXMinusCaloPosXVsCry_  = 0;
    if ( meHodoPosYMinusCaloPosYVsCry_  ) dqmStore_->removeElement( meHodoPosYMinusCaloPosYVsCry_->getName() );
    meHodoPosYMinusCaloPosYVsCry_  = 0;
    if ( meTDCTimeMinusCaloTimeVsCry_  ) dqmStore_->removeElement( meTDCTimeMinusCaloTimeVsCry_->getName() );
    meTDCTimeMinusCaloTimeVsCry_  = 0;
    if ( meMissingCollections_  ) dqmStore_->removeElement( meMissingCollections_->getName() );
    meMissingCollections_  = 0;

  }

  init_ = false;

}

void EEBeamHodoTask::endJob(void){

  edm::LogInfo("EEBeamHodoTask") << "analyzed " << ievt_ << " events";

  if ( enableCleanup_ ) this->cleanup();

}

void EEBeamHodoTask::analyze(const edm::Event& e, const edm::EventSetup& c){

  bool enable = false;
  edm::Handle<EcalTBEventHeader> pHeader;
  const EcalTBEventHeader* Header =0;

  if ( e.getByLabel(EcalTBEventHeader_, pHeader) ) {
    Header = pHeader.product(); // get a ptr to the product
    if (!Header) {
      edm::LogWarning("EEBeamHodoTask") << "Event header not found. Returning. ";
      meMissingCollections_-> Fill(0); // bin1: missing CMSSW edm::Event header
      return;
    }
    tableIsMoving_     = Header->tableIsMoving();
    cryInBeam_           = Header->crystalInBeam();  //  cryInBeam_         = Header->nominalCrystalInBeam();
    if (previousCryInBeam_ == -99999 )
      {      previousCryInBeam_ = cryInBeam_ ;    }

    LogDebug("EEBeamHodoTask") << "event: " << ievt_ << " event header found ";
    if (tableIsMoving_){
      LogDebug("EEBeamHodoTask") << "Table is moving. ";  }
    else
      {      LogDebug("EEBeamHodoTask") << "Table is not moving. ";    }
  } else {
    edm::LogWarning("EEBeamHodoTask") << "Event header not found (exception caught). Returning. ";
    return;
  }

  edm::Handle<EcalRawDataCollection> dcchs;

  if ( e.getByLabel(EcalRawDataCollection_, dcchs) ) {

    for ( EcalRawDataCollection::const_iterator dcchItr = dcchs->begin(); dcchItr != dcchs->end(); ++dcchItr ) {

      if ( Numbers::subDet( *dcchItr ) != EcalEndcap ) continue;

      if ( dcchItr->getRunType() == EcalDCCHeaderBlock::BEAMH4 ||
	   dcchItr->getRunType() == EcalDCCHeaderBlock::BEAMH2  ) enable = true;
    }

  } else {
    edm::LogWarning("EcalBeamTask") << EcalRawDataCollection_ << " not available";
    meMissingCollections_-> Fill(1); // bin2: missing DCC headers
    return;
    // see bottom of cc file for compatibility to 2004 data [***]
  }

  if ( ! enable ) return;
  if ( ! init_ ) this->setup();
  ievt_++;

  LV1_ = Header->eventNumber();

  edm::Handle<EcalUncalibratedRecHitCollection> pUncalRH;
  const EcalUncalibratedRecHitCollection* uncalRecH =0;

  if ( e.getByLabel(EcalUncalibratedRecHitCollection_, pUncalRH) ) {
    uncalRecH = pUncalRH.product(); // get a ptr to the product
    int neh = pUncalRH->size();
    LogDebug("EEBeamHodoTask") << EcalUncalibratedRecHitCollection_ << " found in event " << ievt_ << "; hits collection size " << neh;
  } else {
    edm::LogWarning("EEBeamHodoTask") << EcalUncalibratedRecHitCollection_ << " not available";
    meMissingCollections_-> Fill(2); // bin3: missing uncalibRecHits
    return;
  }

  edm::Handle<EcalTBTDCRawInfo> pTDCRaw;
  const EcalTBTDCRawInfo* rawTDC=0;

  if ( e.getByLabel(EcalTBTDCRawInfo_, pTDCRaw) ) {
    rawTDC = pTDCRaw.product();
  } else {
    edm::LogError("EcalBeamTask") << "Error! Can't get the product EcalTBTDCRawInfo. Returning.";
    meMissingCollections_-> Fill(4); // bin5: missing raw TDC
    return;
  }

  edm::Handle<EcalTBHodoscopeRawInfo> pHodoRaw;
  const EcalTBHodoscopeRawInfo* rawHodo=0;

  if ( e.getByLabel(EcalTBHodoscopeRawInfo_, pHodoRaw) ) {
    rawHodo = pHodoRaw.product();
    if(rawHodo->planes() ){
    LogDebug("EcalBeamTask") << "hodoscopeRaw:  num planes: " <<  rawHodo->planes()
			     << " channels in plane 1: "  <<  rawHodo->channels(0);
    }
  } else {
    edm::LogError("EcalBeamTask") << "Error! Can't get the product EcalTBHodoscopeRawInfo. Returning";
    meMissingCollections_-> Fill(3); // bin4: missing raw hodo hits collection
    return;
  }

  if ( !rawTDC ||!rawHodo || !uncalRecH  || !( rawHodo->planes() )) {
      edm::LogWarning("EcalBeamTask") << "analyze: missing a needed collection or hodo collection empty. Returning.";
      return;
  }
  LogDebug("EEBeamHodoTask") << " TDC raw, Hodo raw, uncalRecH and DCCheader found.";



  // table has come to a stop is identified by new value of cry_in_beam
  //   - increase counter of crystals that have been on beam
  //   - set flag for resetting
  if (cryInBeam_ != previousCryInBeam_ ) {
      previousCryInBeam_ = cryInBeam_ ;
      cryInBeamCounter_++;
      resetNow_ = true;

      // since flag "tableIsMoving==false" is reliable (as we can tell, so far),
      // operations due when "table has started moving"
      // can be done after the change in crystal in beam

      LogDebug("EcalBeamTask")  << "At event number : " << LV1_ <<  " switching table status: from moving to still. "
				<< " cry in beam is: " << cryInBeam_ << ", step being: " << cryInBeamCounter_ ;

      // fill here plots which keep history of beamed crystals
      float HodoPosXMinusCaloPosXVsCry_mean  =0;
      float HodoPosXMinusCaloPosXVsCry_rms   =0;
      float HodoPosYMinusCaloPosYVsCry_mean  =0;
      float HodoPosYMinusCaloPosYVsCry_rms   =0;
      float TDCTimeMinusCaloTimeVsCry_mean   =0;
      float TDCTimeMinusCaloTimeVsCry_rms    =0;

      // min number of entries chosen assuming:
      //                 prescaling = 100 X 2FU
      //                 that we want at leas 2k events per crystal

      if (meCaloVsHodoXPos_-> getEntries()  > 10){
	HodoPosXMinusCaloPosXVsCry_mean = meCaloVsHodoXPos_-> getMean(1);
	HodoPosXMinusCaloPosXVsCry_rms  = meCaloVsHodoXPos_-> getRMS(1);
	meHodoPosXMinusCaloPosXVsCry_->  setBinContent( cryInBeamCounter_, HodoPosXMinusCaloPosXVsCry_mean);
	meHodoPosXMinusCaloPosXVsCry_->  setBinError(      cryInBeamCounter_, HodoPosXMinusCaloPosXVsCry_rms);
	LogDebug("EcalBeamTask")  << "At event number: " << LV1_ << " step: " << cryInBeamCounter_
				  <<  " DeltaPosX is: " << (meCaloVsHodoXPos_-> getMean(1))
				  << " +-" << ( meCaloVsHodoXPos_-> getRMS(1));
      }
      if (meCaloVsHodoYPos_-> getEntries()  > 10){
	HodoPosYMinusCaloPosYVsCry_mean = meCaloVsHodoYPos_-> getMean(1);
	HodoPosYMinusCaloPosYVsCry_rms  = meCaloVsHodoYPos_-> getRMS(1);
	meHodoPosYMinusCaloPosYVsCry_->  setBinContent( cryInBeamCounter_, HodoPosYMinusCaloPosYVsCry_mean);
	meHodoPosYMinusCaloPosYVsCry_->  setBinError(      cryInBeamCounter_, HodoPosYMinusCaloPosYVsCry_rms);
	LogDebug("EcalBeamTask")  << "At event number: " << LV1_ << " step: " << cryInBeamCounter_
				  <<  " DeltaPosY is: " << (meCaloVsHodoYPos_-> getMean(1))
				  << " +-" << ( meCaloVsHodoYPos_-> getRMS(1));
      }
      if (meCaloVsTDCTime_-> getEntries()  > 10){
	TDCTimeMinusCaloTimeVsCry_mean     = meCaloVsTDCTime_-> getMean(1);
	TDCTimeMinusCaloTimeVsCry_rms      = meCaloVsTDCTime_-> getRMS(1);
	meTDCTimeMinusCaloTimeVsCry_->  setBinContent(cryInBeamCounter_, TDCTimeMinusCaloTimeVsCry_mean);
	meTDCTimeMinusCaloTimeVsCry_->  setBinError(cryInBeamCounter_, TDCTimeMinusCaloTimeVsCry_rms);
	LogDebug("EcalBeamTask")  << "At event number: " << LV1_ << " step: " << cryInBeamCounter_
				  <<  " DeltaT is: " << (meCaloVsTDCTime_-> getMean(1))
				  << " +-" << ( meCaloVsTDCTime_-> getRMS(1));
      }

      LogDebug("EcalBeamTask")  << "At event number: " << LV1_ <<  " trace histos filled ( cryInBeamCounter_="
				<< cryInBeamCounter_ << ")";

    }




  // if table has come to rest (from movement), reset concerned ME's
  if (resetNow_)
    {
      meEvsXRecProf_->Reset();
      meEvsYRecProf_->Reset();
      meEvsXRecHis_->Reset();
      meEvsYRecHis_->Reset();
      meCaloVsHodoXPos_->Reset();
      meCaloVsHodoYPos_->Reset();
      meCaloVsTDCTime_->Reset();

      resetNow_ = false;
    }




  /**************************************
  // handling histos type I:
  **************************************/
  for (unsigned int planeId=0; planeId <4; planeId++){

    const EcalTBHodoscopePlaneRawHits& planeRaw = rawHodo->getPlaneRawHits(planeId);
    LogDebug("EcalBeamTask")  << "\t plane: " << (planeId+1)
			      << "\t number of fibers: " << planeRaw.channels()
			      << "\t number of hits: " << planeRaw.numberOfFiredHits();
    meHodoOcc_[planeId]-> Fill( planeRaw.numberOfFiredHits() );

    for (unsigned int i=0;i<planeRaw.channels();i++)
      {
	if (planeRaw.isChannelFired(i))
	  {
	    edm::LogInfo("EcalBeamTask")<< " channel " << (i+1) << " has fired";
	    meHodoRaw_[planeId]-> Fill(i+0.5);
	  }
      }
  }



  edm::Handle<EcalTBTDCRecInfo> pTDC;
  const EcalTBTDCRecInfo* recTDC=0;

  if ( e.getByLabel(EcalTBTDCRecInfo_, pTDC) ) {
    recTDC = pTDC.product();
    LogDebug("EEBeamHodoTask") << " TDC offset is: " << recTDC->offset();
  } else {
    edm::LogError("EcalBeamTask") << "Error! Can't get the product EcalTBTDCRecInfo. Returning";
    meMissingCollections_-> Fill(5); // bin6: missing reconstructed TDC
    return;
  }

  edm::Handle<EcalTBHodoscopeRecInfo> pHodo;
  const EcalTBHodoscopeRecInfo* recHodo=0;

  if ( e.getByLabel(EcalTBHodoscopeRecInfo_, pHodo) ) {
    recHodo = pHodo.product();
    LogDebug("EcalBeamTask") << "hodoscopeReco:    x: " << recHodo->posX()
			     << "\ty: " << recHodo->posY()
			     << "\t sx: " << recHodo->slopeX() << "\t qualx: " << recHodo->qualX()
			     << "\t sy: " << recHodo->slopeY() << "\t qualy: " << recHodo->qualY();
  } else {
    edm::LogError("EcalBeamTask") << "Error! Can't get the product EcalTBHodoscopeRecInfo";
    meMissingCollections_-> Fill(6); // bin7: missing reconstructed hodoscopes
    return;
  }

  if ( (!recHodo) || (!recTDC) ) {
      edm::LogWarning("EcalBeamTask") << "analyze: missing a needed collection, recHodo or recTDC. Returning.";
      return;
  }
  LogDebug("EEBeamHodoTask") << " Hodo reco and TDC reco found.";

  meTDCRec_->Fill( recTDC->offset());

  meHodoPosRecXY_->Fill( recHodo->posX(), recHodo->posY() );
  meHodoPosRecX_->Fill( recHodo->posX());
  meHodoPosRecY_->Fill( recHodo->posY() );
  meHodoSloXRec_->Fill( recHodo->slopeX());
  meHodoSloYRec_->Fill( recHodo->slopeY());
  meHodoQuaXRec_->Fill( recHodo->qualX());
  meHodoQuaYRec_->Fill( recHodo->qualY());

  /**************************************
  // handling histos type II:
  **************************************/

  if (tableIsMoving_) {
      LogDebug("EcalBeamTask")<< "At event number:" << LV1_ << " table is moving. Not filling concerned monitoring elements. ";
      return;
  } else {
      LogDebug("EcalBeamTask")<< "At event number:" << LV1_ << " table is not moving - thus filling alos monitoring elements requiring so.";
  }

  float maxE =0;
  EBDetId maxHitId(0);
  for (  EEUncalibratedRecHitCollection::const_iterator uncalHitItr = pUncalRH->begin();  uncalHitItr!= pUncalRH->end(); uncalHitItr++ ) {
    double e = (*uncalHitItr).amplitude();
    if ( e <= 0. ) e = 0.0;
    if ( e > maxE )  {
      maxE       = e;
      maxHitId = (*uncalHitItr).id();
    }
  }
  if (  maxHitId == EBDetId(0) ) {
      edm::LogError("EEBeamHodoTask") << "No positive UncalRecHit found in ECAL in event " << ievt_ << " - returning.";
      return;
  }

  meEvsXRecProf_-> Fill(recHodo->posX(), maxE);
  meEvsYRecProf_-> Fill(recHodo->posY(), maxE);
  meEvsXRecHis_-> Fill(recHodo->posX(), maxE);
  meEvsYRecHis_-> Fill(recHodo->posY(), maxE);

  edm::LogInfo("EcalBeamTask")<< " channel with max is " << maxHitId;

  bool mat5x5 =true;
  int ietaMax = maxHitId.ieta();
  int iphiMax = (maxHitId.iphi() % 20);
  if (ietaMax ==1 || ietaMax ==2 || ietaMax ==84 || ietaMax == 85 ||
      iphiMax ==1 || iphiMax ==2 || iphiMax ==19 || iphiMax == 20 )
    {mat5x5 =false;}

  if (!mat5x5) return;

  EBDetId Xtals5x5[25];
  double   ene5x5[25];
  double   e25    =0;
  for (unsigned int icry=0;icry<25;icry++)
    {
      unsigned int row = icry / 5;
      unsigned int column= icry %5;
      if ( EBDetId::validDetId(maxHitId.ieta()+column-2,maxHitId.iphi()+row-2) ) {
	Xtals5x5[icry]=EBDetId(maxHitId.ieta()+column-2,maxHitId.iphi()+row-2,EBDetId::ETAPHIMODE);
	double e = ( *pUncalRH->find( Xtals5x5[icry] )  ).amplitude();
	if ( e <= 0. ) e = 0.0;
	ene5x5[icry] =e;
	e25 +=e;
      } else {
	LogDebug("EcalBeamTask")<< "Cannot construct 5x5 matrix around EBDetId " << maxHitId;
	mat5x5 =false;
      }
    }

  if (!mat5x5) return;
  LogDebug("EcalBeamTask")<< "Could construct 5x5 matrix around EBDetId " << maxHitId;

  // im mm
  float sideX =24.06;
  float sideY =22.02;
  float caloX =0;
  float caloY =0;
  float weight=0;
  float sumWeight=0;
  // X and Y calculated from log-weighted energy center of mass
  for (unsigned int icry=0;icry<25;icry++)
    {
      weight = log( ene5x5[icry] / e25) + 3.8;
      if (weight>0)
	{
	  unsigned int row      = icry / 5;
	  unsigned int column = icry %5;
	  caloX +=  (column-2) * sideX * weight;
	  caloY -=  (row-2) * sideY * weight;
	  sumWeight += weight;
	}
    }
  caloX /=sumWeight;
  caloY /=sumWeight;

  meCaloVsHodoXPos_->Fill( recHodo->posX()-caloX );
  meCaloVsHodoYPos_->Fill( recHodo->posY()-caloY );
  meCaloVsTDCTime_->Fill( (*pUncalRH->find( maxHitId )  ).jitter()  -  recTDC->offset() - 3);
  LogDebug("EcalBeamTask")<< "jiitter from uncalRecHit: " <<  (*pUncalRH->find( maxHitId )  ).jitter();

}

