/****************************************************************************
*
* This is a part of the TOTEM offline software.
* Authors:
*    Hubert Niewiadomski
*    Jan Kašpar (jan.kaspar@gmail.com)
*
* $$RCSfile: TotemRPProtonTransportSetup.h,v $: $
* $Revision: 1.1.1.1.6.2 $
* $Date: 2009/11/16 16:55:54 $
*
****************************************************************************/

#ifndef SimG4Core_Application_TotemRPProtonTransportSetup_h_
#define SimG4Core_Application_TotemRPProtonTransportSetup_h_

//#define G4V7

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimG4Core/Application/interface/TotemRPProtonTransportModel.h"
#include "FWCore/Framework/interface/ESWatcher.h"

#include "G4String.hh"
#include "G4ThreeVector.hh"

namespace edm {
    class EventSetup;

    class Run;
}

class ProtonTransportRcd;

/**
 * \brief Singleton class to build and maintain proton transport infrastructure.
 * Creates fast simulation model for fast proton transport (TotemRPProtonTransportModel)
 * for the following beam pipe segments:
 *  - Beam_IP_150_R
 *  - Beam_IP_150_L
 * defined in detector xml description
**/
class TotemRPProtonTransportSetup {
public:
    TotemRPProtonTransportSetup(edm::ParameterSet const &p);

    ~TotemRPProtonTransportSetup();

    void UpdateSetup(const edm::EventSetup &);

private:
    edm::ParameterSet parameters;
    bool verbosity;
    std::string model_root_file;
    std::string model_ip_150_r_name;
    std::string model_ip_150_l_name;

    // in m
    double model_ip_150_r_zmin;
    double model_ip_150_r_zmax;
    double model_ip_150_l_zmin;
    double model_ip_150_l_zmax;

    G4String Beam_IP_150_R_LV_Name = "Beam_IP_150_R";
    G4String Beam_IP_150_L_LV_Name = "Beam_IP_150_L";

    G4LogicalVolume *Beam_IP_150_R_LV = nullptr;
    G4LogicalVolume *Beam_IP_150_L_LV = nullptr;

    LHCOpticsApproximator *aprox_ip_150_r = nullptr;
    LHCOpticsApproximator *aprox_ip_150_l = nullptr;

    TotemRPProtonTransportModel *model_ip_150_r = nullptr;
    TotemRPProtonTransportModel *model_ip_150_l = nullptr;

    void FindLogicalVolumes();

    void ReadParametrization();

    void BuildTransportModels();
};


#endif  //SimG4Core_Application_TotemRPProtonTransportSetup_h_

