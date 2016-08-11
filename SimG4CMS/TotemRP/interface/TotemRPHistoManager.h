#ifndef TotemRP_TotemRPHistoManager_h
#define TotemRP_TotemRPHistoManager_h 1
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
// $Id: TotemRPHistoManager.h,v 1.1.1.1 2007/05/16 15:44:39 hniewiad Exp $
//
 
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TTree.h"

#include <string>

class TotemRPHistoClass;

class TotemRPHistoManager {

public: 

  TotemRPHistoManager(const std::string &);
  virtual ~TotemRPHistoManager();

  void fillTree(TotemRPHistoClass *  histos);

private:
  edm::Service<TFileService> fs;
  TTree                     *tree;
  TotemRPHistoClass         *h;
  int                       kount;
};

#endif  //TotemRP_TotemRPHistoManager_h
