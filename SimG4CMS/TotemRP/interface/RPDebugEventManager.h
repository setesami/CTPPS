#ifndef TotemRP_RPDebugEventManager_h
#define TotemRP_RPDebugEventManager_h 1
// -*- C++ -*-
//
// Package:     Forward
// Class  :     TotemRPHistoManager
//
/**\class TotemRPHistoManager TotemRPHistoManager.h SimG4CMS/TotemRP/interface/TotemRPHistoManager.h
 
 Description: Manages Root file creation for Totem RP Tests
 
 Usage:
    Used in testing Totem RP simulation
 
*/
//
// Original Author: 
//         Created:  Tue May 16 10:14:34 CEST 2006
// $Id: RPDebugEventManager.h,v 1.1.1.1 2007/05/16 15:44:39 hniewiad Exp $
//
 
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TTree.h"

#include <string>

class RPDebugEvent;

class RPDebugEventManager {

public: 

  RPDebugEventManager(const std::string &);
  virtual ~RPDebugEventManager();

  void fillTree(RPDebugEvent *  debug_event);

private:
  edm::Service<TFileService> fs;
  TTree                     *tree;
  RPDebugEvent              *h;
  int                       kount;
};

#endif  //TotemRP_RPDebugEventManager_h
