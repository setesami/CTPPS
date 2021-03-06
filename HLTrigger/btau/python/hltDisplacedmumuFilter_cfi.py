import FWCore.ParameterSet.Config as cms

hltFilterDisplacedmumu = cms.EDFilter("HLTDisplacedmumuFilter",
    MuonTag = cms.InputTag( "hltL3MuonCandidates" ),
    DisplacedVertexTag = cms.InputTag( "hltDisplacedmumuVtx" ),
    FastAccept = cms.bool( False ),
    MinLxySignificance = cms.double( 0.0 ),
    MaxNormalisedChi2 = cms.double( 10.0 ),
    MinCosinePointingAngle = cms.double( -2.0 ),
    SaveTag = cms.untracked.bool( True ),
    BeamSpotTag = cms.InputTag( "hltOnlineBeamSpot" )
)
