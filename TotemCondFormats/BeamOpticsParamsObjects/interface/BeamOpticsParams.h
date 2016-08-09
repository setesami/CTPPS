#ifndef ConfigurationTotemOpticsConfigurationBeamOpticsParams_h
#define ConfigurationTotemOpticsConfigurationBeamOpticsParams_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "HepMC/SimpleVector.h"
#include "TotemCondFormats/BeamOpticsParamsObjects/interface/RPRecoProtMADXVariables.h"
#include <string>

class BeamOpticsParams
{
  public:
    BeamOpticsParams(const edm::ParameterSet &p);
    BeamOpticsParams() {};
    virtual ~BeamOpticsParams(){};

    double GetBeamEnergy() const;  //GeV
    double GetBeamMomentum() const;  //GeV
    double GetNormalizedEmittanceX() const;  //mrad
    double GetNormalizedEmittanceY() const;  //mrad
    double GetBetaStarX() const;  //m
    double GetBetaStarY() const;  //m
    double GetBeamDivergenceX() const;  //rad
    double GetBeamDivergenceY() const;  //rad
    double GetBeamSizeX() const;  //m
    double GetBeamSizeY() const;  //m
    double GetBunchSizeZ() const;  //m
    double GetPrimVertSizeX() const;  //m
    double GetPrimVertSizeY() const;  //m
    double GetPrimVertSizeZ() const;  //m
    double GetCrossingAngleX() const;  //rad
    double GetCrossingAngleY() const;  //rad
    double GetProtonMass() const;  //GeV
    double GetLightSpeed() const;  //m/s
    double GetGamma() const;
    double GetBeamDisplacementX() const;  //m
    double GetBeamDisplacementY() const;  //m
    double GetBeamDisplacementZ() const;  //m
    double GetMeanXi() const;
    double GetSigmaXi() const;
    
    //GeV
    double IPSmearedProtonMomentumToXi(double px, double py, double pz) const;
    double IPNonSmearedProtonMomentumToXi(double px, double py, double pz) const;
    double IPSmearedProtonMomentumTot(double px, double py, double pz) const;
    double IPNonSmearedProtonMomentumTot(double px, double py, double pz) const;
    //Angles as used by madx, px/p_nominal, py/p_nominal, Xi=(p-p_nom)/p
    double CanonicalAnglesTot(double ThetaX, double ThetaY, double Xi) const;
    double ComputeSmearedProtonPhi(const HepMC::FourVector &ip_proton) const;
    double ComputeNonSmearedProtonPhi(const HepMC::FourVector &ip_proton) const;
    double ComputeCrossingAngleCorrectedThetaX(const HepMC::FourVector &ip_proton) const;
    double ComputeCrossingAngleCorrectedThetaY(const HepMC::FourVector &ip_proton) const;
    double MADXCanonicalVariablesTot(const RPRecoProtMADXVariables &prot) const;
    double MADXCanonicalVariablesToCrossingAngleCorrectedPhi(
        const RPRecoProtMADXVariables &prot) const;
    double MADXCanonicalVariablesToM(const RPRecoProtMADXVariables &protr, 
        const RPRecoProtMADXVariables &protl) const; 
    double MADXCanonicalVariablesToElasticPhysicsThx(
        const RPRecoProtMADXVariables &protr, const RPRecoProtMADXVariables &protl) const;
    double MADXCanonicalVariablesToElasticPhysicsThy(
        const RPRecoProtMADXVariables &protr, const RPRecoProtMADXVariables &protl) const;
    double MADXCanonicalVariablesToElasticPhysicsPhi(
        const RPRecoProtMADXVariables &protr, const RPRecoProtMADXVariables &protl) const;
    double MADXCanonicalVariablesToElasticPhysics_t(
        const RPRecoProtMADXVariables &protr, const RPRecoProtMADXVariables &protl) const;
        
    double HEPMCNonSmearedProtonstoM(const HepMC::FourVector &ip_protonr, 
        const HepMC::FourVector &ip_protonl) const;
    double HEPMCSmearedProtonstoM(const HepMC::FourVector &ip_protonr, 
        const HepMC::FourVector &ip_protonl) const;
    HepMC::FourVector LeftProtonMADXCanonicalVariablesToP(
        const RPRecoProtMADXVariables &protl) const;
    HepMC::FourVector RightProtonMADXCanonicalVariablesToP(
        const RPRecoProtMADXVariables &protr) const;
    HepMC::FourVector ProtonMADXCanonicalVariablesToP(
        const RPRecoProtMADXVariables &prot, double z_direction) const;
    double IPSmearedProtonMomentumToMADThetaX(const HepMC::FourVector &ip_proton) const;
    double IPSmearedProtonMomentumToMADThetaY(const HepMC::FourVector &ip_proton) const;
    inline double IPSmearedProtonMomentumToXi(const HepMC::FourVector &ip_proton) const
          {return IPSmearedProtonMomentumToXi(ip_proton.px(), ip_proton.py(), ip_proton.pz());}

    void GetStdRecoGeometryFile(std::string &fileName) const;
    void GetStdSimGeometryFile(std::string &fileName) const;

    double IPNonSmearedProtonMomentumToThetaX(const HepMC::FourVector &ip_proton);
    double IPNonSmearedProtonMomentumToThetaY(const HepMC::FourVector &ip_proton);

  private:
    double beam_energy_;
    double beam_momentum_;
    double normalized_emittance_x_;
    double normalized_emittance_y_;
    double beta_star_x_;
    double beta_star_y_;
    double beam_div_x_;
    double beam_div_y_;
    double beam_size_x_;
    double beam_size_y_;
    double bunch_size_z_;
    double crossing_angle_x_;
    double crossing_angle_y_;
    double proton_mass_; //[GeV/c^2]
    double light_speed_; //[m/s]
    double gamma_;
    double displ_x_;
    double displ_y_;
    double displ_z_;
    double mean_xi_;
    double sigma_xi_;
    
    double nominal_beam1_px_smeared_;
    double nominal_beam1_py_smeared_;
    double nominal_beam1_pz_smeared_;

	std::string GetTagString() const;
};

#endif
