import FWCore.ParameterSet.Config as cms

from Configuration.TotemStandardSequences.prodRPinelasticDefault_cfg import *

process.setName_("prodRPinelasticBeta90Energy6500GeV")

process.load("Configuration.TotemCommon.LoggerMin_cfi")
#process.load("Configuration.TotemCommon.LoggerMax_cfi")

# Specify the maximum events to simulate
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32(1000)
)

# Specify the output filename
exec 'process.' + str(process.outpath) + '.fileName = cms.untracked.string("file:PPSTiming_prodRPinelasticBeta90Energy6500GeV.root")'

# particle generator paramteres
process.load("IOMC.FlatProtonLogKsiLogTGun.Beta90Energy6500GeV_cfi")

# optics
process.load("Configuration.TotemOpticsConfiguration.OpticsConfig_6500GeV_90_cfi")

# G4 geometry
#process.load("SimG4CMS.PPS.MYgeometryRP_cfi")
#process.load("justlbarswithwindowbothsidewithoutRPs_cfi")
#process.load("mytestpps_cfi")
#process.load("simpletest_cfi")

#process.load("justlbarswithwindowbothside_cfi")
#process.load("justlbarswithwindowbothsidewithboxes_cfi")
#process.load("cylinder_boxcutcorner_lbars_cfi")
#process.load("PPSTiminggeometryRP_cfi")

process.load("finalGeometry_barsboxcylinderstation_cfi")
process.XMLIdealGeometryESSource.geomXMLFiles.append('Geometry/TotemRPData/data/RP_Beta_90/RP_Dist_Beam_Cent.xml')

process.load("RecoTotemRP.RPInelasticReconstruction.Rec_6500GeV_beta_90_cfi")
process.RP220Reconst.BeamProtTransportSetup = process.BeamProtTransportSetup

process.g4SimHits.Physics.BeamProtTransportSetup = process.BeamProtTransportSetup


process.g4SimHits.PPSSD = cms.PSet(
 Verbosity = cms.untracked.int32(0)
)


process.p1 = cms.Path(process.generator*process.SmearingGenerator*process.g4SimHits*process.mix
#*process.RPSiDetDigitizer*process.RPClustProd*process.RPHecoHitProd*process.RPSinglTrackCandFind*process.RPSingleTrackCandCollFit*process.RP220Reconst
)
