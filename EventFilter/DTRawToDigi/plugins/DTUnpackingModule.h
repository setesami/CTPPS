#ifndef DTRawToDigi_DTUnpackingModule_h
#define DTRawToDigi_DTUnpackingModule_h

/** \class DTUnpackingModule
 *  The unpacking module for DTs.
 *
 *  $Date: 2008/06/19 13:37:36 $
 *  $Revision: 1.5 $
 * \author N. Amapane - S. Argiro' - M. Zanetti
 */

#include <FWCore/Framework/interface/EDProducer.h>

#include <iostream>

class DTUnpacker;

class DTUnpackingModule: public edm::EDProducer {
 public:
  /// Constructor
  DTUnpackingModule(const edm::ParameterSet& pset);

  /// Destructor
  virtual ~DTUnpackingModule();
    
  /// Call the Unpackers and create the digis 
  void produce(edm::Event & e, const edm::EventSetup& c);


 private:

  DTUnpacker * unpacker;

  /// get the FED payload by type?
  bool fedbyType_;
  /// if not you need the label
  edm::InputTag inputLabel;
  /// do you want to use the standard DT FED ID's, i.e. [770-775]? (why the hell 6??)
  bool useStandardFEDid_;
  /// if not you need to set the range by hand
  int minFEDid_;
  int maxFEDid_;
  bool dqmOnly;
};

#endif
