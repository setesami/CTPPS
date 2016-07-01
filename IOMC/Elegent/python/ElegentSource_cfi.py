import FWCore.ParameterSet.Config as cms

# returns the default file name for the given energy
def ElegentDefaultFileName(energy):
  if energy == '7000':
    return 'IOMC/Elegent/data/t-distributions,pp,14000GeV.root'
  if energy == '6500':
    return 'IOMC/Elegent/data/t-distributions,pp,13000GeV.root'
  if energy == '4000':
    return 'IOMC/Elegent/data/t-distributions,pp,8000GeV.root'
  if energy == '3500':
    return 'IOMC/Elegent/data/t-distributions,pp,7000GeV.root'
  if energy == '1380':
    return 'IOMC/Elegent/data/t-distributions,pp,2760GeV.root'
  raise Exception('Energy ' + energy + ' is not supported.')


generator = cms.EDProducer("ElegentSource",
    verbosity = cms.untracked.uint32(1),

    # the name of ROOT file with CDFs
    fileName = cms.string(''),

    # choice of interaction model (for details see http://elegent.hepforge.org/)
    #   the naming scheme is
    #     <range>/<hadronic model>/<coulomb-interaction model>
    #   available ranges are
    #     'full range': samples linear in |t|
    #     'low |t|': samples linear in log |t|
    #   available hadronic-model tags can be obtained by running:
    #     ElegentTDistributionSampler -model-list
    #   available coulomb-interaction tags:
    #     PH = pure hadron, i.e. no Coulomb effects included
	# 	  WY = Coulomb effects included via West-Yennie formula
	# 	  KL = Coulomb effects included via Kundrat-Lokajicek formula
    model = cms.string('full range/petrov (3p) [02]/KL'),

    # |t| range restrictions, in GeV^2
    t_min = cms.double(0.0),
    t_max = cms.double(10.0),
)
