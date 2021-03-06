#ifndef TkDetLayers_TIBLayerBuilder_h
#define TkDetLayers_TIBLayerBuilder_h


#include "RecoTracker/TkDetLayers/interface/TIBLayer.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"

/** A concrete builder for TIBLayer 
 */

class TIBLayerBuilder {  
 public:
  TIBLayerBuilder(){};
  TIBLayer* build(const GeometricDet* aTIBLayer,
		  const TrackerGeometry* theGeomDetGeometry);

  void constructRings(std::vector<const GeometricDet*>& theGeometricRods,
		      std::vector<std::vector<const GeometricDet*> >& innerGeometricDetRings,
		      std::vector<std::vector<const GeometricDet*> >& outerGeometricDetRings);
  
};


#endif 
