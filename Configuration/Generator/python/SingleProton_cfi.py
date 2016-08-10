import FWCore.ParameterSet.Config as cms

generator = cms.EDProducer("FlatRandomEGunProducer",
    PGunParameters = cms.PSet(
        PartID = cms.vint32(2212),
        MaxEta = cms.double(12),
        MaxPhi = cms.double(3.141592654),
        MinEta = cms.double(8),
        MinE = cms.double(6499),
        MinPhi = cms.double(-3.141592654), ## in radians

        MaxE = cms.double(6500)
    ),
    Verbosity = cms.untracked.int32(10), ## set to 1 (or greater)  for printouts

    psethack = cms.string('single proton'),
    AddAntiParticle = cms.bool(False),
    firstRun = cms.untracked.uint32(1)
)
