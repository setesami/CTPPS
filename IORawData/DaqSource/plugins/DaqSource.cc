/** \file 
 *
 *  $Date: 2011/02/14 15:29:39 $
 *  $Revision: 1.47 $
 *  \author N. Amapane - S. Argiro'
 */

#include "DaqSource.h"

#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "EventFilter/FEDInterface/interface/GlobalEventNumber.h"

#include "IORawData/DaqSource/interface/DaqBaseReader.h"
#include "IORawData/DaqSource/interface/DaqReaderPluginFactory.h"

#include "DataFormats/Provenance/interface/Timestamp.h" 
#include "FWCore/Framework/interface/LuminosityBlockPrincipal.h"
#include "FWCore/Framework/interface/EventPrincipal.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Provenance/interface/EventAuxiliary.h"
#include "DataFormats/Provenance/interface/LuminosityBlockAuxiliary.h"
#include "DataFormats/Provenance/interface/RunAuxiliary.h"
#include "DataFormats/Provenance/interface/EventID.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <string>
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>

#include "xgi/Method.h"
#include "xgi/Utils.h"

#include "cgicc/Cgicc.h"
#include "cgicc/FormEntry.h"
#include "cgicc/HTMLClasses.h"

#include "boost/tokenizer.hpp"


////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////



namespace edm {
 namespace daqsource{
  static unsigned int gtpEvmId_ =  FEDNumbering::MINTriggerGTPFEDID;
  static unsigned int gtpeId_ =  FEDNumbering::MINTriggerEGTPFEDID;
 }

  //______________________________________________________________________________
  DaqSource::DaqSource(const ParameterSet& pset, 
		     const InputSourceDescription& desc) 
    : InputSource(pset,desc)
    , evf::ModuleWeb("DaqSource")
    , reader_(0)
    , lumiSegmentSizeInEvents_(pset.getUntrackedParameter<unsigned int>("evtsPerLS",0))
    , useEventCounter_(pset.getUntrackedParameter<bool>("useEventCounter",false))
    , eventCounter_(0)
    , keepUsingPsidFromTrigger_(pset.getUntrackedParameter<bool>("keepUsingPsidFromTrigger",false))
    , fakeLSid_(lumiSegmentSizeInEvents_ != 0)
    , runNumber_(RunID::firstValidRun().run())
    , luminosityBlockNumber_(LuminosityBlockID::firstValidLuminosityBlock().luminosityBlock())
    , noMoreEvents_(false)
    , newRun_(true)
    , newLumi_(true)
    , eventCached_(false)
    , alignLsToLast_(false)
    , is_(0)
    , mis_(0)
    , thisEventLSid(0)
    , goToStopping(false)
  {
    count = 0;
    pthread_mutex_init(&mutex_,0);
    pthread_mutex_init(&signal_lock_,0);
    pthread_cond_init(&cond_,0);
    produces<FEDRawDataCollection>();
    setTimestamp(Timestamp::beginOfTime());
    
    // Instantiate the requested data source
    std::string reader = pset.getUntrackedParameter<std::string>("readerPluginName");
    
    try{
      reader_=
        DaqReaderPluginFactory::get()->create(reader,
  					    pset.getUntrackedParameter<ParameterSet>("readerPset"));
      reader_->setRunNumber(runNumber_);
    }
    catch(edm::Exception &e) {
      if(e.category() == "Configuration" && reader_ == 0) {
  	reader_ = DaqReaderPluginFactoryU::get()->create(reader);
  	if(reader_ == 0) throw;
	else reader_->setRunNumber(runNumber_);
      }
      else {
        throw;
      }
    }
  }
  
  //______________________________________________________________________________
  DaqSource::~DaqSource() {
    delete reader_;
  }
  
  
  ////////////////////////////////////////////////////////////////////////////////
  // implementation of member functions
  ////////////////////////////////////////////////////////////////////////////////
  
