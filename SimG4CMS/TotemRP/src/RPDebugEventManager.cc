// -*- C++ -*-
//
// Package:     Forward
// Class  :     RPDebugEventManager
//
// Implementation:
//     <Notes on implementation>
//
// Original Author: 
//         Created:  Tue May 16 10:14:34 CEST 2006
// $Id: RPDebugEventManager.cc,v 1.1.1.1.6.1 2009/07/13 14:37:06 jkaspar Exp $
//

// system include files
#include <iostream>
#include <cmath>

// user include files
#include "SimG4CMS/TotemRP/interface/RPDebugEventManager.h"
#include "SimG4CMS/TotemRP/interface/RPDebugEvent.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/PluginManager/interface/PluginManager.h"

//
// constructors and destructor
//

RPDebugEventManager::RPDebugEventManager(const std::string & file):
	tree(0), h(0), kount(0) {
	if (fs.isAvailable()) {
		h    = new RPDebugEvent();

		tree = fs->make<TTree>("TotemRP", "TotemRP");
		tree->SetAutoSave(10000);
		tree->Branch("RPDebug", "RPDebugEvent", &h);
		edm::LogInfo("TotemRP") << "RPDebugEventManager:===>>>  Book the Tree";
	} else {
		edm::LogInfo("TotemRP") << "RPDebugEventManager:===>>> No file provided";
	}
}

RPDebugEventManager::~RPDebugEventManager() {

	edm::LogInfo("TotemRP") << "============================================="
	                        << "========================================\n"
	                        << "=== RPDebugEventManager: Start writing user "
	                        << "histograms after " << kount << " events ";
	if (h) delete h;
}

void RPDebugEventManager::fillTree(RPDebugEvent *  debug_event) {

	kount++;
	LogDebug("TotemRP") << "RPDebugEventManager: tree pointer for " << kount
	                    << " = " << debug_event;
	if (tree) {
	    h = debug_event;
	    tree->Fill();
	}
}
