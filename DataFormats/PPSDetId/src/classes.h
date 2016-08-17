#ifndef DATAFORMATS_PPS_TRACKER_ID_HH_
#define DATAFORMATS_PPS_TRACKER_ID_HH_

#include "DataFormats/PPSDetId/interface/PPSTrackerDetId.h"
#include "DataFormats/Common/interface/Wrapper.h"
#include "DataFormats/PPSDetId/interface/PPSTimingDetId.h"

#include <map>

namespace{ 
  namespace {

    PPSTrackerDetId d;
    std::pair<PPSTrackerDetId,std::pair<unsigned int,unsigned int> > dummy1;
    
    edm::Wrapper<std::pair<PPSTrackerDetId,std::pair<unsigned int,unsigned int> > > dummy2;
  }
}

#endif // DATAFORMATS_PPS_TRACKER_ID_HH_
