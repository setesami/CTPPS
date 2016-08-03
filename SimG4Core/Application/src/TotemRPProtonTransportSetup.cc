/****************************************************************************
*
* This is a part of the TOTEM offline software.
* Authors:
*    Hubert Niewiadomski
*    Jan Kašpar (jan.kaspar@gmail.com)
*
* $$RCSfile: TotemRPProtonTransportSetup.cc,v $: $
* $Revision: 1.5.2.3 $
* $Date: 2009/12/07 12:50:56 $
*
****************************************************************************/
#include "SimG4Core/Application/interface/TotemRPProtonTransportModel.h"
#include "SimG4Core/Application/interface/TotemRPProtonTransportSetup.h"

#include "SimG4Core/Geometry/interface/G4LogicalVolumeToDDLogicalPartMap.h" 
#include "DetectorDescription/Core/interface/DDName.h"
#include "DetectorDescription/Core/interface/DDMaterial.h"
#include "DetectorDescription/Core/interface/DDCurrentNamespace.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Electron.hh"
#include "G4FastSimulationManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"

#include <string>

#include "TFile.h"

#include "G4NistManager.hh"
#include "G4RegionStore.hh"
#include <stdlib.h>
#include <thread>

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4RegionStore.hh"
#include "G4Box.hh"
#include "CLHEP/Units/GlobalSystemOfUnits.h"
#include "CLHEP/Units/GlobalPhysicalConstants.h"

#include "G4FastSimulationManagerProcess.hh"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

using namespace edm;
using namespace std;

G4ThreadLocal TotemRPProtonTransportSetup* instance = 0;

TotemRPProtonTransportSetup::TotemRPProtonTransportSetup(const edm::ParameterSet &p) :
        parameters(p.getParameter<edm::ParameterSet>("BeamProtTransportSetup")),
        verbosity(parameters.getParameter<bool>("Verbosity")),
        model_root_file(parameters.getParameter<std::string>("ModelRootFile")),
        model_ip_150_r_name(parameters.getParameter<std::string>("Model_IP_150_R_Name")),
        model_ip_150_l_name(parameters.getParameter<std::string>("Model_IP_150_L_Name")),
        model_ip_150_r_zmin(parameters.getParameter<double>("Model_IP_150_R_Zmin")),
        model_ip_150_r_zmax(parameters.getParameter<double>("Model_IP_150_R_Zmax")),
        model_ip_150_l_zmin(parameters.getParameter<double>("Model_IP_150_L_Zmin")),
        model_ip_150_l_zmax(parameters.getParameter<double>("Model_IP_150_L_Zmax")) {
    if (instance)
        throw cms::Exception("TotemRPProtonTransportSetup") <<
              "TotemRPProtonTransportSetup is a singleton class and has already been initialized.\n";
    instance = this;

    FindLogicalVolumes();
    ReadParametrization();
    BuildTransportModels();
}

void TotemRPProtonTransportSetup::FindLogicalVolumes() {
    edm::LogInfo("TotemRPProtonTransportSetup") << "Proton parameterisation initialization begin !!";

    // find beampipes' logical volumes
    G4LogicalVolumeStore *theStore = G4LogicalVolumeStore::GetInstance();
    G4LogicalVolumeStore::const_iterator it;
    for (it = theStore->begin(); it != theStore->end(); it++) {
        G4LogicalVolume *v = *it;
        if (v->GetName() == Beam_IP_150_R_LV_Name)
            Beam_IP_150_R_LV = v;
        else if (v->GetName() == Beam_IP_150_L_LV_Name)
            Beam_IP_150_L_LV = v;
    }

    // fail if not found
    if (Beam_IP_150_R_LV == nullptr || Beam_IP_150_L_LV == nullptr) {
        edm::LogError("TotemRPProtonTransportSetup") <<
        "Logical volumes " << Beam_IP_150_R_LV_Name << "/" << Beam_IP_150_L_LV_Name <<
        "could not be found in G4LogicalVolumeStore";
        exit(1);
    }
    edm::LogInfo("TotemRPProtonTransportSetup") <<
    "Logical volumes found for" << Beam_IP_150_R_LV_Name << " " << Beam_IP_150_L_LV_Name <<
    ", pointers: " << aprox_ip_150_r << " " << aprox_ip_150_l << " ";
}

void TotemRPProtonTransportSetup::ReadParametrization() {
    // open optics .root file
    char *cmsswPath = getenv("CMSSW_BASE");
    std::string fileName = std::string(cmsswPath) + std::string("/src/") + model_root_file;
    TFile *f = TFile::Open(fileName.c_str(), "read");
    if (!f) {
        edm::LogError("TotemRPProtonTransportSetup") << "File " << fileName << " not found. Exiting.";
        exit(1);
    }
    edm::LogInfo("TotemRPProtonTransportSetup") << "Root file opened, pointer:" << f;

    // reaad parametrization
    aprox_ip_150_r = (LHCOpticsApproximator *) f->Get(model_ip_150_r_name.c_str());
    aprox_ip_150_l = (LHCOpticsApproximator *) f->Get(model_ip_150_l_name.c_str());
    f->Close();

    // fail if not found
    if (aprox_ip_150_r == nullptr || aprox_ip_150_l == nullptr) {
        edm::LogError("TotemRPProtonTransportSetup") <<
        "Parameterisation " << model_ip_150_r_name << "/" << model_ip_150_l_name <<
        " missing in file " << fileName;
        exit(1);
    }
    edm::LogInfo("TotemRPProtonTransportSetup") <<
    "Parameterizations read from file, pointers:" << aprox_ip_150_r << " " << aprox_ip_150_l << " ";
}

void TotemRPProtonTransportSetup::BuildTransportModels() {
    G4RegionStore *regionStore = G4RegionStore::GetInstance();
    G4ProductionCuts *dummyPC = new G4ProductionCuts();

    G4Region *region_ip_150_r = regionStore->FindOrCreateRegion(Beam_IP_150_R_LV_Name);
    Beam_IP_150_R_LV->SetRegion(region_ip_150_r);
    region_ip_150_r->AddRootLogicalVolume(Beam_IP_150_R_LV);
    region_ip_150_r->SetProductionCuts(dummyPC);
    model_ip_150_r = new TotemRPProtonTransportModel(Beam_IP_150_R_LV_Name,
                                                region_ip_150_r, *aprox_ip_150_r, model_ip_150_r_zmin,
                                                model_ip_150_r_zmax, verbosity);

    G4Region *region_ip_150_l = regionStore->FindOrCreateRegion(Beam_IP_150_L_LV_Name);
    region_ip_150_l->SetProductionCuts(dummyPC);
    Beam_IP_150_L_LV->SetRegion(region_ip_150_l);
    region_ip_150_l->AddRootLogicalVolume(Beam_IP_150_L_LV);
    model_ip_150_l = new TotemRPProtonTransportModel(Beam_IP_150_L_LV_Name,
                                                region_ip_150_l, *aprox_ip_150_l, model_ip_150_l_zmin,
                                                model_ip_150_l_zmax, verbosity);

    edm::LogInfo("TotemRPProtonTransportSetup") << "Fast transport models have been initialized.";
}

TotemRPProtonTransportSetup::~TotemRPProtonTransportSetup() {
    if (model_ip_150_r) delete model_ip_150_r;
    if (model_ip_150_l) delete model_ip_150_l;
}

void TotemRPProtonTransportSetup::UpdateSetup(const edm::EventSetup &es) { }
