#include "TotemCondFormats/BeamOpticsParamsObjects/interface/BeamOpticsParams.h"
#include "TMath.h"
#include <iostream>
#include <sstream>

using namespace std;

BeamOpticsParams::BeamOpticsParams(const edm::ParameterSet &p)
{
  beam_energy_ = p.getParameter<double>("BeamEnergy"); //[GeV]
  proton_mass_ = p.getParameter<double>("ProtonMass");  //[Gev]
  beam_momentum_ = TMath::Sqrt(beam_energy_*beam_energy_ - proton_mass_*proton_mass_); //[GeV]
  light_speed_ = p.getParameter<double>("LightSpeed"); //[m/s]
  normalized_emittance_x_ = p.getParameter<double>("NormalizedEmittanceX");  //uradm
  normalized_emittance_y_ = p.getParameter<double>("NormalizedEmittanceY");  //uradm
  beta_star_x_ = p.getParameter<double>("BetaStarX");  //[m]
  beta_star_y_ = p.getParameter<double>("BetaStarY");  //[m]
  gamma_ = beam_energy_/proton_mass_;
  beam_div_x_ = TMath::Sqrt(normalized_emittance_x_/(gamma_*beta_star_x_));
  beam_div_y_ = TMath::Sqrt(normalized_emittance_y_/(gamma_*beta_star_y_));
  beam_size_x_ = TMath::Sqrt(normalized_emittance_x_*beta_star_x_/gamma_);
  beam_size_y_ = TMath::Sqrt(normalized_emittance_y_*beta_star_y_/gamma_);
  bunch_size_z_ = p.getParameter<double>("BunchSizeZ");  //[m]
  crossing_angle_x_ = p.getParameter<double>("CrossingAngleX");  //[rad]
  crossing_angle_y_ = p.getParameter<double>("CrossingAngleY");  //[rad]
  displ_x_ = p.getParameter<double>("BeamDisplacementX");  //[m]
  displ_y_ = p.getParameter<double>("BeamDisplacementY");  //[m]
  displ_z_ = p.getParameter<double>("BeamDisplacementZ");  //[m]
  mean_xi_ = p.getParameter<double>("MeanXi");  //
  sigma_xi_ = p.getParameter<double>("SigmaXi");  //
  
  nominal_beam1_px_smeared_ = beam_momentum_*TMath::Sin(crossing_angle_x_);
  nominal_beam1_py_smeared_ = beam_momentum_*TMath::Sin(crossing_angle_y_);
  double beam1_theta = TMath::Sqrt(crossing_angle_x_*crossing_angle_x_ + crossing_angle_y_*crossing_angle_y_);
  nominal_beam1_pz_smeared_ = beam_momentum_*TMath::Cos(beam1_theta);
}

double BeamOpticsParams::GetBeamEnergy() const
{
  return beam_energy_;
}

double BeamOpticsParams::GetBeamMomentum() const
{
  return beam_momentum_;
}

double BeamOpticsParams::GetNormalizedEmittanceX() const
{
  return normalized_emittance_x_;
}

double BeamOpticsParams::GetNormalizedEmittanceY() const
{
  return normalized_emittance_y_;
}

double BeamOpticsParams::GetBetaStarX() const
{
  return beta_star_x_;
}
double BeamOpticsParams::GetBetaStarY() const
{
  return beta_star_y_;
}

double BeamOpticsParams::GetBeamDivergenceX() const
{
  return beam_div_x_;
}

double BeamOpticsParams::GetBeamDivergenceY() const
{
  return beam_div_y_;
}

double BeamOpticsParams::GetBeamSizeX() const
{
  return beam_size_x_;
}

double BeamOpticsParams::GetBeamSizeY() const
{
  return beam_size_y_;
}

double BeamOpticsParams::GetBunchSizeZ() const
{
  return bunch_size_z_;
}

double BeamOpticsParams::GetPrimVertSizeX() const
{
  /// y crossing angle not taken into account
  return beam_size_x_ / sqrt(2.) / cos(crossing_angle_x_);
}

double BeamOpticsParams::GetPrimVertSizeY() const
{
  /// y crossing angle not taken into account
  return beam_size_y_ / sqrt(2.);
}

double BeamOpticsParams::GetPrimVertSizeZ() const
{
  /// y crossing angle not taken into account
  double c2 = cos(crossing_angle_x_);
  c2 = c2*c2;
  double s2 = 1. - c2;
  double denominator = sqrt(bunch_size_z_*bunch_size_z_*s2 + beam_size_x_*beam_size_x_*c2);
  if (denominator == 0.) return 0.;
  return bunch_size_z_ / sqrt(2.) * beam_size_x_ / denominator;
}