  //______________________________________________________________________________
  InputSource::ItemType 
  DaqSource::getNextItemType() {
    //    std::cout << getpid() << " enter getNextItemType " << std::endl;
    if(goToStopping){noMoreEvents_ = true; goToStopping=false;}
    if (noMoreEvents_) {
      pthread_mutex_lock(&mutex_);
      pthread_cond_signal(&cond_);
      pthread_mutex_unlock(&mutex_);
      return IsStop;
    }
    if (newRun_) {
      return IsRun;
    }
    if (newLumi_ && luminosityBlockAuxiliary()) {
      //      std::cout << "newLumi & lumiblock valid " << std::endl;
      return IsLumi;
    }
    if (alignLsToLast_) { //here we are recovering from a gap in Ls number so an event may already be cached but 
      // we hold onto it until we have issued all the necessary endLumi/beginLumi
//       std::cout << getpid() << "alignLsToLast was set and ls number is " 
// 		<< luminosityBlockNumber_ << " before signaling" << std::endl;
      signalWaitingThreadAndBlock();
      luminosityBlockNumber_++;
//       std::cout << getpid() << "alignLsToLast signaled and incremented " 
// 		<< luminosityBlockNumber_ << " eventcached " 
// 		<< eventCached_ << std::endl;
      newLumi_ = true;
      lumiSectionIndex_->value_ = luminosityBlockNumber_;
      resetLuminosityBlockAuxiliary();
      if(luminosityBlockNumber_ == thisEventLSid+1) 
	{
	  alignLsToLast_ = false;
	}
      if (!luminosityBlockAuxiliary() || luminosityBlockAuxiliary()->luminosityBlock() != luminosityBlockNumber_) {
	setLuminosityBlockAuxiliary(new LuminosityBlockAuxiliary(
								 runNumber_, luminosityBlockNumber_, timestamp(), Timestamp::invalidTimestamp()));
	
	readAndCacheLumi();
	setLumiPrematurelyRead();
	//      std::cout << "nextItemType: dealt with new lumi block principal, retval is " << retval << std::endl;
      }
      return IsLumi;
    }
    if (eventCached_) {
      //      std::cout << "read event already cached " << std::endl;
      return IsEvent;
    }
    if(reader_ == 0) {
      throw edm::Exception(errors::LogicError)
        << "DaqSource is used without a reader. Check your configuration !";
    }
    EventID eventId;
    TimeValue_t time = 0LL;
    timeval stv;
    gettimeofday(&stv,0);
    time = stv.tv_sec;
    time = (time << 32) + stv.tv_usec;
    Timestamp tstamp(time);

    int bunchCrossing = EventAuxiliary::invalidBunchXing;
    int orbitNumber   = EventAuxiliary::invalidBunchXing;
    
    // pass a 0 pointer to fillRawData()!
    FEDRawDataCollection* fedCollection(0);

    edm::EventAuxiliary::ExperimentType evttype = EventAuxiliary::Undefined;
  
    // let reader_ fill the fedCollection 
    int retval = reader_->fillRawData(eventId, tstamp, fedCollection);
    if(retval==0) {
      // fillRawData() failed, clean up the fedCollection in case it was allocated!
      if (0 != fedCollection) delete fedCollection;
      noMoreEvents_ = true;
      pthread_mutex_lock(&mutex_);
      pthread_cond_signal(&cond_);
      pthread_mutex_unlock(&mutex_);
      return IsStop;
    }
    else if(retval<0)
      {
 
	unsigned int nextLsFromSignal = (-1)*retval+1;
// 	std::cout << getpid() << "::got end-of-lumi for " << (-1)*retval
// 		  << " was " << luminosityBlockNumber_ << std::endl;
	if(luminosityBlockNumber_ < nextLsFromSignal)
	  {
	    if(lsToBeRecovered_->value_){
// 	      std::cout << getpid() << "eol::recover ls::for " << (-1)*retval << std::endl;
	      signalWaitingThreadAndBlock();
	      luminosityBlockNumber_++;
	      newLumi_ = true;
	      lumiSectionIndex_->value_ = luminosityBlockNumber_;
	      resetLuminosityBlockAuxiliary();
	      thisEventLSid = nextLsFromSignal - 1;
	      if(luminosityBlockNumber_ != thisEventLSid+1) 
		alignLsToLast_ = true;
	      //	      std::cout << getpid() << "eol::::alignLsToLast_ " << alignLsToLast_ << std::endl;
	    }
	    else{
	      //	      std::cout << getpid() << "eol::realign ls::for " << (-1)*retval << std::endl;
	      luminosityBlockNumber_ = nextLsFromSignal;
	      newLumi_ = true;
	      lumiSectionIndex_->value_ = luminosityBlockNumber_;
	      resetLuminosityBlockAuxiliary();
	    }
	  }
	//	else
	//	  std::cout << getpid() << "::skipping end-of-lumi for " << (-1)*retval << std::endl;
      }
    else
      {
	if (eventId.event() == 0) {
	  throw edm::Exception(errors::LogicError)
	    << "The reader used with DaqSource has returned an invalid (zero) event number!\n"
	    << "Event numbers must begin at 1, not 0.";
	}
	EventSourceSentry(*this);
	setTimestamp(tstamp);
    
	unsigned char *gtpFedAddr = fedCollection->FEDData(daqsource::gtpEvmId_).size()!=0 ? fedCollection->FEDData(daqsource::gtpEvmId_).data() : 0;
	uint32_t gtpsize = 0;
	if(gtpFedAddr !=0) gtpsize = fedCollection->FEDData(daqsource::gtpEvmId_).size();
	unsigned char *gtpeFedAddr = fedCollection->FEDData(daqsource::gtpeId_).size()!=0 ? fedCollection->FEDData(daqsource::gtpeId_).data() : 0; 

	unsigned int nextFakeLs	= 0;
	eventCounter_++;
	if(fakeLSid_ && luminosityBlockNumber_ != 
	   (nextFakeLs = useEventCounter_ ? ((eventCounter_-1)/lumiSegmentSizeInEvents_ + 1) :
	    ((eventId.event() - 1)/lumiSegmentSizeInEvents_ + 1))) {
	  lastLumiPrescaleIndex_->value_ = prescaleSetIndex_->value_;
	  prescaleSetIndex_->value_ = 0; // since we do not know better but we want to be able to run
	  
	  if(luminosityBlockNumber_ == nextFakeLs-1)
	    signalWaitingThreadAndBlock();
	  luminosityBlockNumber_ = nextFakeLs;
	  thisEventLSid = nextFakeLs-1;
	  newLumi_ = true;
	  lumiSectionIndex_->value_ = luminosityBlockNumber_;
	  resetLuminosityBlockAuxiliary();
	  if(keepUsingPsidFromTrigger_ && 
	     gtpFedAddr!=0 && evf::evtn::evm_board_sense(gtpFedAddr,gtpsize)){
	    prescaleSetIndex_->value_  = (evf::evtn::getfdlpsc(gtpFedAddr) & 0xffff);
	  }	  
	}
	else if(!fakeLSid_){ 

	  if(gtpFedAddr!=0 && evf::evtn::evm_board_sense(gtpFedAddr,gtpsize)){
	    lastLumiPrescaleIndex_->value_ = prescaleSetIndex_->value_;
	    thisEventLSid = evf::evtn::getlbn(gtpFedAddr);
	    prescaleSetIndex_->value_  = (evf::evtn::getfdlpsc(gtpFedAddr) & 0xffff);
	    evttype =  edm::EventAuxiliary::ExperimentType(evf::evtn::getevtyp(gtpFedAddr));
	    if(luminosityBlockNumber_ != (thisEventLSid + 1)){
	      // we got here in a running process and some Ls might have been skipped so set the flag, 
	      // increase by one, check and if appropriate set the flag then continue
	      if(lsToBeRecovered_->value_){
		//		std::cout << getpid() << "eve::recover ls::for " << thisEventLSid << std::endl;
		signalWaitingThreadAndBlock();
		luminosityBlockNumber_++;
		newLumi_ = true;
		lumiSectionIndex_->value_ = luminosityBlockNumber_;
		resetLuminosityBlockAuxiliary();
		if(luminosityBlockNumber_ != thisEventLSid+1) alignLsToLast_ = true;
		//		std::cout << getpid() << "eve::::alignLsToLast_ " << alignLsToLast_ << std::endl;
	      }
	      else{ // we got here because the process was restarted. just realign the ls id and proceed with this event
		//		std::cout << getpid() << "eve::realign ls::for " << thisEventLSid << std::endl;
		luminosityBlockNumber_ = thisEventLSid + 1;
		newLumi_ = true;
		lumiSectionIndex_->value_ = luminosityBlockNumber_;
		resetLuminosityBlockAuxiliary();
		lsToBeRecovered_->value_ = true;
	      }
	    }
	  }
	  else if(gtpeFedAddr!=0 && evf::evtn::gtpe_board_sense(gtpeFedAddr)){
	    lastLumiPrescaleIndex_->value_ = prescaleSetIndex_->value_;
	    thisEventLSid = evf::evtn::gtpe_getlbn(gtpeFedAddr);
	    prescaleSetIndex_->value_ = 0; //waiting to get a PS index from gtpe
	    evttype =  edm::EventAuxiliary::PhysicsTrigger; 
	    if(luminosityBlockNumber_ != (thisEventLSid + 1)){
	      if(luminosityBlockNumber_ == thisEventLSid)
		signalWaitingThreadAndBlock();
	      luminosityBlockNumber_ = thisEventLSid + 1;
	      newLumi_ = true;
	      lumiSectionIndex_->value_ = luminosityBlockNumber_;
	      resetLuminosityBlockAuxiliary();
	    }
	  }
	}
	if(gtpFedAddr!=0 && evf::evtn::evm_board_sense(gtpFedAddr,gtpsize)){
	  bunchCrossing =  int(evf::evtn::getfdlbx(gtpFedAddr));
	  orbitNumber =  int(evf::evtn::getorbit(gtpFedAddr));
	  TimeValue_t time = evf::evtn::getgpshigh(gtpFedAddr);
	  time = (time << 32) + evf::evtn::getgpslow(gtpFedAddr);
	  Timestamp tstamp(time);
	  setTimestamp(tstamp);      
	}
	else if(gtpeFedAddr!=0 && evf::evtn::gtpe_board_sense(gtpeFedAddr)){
	  bunchCrossing =  int(evf::evtn::gtpe_getbx(gtpeFedAddr));
	  orbitNumber =  int(evf::evtn::gtpe_getorbit(gtpeFedAddr));
	}
      }    
          
    //    std::cout << "lumiblockaux = " << luminosityBlockAuxiliary() << std::endl;
    // If there is no luminosity block principal, make one.
    if (!luminosityBlockAuxiliary() || luminosityBlockAuxiliary()->luminosityBlock() != luminosityBlockNumber_) {
      newLumi_ = true;
      setLuminosityBlockAuxiliary(new LuminosityBlockAuxiliary(
	runNumber_, luminosityBlockNumber_, timestamp(), Timestamp::invalidTimestamp()));

      readAndCacheLumi();
      setLumiPrematurelyRead();
      //      std::cout << "nextItemType: dealt with new lumi block principal, retval is " << retval << std::endl;
    }
    //    std::cout << "here retval = " << retval << std::endl;
    if(retval<0){
      //      std::cout << getpid() << " returning from getnextitem because retval < 0 - IsLumi "
      //		<< IsLumi << std::endl;
      if(newLumi_) return IsLumi; else return getNextItemType();
    }

    // make a brand new event
    eventId = EventID(runNumber_,thisEventLSid+1, eventId.event());
    std::auto_ptr<EventAuxiliary> eventAux(
      new EventAuxiliary(eventId, processGUID(),
			 timestamp(),
			 true,
			 evttype,
			 bunchCrossing,
			 EventAuxiliary::invalidStoreNumber,
			 orbitNumber));
    eventPrincipalCache()->fillEventPrincipal(eventAux, luminosityBlockPrincipal());
    eventCached_ = true;
    
    // have fedCollection managed by a std::auto_ptr<>
    std::auto_ptr<FEDRawDataCollection> bare_product(fedCollection);

    std::auto_ptr<Event> e(new Event(*eventPrincipalCache(), moduleDescription()));
    // put the fed collection into the transient event store
    e->put(bare_product);
    // The commit is needed to complete the "put" transaction.
    e->commit_();
    if (newLumi_) {
      return IsLumi;
    }
    return IsEvent;
  }

