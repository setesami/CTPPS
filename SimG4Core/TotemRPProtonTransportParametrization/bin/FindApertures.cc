
#include "SimG4Core/TotemRPProtonTransportParametrization/interface/MADParamGenerator.h"
#include "SimG4Core/TotemRPProtonTransportParametrization/interface/LHCOpticsApproximator.h"
#include <iostream>

int main(int argc, char *args[])  //configuration file needed
{
  if(argc!=2)
  {
    std::cout<<"Configuration file needed!"<<std::endl;
    return 0;
  }

  MADParamGenerator mad_conf_gen;
  mad_conf_gen.OpenXMLConfigurationFile(args[1]);
  mad_conf_gen.IdentifyAperturesForAll();

  return 0;
}