double BeamOpticsParams::GetCrossingAngleX() const
{
  return crossing_angle_x_;
}

double BeamOpticsParams::GetCrossingAngleY() const
{
  return crossing_angle_y_;
}

double BeamOpticsParams::GetProtonMass() const
{
  return proton_mass_; //[GeV/c^2]
}

double BeamOpticsParams::GetLightSpeed() const
{
  return light_speed_; //[m/s]
}

double BeamOpticsParams::GetGamma() const
{
  return gamma_;
}

double BeamOpticsParams::GetBeamDisplacementX() const
{
  return displ_x_;
}

double BeamOpticsParams::GetBeamDisplacementY() const
{
  return displ_y_;
}

double BeamOpticsParams::GetBeamDisplacementZ() const
{
  return displ_z_;
}

double BeamOpticsParams::GetMeanXi() const
{
  return mean_xi_;
}

double BeamOpticsParams::GetSigmaXi() const
{
  return sigma_xi_;
}

//px, py, pz assumed to appear as in the IP, crossing angle included
double BeamOpticsParams::IPSmearedProtonMomentumToXi(double px, double py, double pz) const //GeV
{
  double part_momentum = TMath::Sqrt(px*px + py*py + pz*pz);
  double xi = (part_momentum-beam_momentum_)/beam_momentum_;
  return xi;
}

//px, py, pz assumed to appear as in the IP, crossing angle included
double BeamOpticsParams::IPNonSmearedProtonMomentumToXi(double px, double py, double pz) const //GeV
{
  double part_momentum = TMath::Sqrt(px*px + py*py + pz*pz);
  double xi = (part_momentum-beam_momentum_)/beam_momentum_;
  return xi;
}

double BeamOpticsParams::IPSmearedProtonMomentumTot(double px, double py, double pz) const //GeV
{
  long double part_en = TMath::Sqrt(px*px + py*py + pz*pz + proton_mass_*proton_mass_);
  long double energy_diff = part_en - beam_energy_;
  long double px_diff = px - nominal_beam1_px_smeared_;
  long double py_diff = py - nominal_beam1_py_smeared_;
  long double pz_diff = TMath::Abs(pz) - nominal_beam1_pz_smeared_;
  
  long double t = (energy_diff*energy_diff - pz_diff*pz_diff) - px_diff*px_diff - py_diff*py_diff;
  return (double) t;
}

double BeamOpticsParams::IPNonSmearedProtonMomentumTot(double px, double py, double pz) const //GeV
{
  long double part_en = TMath::Sqrt(px*px + py*py + pz*pz + proton_mass_*proton_mass_);
  long double energy_diff = part_en - beam_energy_;
  long double px_diff = px;
  long double py_diff = py;
  long double pz_diff = TMath::Abs(pz) - beam_momentum_;
  
//  std::cout<<" px_diff="<<px_diff<<" py_diff="<<py_diff<<" pz_diff="<<pz_diff<<" energy_diff="<<energy_diff<<std::endl;
//  std::cout<<" px_diff^2="<<px_diff*px_diff
//    <<" py_diff^2="<<py_diff*py_diff<<" pz_diff^2="<<pz_diff*pz_diff
//    <<" energy_diff^2="<<energy_diff*energy_diff<<std::endl;
//  std::cout<<"t="<<energy_diff*energy_diff - pz_diff*pz_diff- px_diff*px_diff - py_diff*py_diff<<std::endl;
//  std::cout<<"t="<<energy_diff*energy_diff - px_diff*px_diff - py_diff*py_diff - pz_diff*pz_diff<<std::endl;
//  std::cout<<"energy_diff^2-pz_diff^2="<<energy_diff*energy_diff-pz_diff*pz_diff<<std::endl;
//  std::cout<<"proton_mass="<<proton_mass_<<std::endl;
  
  long double t = (energy_diff*energy_diff - pz_diff*pz_diff) - px_diff*px_diff - py_diff*py_diff;
  return (double)t;
}

//Canonical angles used by MADX are defined as px/p_nominal and py/p_nominal
//they are referred to the nominal beam momentum not the actual particle momentum
double BeamOpticsParams::CanonicalAnglesTot(double ThetaX, double ThetaY, double Xi) const //GeV
{
  long double px = ThetaX*beam_momentum_;
  long double py = ThetaY*beam_momentum_;
  long double part_momentum = beam_momentum_*(1.0+Xi);
  long double pz = TMath::Sqrt(part_momentum*part_momentum - px*px - py*py);
  
  return IPSmearedProtonMomentumTot((double) px, (double) py, (double) pz);
}

