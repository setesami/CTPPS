#ifndef RecoTracker_TkDetLayers_ForwardDiskSectorBuilderFromDet_h
#define RecoTracker_TkDetLayers_ForwardDiskSectorBuilderFromDet_h

#include "RecoTracker/TkDetLayers/interface/BoundDiskSector.h"
#include "RecoTracker/TkDetLayers/interface/DiskSectorBounds.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include <utility>
#include <vector>
#include <iostream>

/** As it's name indicates, it's a builder of a BoundDiskSector from a collection of
 *  Dets. The disk sector has the minimal size fully containing all Dets.
 */

class ForwardDiskSectorBuilderFromDet {
public:

  /// Warning, remember to assign this pointer to a ReferenceCountingPointer!
  /// Should be changed to return a ReferenceCountingPointer<BoundDisk>
  BoundDiskSector* operator()( const std::vector<const GeomDet*>& dets) const;
  
  std::pair<DiskSectorBounds, GlobalVector>
  computeBounds( const std::vector<const GeomDet*>& dets) const;

private:

  Surface::RotationType
  computeRotation( const std::vector<const GeomDet*>& dets, Surface::PositionType pos) const;

  std::vector<GlobalPoint> 
  computeTrapezoidalCorners( const GeomDet* detu) const;

};

#endif
