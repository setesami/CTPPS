import FWCore.ParameterSet.Config as cms

from DQMOffline.Trigger.HLTTauDQMOffline_cfi import *
from DQMOffline.Trigger.HLTTauPostProcessor_cfi import *
from DQMOffline.Trigger.HLTTauQualityTester_cfi import *
from DQMOffline.Trigger.HLTTauCertifier_cfi import *

HLTTauDQMOffline = cms.Sequence(TauRefProducer+
                                hltTauOfflineMonitor_PFTaus
                                +hltTauOfflineMonitor_Photons
                                +hltTauOfflineMonitor_HPD
                                +hltTauOfflineMonitor_Inclusive
                                )

HLTTauDQMOfflineHarvesting = cms.Sequence(HLTTauPostProcess)


                               
HLTTauDQMOfflineQuality = cms.Sequence(hltTauOfflineQualityTests)

HLTTauDQMOfflineCertification = cms.Sequence(hltTauOfflineCertification)
