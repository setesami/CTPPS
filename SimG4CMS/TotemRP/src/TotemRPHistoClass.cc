// -*- C++ -*-
//
// Package:     Forward
// Class  :     TotemRPHistoClass
//
// Implementation:
//     <Notes on implementation>
//
// Original Author: 
//         Created:  Tue May 16 10:14:34 CEST 2006
// $Id: TotemRPHistoClass.cc,v 1.1.1.1 2007/05/16 15:44:39 hniewiad Exp $
//

// system include files
#include <iostream>
#include <cmath>

// user include files
#include "SimG4CMS/TotemRP/interface/TotemRPHistoClass.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//
// constructors and destructor
//

TotemRPHistoClass::TotemRPHistoClass() : evt(0), hits(0)
{
}

TotemRPHistoClass::~TotemRPHistoClass() {}

void TotemRPHistoClass::fillHit(int UID_, int Ptype_, int TID_, int PID_, double ELoss_, double PABS_, 
    double p_x_, double p_y_, double p_z_, double vx_, double vy_, double vz_, double x_, 
    double y_, double z_, double lx_, double ly_, double lz_, 
    double x_ex_, double y_ex_, double z_ex_, double lx_ex_, double ly_ex_, 
    double lz_ex_, int prim_ver_id_)
{
  TotemRPHistoClass::Hit h;
  
  h.UID = UID_;
  h.Ptype = Ptype_;
  h.TID = TID_;
  h.PID = PID_;
  h.ELoss = ELoss_;
  h.PABS = PABS_;
  h.p_x = p_x_;
  h.p_y = p_y_;
  h.p_z = p_z_;
  h.vx = vx_;
  h.vy = vy_;
  h.vz = vz_;
  h.x = x_;
  h.y = y_;
  h.z = z_;
  h.lx = lx_;
  h.ly = ly_;
  h.lz = lz_;
  h.x_ex = x_ex_;
  h.y_ex = y_ex_;
  h.z_ex = z_ex_;
  h.lx_ex = lx_ex_;
  h.ly_ex = ly_ex_;
  h.lz_ex = lz_ex_;
  h.prim_ver_id = prim_ver_id_;

  hit.push_back(h);
  hits++;
  
//  if(verbosity_)
//  {
//    LogDebug("TotemRP") << "TotemRPHistoClass : Hit " << hits << " " << UID_
//       << ", " << Ptype_ << ", " << TID_ << ", " << PID_ << ", "
//       << ELoss_ << ", " << PABS_ << ", " << vx_ << ", " << vy_
//       << ", " << vz_ << ", " << x_ << ", " << y_ << ", " << z_;
//  }
}
