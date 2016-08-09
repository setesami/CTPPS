import FWCore.ParameterSet.Config as cms
process = cms.Process("T1ChannelsHitRate")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10000)
)

process.load("Configuration.TotemCommon.LoggerMin_cfi")

# raw data source
process.source = cms.Source("RawDataSource",
    verbosity = cms.untracked.uint32(0),
    eventsToCheck = cms.uint32(10),
    skipCorruptedEvents = cms.bool(False),
    performChecks = cms.bool(True), 
    setRunNumberFromFileName=cms.bool(False),    
    fileNames = cms.untracked.vstring('/castor/cern.ch/totem/LHCRawData/2011/Physics/run_5656.000.vmea')
)

process.t1channes = cms.EDAnalyzer("T1DeadChannelDataAnalyzer",
                                    verbosity = cms.untracked.uint32(0),
                                    outputFile = cms.string("T1RawDeadChannelsList.txt"),
                                    onlyStrips = cms.bool(True),
        deadCutValue=cms.untracked.uint32(100)

)

process.DAQInformationSourceXML = cms.ESSource("DAQInformationSourceXML",
    xmlFileName = cms.string('TotemRawData/RawToDigi/python/T1_all_0.xml')
)


process.o1 = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('./deadChannels.root'),
    outputCommands=cms.untracked.vstring('drop totemRawEvent_*_*_*')
)

process.p1 = cms.Path(process.t1channes)

process.outpath = cms.EndPath(process.o1)