  void
  DaqSource::setRun(RunNumber_t r) {
    assert(!eventCached_);
    reset();
    newRun_ = newLumi_ = true;
    runNumber_ = r;
    if (reader_) reader_->setRunNumber(runNumber_);
    noMoreEvents_ = false;
    resetLuminosityBlockAuxiliary();
  }

  boost::shared_ptr<RunAuxiliary>
  DaqSource::readRunAuxiliary_() {
    assert(newRun_);
    assert(!noMoreEvents_);
    newRun_ = false;
    return boost::shared_ptr<RunAuxiliary>(new RunAuxiliary(runNumber_, timestamp(), Timestamp::invalidTimestamp()));
  }

  boost::shared_ptr<LuminosityBlockAuxiliary>
  DaqSource::readLuminosityBlockAuxiliary_() {
    assert(!newRun_);
    assert(newLumi_);
    assert(!noMoreEvents_);
    assert(luminosityBlockAuxiliary());
    //assert(eventCached_); //the event may or may not be cached - rely on 
    // the call to getNextItemType to detect that.
    newLumi_ = false;
    return luminosityBlockAuxiliary();
  }

  EventPrincipal*
  DaqSource::readEvent_() {
    //    std::cout << "assert not newRun " << std::endl;
    assert(!newRun_);
    //    std::cout << "assert not newLumi " << std::endl;
    assert(!newLumi_);
    //    std::cout << "assert not noMoreEvents " << std::endl;
    assert(!noMoreEvents_);
    //    std::cout << "assert eventCached " << std::endl;
    assert(eventCached_);
    //    std::cout << "asserts done " << std::endl;
    eventCached_ = false;
    return eventPrincipalCache();
  }

