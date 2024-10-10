import FWCore.ParameterSet.Config as cms
import copy
# ------------------------------------------------------------------------------
# configure a filter to run only on the events selected by TkAlMinBias AlcaReco
from Alignment.CommonAlignmentProducer.ALCARECOPromptCalibProdSiPixelAli_cff import *
ALCARECOTkAlMinBiasFilterForSiPixelAliHLT = ALCARECOTkAlMinBiasFilterForSiPixelAli.clone() # ??? 

from Alignment.CommonAlignmentProducer.LSNumberFilter_cfi import *

# Ingredient: offlineBeamSpot
from RecoVertex.BeamSpotProducer.BeamSpot_cfi import offlineBeamSpot

# Ingredient: AlignmentTrackSelector
# track selector for HighPurity tracks
#-- AlignmentTrackSelector
from Alignment.CommonAlignmentProducer.AlignmentTrackSelector_cfi import AlignmentTrackSelector
SiPixelAliHighPuritySelectorHLT = AlignmentTrackSelector.clone(
    applyBasicCuts = True,
    #filter = True,
    src = cms.InputTag("hltMergedTracks"), # ???
    trackQualities = ["highPurity"],
    pMin = 4.,
)



# track selection for alignment
SiPixelAliTrackSelectorHLT = SiPixelAliTrackSelector.clone( 
	src = 'SiPixelAliTrackFitterHLT',
)


# Ingredient: SiPixelAliTrackRefitter0
# refitting
from RecoTracker.IterativeTracking.InitialStep_cff import *
from RecoTracker.Configuration.RecoTrackerP5_cff import *
from RecoTracker.TrackProducer.TrackRefitter_cfi import *
# In the following use
# TrackRefitter (normal tracks), TrackRefitterP5 (cosmics) or TrackRefitterBHM (beam halo)

SiPixelAliTrackRefitterHLT0 = TrackRefitter.clone(
        src = 'SiPixelAliHighPuritySelectorHLT',   #'ALCARECOTkAlMinBias'#'ALCARECOTkAlCosmicsCTF0T' #'ALCARECOTkAlMuonIsolated'
        NavigationSchool = '',            # to avoid filling hit pattern
        TTRHBuilder = 'hltESPTTRHBWithTrackAngle' # Defined below ???
        )

SiPixelAliTrackRefitterHLT1 = SiPixelAliTrackRefitter0.clone(
	src = 'SiPixelAliTrackSelectorHLT'
)

#-- Alignment producer
from Alignment.MillePedeAlignmentAlgorithm.MillePedeAlignmentAlgorithm_cfi import *
from Alignment.CommonAlignmentProducer.AlignmentProducerAsAnalyzer_cff import AlignmentProducer
SiPixelAliMilleAlignmentProducerHLT = SiPixelAliMilleAlignmentProducer.clone(
    tjTkAssociationMapTag = 'SiPixelAliTrackRefitterHLT1',
    algoConfig = MillePedeAlignmentAlgorithm.clone(
        binaryFile = 'milleBinaryHLT_0.dat',
        treeFile = 'treeFileHLT.root',
        monitorFile = 'millePedeMonitorHLT.root'
        )
)
# Does anything else of the AlignmentProducer need to be overwritten ???




# Ingredient: SiPixelAliTrackerTrackHitFilter
SiPixelAliTrackerTrackHitFilterHLT = SiPixelAliTrackerTrackHitFilter.clone(
	src = 'SiPixelAliTrackRefitterHLT0'
)


# Ingredient: SiPixelAliSiPixelAliTrackFitter
import RecoTracker.TrackProducer.CTFFinalFitWithMaterial_cff as fitWithMaterial
SiPixelAliTrackFitterHLT = fitWithMaterial.ctfWithMaterialTracks.clone(
        src = 'SiPixelAliTrackerTrackHitFilterHLT',
        # TTRHBuilder = 'hltESPTTRHBWithTrackAngle', #should already be default ???
        NavigationSchool = ''
        )

