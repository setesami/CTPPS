#include "DataFormats/ParticleFlowReco/interface/PFBlockElement.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementTrack.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementGsfTrack.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementBrem.h"     
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementCluster.h"


using namespace reco;

// int PFBlockElement::instanceCounter_ = 0;

// int PFBlockElement::instanceCounter() {
//   return instanceCounter_;
// }

void PFBlockElement::Dump(std::ostream& out, 
                          const char* pad) const {
  if(!out) return;
  out<<pad<<"base element";
}

std::ostream& reco::operator<<( std::ostream& out, 
                                const PFBlockElement& element ) {
  
  if(! out) return out;
  
  out<<"element "<<element.index()<<"- type "<<element.type_<<" ";
  
  try {
    switch(element.type_) {
    case PFBlockElement::TRACK:
      {
        const reco::PFBlockElementTrack& et =
          dynamic_cast<const reco::PFBlockElementTrack &>( element );
        et.Dump(out);
        if( et.trackType(PFBlockElement::T_FROM_DISP) ) out<<" from displaced;";
        if( et.trackType(PFBlockElement::T_TO_DISP) ) out<<" to displaced;";
	if( et.trackType(PFBlockElement::T_FROM_GAMMACONV) ) out<<" from gammaconv;";  
	if( et.trackType(PFBlockElement::T_FROM_V0) ) out<<" from v0 decay;";  
        break;
      }
    case PFBlockElement::ECAL:
    case PFBlockElement::HCAL:
    case PFBlockElement::HFEM:
    case PFBlockElement::HFHAD:
    case PFBlockElement::PS1:
    case PFBlockElement::PS2:    
      {
        const reco::PFBlockElementCluster& ec =
          dynamic_cast<const reco::PFBlockElementCluster &>( element );
        ec.Dump(out);
        break;
      }
    case PFBlockElement::GSF:
      {
	const reco::PFBlockElementGsfTrack& eg =
          dynamic_cast<const reco::PFBlockElementGsfTrack &>( element );
        eg.Dump(out);
	out<<" from gsf;";
	break;
      }
    case PFBlockElement::BREM:
      {
	const reco::PFBlockElementBrem& em =
          dynamic_cast<const reco::PFBlockElementBrem &>( element );
        em.Dump(out);
	out<<" from brem;";
	break;
      }
    default:
      out<<" unknown type"<<std::endl;
      break;
    }
  }
  catch( std::exception& err) {
    out<<err.what()<<std::endl;
  }
  
  return out;
}