  void
  DaqSource::setLumi(LuminosityBlockNumber_t) {
      throw edm::Exception(errors::LogicError,"DaqSource::setLumi(LuminosityBlockNumber_t lumiNumber)")
        << "The luminosity block number cannot be set externally for DaqSource.\n"
        << "Contact a Framework developer.\n";
  }

  EventPrincipal*
  DaqSource::readIt(EventID const&) {
      throw edm::Exception(errors::LogicError,"DaqSource::readIt(EventID const& eventID)")
        << "Random access read cannot be used for DaqSource.\n"
        << "Contact a Framework developer.\n";
  }

  void
  DaqSource::skip(int) {
      throw edm::Exception(errors::LogicError,"DaqSource::skip(int offset)")
        << "Random access skip cannot be used for DaqSource\n"
        << "Contact a Framework developer.\n";
  }

  void DaqSource::publish(xdata::InfoSpace *is)
  {
    is_ = is;
    lumiSectionIndex_      = (xdata::UnsignedInteger32*)is_->find("lumiSectionIndex");
    prescaleSetIndex_      = (xdata::UnsignedInteger32*)is_->find("prescaleSetIndex");
    lastLumiPrescaleIndex_ = (xdata::UnsignedInteger32*)is_->find("lastLumiPrescaleIndex");
    lsTimedOut_            = (xdata::Boolean*)is_->find("lsTimedOut");
    lsToBeRecovered_       = (xdata::Boolean*)is_->find("lsToBeRecovered");
  }
  void DaqSource::publishToXmas(xdata::InfoSpace *is)
  {
    mis_ = is;
  }