double BeamOpticsParams::ComputeSmearedProtonPhi(const HepMC::FourVector &ip_proton) const
{
  double rho = ip_proton.rho();
  double thx_comp_corrected = ip_proton.px()/rho - crossing_angle_x_;
  double thy_comp_corrected = ip_proton.py()/rho - crossing_angle_y_;
  return TMath::ATan2(thy_comp_corrected, thx_comp_corrected);
}

double BeamOpticsParams::ComputeNonSmearedProtonPhi(const HepMC::FourVector &ip_proton) const
{
  double rho = ip_proton.rho();
  double thx_comp_corrected = ip_proton.px()/rho;
  double thy_comp_corrected = ip_proton.py()/rho;
  return TMath::ATan2(thy_comp_corrected, thx_comp_corrected);
}

double BeamOpticsParams::IPNonSmearedProtonMomentumToThetaX(const HepMC::FourVector &ip_proton){
	double rho = ip_proton.rho();
	return ip_proton.px() / rho;
}

double BeamOpticsParams::IPNonSmearedProtonMomentumToThetaY(const HepMC::FourVector &ip_proton){
	double rho = ip_proton.rho();
	return ip_proton.py() / rho;
}

double BeamOpticsParams::ComputeCrossingAngleCorrectedThetaX(const HepMC::FourVector &ip_proton) const
{
  double rho = ip_proton.rho();
  double thx_comp_corrected = ip_proton.px()/rho - crossing_angle_x_;
  return thx_comp_corrected;
}

double BeamOpticsParams::ComputeCrossingAngleCorrectedThetaY(const HepMC::FourVector &ip_proton) const
{
  double rho = ip_proton.rho();
  double thy_comp_corrected = ip_proton.py()/rho - crossing_angle_y_;
  return thy_comp_corrected;
}

double BeamOpticsParams::MADXCanonicalVariablesTot(const RPRecoProtMADXVariables &prot) const
{
  return CanonicalAnglesTot(prot.ThetaX, prot.ThetaY, prot.Xi);
}

double BeamOpticsParams::MADXCanonicalVariablesToCrossingAngleCorrectedPhi(
    const RPRecoProtMADXVariables &prot) const
{
  double thx_comp_corrected = prot.ThetaX/(1+prot.Xi) - crossing_angle_x_;
  double thy_comp_corrected = prot.ThetaY/(1+prot.Xi) - crossing_angle_y_;
  return TMath::ATan2(thy_comp_corrected, thx_comp_corrected);
}

//perhaps at some point the angles need to be inluded and a full formula used
double BeamOpticsParams::MADXCanonicalVariablesToM(
    const RPRecoProtMADXVariables &protr, const RPRecoProtMADXVariables &protl) const
{
  return 2.0*beam_energy_*TMath::Sqrt(protr.Xi * protl.Xi); 
}

//right proton taken as a reference
//RPRecoProtMADXVariables always in beam direction
double BeamOpticsParams::MADXCanonicalVariablesToElasticPhysicsThx(
const RPRecoProtMADXVariables &protr, const RPRecoProtMADXVariables &protl) const
{
  return ((protr.ThetaX-crossing_angle_x_)-(protl.ThetaX-crossing_angle_x_))/2.0;
}

//right proton taken as a reference
//RPRecoProtMADXVariables always in beam direction
double BeamOpticsParams::MADXCanonicalVariablesToElasticPhysicsThy(
const RPRecoProtMADXVariables &protr, const RPRecoProtMADXVariables &protl) const
{
  return ((protr.ThetaY-crossing_angle_y_)-(protl.ThetaY-crossing_angle_y_))/2.0;
}

//right proton taken as a reference
//RPRecoProtMADXVariables always in beam direction
double BeamOpticsParams::MADXCanonicalVariablesToElasticPhysicsPhi(
const RPRecoProtMADXVariables &protr, const RPRecoProtMADXVariables &protl) const
{
  double thx = MADXCanonicalVariablesToElasticPhysicsThx(protr, protl);
  double thy = MADXCanonicalVariablesToElasticPhysicsThy(protr, protl);
  return TMath::ATan2(thy,thx);
}

