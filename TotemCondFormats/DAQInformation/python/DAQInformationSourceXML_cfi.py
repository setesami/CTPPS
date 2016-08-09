import FWCore.ParameterSet.Config as cms

DAQInformationSourceXML = cms.ESSource("DAQInformationSourceXML",
    verbosity = cms.untracked.uint32(0),
    xmlFileName = cms.string('')
)