  void DaqSource::openBackDoor(unsigned int timeout_sec)
  {
    count++;
    if(count==2) throw;
    pthread_mutex_lock(&mutex_);
    pthread_mutex_unlock(&signal_lock_);
    timespec ts;
#if _POSIX_TIMERS > 0
    clock_gettime(CLOCK_REALTIME, &ts);
#else
    struct timeval tv; 
    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec + 0;
    ts.tv_nsec = 0;
#endif
    ts.tv_sec += timeout_sec;

    int rc = pthread_cond_timedwait(&cond_, &mutex_, &ts);
    if(rc == ETIMEDOUT) lsTimedOut_->value_ = true; 
  }
  
  void DaqSource::closeBackDoor()
  {
    count--;
    pthread_cond_signal(&cond_);
    pthread_mutex_unlock(&mutex_);
    pthread_mutex_lock(&signal_lock_);
    lsTimedOut_->value_ = false; 
  }

  void DaqSource::signalWaitingThreadAndBlock()
  {
    pthread_mutex_lock(&signal_lock_);
    pthread_mutex_lock(&mutex_);
    pthread_mutex_unlock(&signal_lock_);
    //    std::cout << getpid() << " DS::signal from evloop " << std::endl;
    pthread_cond_signal(&cond_);
    //    std::cout << getpid() << " DS::go to wait for scalers wl " << std::endl;
    pthread_cond_wait(&cond_, &mutex_);
    pthread_mutex_unlock(&mutex_);
    ::usleep(1000);//allow other thread to lock
  }  

