#include "SimG4CMS/PPS/interface/PPSSD.h"
#include "SimG4CMS/PPS/interface/PPS_Timing_SD.h"

#include "SimG4Core/SensitiveDetector/interface/SensitiveDetectorPluginFactory.h"
#include "SimG4Core/Watcher/interface/SimWatcherFactory.h"
#include "FWCore/PluginManager/interface/ModuleDef.h"
  

typedef PPSSD PPSSensitiveDetector;
DEFINE_SENSITIVEDETECTOR(PPSSensitiveDetector);

typedef PPS_Timing_SD PPSTimingSensitiveDetector;
DEFINE_SENSITIVEDETECTOR(PPSTimingSensitiveDetector);

//DEFINE_SIMWATCHER (TotemTestGem);




