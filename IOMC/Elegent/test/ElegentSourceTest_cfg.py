import FWCore.ParameterSet.Config as cms

process = cms.Process("ElegentSourceTest")

# Specify the maximum events to simulate
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(200)
)

process.load("Configuration.TotemCommon.LoggerMin_cfi")

# random number seeds
process.load("Configuration.TotemCommon.RandomNumbers_cfi")

# use particle table
#process.load("SimGeneral.HepPDTESSource.pdt_cfi")

process.source = cms.Source("EmptySource")

# Use random number generator service
process.load("Configuration.TotemCommon.RandomNumbers_cfi")

import IOMC.Elegent.ElegentSource_cfi
process.generator = IOMC.Elegent.ElegentSource_cfi.generator
energy = "4000"
process.generator.fileName = IOMC.Elegent.ElegentSource_cfi.ElegentDefaultFileName(energy)

process.p1 = cms.Path(process.generator)