  void DaqSource::defaultWebPage(xgi::Input *in, xgi::Output *out)
  {
      std::string path;
      std::string urn;
      std::string mname;
      std::string query;
      std::string original_referrer_;
      try 
	{
	  cgicc::Cgicc cgi(in);
	  if ( xgi::Utils::hasFormElement(cgi,"gotostopping") )
	    {
	      goToStopping=true;
	    }
	  if ( xgi::Utils::hasFormElement(cgi,"module") )
	    mname = xgi::Utils::getFormElement(cgi, "module")->getValue();
	  cgicc::CgiEnvironment cgie(in);
	  if(original_referrer_ == "")
	    original_referrer_ = cgie.getReferrer();
	  path = cgie.getPathInfo();
	  query = cgie.getQueryString();
	}
      catch (const std::exception & e) 
	{
	  // don't care if it did not work
	}

      using std::endl;
      *out << "<html>"                                                   << endl;
      *out << "<head>"                                                   << endl;


      *out << "<STYLE type=\"text/css\"> #T1 {border-width: 2px; border: solid blue; text-align: center} </STYLE> "                                      << endl; 
      *out << "<link type=\"text/css\" rel=\"stylesheet\"";
      *out << " href=\"/" <<  urn
	   << "/styles.css\"/>"                   << endl;

      *out << "<title>" << moduleName_
	   << " MAIN</title>"                                            << endl;

      *out << "</head>"                                                  << endl;
      *out << "<body onload=\"loadXMLDoc()\">"                           << endl;
      *out << "<table border=\"0\" width=\"100%\">"                      << endl;
      *out << "<tr>"                                                     << endl;
      *out << "  <td align=\"left\">"                                    << endl;
      *out << "    <img"                                                 << endl;
      *out << "     align=\"middle\""                                    << endl;
      *out << "     src=\"/evf/images/bugicon.jpg\""	                 << endl;
      *out << "     alt=\"main\""                                        << endl;
      *out << "     width=\"90\""                                        << endl;
      *out << "     height=\"64\""                                       << endl;
      *out << "     border=\"\"/>"                                       << endl;
      *out << "    <b>"                                                  << endl;
      *out <<             moduleName_                                    << endl;
      *out << "    </b>"                                                 << endl;
      *out << "  </td>"                                                  << endl;
      *out << "  <td width=\"32\">"                                      << endl;
      *out << "    <a href=\"/urn:xdaq-application:lid=3\">"             << endl;
      *out << "      <img"                                               << endl;
      *out << "       align=\"middle\""                                  << endl;
      *out << "       src=\"/hyperdaq/images/HyperDAQ.jpg\""             << endl;
      *out << "       alt=\"HyperDAQ\""                                  << endl;
      *out << "       width=\"32\""                                      << endl;
      *out << "       height=\"32\""                                     << endl;
      *out << "       border=\"\"/>"                                     << endl;
      *out << "    </a>"                                                 << endl;
      *out << "  </td>"                                                  << endl;
      *out << "  <td width=\"32\">"                                      << endl;
      *out << "  </td>"                                                  << endl;
      *out << "  <td width=\"32\">"                                      << endl;
      *out << "    <a href=\"" << original_referrer_  << "\">"           << endl;
      *out << "      <img"                                               << endl;
      *out << "       align=\"middle\""                                  << endl;
      *out << "       src=\"/evf/images/spoticon.jpg\""			 << endl;
      *out << "       alt=\"main\""                                      << endl;
      *out << "       width=\"32\""                                      << endl;
      *out << "       height=\"32\""                                     << endl;
      *out << "       border=\"\"/>"                                     << endl;
      *out << "    </a>"                                                 << endl;
      *out << "  </td>"                                                  << endl;
      *out << "</tr>"                                                    << endl;
      *out << "</table>"                                                 << endl;

      *out << "<hr/>"                                                    << endl;
  
      *out << cgicc::form().set("method","GET").set("action", path ) 
	   << std::endl;
      boost::char_separator<char> sep("&");
      boost::tokenizer<boost::char_separator<char> > tokens(query, sep);
      for (boost::tokenizer<boost::char_separator<char> >::iterator tok_iter = tokens.begin();
	   tok_iter != tokens.end(); ++tok_iter){
	size_t pos = (*tok_iter).find_first_of("=");
	if(pos != std::string::npos){
	  std::string first  = (*tok_iter).substr(0    ,                        pos);
	  std::string second = (*tok_iter).substr(pos+1, (*tok_iter).length()-pos-1);
	  *out << cgicc::input().set("type","hidden").set("name",first).set("value", second) 
	       << std::endl;
	}
      }

      *out << cgicc::input().set("type","hidden").set("name","gotostopping").set("value","true")
	   << std::endl;
      *out << cgicc::input().set("type","submit").set("value","Go To Stopping")  	     << std::endl;
      *out << cgicc::form()						   << std::endl;  

      *out << "</body>"                                                  << endl;
      *out << "</html>"                                                  << endl;
  }
}