# Ingredient: hltESPTTRHBWithTrackAngle
hltESPPixelCPEGeneric = cms.ESProducer( 
    "PixelCPEGenericESProducer",
    LoadTemplatesFromDB = cms.bool( True ),
    Alpha2Order = cms.bool( True ),
    ClusterProbComputationFlag = cms.int32( 0 ),
    useLAWidthFromDB = cms.bool( False ),
    lAOffset = cms.double( 0.0 ),
    lAWidthBPix = cms.double( 0.0 ),
    lAWidthFPix = cms.double( 0.0 ),
    doLorentzFromAlignment = cms.bool( False ),
    useLAFromDB = cms.bool( True ),
    xerr_barrel_l1 = cms.vdouble( 0.00115, 0.0012, 8.8E-4 ),
    yerr_barrel_l1 = cms.vdouble( 0.00375, 0.0023, 0.0025, 0.0025, 0.0023, 0.0023, 0.0021, 0.0021, 0.0024 ),
    xerr_barrel_ln = cms.vdouble( 0.00115, 0.0012, 8.8E-4 ),
    yerr_barrel_ln = cms.vdouble( 0.00375, 0.0023, 0.0025, 0.0025, 0.0023, 0.0023, 0.0021, 0.0021, 0.0024 ),
    xerr_endcap = cms.vdouble( 0.002, 0.002 ),
    yerr_endcap = cms.vdouble( 0.0021 ),
    xerr_barrel_l1_def = cms.double( 0.0103 ),
    yerr_barrel_l1_def = cms.double( 0.0021 ),
    xerr_barrel_ln_def = cms.double( 0.0103 ),
    yerr_barrel_ln_def = cms.double( 0.0021 ),
    xerr_endcap_def = cms.double( 0.002 ),
    yerr_endcap_def = cms.double( 7.5E-4 ),
    eff_charge_cut_highX = cms.double( 1.0 ),
    eff_charge_cut_highY = cms.double( 1.0 ),
    eff_charge_cut_lowX = cms.double( 0.0 ),
    eff_charge_cut_lowY = cms.double( 0.0 ),
    size_cutX = cms.double( 3.0 ),
    size_cutY = cms.double( 3.0 ),
    EdgeClusterErrorX = cms.double( 50.0 ),
    EdgeClusterErrorY = cms.double( 85.0 ),
    inflate_errors = cms.bool( False ),
    inflate_all_errors_no_trk_angle = cms.bool( False ),
    NoTemplateErrorsWhenNoTrkAngles = cms.bool( False ),
    UseErrorsFromTemplates = cms.bool( True ),
    TruncatePixelCharge = cms.bool( True ),
    IrradiationBiasCorrection = cms.bool( True ),
    DoCosmics = cms.bool( False ),
    isPhase2 = cms.bool( False ),
    SmallPitch = cms.bool( False ),
    ComponentName = cms.string( "hltESPPixelCPEGeneric" ),
    MagneticFieldRecord = cms.ESInputTag( "","" ),
    appendToDataLabel = cms.string( "" )
)

hltESPTTRHBWithTrackAngle = cms.ESProducer( 
    "TkTransientTrackingRecHitBuilderESProducer",
    ComponentName = cms.string( "hltESPTTRHBWithTrackAngle" ),
    ComputeCoarseLocalPositionFromDisk = cms.bool( False ),
    StripCPE = cms.string( "hltESPStripCPEfromTrackAngle" ),
    PixelCPE = cms.string( "hltESPPixelCPEGeneric" ),
    Matcher = cms.string( "StandardMatcher" ),
    Phase2StripCPE = cms.string( "" ),
    appendToDataLabel = cms.string( "" )
)

### Ingredient: MillePedeFileConverter
##from Alignment.CommonAlignmentProducer.MillePedeFileConverter_cfi import millePedeFileConverter
### We configure the input file name of the millePedeFileConverter
###         with the output file name of the alignmentProducer (=looper).
### Like this we are sure that they are well connected.
##SiPixelAliMillePedeFileConverter = millePedeFileConverter.clone(
##        fileDir = looper.algoConfig.fileDir,
##        binaryFile = looper.algoConfig.binaryFile,
##        )

SiPixelAliMillePedeFileConverterHLT = cms.EDProducer( 
    "MillePedeFileConverter",
    #FIXME: convert to untracked?
    fileDir = cms.string(SiPixelAliMilleAlignmentProducerHLT.algoConfig.fileDir.value()),
    inputBinaryFile = cms.string(SiPixelAliMilleAlignmentProducerHLT.algoConfig.binaryFile.value()),
    #FIXME: why was the label removed? Don't we want a label?
    fileBlobLabel = cms.string(''),
)



seqALCARECOPromptCalibProdSiPixelAli = cms.Sequence(
    ALCARECOTkAlMinBiasFilterForSiPixelAliHLT*
    LSNumberFilter*
    offlineBeamSpot*
    SiPixelAliHighPuritySelectorHLT*
    SiPixelAliTrackRefitterHLT0*
    SiPixelAliTrackerTrackHitFilterHLT*
    SiPixelAliTrackFitterHLT*
    SiPixelAliTrackSelectorHLT*
    SiPixelAliTrackRefitterHLT1*
    SiPixelAliMilleAlignmentProducerHLT*
    SiPixelAliMillePedeFileConverterHLT
)
