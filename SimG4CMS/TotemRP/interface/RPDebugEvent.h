#ifndef TOTEMRP_TOTEMRPDEBUGEVENT_H_
#define TOTEMRP_TOTEMRPDEBUGEVENT_H_

// system include files
#include <vector>
#include <map>

#include "Rtypes.h"

// user include files


class RPDebugEvent
{
public:
  // ---------- Constructor and destructor -----------------
  
  struct RPDebugParticle
  {
    RPDebugParticle() {}
    double x,y,z,PABS,vx,vy,vz,p_x,p_y,p_z;
    int UID,Ptype,TID,PID,prim_ver_id;
  };
  
  struct RPDebugProtSecVertex
  {
    RPDebugProtSecVertex() {}
    double x,y,z,PABS,p_x,p_y,p_z;
    int rp_id, mat_type, cp_no; 
    int TID, PID;
  };
  
  typedef std::map<unsigned int, std::vector<RPDebugParticle> > Container;
  typedef std::map<unsigned int, std::vector<RPDebugProtSecVertex> > VertContainer;
  RPDebugEvent();
  virtual ~RPDebugEvent();
   
  // ---------- Member functions --------------------------- 
  void setEVT(int v)      {evt=v;}
  

//private: 
   
  // ---------- Private Data members ----------------------- 
  int evt;
  Container prim_prots;
  Container prot_pipe_in;
  Container prot_pipe_out;
  Container all_particles_rp_in;
  Container all_particles_rp_out;
  Container prim_prot_rp_in;
  Container prim_prot_rp_out;
  Container input_window_out;
  VertContainer rp_prot_sec_vert;
  bool IsPrimaryProton(int PID) {return true;};
};



#endif /*TOTEMRP_TOTEMRPDEBUGEVENT_H_*/
