/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors:
*   Jan Kašpar (jan.kaspar@gmail.com)
*
* $$RCSfile: OpticsInformation.cc,v $: $
* $Revision: 1.1.2.2 $
* $Date: 2009/11/09 12:14:28 $
*
****************************************************************************/

#include "FWCore/Framework/interface/MakerMacros.h"
//#include "FWCore/Framework/interface/ModuleFactory.h"
//#include "FWCore/Framework/interface/SourceFactory.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "TotemCondFormats/DataRecord/interface/BeamOpticsParamsRcd.h"
#include "TotemCondFormats/DataRecord/interface/ProtonTransportRcd.h"
#include "TotemCondFormats/BeamOpticsParamsObjects/interface/BeamOpticsParams.h"
#include "TotemCondFormats/ProtonTransportFunctions/interface/ProtonTransportFunctions.h"
#include "SimG4Core/TotemRPProtonTransportParametrization/interface/LHCOpticsApproximator.h"
#include "FWCore/Framework/interface/EventSetup.h"


class OpticsInformation : public edm::EDAnalyzer
{
  public:
    OpticsInformation(const edm::ParameterSet &);
  	~OpticsInformation() {}

  private:
    unsigned int detailsLevel;

    virtual void beginRun(edm::Run const&, edm::EventSetup const&);
    virtual void analyze(const edm::Event&, const edm::EventSetup&) {}
    virtual void endJob() {}
};



using namespace std;
using namespace edm;

//----------------------------------------------------------------------------------------------------

OpticsInformation::OpticsInformation(const edm::ParameterSet &ps) : 
  detailsLevel(ps.getUntrackedParameter<unsigned int>("detailsLevel", 0))
{
}

//----------------------------------------------------------------------------------------------------

void OpticsInformation::beginRun(edm::Run const&, edm::EventSetup const& es)
{	
  edm::ESHandle<BeamOpticsParams> optPar;
  es.get<BeamOpticsParamsRcd>().get(optPar);
  if(!optPar.isValid())
    throw cms::Exception("RPElasticReconstruction::beginJob") << " edm::ESHandle<BeamOpticsParams> is invalid";

  edm::ESHandle<ProtonTransportFunctions> optFun;
  es.get<ProtonTransportRcd>().get(optFun);
  if(!optFun.isValid())
	throw cms::Exception("RPElasticReconstruction::beginJob") << " edm::ESHandle<ProtonTransportFunctions> is invalid";

  printf("\n  ID & Cx (um)  & Lx (m)     & vx         & Cy (um)  & Ly (m)     & vy         & D          \\cr\\bln\n");

  const ProtonTransportFunctions::MapType& funs = optFun->GetFunctionMap();
  for (ProtonTransportFunctions::MapType::const_iterator it = funs.begin(); it != funs.end(); ++it) {
	double Cx = 0., Lx = 0., vx = 0., Cy = 0., Ly = 0., vy = 0., D = 0.;
	double in_null[5] = {optPar->GetBeamDisplacementX(), optPar->GetCrossingAngleX(), optPar->GetBeamDisplacementY(), optPar->GetCrossingAngleY(), 0.};
	it->second.real->GetLinearApproximation(in_null, Cx, Lx, vx, Cy, Ly, vy, D);

    printf("%3i  &  %6.1f  &  %8.3f  &  %8.3f  &  %6.1f  &  %8.3f  &  %8.3f  &  %8.3f  \\cr\\ln\n",
        it->first, Cx*1E6, Lx, vx, Cy*1E6, Ly, vy, D);
  }

  if (detailsLevel > 0)
    for (ProtonTransportFunctions::MapType::const_iterator it = funs.begin(); it != funs.end(); ++it) {
      printf("ID %u\n", it->first);

      TMatrixD M_x;
      it->second.real->GetLineariasedTransportMatrixX(optPar->GetBeamDisplacementX(), optPar->GetCrossingAngleX(),
        optPar->GetBeamDisplacementY(), optPar->GetCrossingAngleY(), 0., M_x, 1E-5, 1E-5);
      printf("\tM_x\n\t\t%+.2E\t%+.2E\n\t\t%+.2E\t%+.2E\n", M_x[0][0], M_x[0][1], M_x[1][0], M_x[1][1]);

      TMatrixD M_y;
      it->second.real->GetLineariasedTransportMatrixY(optPar->GetBeamDisplacementX(), optPar->GetCrossingAngleX(),
          optPar->GetBeamDisplacementY(), optPar->GetCrossingAngleY(), 0., M_y, 1E-5, 1E-5);
      printf("\tM_y\n\t\t%+.2E\t%+.2E\n\t\t%+.2E\t%+.2E\n", M_y[0][0], M_y[0][1], M_y[1][0], M_y[1][1]);
    }
}


DEFINE_FWK_MODULE(OpticsInformation);
