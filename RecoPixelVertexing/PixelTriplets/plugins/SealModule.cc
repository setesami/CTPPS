#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"


#include "RecoPixelVertexing/PixelTriplets/interface/HitTripletGeneratorFromPairAndLayers.h"
#include "RecoPixelVertexing/PixelTriplets/interface/HitTripletGeneratorFromPairAndLayersFactory.h"
#include "RecoPixelVertexing/PixelTriplets/src/PixelTripletHLTGenerator.h"
DEFINE_EDM_PLUGIN(HitTripletGeneratorFromPairAndLayersFactory, PixelTripletHLTGenerator, "PixelTripletHLTGenerator");

#include "RecoPixelVertexing/PixelTriplets/src/PixelTripletLargeTipGenerator.h"
DEFINE_EDM_PLUGIN(HitTripletGeneratorFromPairAndLayersFactory, PixelTripletLargeTipGenerator, "PixelTripletLargeTipGenerator");

#include "RecoPixelVertexing/PixelTriplets/src/PixelTripletNoTipGenerator.h"
DEFINE_EDM_PLUGIN(HitTripletGeneratorFromPairAndLayersFactory,PixelTripletNoTipGenerator,"PixelTripletNoTipGenerator"); 

#include "RecoTracker/TkTrackingRegions/interface/OrderedHitsGeneratorFactory.h"
#include "RecoTracker/TkTrackingRegions/interface/OrderedHitsGenerator.h"
#include "RecoPixelVertexing/PixelTriplets/interface/CombinedHitTripletGenerator.h"
DEFINE_EDM_PLUGIN(OrderedHitsGeneratorFactory, CombinedHitTripletGenerator, "StandardHitTripletGenerator");
