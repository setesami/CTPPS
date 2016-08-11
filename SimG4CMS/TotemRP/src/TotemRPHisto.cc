#include "SimG4CMS/TotemRP/interface/TotemRPHisto.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <iostream>
#include <string>
#include <cmath>

//#define IT_DEBUG

TotemRPHisto::TotemRPHisto()
{
  TotemRPHisto("RPDefaultOldFile.root");
}

TotemRPHisto::TotemRPHisto(const std::string &file_name)
{
  edm::LogInfo("TotemRP") << "========================================================" << std::endl;  
  edm::LogInfo("TotemRP") << "=== TotemRPHisto: Start writing user hits ===" << std::endl;
  
  std::string TotemRPOutputFileName = file_name;
  rt_hf = TFile::Open(TotemRPOutputFileName.c_str(), "RECREATE");
  rt_hf->SetCompressionLevel(5);
  edm::LogInfo("TotemRP") << "I have already created root file" << std::endl;
  
  ntuple = new TNtupleD("ntuple", "Ntuple","Event:UnitID:Ptype:TrackID:ParentID:ELoss:PABS:vx:vy:vz:x:y:z:lx:ly:lz:x_ex:y_ex:z_ex:lx_ex:ly_ex:lz_ex:p_x:p_y:p_z:prim_ver_id");
  edm::LogInfo("TotemRP") << std::endl << "===>>> Done booking user histograms and ntuples " << std::endl;

  set_EVT(0);
  set_X(0.);
  set_Y(0.);
  set_Z(0.);
  set_Loc_X(0.);
  set_Loc_Y(0.);
  set_Loc_Z(0.);
  set_X_Exit(0.);
  set_Y_Exit(0.);
  set_Z_Exit(0.);
  set_Loc_X_Exit(0.);
  set_Loc_Y_Exit(0.);
  set_Loc_Z_Exit(0.);
  set_UID(0);
  set_Ptype(0);
  set_TID(0);
  set_PID(0);
  set_ELoss(0.);
  set_PABS(0.);
  set_VX(0.);
  set_VY(0.);
  set_VZ(0.);
  set_p_x(0.0);
  set_p_y(0.0);
  set_p_z(0.0);
  set_prim_ver_id(-1);
}

TotemRPHisto::~TotemRPHisto()
{
  rt_hf->cd();
  TotemRPHisto::ntuple->Write();

  rt_hf->Close();
  delete rt_hf;

  edm::LogInfo("TotemRP") << std::endl << "TotemRPHisto: End writing user histograms " << std::endl;
}

void TotemRPHisto::fillNtuple()
{
  //edm::LogInfo("TotemRP") << "RIEMPO L'NTUPLA DI ROOT " <<std::endl;

  rootvec[0]=evt;
  rootvec[1]=UID;
  rootvec[2]=Ptype;
  rootvec[3]=TID;
  rootvec[4]=PID;
  rootvec[5]=ELoss;
  rootvec[6]=PABS;
  rootvec[7]=vx;
  rootvec[8]=vy;
  rootvec[9]=vz;
  rootvec[10]=x;
  rootvec[11]=y;
  rootvec[12]=z;
  rootvec[13]=lx;
  rootvec[14]=ly;
  rootvec[15]=lz;
  rootvec[16]=x_ex;
  rootvec[17]=y_ex;
  rootvec[18]=z_ex;
  rootvec[19]=lx_ex;
  rootvec[20]=ly_ex;
  rootvec[21]=lz_ex;
  rootvec[22]=p_x;
  rootvec[23]=p_y;
  rootvec[24]=p_z;
  rootvec[25]=prim_ver_id;

  if(ntuple)
  {
    ntuple->Fill(&(rootvec[0]));
  }
}
