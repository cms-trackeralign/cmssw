import FWCore.ParameterSet.Config as cms

# AlCaReco for track based alignment using ZMuMu events
OutALCARECOTkAlHLTTracksZMuMu_noDrop = cms.PSet(
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOTkAlHLTTracksZMuMu')
    ),
    outputCommands = cms.untracked.vstring(
        'keep recoTracks_ALCARECOTkAlHLTTracksZMuMu_*_*',
        'keep recoTrackExtras_ALCARECOTkAlHLTTracksZMuMu_*_*',
        'keep TrackingRecHitsOwned_ALCARECOTkAlHLTTracksZMuMu_*_*',
        'keep SiPixelClusteredmNewDetSetVector_ALCARECOTkAlHLTTracksZMuMu_*_*',
        'keep SiStripClusteredmNewDetSetVector_ALCARECOTkAlHLTTracksZMuMu_*_*',
        'keep L1AcceptBunchCrossings_*_*_*',
        'keep L1GlobalTriggerReadoutRecord_gtDigis_*_*',
        'keep *_TriggerResults_*_*',
        'keep DcsStatuss_scalersRawToDigi_*_*',
	'keep *_offlinePrimaryVertices_*_*')
)

# add branches for MC truth evaluation
from GeneratorInterface.Configuration.GeneratorInterface_EventContent_cff import GeneratorInterfaceAOD
from SimGeneral.Configuration.SimGeneral_EventContent_cff import SimGeneralAOD

OutALCARECOTkAlHLTTracksZMuMu_noDrop.outputCommands.extend(GeneratorInterfaceAOD.outputCommands)
_modifiedCommandsForGEN =  OutALCARECOTkAlHLTTracksZMuMu_noDrop.outputCommands.copy()
_modifiedCommandsForGEN.remove('keep *_genParticles_*_*')    # full genParticles list is too heavy
_modifiedCommandsForGEN.append('keep *_TkAlZMuMuGenMuonSelector_*_*') # Keep only the filtered gen muons
OutALCARECOTkAlHLTTracksZMuMu_noDrop.outputCommands = _modifiedCommandsForGEN

OutALCARECOTkAlHLTTracksZMuMu_noDrop.outputCommands.extend(SimGeneralAOD.outputCommands)

