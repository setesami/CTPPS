#ifndef TOTEMRP_TOTEMRPHISTOCLASS_H_
#define TOTEMRP_TOTEMRPHISTOCLASS_H_

// system include files
#include <vector>

// user include files

class TotemRPHistoClass {

public:

  // ---------- Constructor and destructor -----------------
  TotemRPHistoClass();
  ~TotemRPHistoClass();
   
  // ---------- Member functions --------------------------- 
  void setEVT(int v)      {evt=v;}
  void fillHit(int UID_, int Ptype_, int TID_, int PID_, double ELoss_, double PABS_, 
    double p_x_, double p_y_, double p_z_, double vx_, double vy_, double vz_, double x_, 
    double y_, double z_, double lx_, double ly_, double lz_, 
    double x_ex_, double y_ex_, double z_ex_, double lx_ex_, double ly_ex_, double lz_ex_, 
    int prim_ver_id_);

  struct Hit {
    Hit() {}
    double x,y,z,ELoss,PABS,vx,vy,vz,lx,ly,lz,x_ex,y_ex,z_ex,lx_ex,ly_ex,lz_ex,p_x,p_y,p_z;
    int UID,Ptype,TID,PID,prim_ver_id;
  };

private: 
   
  // ---------- Private Data members ----------------------- 
  int              evt, hits;
  std::vector<Hit> hit;
};



#endif /*TOTEMRP_TOTEMRPHISTOCLASS_H_*/
