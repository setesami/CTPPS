#include "RecoTracker/TkDetLayers/interface/TIDRingBuilder.h"

using namespace edm;
using namespace std;

TIDRing* TIDRingBuilder::build(const GeometricDet* aTIDRing,
			 const TrackerGeometry* theGeomDetGeometry)
{
  vector<const GeometricDet*>  theGeometricDets = aTIDRing->components();
  //edm::LogInfo(TkDetLayers) << "theGeometricDets.size(): " << theGeometricDets.size() ;


  vector<const GeomDet*> innerGeomDets;
  vector<const GeomDet*> outerGeomDets;

  //---- to evaluate meanZ
  double meanZ = 0;
  for(vector<const GeometricDet*>::const_iterator it=theGeometricDets.begin();
      it!=theGeometricDets.end();it++){
    meanZ = meanZ + (*it)->positionBounds().z();
  }
  meanZ = meanZ/theGeometricDets.size();
  //----

  for(vector<const GeometricDet*>::const_iterator it=theGeometricDets.begin();
      it!=theGeometricDets.end();it++){

    const GeomDet* theGeomDet = theGeomDetGeometry->idToDet( (*it)->geographicalID() );

    if( fabs( (*it)->positionBounds().z() ) < fabs(meanZ))
      innerGeomDets.push_back(theGeomDet);

    if( fabs( (*it)->positionBounds().z() ) > fabs(meanZ))
      outerGeomDets.push_back(theGeomDet);      
  }

  //edm::LogInfo(TkDetLayers) << "innerGeomDets.size(): " << innerGeomDets.size() ;
  //edm::LogInfo(TkDetLayers) << "outerGeomDets.size(): " << outerGeomDets.size() ;

  return new TIDRing(innerGeomDets,outerGeomDets);
}
