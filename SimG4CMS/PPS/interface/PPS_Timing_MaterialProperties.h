#ifndef PPS_Timing_MaterialProperties_h
#define PPS_Timing_MaterialProperties_h

 /** \class PPS_Timing_MaterialProperties
  *  Class to define custom material properties for PPS_timing detector with PPS_timing_box, window_box and Lbar sub detectorss.
  *
  *  $Date: 2015/06/24 21:01:00 $
  *  $Revision: 0.0 $
  *  \author Mohsen Naseri
  */
 
 #include "G4MaterialPropertiesTable.hh"
 #include "G4SDManager.hh"
#include "G4OpticalSurface.hh" 
 class PPS_Timing_MaterialProperties
 {
  public:
     /// two different constructors to have more flexibility.
   PPS_Timing_MaterialProperties(int DebugLevel);
  // PPS_Timing_MaterialProperties();
     /// destructor
   ~PPS_Timing_MaterialProperties();


   static void DumpSurfaceInfo();
 
  private:
     /// define optical properties of materials in the timing detector of PPS project
   void setMaterialProperties();
 
  private:
   const G4MaterialTable * theMaterialTable;
   void  SetSurfaceProperties(G4OpticalSurface* OpSilSurface);
 
  private:
   int theMPDebugLevel;
   G4MaterialPropertiesTable * theMPT;
   G4Material * theAir;
   G4Material * theSil;
   G4Material * theGlass;
 };
 #endif
