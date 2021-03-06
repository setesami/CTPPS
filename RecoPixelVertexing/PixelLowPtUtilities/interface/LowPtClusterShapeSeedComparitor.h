#ifndef _LowPtClusterShapeSeedComparitor_h_
#define _LowPtClusterShapeSeedComparitor_h_


#include "RecoTracker/TkSeedingLayers/interface/SeedComparitor.h"

#include "DataFormats/GeometryVector/interface/GlobalTag.h"
#include "DataFormats/GeometryVector/interface/Vector2DBase.h"
typedef Vector2DBase<float,GlobalTag> Global2DVector;

//#include "DataFormats/GeometryVector/interface/LocalVector.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"

#include <vector>

namespace edm { class ParameterSet; class EventSetup; }

class TrackerGeometry;
class TrackingRecHit;
class ClusterShapeHitFilter;

class LowPtClusterShapeSeedComparitor : public SeedComparitor
{
 public:
  LowPtClusterShapeSeedComparitor(const edm::ParameterSet& ps);
  virtual ~LowPtClusterShapeSeedComparitor();
  virtual bool compatible(const SeedingHitSet &hits, const edm::EventSetup &es);

 private:
  float areaParallelogram(const Global2DVector & a,
                          const Global2DVector & b);
  std::vector<GlobalVector>
    getGlobalDirs(const std::vector<GlobalPoint> & globalPoss);
  std::vector<GlobalPoint>
    getGlobalPoss(const TransientTrackingRecHit::ConstRecHitContainer & recHits);
 
   const ClusterShapeHitFilter * theFilter;
};

#endif

