#ifndef MixingWorkerBase_h
#define MixingWorkerBase_h

/** \class MixingWorkerBase
 *
 * MixingWorkerBase is an auxiliary class for the MixingModule
 *
 * \author Ursula Berthon, LLR Palaiseau
 *
 * \version   1st Version JMarch 2008

 *
 ************************************************************/

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Principal.h"
#include "FWCore/Framework/interface/Selector.h"
#include "Mixing/Base/interface/PileUp.h"
#include "DataFormats/Provenance/interface/EventID.h"

namespace edm
{
  class MixingModule;

  class MixingWorkerBase 
    {
    public:

      /** standard constructor*/
      explicit MixingWorkerBase():
	minBunch_(-5), 
	maxBunch_(3),
	bunchSpace_(75),
	subdet_(std::string(" ")),
	label_(std::string(" ")),
	labelCF_(std::string(" ")),
	maxNbSources_(5),
	mixProdStep2_(false)
 	{
	  tag_=InputTag();
	  tagSignal_=InputTag();
	}
  
      /*Normal constructor*/ 
      MixingWorkerBase(int minBunch,int maxBunch,int bunchSpace,std::string &subdet, std::string& label,std::string& labelCF, unsigned int maxNbSources, InputTag &tag, InputTag &tagCF ,bool mixProdStep2);

      /**Default destructor*/
      virtual ~MixingWorkerBase();
      virtual void put(edm::Event &e) =0;
      virtual void createnewEDProduct()=0; 
      virtual bool checkSignal(const edm::Event &e)=0;
      virtual void addSignals(const edm::Event &e) =0;
      virtual void addPileups(const int bcr, EventPrincipal *,unsigned int EventNr,int vertexOffset=0)=0;
      virtual void setBcrOffset()=0;
      virtual void setSourceOffset(const unsigned int s)=0;
      virtual void setTof()=0;
           
    protected:
      int const minBunch_;
      int const maxBunch_;
      int const bunchSpace_;
      std::string const subdet_;
      std::string const label_;
      std::string const labelCF_;
      unsigned int const maxNbSources_;
      InputTag tag_;
      InputTag tagSignal_;
      bool mixProdStep2_;
 
    private:
      unsigned int eventNr_;

    };
}//edm

#endif
