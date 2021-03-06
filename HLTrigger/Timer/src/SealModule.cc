#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"

#include "HLTrigger/Timer/interface/TimerService.h"
#include "HLTrigger/Timer/interface/Timer.h"

typedef edm::serviceregistry::AllArgsMaker<TimerService> maker_cputs;

DEFINE_FWK_MODULE(Timer);
DEFINE_FWK_SERVICE_MAKER(TimerService,maker_cputs);
