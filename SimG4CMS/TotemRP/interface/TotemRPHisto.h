#ifndef TotemRP_TotemRPHisto_H
#define TotemRP_TotemRPHisto_H


#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TNtupleD.h"
#include "TRandom.h"

#include <string>

#define NUMVARROOT_RP 26
#include <vector>

class TotemRPHisto : public TObject {

public: 
  TotemRPHisto(const std::string &file_name);
  TotemRPHisto();
  virtual ~TotemRPHisto();

  void set_EVT(int v){evt=v;}
  void set_X(double v) {x = v;}
  void set_Y(double v) {y = v;}
  void set_Z(double v) {z = v;}
  void set_Loc_X(double v) {lx = v;}
  void set_Loc_Y(double v) {ly = v;}
  void set_Loc_Z(double v) {lz = v;}
  void set_X_Exit(double v) {x_ex = v;}
  void set_Y_Exit(double v) {y_ex = v;}
  void set_Z_Exit(double v) {z_ex = v;}
  void set_Loc_X_Exit(double v) {lx_ex = v;}
  void set_Loc_Y_Exit(double v) {ly_ex = v;}
  void set_Loc_Z_Exit(double v) {lz_ex = v;}
  void set_UID(int v) {UID = v;}
  void set_Ptype(int v) {Ptype = v;}
  void set_TID(int v) {TID = v;}
  void set_PID(int v) {PID = v;}
  void set_ELoss(double v) {ELoss = v;}
  void set_PABS(double v) {PABS = v;}
  void set_VX(double v) {vx = v;}
  void set_VY(double v) {vy = v;}
  void set_VZ(double v) {vz = v;}
  void set_p_x(double p) {p_x = p;}
  void set_p_y(double p) {p_y = p;}
  void set_p_z(double p) {p_z = p;}
  void set_prim_ver_id(int id) {prim_ver_id = id;}

  void fillNtuple();

private: 
  TFile *rt_hf;

  TNtupleD *ntuple;

  double rootvec[NUMVARROOT_RP];

  double x,y,z,ELoss,PABS,vx,vy,vz,lx,ly,lz,x_ex,y_ex,z_ex,lx_ex,ly_ex,lz_ex,p_x,p_y,p_z;
  int UID,Ptype,TID,PID,evt,prim_ver_id;
};
#endif  //TotemRP_TotemRPHisto_H
