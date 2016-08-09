/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors:
*   Jan Kašpar (jan.kaspar@gmail.com)
*
* $$RCSfile: ProtonTransportRcd.h,v $: $
* $Revision: 1.1.2.1 $
* $Date: 2009/10/30 18:06:37 $
*
****************************************************************************/

#ifndef TotemCondFormatsDataRecordProtonTransportRcd_h
#define TotemCondFormatsDataRecordProtonTransportRcd_h

#include "boost/mpl/vector.hpp"
//#include "FWCore/Framework/interface/EventSetupRecordImplementation.h"
#include "FWCore/Framework/interface/DependentRecordImplementation.h"
#include "TotemCondFormats/DataRecord/interface/BeamOpticsParamsRcd.h"
#include "Geometry/TotemRecords/interface/RealGeometryRecord.h"
 
/**
 *\brief EventSetup record for proton transport data.
 **/

//class ProtonTransportRcd : public edm::eventsetup::EventSetupRecordImplementation<ProtonTransportRcd> {};
class ProtonTransportRcd : public edm::eventsetup::DependentRecordImplementation<ProtonTransportRcd, 
  boost::mpl::vector<BeamOpticsParamsRcd, RealGeometryRecord> >
{
};
 
#endif

