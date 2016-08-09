import FWCore.ParameterSet.Config as cms

from Configuration.TotemStandardSequences.prodRPinelasticDefault_cfg import *

process.setName_("prodRPinelasticBeta90Energy7TeV")

# Specify the maximum events to simulate
process.maxEvents = cms.untracked.PSet(
  input=cms.untracked.int32(50)
)

# Specify the output filename
exec 'process.' + str(process.outpath) + '.fileName = cms.untracked.string("file:prodRPinelasticBeta90Energy7TeV_withDeadChannels.root")' 

process.source = cms.Source("EmptySource")

# particle generator paramteres
process.load("IOMC.FlatProtonLogKsiLogTGun.Beta90_cfi")

# optics
process.load("Configuration.TotemOpticsConfiguration.OpticsConfig_7000GeV_90_cfi")

# G4 geometry
process.load("Configuration.TotemCommon.geometryRP_cfi")
process.XMLIdealGeometryESSource.geomXMLFiles.append('Geometry/TotemRPData/data/RP_Beta_90/RP_Dist_Beam_Cent.xml')

process.load('TotemCondFormats.DAQInformation.DAQMappingSourceXML_cfi')

# mapping files
process.DAQMappingSourceXML.mappingFileNames.append('TotemCondFormats/DAQInformation/data/rp_220.xml')
process.DAQMappingSourceXML.mappingFileNames.append('TotemCondFormats/DAQInformation/data/rp_147.xml')
process.DAQMappingSourceXML.mappingFileNames.append('TotemCondFormats/DAQInformation/data/t1_all_run1.xml')
process.DAQMappingSourceXML.mappingFileNames.append('TotemCondFormats/DAQInformation/data/t2_4quarters.xml')

# mask files
process.DAQMappingSourceXML.maskFileNames.append('TotemCondFormats/DAQInformation/data/deadChannels_220.xml')
process.DAQMappingSourceXML.maskFileNames.append('TotemCondFormats/DAQInformation/data/deadChannels_147.xml')

process.RPSiDetDigitizer.simulateDeadChannels = cms.bool(True)

process.load("RecoTotemRP.RPInelasticReconstruction.RPRec220_cfi")
process.RP220Reconst.BeamProtTransportSetup = process.BeamProtTransportSetup

process.g4SimHits.Physics.BeamProtTransportSetup = process.BeamProtTransportSetup

process.p1 = cms.Path(process.generator * process.SmearingGenerator * process.g4SimHits * process.mix * process.RPSiDetDigitizer * process.RPClustProd * process.RPHecoHitProd * process.RPSinglTrackCandFind * process.RPSingleTrackCandCollFit * process.RP220Reconst)
