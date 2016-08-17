#ifndef _PPS_G4HitCollection_h
#define _PPS_G4HitCollection_h 1
// -*- C++ -*-
//
// Package:     PPS
// Class  :     PPSG4HitCollection
//
/**\class PPSG4HitCollection PPSG4HitCollection.h SimG4CMS/PPS/interface/PPSG4HitCollection.h
 
 Description: Hit collection class for PPS transient hits
 
 Usage: 
 
*/
//
// Original Author:
//         Created:  Tue May 16 10:14:34 CEST 2006
//
 
// system include files

// user include files
#include "SimG4CMS/PPS/interface/PPSG4Hit.h"
#include "G4THitsCollection.hh"

typedef G4THitsCollection<PPSG4Hit> PPSG4HitCollection;

#endif
