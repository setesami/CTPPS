// -*- C++ -*-
//
// Package:    TrackerOnlyConversionProducer
// Class:      TrackerOnlyConversionProducer
//
/**\class TrackerOnlyConversionProducer 

 Description: Produces converted photon objects using default track collections

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Hongliang Liu
//         Created:  Thu Mar 13 17:40:48 CDT 2008
// $Id: TrackerOnlyConversionProducer.cc,v 1.40 2010/11/22 02:02:08 bendavid Exp $
//
//


// system include files
#include <memory>
#include <map>


#include "RecoEgamma/EgammaPhotonProducers/interface/TrackerOnlyConversionProducer.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "DataFormats/TrajectoryState/interface/PTrajectoryStateOnDet.h"

#include "TrackingTools/GeomPropagators/interface/AnalyticalPropagator.h"
#include "TrackingTools/MaterialEffects/interface/PropagatorWithMaterial.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"

#include "TrackingTools/TransientTrack/interface/TrackTransientTrack.h"
#include "DataFormats/EgammaTrackReco/interface/ConversionTrack.h"


#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"

#include "TrackingTools/PatternTools/interface/ClosestApproachInRPhi.h"
#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistance.h"

#include "CommonTools/Statistics/interface/ChiSquaredProbability.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "RecoEgamma/EgammaPhotonAlgos/interface/ConversionVertexFinder.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "RecoEgamma/EgammaPhotonAlgos/interface/TangentApproachInRPhi.h"
#include "RecoEgamma/EgammaPhotonAlgos/interface/ConversionHitChecker.h"


//Kinematic constraint vertex fitter
#include "RecoVertex/KinematicFitPrimitives/interface/ParticleMass.h"
#include "RecoVertex/KinematicFitPrimitives/interface/MultiTrackKinematicConstraint.h"
#include <RecoVertex/KinematicFitPrimitives/interface/KinematicParticleFactoryFromTransientTrack.h>
#include "RecoVertex/KinematicFit/interface/KinematicConstrainedVertexFitter.h"
#include "RecoVertex/KinematicFit/interface/TwoTrackMassKinematicConstraint.h"
#include "RecoVertex/KinematicFit/interface/KinematicParticleVertexFitter.h"
#include "RecoVertex/KinematicFit/interface/KinematicParticleFitter.h"
#include "RecoVertex/KinematicFit/interface/MassKinematicConstraint.h"
#include "RecoVertex/KinematicFit/interface/ColinearityKinematicConstraint.h"



TrackerOnlyConversionProducer::TrackerOnlyConversionProducer(const edm::ParameterSet& iConfig):
 theVertexFinder_(0)

{
    algoName_ = iConfig.getParameter<std::string>( "AlgorithmName" );

    src_ = iConfig.getParameter<edm::InputTag>("src");
    maxNumOfTrackInPU_ = iConfig.getParameter<int>("maxNumOfTrackInPU");
    allowTrackBC_ = iConfig.getParameter<bool>("AllowTrackBC");
    allowD0_ = iConfig.getParameter<bool>("AllowD0");
    allowDeltaPhi_ = iConfig.getParameter<bool>("AllowDeltaPhi");
    allowDeltaCot_ = iConfig.getParameter<bool>("AllowDeltaCot");
    allowMinApproach_ = iConfig.getParameter<bool>("AllowMinApproach");
    allowOppCharge_ = iConfig.getParameter<bool>("AllowOppCharge");

    allowVertex_ = iConfig.getParameter<bool>("AllowVertex");

    halfWayEta_ = iConfig.getParameter<double>("HalfwayEta");//open angle to search track matches with BC

    if (allowD0_)
	d0Cut_ = iConfig.getParameter<double>("d0");
    
    usePvtx_ = iConfig.getParameter<bool>("UsePvtx");//if use primary vertices

    if (usePvtx_){
	vertexProducer_   = iConfig.getParameter<std::string>("primaryVertexProducer");
    }

    if (allowTrackBC_) {
	//Track-cluster matching eta and phi cuts
	dEtaTkBC_ = iConfig.getParameter<double>("dEtaTrackBC");//TODO research on cut endcap/barrel
	dPhiTkBC_ = iConfig.getParameter<double>("dPhiTrackBC");

	bcBarrelCollection_     = iConfig.getParameter<edm::InputTag>("bcBarrelCollection");
	bcEndcapCollection_     = iConfig.getParameter<edm::InputTag>("bcEndcapCollection");

	energyBC_ = iConfig.getParameter<double>("EnergyBC");//BC energy cut
	energyTotalBC_ = iConfig.getParameter<double>("EnergyTotalBC");//BC energy cut

    }

    //Track cuts on left right track: at least one leg reaches ECAL
    //Left track: must exist, must reach Ecal and match BC, so loose cut on Chi2 and tight on hits
    //Right track: not necessary to exist (if allowSingleLeg_), not necessary to reach ECAL or match BC, so tight cut on Chi2 and loose on hits
    maxChi2Left_ =  iConfig.getParameter<double>("MaxChi2Left");
    maxChi2Right_ =  iConfig.getParameter<double>("MaxChi2Right");
    minHitsLeft_ = iConfig.getParameter<int>("MinHitsLeft");
    minHitsRight_ = iConfig.getParameter<int>("MinHitsRight");

    //Track Open angle cut on delta cot(theta) and delta phi
    if (allowDeltaCot_)
	deltaCotTheta_ = iConfig.getParameter<double>("DeltaCotTheta");
    if (allowDeltaPhi_)
	deltaPhi_ = iConfig.getParameter<double>("DeltaPhi");
    if (allowMinApproach_){
        minApproachLow_ = iConfig.getParameter<double>("MinApproachLow");
	minApproachHigh_ = iConfig.getParameter<double>("MinApproachHigh");
    }

    // if allow single track collection, by default False
    allowSingleLeg_ = iConfig.getParameter<bool>("AllowSingleLeg");
    rightBC_ = iConfig.getParameter<bool>("AllowRightBC");

    //track inner position dz cut, need RECO
    dzCut_ = iConfig.getParameter<double>("dz");
    //track analytical cross cut
    r_cut = iConfig.getParameter<double>("rCut");
    vtxChi2_ = iConfig.getParameter<double>("vtxChi2");


    theVertexFinder_ = new ConversionVertexFinder ( iConfig );

    thettbuilder_ = 0;

    //output
    ConvertedPhotonCollection_     = iConfig.getParameter<std::string>("convertedPhotonCollection");

    produces< reco::ConversionCollection >(ConvertedPhotonCollection_);

}


TrackerOnlyConversionProducer::~TrackerOnlyConversionProducer()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
  delete theVertexFinder_;
}


// ------------ method called to produce the data  ------------
    void
TrackerOnlyConversionProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;

    reco::ConversionCollection outputConvPhotonCollection;
    std::auto_ptr<reco::ConversionCollection> outputConvPhotonCollection_p(new reco::ConversionCollection);

    //std::cout << " TrackerOnlyConversionProducer::produce " << std::endl;
    //Read multiple track input collections

    edm::Handle<reco::ConversionTrackCollection> trackCollectionHandle;
    iEvent.getByLabel(src_,trackCollectionHandle);    
    
    edm::Handle<edm::View<reco::CaloCluster> > bcBarrelHandle;//TODO error handling if no collection found
    edm::Handle<edm::View<reco::CaloCluster> > bcEndcapHandle;//TODO check cluster type if BasicCluster or PFCluster
    if (allowTrackBC_){
	iEvent.getByLabel( bcBarrelCollection_, bcBarrelHandle);
	iEvent.getByLabel( bcEndcapCollection_, bcEndcapHandle);
    }

    edm::Handle<reco::VertexCollection> vertexHandle;
    reco::VertexCollection vertexCollection;
    if (usePvtx_){
	iEvent.getByLabel(vertexProducer_, vertexHandle);
	if (!vertexHandle.isValid()) {
	    edm::LogError("TrackerOnlyConversionProducer") << "Error! Can't get the product primary Vertex Collection "<< "\n";
	    usePvtx_ = false;
	}
	if (usePvtx_)
	    vertexCollection = *(vertexHandle.product());
    }

    edm::ESHandle<TransientTrackBuilder> hTransientTrackBuilder;
    iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",hTransientTrackBuilder);
    thettbuilder_ = hTransientTrackBuilder.product();

    reco::Vertex the_pvtx;
    //because the priamry vertex is sorted by quality, the first one is the best
    if (!vertexCollection.empty())
	the_pvtx = *(vertexCollection.begin());

    if (trackCollectionHandle->size()> maxNumOfTrackInPU_ ){
	iEvent.put( outputConvPhotonCollection_p, ConvertedPhotonCollection_);
	return;
    }

    //2. select track by propagating
    //2.0 build Basic cluster geometry map to search in eta bounds for clusters
    std::multimap<double, reco::CaloClusterPtr> basicClusterPtrs;
    edm::Handle<edm::View<reco::CaloCluster> > bcHandle = bcBarrelHandle;

    if (allowTrackBC_){
	for (unsigned jj = 0; jj < 2; ++jj ){
	    for (unsigned ii = 0; ii < bcHandle->size(); ++ii ) {
		if (bcHandle->ptrAt(ii)->energy()>energyBC_)
		    basicClusterPtrs.insert(std::make_pair(bcHandle->ptrAt(ii)->position().eta(), bcHandle->ptrAt(ii)));
	    }
	    bcHandle = bcEndcapHandle;
	}
    }

    buildCollection( iEvent, iSetup, *trackCollectionHandle.product(), basicClusterPtrs, the_pvtx, outputConvPhotonCollection);//allow empty basicClusterPtrs

    outputConvPhotonCollection_p->assign(outputConvPhotonCollection.begin(), outputConvPhotonCollection.end());
    iEvent.put( outputConvPhotonCollection_p, ConvertedPhotonCollection_);

}




void TrackerOnlyConversionProducer::buildCollection(edm::Event& iEvent, const edm::EventSetup& iSetup,
						    const reco::ConversionTrackCollection& allTracks,
						    const std::multimap<double, reco::CaloClusterPtr>& basicClusterPtrs,
						    const reco::Vertex& the_pvtx,
						    reco::ConversionCollection & outputConvPhotonCollection){

  
  edm::ESHandle<TrackerGeometry> trackerGeomHandle;
  edm::ESHandle<MagneticField> magFieldHandle;
  iSetup.get<TrackerDigiGeometryRecord>().get( trackerGeomHandle );
  iSetup.get<IdealMagneticFieldRecord>().get( magFieldHandle );
  
    const TrackerGeometry* trackerGeom = trackerGeomHandle.product();
    const MagneticField* magField = magFieldHandle.product();;

    std::vector<math::XYZPoint> trackImpactPosition;
    trackImpactPosition.reserve(allTracks.size());//track impact position at ECAL
    std::vector<bool> trackValidECAL;//Does this track reach ECAL basic cluster (reach ECAL && match with BC)
    trackValidECAL.assign(allTracks.size(), false);

    std::vector<reco::CaloClusterPtr> trackMatchedBC;
    reco::CaloClusterPtr empty_bc;
    trackMatchedBC.assign(allTracks.size(), empty_bc);//TODO find a better way to avoid copy constructor

    std::vector<int> bcHandleId;//the associated BC handle id, -1 invalid, 0 barrel 1 endcap
    bcHandleId.assign(allTracks.size(), -1);

    std::multimap<double, int> trackInnerEta;//Track innermost state Eta map to TrackRef index, to be used in track pair sorting


    ConversionHitChecker hitChecker;

    
    //2 propagate all tracks into ECAL, record its eta and phi
    for (reco::ConversionTrackCollection::const_iterator tk_ref = allTracks.begin(); tk_ref != allTracks.end(); ++tk_ref ){
      const reco::Track* tk = tk_ref->trackRef().get()  ;

	//map TrackRef to Eta
      trackInnerEta.insert(std::make_pair(tk->innerMomentum().eta(), tk_ref-allTracks.begin()));
      
      if (allowTrackBC_){
	//check impact position then match with BC
	    math::XYZPoint ew;
	    if ( getTrackImpactPosition(tk, trackerGeom, magField, ew) ){
		trackImpactPosition.push_back(ew);

		reco::CaloClusterPtr closest_bc;//the closest matching BC to track

		if ( getMatchedBC(basicClusterPtrs, ew, closest_bc) ){
		    trackMatchedBC[tk_ref-allTracks.begin()] = closest_bc;
		    trackValidECAL[tk_ref-allTracks.begin()] = true;
		    bcHandleId[tk_ref-allTracks.begin()] = (fabs(closest_bc->position().eta())>1.479)?1:0;
		}
	    } else {
		trackImpactPosition.push_back(ew);
		continue;
	    }

	}
    }




    //3. pair up : to select one track from matched EBC, and select the other track to fit cot theta and phi open angle cut
    //TODO it is k-Closest pair of point problem
    //std::cout << " allTracks.size() " <<  allTracks.size() << std::endl;
    for(reco::ConversionTrackCollection::const_iterator ll = allTracks.begin(); ll != allTracks.end(); ++ ll ) {
      bool track1HighPurity=true;
      //std::cout << " Loop on allTracks " << std::endl;
      const  edm::RefToBase<reco::Track> & left = ll->trackRef();
      
      //TODO: This is a workaround, should be fixed with a proper function in the TTBuilder
      //(Note that the TrackRef and GsfTrackRef versions of the constructor are needed
      // to properly get refit tracks in the output vertex)
      reco::TransientTrack ttk_l;
      if (dynamic_cast<const reco::GsfTrack*>(left.get())) {
        ttk_l = thettbuilder_->build(left.castTo<reco::GsfTrackRef>());
      }
      else {
        ttk_l = thettbuilder_->build(left.castTo<reco::TrackRef>());
      }
      

      if ((allowTrackBC_ && !trackValidECAL[ll-allTracks.begin()]) )//this Left leg should have valid BC
        continue;
      
      if (the_pvtx.isValid()){
        if (!(trackD0Cut(left, the_pvtx)))   track1HighPurity=false;
      } else {
        if (!(trackD0Cut(left)))  track1HighPurity=false;
      }

      std::vector<int> right_candidates;//store all right legs passed the cut (theta/approach and ref pair)
      std::vector<double> right_candidate_theta, right_candidate_approach;
      std::vector<std::pair<bool, reco::Vertex> > vertex_candidates;


      for (reco::ConversionTrackCollection::const_iterator rr = ll+1; rr != allTracks.end(); ++ rr ) {
          bool track2HighPurity = true;
          bool highPurityPair = true;
          
          const  edm::RefToBase<reco::Track> & right = rr->trackRef();
       
          //TODO: This is a workaround, should be fixed with a proper function in the TTBuilder
          reco::TransientTrack ttk_r;
          if (dynamic_cast<const reco::GsfTrack*>(right.get())) {
            ttk_r = thettbuilder_->build(right.castTo<reco::GsfTrackRef>());
          }
          else {
            ttk_r = thettbuilder_->build(right.castTo<reco::TrackRef>());
          }
          //std::cout << " This track is " <<  right->algoName() << std::endl;

          
          //all vertexing preselection should go here

          //check for opposite charge
          if (  allowOppCharge_ && (left->charge()*right->charge() > 0) )  
            continue; //same sign, reject pair

          if ( (allowTrackBC_ && !trackValidECAL[rr-allTracks.begin()] && rightBC_) )// if right track matches ECAL
            continue;
          
          
          double approachDist = -999.;
          //apply preselection to track pair, unless one or both tracks are gsf
          if (!preselectTrackPair(ttk_l,ttk_r, approachDist) && left->algo()!=29 && right->algo()!=29) {
            continue;
          }
                    
          //do the actual vertex fit
          reco::Vertex theConversionVertex;//by default it is invalid          
          bool goodVertex = checkVertex(ttk_l, ttk_r, magField, theConversionVertex);
          
          //bail as early as possible in case the fit didn't return a good vertex
          if (!goodVertex) {
            continue;
          }

          
          
          //track pair pass the quality cut
          if (   !( (trackQualityFilter(left, true) && trackQualityFilter(right, false))
                  || (trackQualityFilter(left, false) && trackQualityFilter(right, true)) ) ) {
            highPurityPair=false;
          }

          if (the_pvtx.isValid()){
              if (!(trackD0Cut(right, the_pvtx))) track2HighPurity=false; 
          } else {
              if (!(trackD0Cut(right))) track2HighPurity=false; 
          }


          const std::pair<edm::RefToBase<reco::Track>, reco::CaloClusterPtr> the_left  = std::make_pair(left, trackMatchedBC[ll-allTracks.begin()]);
          const std::pair<edm::RefToBase<reco::Track>, reco::CaloClusterPtr> the_right = std::make_pair(right, trackMatchedBC[rr-allTracks.begin()]);

          
         
          //signature cuts, then check if vertex, then post-selection cuts
          highPurityPair=  highPurityPair && track1HighPurity &&  track2HighPurity && checkTrackPair(the_left, the_right) ;
          highPurityPair = highPurityPair && goodVertex && checkPhi(left, right, trackerGeom, magField, theConversionVertex) ;

          //if all cuts passed, go ahead to make conversion candidates
          std::vector<edm::RefToBase<reco::Track> > trackPairRef;
          trackPairRef.push_back(left);//left track
          trackPairRef.push_back(right);//right track

          std::vector<math::XYZVector> trackPin;
          std::vector<math::XYZVector> trackPout;
          std::vector<math::XYZPoint> trackInnPos;
          std::vector<uint8_t> nHitsBeforeVtx;
          std::vector<Measurement1DFloat> dlClosestHitToVtx;

          if (left->extra().isNonnull() && right->extra().isNonnull()){//only available on TrackExtra
            trackInnPos.push_back(  left->innerPosition());
            trackInnPos.push_back(  right->innerPosition());
            trackPin.push_back(left->innerMomentum());
            trackPin.push_back(right->innerMomentum());
            trackPout.push_back(left->outerMomentum());
            trackPout.push_back(right->outerMomentum());
          }
          
          if (ll->trajRef().isNonnull() && rr->trajRef().isNonnull()) {
            std::pair<uint8_t,Measurement1DFloat> leftWrongHits = hitChecker.nHitsBeforeVtx(*ll->trajRef().get(),theConversionVertex);
            std::pair<uint8_t,Measurement1DFloat> rightWrongHits = hitChecker.nHitsBeforeVtx(*rr->trajRef().get(),theConversionVertex);
            nHitsBeforeVtx.push_back(leftWrongHits.first);
            nHitsBeforeVtx.push_back(rightWrongHits.first);
            dlClosestHitToVtx.push_back(leftWrongHits.second);
            dlClosestHitToVtx.push_back(rightWrongHits.second);            
          }
          
          uint8_t nSharedHits = hitChecker.nSharedHits(*left.get(),*right.get());

          reco::CaloClusterPtrVector scPtrVec;
          //if using kinematic fit, check with chi2 post cut
          if (theConversionVertex.isValid()){
            const float chi2Prob = ChiSquaredProbability(theConversionVertex.chi2(), theConversionVertex.ndof());
            if (chi2Prob<vtxChi2_)  highPurityPair=false;
          }

          //std::cout << "  highPurityPair after vertex cut " <<  highPurityPair << std::endl;
          std::vector<math::XYZPoint> trkPositionAtEcal;
          std::vector<reco::CaloClusterPtr> matchingBC;

          if (allowTrackBC_){//TODO find out the BC ptrs if not doing matching, otherwise, leave it empty
              const int lbc_handle = bcHandleId[ll-allTracks.begin()],
                    rbc_handle = bcHandleId[rr-allTracks.begin()];

              trkPositionAtEcal.push_back(trackImpactPosition[ll-allTracks.begin()]);//left track
              if (trackValidECAL[rr-allTracks.begin()])//second track ECAL position may be invalid
                  trkPositionAtEcal.push_back(trackImpactPosition[rr-allTracks.begin()]);

              matchingBC.push_back(trackMatchedBC[ll-allTracks.begin()]);//left track
              scPtrVec.push_back(trackMatchedBC[ll-allTracks.begin()]);//left track
              if (trackValidECAL[rr-allTracks.begin()]){//second track ECAL position may be invalid
                  matchingBC.push_back(trackMatchedBC[rr-allTracks.begin()]);
                  if (!(trackMatchedBC[rr-allTracks.begin()] == trackMatchedBC[ll-allTracks.begin()])//avoid 1 bc 2 tk
                          && lbc_handle == rbc_handle )//avoid ptr from different collection
                      scPtrVec.push_back(trackMatchedBC[rr-allTracks.begin()]);
              }
          }
          const float minAppDist = approachDist;

          reco::Conversion::ConversionAlgorithm algo = reco::Conversion::algoByName(algoName_);
          float dummy=0;
          reco::Conversion  newCandidate(scPtrVec,  trackPairRef, trkPositionAtEcal, theConversionVertex, matchingBC, minAppDist,  trackInnPos, trackPin, trackPout, nHitsBeforeVtx, dlClosestHitToVtx, nSharedHits, dummy, algo );
          newCandidate.setQuality(reco::Conversion::highPurity,  highPurityPair);
          
          bool generalTracksOnly = ll->isTrackerOnly() && rr->isTrackerOnly() && !dynamic_cast<const reco::GsfTrack*>(ll->trackRef().get()) && !dynamic_cast<const reco::GsfTrack*>(rr->trackRef().get());
          bool arbitratedEcalSeeded = ll->isArbitratedEcalSeeded() && rr->isArbitratedEcalSeeded();
          bool arbitratedMerged = ll->isArbitratedMerged() && rr->isArbitratedMerged();
          bool arbitratedMergedEcalGeneral = ll->isArbitratedMergedEcalGeneral() && rr->isArbitratedMergedEcalGeneral();          
          
          newCandidate.setQuality(reco::Conversion::generalTracksOnly,  generalTracksOnly);
          newCandidate.setQuality(reco::Conversion::arbitratedEcalSeeded,  arbitratedEcalSeeded);
          newCandidate.setQuality(reco::Conversion::arbitratedMerged,  arbitratedMerged);
          newCandidate.setQuality(reco::Conversion::arbitratedMergedEcalGeneral,  arbitratedMergedEcalGeneral);          
          
          outputConvPhotonCollection.push_back(newCandidate);

      }
      
  }






}





//
// member functions
//

inline bool TrackerOnlyConversionProducer::trackQualityFilter(const  edm::RefToBase<reco::Track>&   ref, bool isLeft){
    bool pass = true;
    if (isLeft){
	pass = (ref->normalizedChi2() < maxChi2Left_ && ref->found() >= minHitsLeft_);
    } else {
	pass = (ref->normalizedChi2() < maxChi2Right_ && ref->found() >= minHitsRight_);
    }

    return pass;
}

inline bool TrackerOnlyConversionProducer::trackD0Cut(const  edm::RefToBase<reco::Track>&  ref){
    //NOTE if not allow d0 cut, always true
    return ((!allowD0_) || !(ref->d0()*ref->charge()/ref->d0Error()<d0Cut_));
}

inline bool TrackerOnlyConversionProducer::trackD0Cut(const edm::RefToBase<reco::Track>&  ref, const reco::Vertex& the_pvtx){
    //
    return ((!allowD0_) || !(-ref->dxy(the_pvtx.position())*ref->charge()/ref->dxyError()<d0Cut_));
}


bool TrackerOnlyConversionProducer::getTrackImpactPosition(const reco::Track* tk_ref,
	const TrackerGeometry* trackerGeom, const MagneticField* magField,
	math::XYZPoint& ew){

    PropagatorWithMaterial propag( alongMomentum, 0.000511, magField );
    TrajectoryStateTransform transformer;
    ReferenceCountingPointer<Surface> ecalWall(
	    new  BoundCylinder( GlobalPoint(0.,0.,0.), TkRotation<float>(),
		SimpleCylinderBounds( 129, 129, -320.5, 320.5 ) ) );
    const float epsilon = 0.001;
    Surface::RotationType rot; // unit rotation matrix
    const float barrelRadius = 129.f;
    const float barrelHalfLength = 270.9f;
    const float endcapRadius = 171.1f;
    const float endcapZ = 320.5f;
    ReferenceCountingPointer<BoundCylinder>  theBarrel_(new BoundCylinder( Surface::PositionType(0,0,0), rot,
		SimpleCylinderBounds( barrelRadius-epsilon, barrelRadius+epsilon, -barrelHalfLength, barrelHalfLength)));
    ReferenceCountingPointer<BoundDisk>      theNegativeEtaEndcap_(
	    new BoundDisk( Surface::PositionType( 0, 0, -endcapZ), rot,
		SimpleDiskBounds( 0, endcapRadius, -epsilon, epsilon)));
    ReferenceCountingPointer<BoundDisk>      thePositiveEtaEndcap_(
	    new BoundDisk( Surface::PositionType( 0, 0, endcapZ), rot,
		SimpleDiskBounds( 0, endcapRadius, -epsilon, epsilon)));

    //const TrajectoryStateOnSurface myTSOS = transformer.innerStateOnSurface(*(*ref), *trackerGeom, magField);
    const TrajectoryStateOnSurface myTSOS = transformer.outerStateOnSurface(*tk_ref, *trackerGeom, magField);
    TrajectoryStateOnSurface  stateAtECAL;
    stateAtECAL = propag.propagate(myTSOS, *theBarrel_);
    if (!stateAtECAL.isValid() || ( stateAtECAL.isValid() && fabs(stateAtECAL.globalPosition().eta() ) >1.479 )  ) {
	//endcap propagator
	if (myTSOS.globalPosition().eta() > 0.) {
	    stateAtECAL = propag.propagate(myTSOS, *thePositiveEtaEndcap_);
	} else {
	    stateAtECAL = propag.propagate(myTSOS, *theNegativeEtaEndcap_);
	}
    }       
    if (stateAtECAL.isValid()){
	ew = stateAtECAL.globalPosition();
	return true;
    }
    else
	return false;
}

bool TrackerOnlyConversionProducer::getMatchedBC(const std::multimap<double, reco::CaloClusterPtr>& bcMap, 
	const math::XYZPoint& trackImpactPosition,
	reco::CaloClusterPtr& closestBC){
    const double track_eta = trackImpactPosition.eta();
    const double track_phi = trackImpactPosition.phi();

    double min_eta = 999., min_phi = 999.;
    reco::CaloClusterPtr closest_bc;
    for (std::multimap<double, reco::CaloClusterPtr>::const_iterator bc = bcMap.lower_bound(track_eta - halfWayEta_);
	    bc != bcMap.upper_bound(track_eta + halfWayEta_); ++bc){//use eta map to select possible BC collection then loop in
	const reco::CaloClusterPtr& ebc = bc->second;
	const double delta_eta = track_eta-(ebc->position().eta());
	const double delta_phi = reco::deltaPhi(track_phi, (ebc->position().phi()));
	if (fabs(delta_eta)<dEtaTkBC_ && fabs(delta_phi)<dPhiTkBC_){
	    if (fabs(min_eta)>fabs(delta_eta) && fabs(min_phi)>fabs(delta_phi)){//take the closest to track BC
		min_eta = delta_eta;
		min_phi = delta_phi;
		closest_bc = bc->second;
		//TODO check if min_eta>delta_eta but min_phi<delta_phi
	    }
	}
    }

    if (min_eta < 999.){
	closestBC = closest_bc;
	return true;
    } else
	return false;
}

bool TrackerOnlyConversionProducer::getMatchedBC(const reco::CaloClusterPtrVector& bcMap,
	const math::XYZPoint& trackImpactPosition,
	reco::CaloClusterPtr& closestBC){
    const double track_eta = trackImpactPosition.eta();
    const double track_phi = trackImpactPosition.phi();

    double min_eta = 999., min_phi = 999.;
    reco::CaloClusterPtr closest_bc;
    for (reco::CaloClusterPtrVector::const_iterator bc = bcMap.begin();
	    bc != bcMap.end(); ++bc){//use eta map to select possible BC collection then loop in
	const reco::CaloClusterPtr& ebc = (*bc);
	const double delta_eta = track_eta-(ebc->position().eta());
	const double delta_phi = reco::deltaPhi(track_phi, (ebc->position().phi()));
	if (fabs(delta_eta)<dEtaTkBC_ && fabs(delta_phi)<dPhiTkBC_){
	    if (fabs(min_eta)>fabs(delta_eta) && fabs(min_phi)>fabs(delta_phi)){//take the closest to track BC
		min_eta = delta_eta;
		min_phi = delta_phi;
		closest_bc = ebc;
		//TODO check if min_eta>delta_eta but min_phi<delta_phi
	    }
	}
    }

    if (min_eta < 999.){
	closestBC = closest_bc;
	return true;
    } else
	return false;
}

//check track open angle of phi at vertex
bool TrackerOnlyConversionProducer::checkPhi(const edm::RefToBase<reco::Track>& tk_l, const edm::RefToBase<reco::Track>& tk_r,
	const TrackerGeometry* trackerGeom, const MagneticField* magField,
	const reco::Vertex& vtx){
    if (!allowDeltaPhi_)
	return true;
    //if track has innermost momentum, check with innermost phi
    //if track also has valid vertex, propagate to vertex then calculate phi there
    //if track has no innermost momentum, just return true, because track->phi() makes no sense
    if (tk_l->extra().isNonnull() && tk_r->extra().isNonnull()){
	double iphi1 = tk_l->innerMomentum().phi(), iphi2 = tk_r->innerMomentum().phi();
	if (vtx.isValid()){
	    PropagatorWithMaterial propag( anyDirection, 0.000511, magField );
	    TrajectoryStateTransform transformer;
	    double recoPhoR = vtx.position().Rho();
	    Surface::RotationType rot;
	    ReferenceCountingPointer<BoundCylinder>  theBarrel_(new BoundCylinder( Surface::PositionType(0,0,0), rot,
			SimpleCylinderBounds( recoPhoR-0.001, recoPhoR+0.001, -fabs(vtx.position().z()), fabs(vtx.position().z()))));
	    ReferenceCountingPointer<BoundDisk>      theDisk_(
		    new BoundDisk( Surface::PositionType( 0, 0, vtx.position().z()), rot,
			SimpleDiskBounds( 0, recoPhoR, -0.001, 0.001)));

	    const TrajectoryStateOnSurface myTSOS1 = transformer.innerStateOnSurface(*tk_l, *trackerGeom, magField);
	    const TrajectoryStateOnSurface myTSOS2 = transformer.innerStateOnSurface(*tk_r, *trackerGeom, magField);
	    TrajectoryStateOnSurface  stateAtVtx1, stateAtVtx2;
	    stateAtVtx1 = propag.propagate(myTSOS1, *theBarrel_);
	    if (!stateAtVtx1.isValid() ) {
		stateAtVtx1 = propag.propagate(myTSOS1, *theDisk_);
	    }
	    if (stateAtVtx1.isValid()){
		iphi1 = stateAtVtx1.globalDirection().phi();
	    }
	    stateAtVtx2 = propag.propagate(myTSOS2, *theBarrel_);
	    if (!stateAtVtx2.isValid() ) {
		stateAtVtx2 = propag.propagate(myTSOS2, *theDisk_);
	    }
	    if (stateAtVtx2.isValid()){
		iphi2 = stateAtVtx2.globalDirection().phi();
	    }
	}
	const double dPhi = reco::deltaPhi(iphi1, iphi2);
	return (fabs(dPhi) < deltaPhi_);
    } else {
	return true;
    }
}

bool TrackerOnlyConversionProducer::preselectTrackPair(const reco::TransientTrack &ttk_l, const reco::TransientTrack &ttk_r,
             double& appDist) {
  

  double dCotTheta =  1./tan(ttk_l.track().innerMomentum().theta()) - 1./tan(ttk_r.track().innerMomentum().theta());
  if (allowDeltaCot_ && (std::abs(dCotTheta) > deltaCotTheta_)) {
    return false;
  }
  
  //non-conversion hypothesis, reject prompt track pairs
  ClosestApproachInRPhi closest;
  closest.calculate(ttk_l.innermostMeasurementState(),ttk_r.innermostMeasurementState());
  if (!closest.status()) {
    return false;
  }
  
  if (closest.crossingPoint().perp() < r_cut) {
    return false;
  }

  
  //compute tangent point btw tracks (conversion hypothesis)
  TangentApproachInRPhi tangent;
  tangent.calculate(ttk_l.innermostMeasurementState(),ttk_r.innermostMeasurementState());
  if (!tangent.status()) {
    return false;
  }
  
  GlobalPoint tangentPoint = tangent.crossingPoint();
  double rho = tangentPoint.perp();
  
  //reject candidates well outside of tracker bounds
  if (rho > 120.0) {
    return false;
  }
  
  if (std::abs(tangentPoint.z()) > 300.0) {
    return false;
  }
  
  std::pair<GlobalTrajectoryParameters,GlobalTrajectoryParameters> trajs = tangent.trajectoryParameters();
  
  //very large separation in z, no hope
  if (std::abs(trajs.first.position().z() - trajs.second.position().z()) > dzCut_) {
    return false;
  }
  
  
  float minApproach = tangent.perpdist();
  appDist = minApproach;
  
  if (allowMinApproach_ && (minApproach < minApproachLow_ || minApproach > minApproachHigh_) ) {
    return false;
  }
  
  return true;
  
  
}

bool TrackerOnlyConversionProducer::checkTrackPair(const std::pair<edm::RefToBase<reco::Track>, reco::CaloClusterPtr>& ll, 
	const std::pair<edm::RefToBase<reco::Track>, reco::CaloClusterPtr>& rr){

     const reco::CaloClusterPtr& bc_l = ll.second;//can be null, so check isNonnull()
    const reco::CaloClusterPtr& bc_r = rr.second;
    
    //The cuts should be ordered by considering if takes time and if cuts off many fakes
    if (allowTrackBC_){
	//check energy of BC
	double total_e_bc = 0;
	if (bc_l.isNonnull()) total_e_bc += bc_l->energy();
	if (rightBC_) 
	    if (bc_r.isNonnull())
		total_e_bc += bc_r->energy();

	if (total_e_bc < energyTotalBC_) return false;
    }

    return true;
}



//because reco::vertex uses track ref, so have to keep them
bool TrackerOnlyConversionProducer::checkVertex(const reco::TransientTrack &ttk_l, const reco::TransientTrack &ttk_r, 
	const MagneticField* magField,
	reco::Vertex& the_vertex){
    bool found = false;

    std::vector<reco::TransientTrack>  pair;
    pair.push_back(ttk_l);
    pair.push_back(ttk_r);
   
    found = theVertexFinder_->run(pair, the_vertex);



    return found;
}


