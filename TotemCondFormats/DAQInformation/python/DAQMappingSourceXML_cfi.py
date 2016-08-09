import FWCore.ParameterSet.Config as cms

DAQMappingSourceXML = cms.ESSource("DAQMappingSourceXML",
  verbosity = cms.untracked.uint32(0),

  mappingFileNames = cms.untracked.vstring(),
  maskFileNames = cms.untracked.vstring(),

  # DEPRECATED
  xmlMappingFileName = cms.string(""), 
  xmlMaskFileName = cms.string("")
)