//right proton taken as a reference
//RPRecoProtMADXVariables always in beam direction
double BeamOpticsParams::MADXCanonicalVariablesToElasticPhysics_t(
const RPRecoProtMADXVariables &protr, const RPRecoProtMADXVariables &protl) const
{
  double thx = MADXCanonicalVariablesToElasticPhysicsThx(protr, protl);
  double thy = MADXCanonicalVariablesToElasticPhysicsThy(protr, protl);
  double t = -beam_momentum_*beam_momentum_*(thx*thx+thy*thy);
  return t;
}


double BeamOpticsParams::HEPMCNonSmearedProtonstoM(const HepMC::FourVector &ip_protonr, 
    const HepMC::FourVector &ip_protonl) const
{
  double p_tot_r = ip_protonr.rho();
  double p_tot_l = ip_protonl.rho();
  double xir = (p_tot_r-beam_momentum_)/beam_momentum_;
  double xil = (p_tot_l-beam_momentum_)/beam_momentum_;
  return 2.0*beam_energy_*TMath::Sqrt(xir * xil); 
}

double BeamOpticsParams::HEPMCSmearedProtonstoM(const HepMC::FourVector &ip_protonr, 
    const HepMC::FourVector &ip_protonl) const
{
  double p_tot_r = ip_protonr.rho();
  double p_tot_l = ip_protonl.rho();
  double xir = (p_tot_r-beam_momentum_)/beam_momentum_;
  double xil = (p_tot_l-beam_momentum_)/beam_momentum_;
  return 2.0*beam_energy_*TMath::Sqrt(xir * xil); 
}

HepMC::FourVector BeamOpticsParams::LeftProtonMADXCanonicalVariablesToP(
    const RPRecoProtMADXVariables &protl) const
{
  long double px = protl.ThetaX*beam_momentum_;
  long double py = protl.ThetaY*beam_momentum_;
  long double tot_mom = (1.0+protl.Xi)*beam_momentum_; 
  double pz = -TMath::Sqrt(tot_mom*tot_mom - px*px - py*py);
  return HepMC::FourVector(px, py, pz);
}

HepMC::FourVector BeamOpticsParams::RightProtonMADXCanonicalVariablesToP(
    const RPRecoProtMADXVariables &protr) const
{
  long double px = protr.ThetaX*beam_momentum_;
  long double py = protr.ThetaY*beam_momentum_;
  long double tot_mom = (1.0+protr.Xi)*beam_momentum_; 
  double pz = TMath::Sqrt(tot_mom*tot_mom - px*px - py*py);
  return HepMC::FourVector(px, py, pz);
}

HepMC::FourVector BeamOpticsParams::ProtonMADXCanonicalVariablesToP(
    const RPRecoProtMADXVariables &prot, double z_direction) const
{
  long double px = prot.ThetaX*beam_momentum_;
  long double py = prot.ThetaY*beam_momentum_;
  long double tot_mom = (1.0+prot.Xi)*beam_momentum_; 
  double pz = z_direction/TMath::Abs(z_direction)*TMath::Sqrt(tot_mom*tot_mom - px*px - py*py);
  return HepMC::FourVector(px, py, pz);
}

double BeamOpticsParams::IPSmearedProtonMomentumToMADThetaX(
    const HepMC::FourVector &ip_proton) const
{
  return ip_proton.px()/beam_momentum_; 
}

double BeamOpticsParams::IPSmearedProtonMomentumToMADThetaY(
    const HepMC::FourVector &ip_proton) const
{
  return ip_proton.py()/beam_momentum_; 
}


std::string BeamOpticsParams::GetTagString() const
{
  stringstream ss;
  ss << beam_energy_ << "GeV_";
  if( (beta_star_x_ == 2) && (beam_energy_ == 3500)){
    ss << "2.0";
  } else {
    ss << beta_star_x_ << "";
  }

  string tag = ss.str();
  while (true) {
    size_t pos = tag.find('.');
    if (pos == string::npos)
      break;
    tag.replace(pos, 1, "p");
  }

  return tag;
}


void BeamOpticsParams::GetStdRecoGeometryFile(std::string &fileName) const
{
  char *cmsswPath = getenv("CMSSW_BASE");
  fileName = std::string(cmsswPath) + "/src/Geometry/TotemRPOptics/data/parametrization_" + GetTagString() + "_reco.root";
}


void BeamOpticsParams::GetStdSimGeometryFile(std::string &fileName) const
{
  char *cmsswPath = getenv("CMSSW_BASE");
  fileName = std::string(cmsswPath) + "/src/Geometry/TotemRPOptics/data/parametrization_" + GetTagString() + "_transp.root";
}

