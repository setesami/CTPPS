#include "SimG4CMS/TotemRP/interface/TotemRP.h"
#include "SimG4CMS/TotemRP/interface/Totem_RP_SD.h"

#include "SimG4Core/SensitiveDetector/interface/SensitiveDetectorPluginFactory.h"
#include "SimG4Core/Watcher/interface/SimWatcherFactory.h"

#include "FWCore/Framework/interface/MakerMacros.h"  
#include "FWCore/PluginManager/interface/ModuleDef.h"

typedef Totem_RP_SD RomanPotSensitiveDetector;
DEFINE_SENSITIVEDETECTOR(RomanPotSensitiveDetector);

DEFINE_SIMWATCHER(TotemRP);
