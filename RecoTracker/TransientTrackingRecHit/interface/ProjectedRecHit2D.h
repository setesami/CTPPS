#ifndef RECOTRACKER_TRANSIENTRACKINGRECHIT_ProjectedRecHit2D_H
#define RECOTRACKER_TRANSIENTRACKINGRECHIT_ProjectedRecHit2D_H

#include "DataFormats/TrackerRecHit2D/interface/ProjectedSiStripRecHit2D.h"
#include "TrackingTools/TransientTrackingRecHit/interface/GenericTransientTrackingRecHit.h"
#include "TrackingTools/TransientTrackingRecHit/interface/HelpertRecHit2DLocalPos.h"
#include "RecoLocalTracker/ClusterParameterEstimator/interface/StripClusterParameterEstimator.h"

class SiStripRecHit2D;

class ProjectedRecHit2D : public GenericTransientTrackingRecHit {
public:

  virtual void getKfComponents( KfComponentsHolder & holder ) const {
      HelpertRecHit2DLocalPos().getKfComponents(holder, *hit(), *det()); 
  }


  virtual AlgebraicSymMatrix parametersError() const {
    return HelpertRecHit2DLocalPos().parError( localPositionError(), *det()); 
  }

  const GeomDetUnit* detUnit() const {return 0;}
  const GeomDet* originalDet() const {return theOriginalDet;}

  static RecHitPointer build( const GeomDet * geom,
			      const GeomDet* originaldet,
			      const ProjectedSiStripRecHit2D* rh,
			      const StripClusterParameterEstimator* cpe,
			      float weight=1., float annealing=1.,
			      bool computeCoarseLocalPosition=false) {
    return RecHitPointer( new ProjectedRecHit2D( geom, originaldet, rh, cpe, weight, annealing, computeCoarseLocalPosition));
  }

  static RecHitPointer build( const LocalPoint& pos, const LocalError& err, 
			      const GeomDet* det, const GeomDet* originaldet,
			      const TransientTrackingRecHit& originalHit,
			      float weight=1., float annealing=1.) {
    return RecHitPointer( new ProjectedRecHit2D( pos, err, det, originaldet, originalHit, weight, annealing));
  }

  RecHitPointer clone( const TrajectoryStateOnSurface& ts) const;

  const SiStripRecHit2D& originalHit() const { return static_cast<const ProjectedSiStripRecHit2D*>( hit() )->originalHit();}

  virtual ConstRecHitContainer 	transientHits () const;

private:
  const StripClusterParameterEstimator* theCPE;
  const GeomDet* theOriginalDet;

  ProjectedRecHit2D( const LocalPoint& pos, const LocalError& err,
		     const GeomDet* det, const GeomDet* originaldet, 
		     const TransientTrackingRecHit& originalHit,
		     float weight, float annealing);

  ProjectedRecHit2D( const GeomDet * geom, const GeomDet* originaldet,
		     const ProjectedSiStripRecHit2D* rh,
		     const StripClusterParameterEstimator* cpe,
		     float weight, float annealing,
		     bool computeCoarseLocalPosition);

  virtual ProjectedRecHit2D* clone() const {
    return new ProjectedRecHit2D(*this);
  }

};



#endif
