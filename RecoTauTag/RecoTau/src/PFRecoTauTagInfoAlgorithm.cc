#include "RecoTauTag/RecoTau/interface/PFRecoTauTagInfoAlgorithm.h"

using namespace reco;

PFRecoTauTagInfoAlgorithm::PFRecoTauTagInfoAlgorithm(const edm::ParameterSet& parameters){
  // parameters of the considered charged hadr. PFCandidates, based on their rec. tk properties :
  ChargedHadronsAssociationCone_      = parameters.getParameter<double>("ChargedHadrCand_AssociationCone");
  ChargedHadrCand_tkminPt_            = parameters.getParameter<double>("ChargedHadrCand_tkminPt");
  ChargedHadrCand_tkminPixelHitsn_    = parameters.getParameter<int>("ChargedHadrCand_tkminPixelHitsn");
  ChargedHadrCand_tkminTrackerHitsn_  = parameters.getParameter<int>("ChargedHadrCand_tkminTrackerHitsn");
  ChargedHadrCand_tkmaxipt_           = parameters.getParameter<double>("ChargedHadrCand_tkmaxipt");
  ChargedHadrCand_tkmaxChi2_          = parameters.getParameter<double>("ChargedHadrCand_tkmaxChi2");
  // parameters of the considered neutral hadr. PFCandidates, based on their rec. HCAL clus. properties : 
  NeutrHadrCand_HcalclusMinEt_         = parameters.getParameter<double>("NeutrHadrCand_HcalclusMinEt");
  // parameters of the considered gamma PFCandidates, based on their rec. ECAL clus. properties :
  GammaCand_EcalclusMinEt_             = parameters.getParameter<double>("GammaCand_EcalclusMinEt");
  // parameters of the considered rec. Tracks (these ones catched through a JetTracksAssociation object, not through the charged hadr. PFCandidates inside the PFJet ; the motivation for considering them is the need for checking that a selection by the charged hadr. PFCandidates is equivalent to a selection by the rec. Tracks.) :
  tkminPt_                            = parameters.getParameter<double>("tkminPt");
  tkminPixelHitsn_                    = parameters.getParameter<int>("tkminPixelHitsn");
  tkminTrackerHitsn_                  = parameters.getParameter<int>("tkminTrackerHitsn");
  tkmaxipt_                           = parameters.getParameter<double>("tkmaxipt");
  tkmaxChi2_                          = parameters.getParameter<double>("tkmaxChi2");
  // 
  UsePVconstraint_                    = parameters.getParameter<bool>("UsePVconstraint");  
  ChargedHadrCand_tkPVmaxDZ_          = parameters.getParameter<double>("ChargedHadrCand_tkPVmaxDZ");
  tkPVmaxDZ_                          = parameters.getParameter<double>("tkPVmaxDZ");
}
PFTauTagInfo PFRecoTauTagInfoAlgorithm::buildPFTauTagInfo(const PFJetRef& thePFJet,const PFCandidateRefVector& thePFCandsInEvent, const TrackRefVector& theTracks,const Vertex& thePV){
  PFTauTagInfo resultExtended;
  resultExtended.setpfjetRef(thePFJet);
  
  PFCandidateRefVector thePFCands;
  for (PFCandidateRefVector::const_iterator iPFCand=thePFCandsInEvent.begin();iPFCand!=thePFCandsInEvent.end();iPFCand++){
    double delta = ROOT::Math::VectorUtil::DeltaR((*thePFJet).p4().Vect(), (*iPFCand)->p4().Vect());
    if (delta < ChargedHadronsAssociationCone_)  thePFCands.push_back(*iPFCand);   
  }
  if(thePV.z() < -500.) UsePVconstraint_ = false;
  
  PFCandidateRefVector theFilteredPFChargedHadrCands;
  if (UsePVconstraint_) theFilteredPFChargedHadrCands=TauTagTools::filteredPFChargedHadrCands(thePFCands,ChargedHadrCand_tkminPt_,ChargedHadrCand_tkminPixelHitsn_,ChargedHadrCand_tkminTrackerHitsn_,ChargedHadrCand_tkmaxipt_,ChargedHadrCand_tkmaxChi2_,ChargedHadrCand_tkPVmaxDZ_, thePV, thePV.z());
  else theFilteredPFChargedHadrCands=TauTagTools::filteredPFChargedHadrCands(thePFCands,ChargedHadrCand_tkminPt_,ChargedHadrCand_tkminPixelHitsn_,ChargedHadrCand_tkminTrackerHitsn_,ChargedHadrCand_tkmaxipt_,ChargedHadrCand_tkmaxChi2_, thePV);
  resultExtended.setPFChargedHadrCands(theFilteredPFChargedHadrCands);
  resultExtended.setPFNeutrHadrCands(TauTagTools::filteredPFNeutrHadrCands(thePFCands,NeutrHadrCand_HcalclusMinEt_));
  resultExtended.setPFGammaCands(TauTagTools::filteredPFGammaCands(thePFCands,GammaCand_EcalclusMinEt_));
  
  TrackRefVector theFilteredTracks;
  if (UsePVconstraint_) theFilteredTracks=TauTagTools::filteredTracks(theTracks,tkminPt_,tkminPixelHitsn_,tkminTrackerHitsn_,tkmaxipt_,tkmaxChi2_,tkPVmaxDZ_,thePV, thePV.z());
  else theFilteredTracks=TauTagTools::filteredTracks(theTracks,tkminPt_,tkminPixelHitsn_,tkminTrackerHitsn_,tkmaxipt_,tkmaxChi2_,thePV);
  resultExtended.setTracks(theFilteredTracks);

  return resultExtended; 
}

