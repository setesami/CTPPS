#include "SimG4Core/TotemRPProtonTransportParametrization/interface/MADParamGenerator.h"
#include "TTimeStamp.h"
#include "TRandom.h"
#include "TNtupleD.h"

#include "SimG4Core/TotemRPProtonTransportParametrization/interface/FitData.h"

void MADParamGenerator::GenerateMADConfFile(const std::string &base_conf_file, const std::string &out_conf_file, const std::string &from_marker_name, double from_marker_s_pos, bool define_from, const std::string &to_marker_name, double to_marker_s_pos, bool define_to, int particles_number, bool aperture_limit, std::vector<std::string> scoring_planes, const std::string &beam)
{
  std::fstream input;
  input.open(base_conf_file.c_str(), std::ios::in);

  std::fstream output;
  output.open(out_conf_file.c_str(), std::ios::out);

  Conf_file_processing(input, output, from_marker_name, from_marker_s_pos, define_from, to_marker_name, to_marker_s_pos,
      define_to, particles_number, aperture_limit, scoring_planes, beam);
  input.close();
}


void MADParamGenerator::Conf_file_processing(std::fstream &base_conf_file, std::fstream & conf_file, const std::string &from_marker_name,
    double from_marker_s_pos, bool define_from, const std::string &to_marker_name, double to_marker_s_pos, bool define_to, int particles_number,
    bool aperture_limit, const std::vector<std::string> &scoring_planes, const std::string &beam)
{
  while(base_conf_file.good() && !base_conf_file.eof())
  {
    char curr_char;
    base_conf_file.get(curr_char);

    if(curr_char == '#')
    {
      std::string token = GetToken(base_conf_file);
      ProcessToken(conf_file, from_marker_name, from_marker_s_pos, define_from, to_marker_name, to_marker_s_pos, define_to,
          particles_number, aperture_limit, token, scoring_planes, beam);
    }
    else
    {
      conf_file << curr_char;
    }
  }
}


std::string MADParamGenerator::GetToken(std::fstream &base_conf_file)
{
  char character;

  char temp_token[512];
  base_conf_file.get(temp_token, 512, '#');
  if(base_conf_file.good() && !base_conf_file.eof())
    base_conf_file.get(character);

  return std::string(temp_token);
}



void MADParamGenerator::ProcessToken(std::fstream &conf_file, const std::string &from_marker_name, double from_marker_s_pos, bool define_from,
    const std::string & to_marker_name, double to_marker_s_pos, bool define_to, int particles_number, bool aperture_limit,
    const std::string &token, const std::vector<std::string> &scoring_planes, const std::string &beam)
{
  if(token == "header_placement")
  {
    conf_file << "! defines a macro to read initial coordinates" <<std::endl;
    conf_file << "getpart(nx): macro = {" <<std::endl;
    conf_file << " ex   = table(myevent,nx,trx);" <<std::endl;
    conf_file << " epx   = table(myevent,nx,trpx);" <<std::endl;
    conf_file << " ey   = table(myevent,nx,try);" <<std::endl;
    conf_file << " epy   = table(myevent,nx,trpy);" <<std::endl;
    conf_file << " et    = table(myevent,nx,tt);" <<std::endl;
    conf_file << " ept    = table(myevent,nx,tpt);" <<std::endl;
    conf_file << " value,ex,epx,ey,epy,et,ept;" <<std::endl;
    conf_file << "}" <<std::endl;
  }
  else if(token == "scoring_plane_definition")
  {
    //beginning marker definition
    if(define_from)
    {
      conf_file << "" << from_marker_name << " : marker;" <<std::endl;
      conf_file << "seqedit,sequence=lhcb1;" <<std::endl;
      conf_file << "install,element=" << from_marker_name << ",at=" << from_marker_s_pos << ",from=ip5;" <<std::endl;
      conf_file << "endedit;" <<std::endl;
    }

    //end marker definition
    if(define_to)
    {
      conf_file << "" << to_marker_name << " : marker;" <<std::endl;
      conf_file << "seqedit,sequence=lhcb1;" <<std::endl;
      conf_file << "install,element=" << to_marker_name << ",at=" << to_marker_s_pos << ",from=ip5;" <<std::endl;
      conf_file << "endedit;" <<std::endl;
    }
  }
  else if(token == "start_point")
  {
    conf_file << from_marker_name;
  }
  else if(token == "end_point")
  {
    conf_file << to_marker_name;
  }
  else if(token == "scoring_plane_placement")
  {
    conf_file << " observe,place=" << to_marker_name << ";" <<std::endl;
    for(unsigned int i=0; i<scoring_planes.size(); i++)
    {
      conf_file << " observe,place=" << scoring_planes[i] << ";" <<std::endl;
    }
  }
  else if(token == "import_particles")
  {
    conf_file << "readmytable,file=part.in,table=myevent;" << std::endl;
  }
  else if(token == "insert_particles")
  {
    conf_file << "! read in initial coordinates at set for tracking" << std::endl;
    conf_file << "   n=1;" << std::endl;
    conf_file << "   while ( n < " << particles_number << " + 1 ) {" << std::endl;
    conf_file << "      exec,getpart($n);" << std::endl;
    conf_file << "      start,x=ex,px=epx,y=ey,py=epy,t=et,pt=ept;" << std::endl;
    conf_file << "      n = n + 1;" << std::endl;
    conf_file << "   }" << std::endl;
  }
  else if(token == "output_mad_file")
  {
    conf_file << "track";
  }
  else if(token == "options")
  {
    if(aperture_limit)
      conf_file << ",aperture";
    if(aperture_limit)
      conf_file << ",recloss";
  }
  else if(token == "save_lost_particles")
  {
    if(aperture_limit)
      conf_file << "write,table=trackloss,file=\"trackloss\"" << std::endl;
  }
  else if(token == "beam_type")
  {
    if(beam == "lhcb1" || beam == "lhcb2")
      conf_file << beam;
    else
      conf_file << "lhcb1";
  }
  else if(token == "beam_bv")
  {
    if(beam == "lhcb1")
      conf_file << "+1";
    else if(beam == "lhcb2")
      conf_file << "-1";
    else
      conf_file << "+1";
  }
}


void MADParamGenerator::GenerateRandomSamples(int number_of_particles, double x_min, double x_max, double theta_x_min, double theta_x_max, double y_min, double y_max, double theta_y_min, double theta_y_max, double ksi_min, double ksi_max, const std::string &out_file_name)
{
  std::ofstream ofs(out_file_name.c_str());

  double x, theta_x, y, theta_y, ksi;

  TTimeStamp time;

  TRandom r( time.GetSec() + time.GetNanoSec() );

  Int_t i;

  ofs << "@ NAME             %07s \"PARTICLES\"" << std::endl;
  ofs << "@ TYPE             %04s \"USER\"" << std::endl;
  ofs << "@ TITLE            %34s \"EVENT\"" << std::endl;
  ofs << "@ ORIGIN           %19s \"MAD-X 3.00.03 Linux\"" << std::endl;
  ofs << "@ DATE             %08s \"22/02/06\"" << std::endl;
  ofs << "@ TIME             %08s \"11.11.11\"" << std::endl;

  ofs << "*   mken  trx      trpx       try     trpy       tt      tpt" << std::endl;
  ofs << "$   %s    %le      %le        %le     %le        %le     %le" << std::endl;

  for (i = 0; i < number_of_particles; i++)
  {
    x = r.Uniform(x_min, x_max);
    theta_x = r.Uniform(theta_x_min, theta_x_max);
    y = r.Uniform(y_min, y_max);
    theta_y = r.Uniform(theta_y_min, theta_y_max);
    ksi = r.Uniform(ksi_min, ksi_max);
    ofs.precision(25);

    ofs << "    \"" << i + 1 << "\" " << x << " " << theta_x << " " << y << " " << theta_y <<
        " 0.0 " << ksi << " " << std::endl;
  }
  ofs.close();
}


void MADParamGenerator::RunMAD(const std::string &conf_file)
{
  std::string cmd;
  cmd = "rm -f ./trackloss";
  system(cmd.c_str());
  cmd = std::string("madx < ") + conf_file + " >/dev/null";
  system(cmd.c_str());
}


int MADParamGenerator::AppendRootTree(std::string root_file_name, std::string out_prefix, std::string out_station, bool recloss, std::string lost_particles_tree_filename, const std::vector<std::string> &scoring_planes, bool compare_apert)
{
  FitData text2rootconverter;
  text2rootconverter.readIn("part.in");
  text2rootconverter.readOut("trackone", out_station.c_str());
  text2rootconverter.readAdditionalScoringPlanes("trackone", scoring_planes);

  int added_entries=0;

  if(!recloss || compare_apert)
  {
    TFile *f = TFile::Open(root_file_name.c_str(), "UPDATE");
    TTree *tree = (TTree*)f->Get("transport_samples");
    if(!tree)
    {
      tree = CreateSamplesTree(f, out_prefix, scoring_planes);
    }
    added_entries = text2rootconverter.AppendRootFile(tree, out_prefix);
    f->cd();
    tree->SetBranchStatus("*",1);  //enable all branches
    tree->Write(NULL, TObject::kOverwrite);

    if(compare_apert)
    {
      TTree *acc_acept_tree = (TTree*)f->Get("acc_acept_tree");
      if(!acc_acept_tree)
      {
        acc_acept_tree = CreateAccelAcceptTree(f);
      }
      text2rootconverter.AppendAcceleratorAcceptanceRootFile(acc_acept_tree);
      f->cd();
      acc_acept_tree->SetBranchStatus("*",1);  //enable all branches
      acc_acept_tree->Write(NULL, TObject::kOverwrite);
    }

    f->Close();
  }

  if(recloss && !compare_apert)
  {
    TFile *lost_particles_file = TFile::Open(lost_particles_tree_filename.c_str(), "UPDATE");
    TTree *lost_particles_tree = (TTree*) lost_particles_file->Get("lost_particles");
    if(!lost_particles_tree)
    {
       lost_particles_tree = CreateLostParticlesTree(lost_particles_file);
       lost_particles_tree->Print();
    }
    text2rootconverter.readLost("trackloss");
    added_entries = text2rootconverter.AppendLostParticlesRootFile(lost_particles_tree);
    lost_particles_tree->Write(NULL, TObject::kOverwrite);
    lost_particles_file->Close();
  }

  return added_entries;
}


Long64_t MADParamGenerator::GetNumberOfEntries(std::string root_file_name, std::string out_prefix)
{
  TFile *f = TFile::Open(root_file_name.c_str(), "read");
  if(!f->IsOpen())
    return 0;

  TTree *tree = (TTree*)f->Get("transport_samples");
  if(!tree)
    return 0;

  Long64_t entries = tree->GetEntries();
  f->Close();
  return entries;
}

Long64_t MADParamGenerator::GetLostParticlesEntries(const Parametisation_configuration &conf)
{
  TFile *f = TFile::Open(conf.lost_particles_tree_filename.c_str(), "read");
  if(!f->IsOpen())
    return 0;

  TTree *tree = (TTree*)f->Get("lost_particles");
  if(!tree)
    return 0;

  Long64_t entries = tree->GetEntries();
  f->Close();
  return entries;
}


TTree *MADParamGenerator::CreateSamplesTree(TFile *f, std::string out_prefix, const std::vector<std::string> &scoring_planes)
{
  std::string varlist;

  std::string x_in_lab = "x_in";
  std::string theta_x_in_lab = "theta_x_in";
  std::string y_in_lab = "y_in";
  std::string theta_y_in_lab = "theta_y_in";
  std::string ksi_in_lab = "ksi_in";
  std::string s_in_lab = "s_in";

  std::string x_out_lab = out_prefix + "_x_out";
  std::string theta_x_out_lab = out_prefix + "_theta_x_out";
  std::string y_out_lab = out_prefix + "_y_out";
  std::string theta_y_out_lab = out_prefix + "_theta_y_out";
  std::string ksi_out_lab = out_prefix + "_ksi_out";
  std::string s_out_lab = out_prefix + "_s_out";
  std::string valid_out_lab = out_prefix + "_valid_out";

  varlist = x_in_lab + ":" + theta_x_in_lab + ":" + y_in_lab + ":" + theta_y_in_lab + ":" + ksi_in_lab + ":" +
      s_in_lab + ":" + x_out_lab + ":" + theta_x_out_lab + ":" + y_out_lab + ":" + theta_y_out_lab + ":" +
      ksi_out_lab + ":" + s_out_lab + ":" + valid_out_lab;

  for(unsigned int i=0; i<scoring_planes.size(); i++)
  {
    varlist += std::string(":") + scoring_planes[i] + "_x_out";
    varlist += std::string(":") + scoring_planes[i] + "_theta_x_out";
    varlist += std::string(":") + scoring_planes[i] + "_y_out";
    varlist += std::string(":") + scoring_planes[i] + "_theta_y_out";
    varlist += std::string(":") + scoring_planes[i] + "_ksi_out";
    varlist += std::string(":") + scoring_planes[i] + "_s_out";
    varlist += std::string(":") + scoring_planes[i] + "_valid_out";
  }

  f->cd();
  return new TNtupleD("transport_samples", "transport_samples", varlist.c_str());
}


TTree *MADParamGenerator::CreateAccelAcceptTree(TFile *f, std::string name)
{
  std::string varlist = "x:theta_x:y:theta_y:ksi:mad_accept:par_accept";
  f->cd();
  return new TNtupleD(name.c_str(), name.c_str(), varlist.c_str());
}


TTree *MADParamGenerator::CreateLostParticlesTree(TFile *lost_particles_file)
{
  std::string varlist;

  std::string br_x_in_lab = "in_x";
  std::string br_theta_x_in_lab = "in_theta_x";
  std::string br_y_in_lab = "in_y";
  std::string br_theta_y_in_lab = "in_theta_y";
  std::string br_ksi_in_lab = "in_ksi";
  std::string br_s_in_lab = "in_s";

  std::string br_x_out_lab = "out_x";
  std::string br_theta_x_out_lab = "out_theta_x";
  std::string br_y_out_lab = "out_y";
  std::string br_theta_y_out_lab = "out_theta_y";
  std::string br_ksi_out_lab = "out_ksi";
  std::string br_s_out_lab = "out_s";

  std::string br_element_out_lab = "out_element";

  std::string x_in_lab = br_x_in_lab + "/D";
  std::string theta_x_in_lab = br_theta_x_in_lab + "/D";
  std::string y_in_lab = br_y_in_lab + "/D";
  std::string theta_y_in_lab = br_theta_y_in_lab + "/D";
  std::string ksi_in_lab = br_ksi_in_lab + "/D";
  std::string s_in_lab = br_s_in_lab + "/D";

  std::string x_out_lab = br_x_out_lab + "/D";
  std::string theta_x_out_lab = br_theta_x_out_lab + "/D";
  std::string y_out_lab = br_y_out_lab + "/D";
  std::string theta_y_out_lab = br_theta_y_out_lab + "/D";
  std::string ksi_out_lab = br_ksi_out_lab + "/D";
  std::string s_out_lab = br_s_out_lab + "/D";

  std::string element_out_lab = br_element_out_lab + "/C";

  double x_in, theta_x_in, y_in, theta_y_in, ksi_in, s_in;
  double x_out, theta_x_out, y_out, theta_y_out, ksi_out, s_out;
  char element_out[512];
  element_out[0] = 0;

  lost_particles_file->cd();
  TTree *lost_part_tree = new TTree("lost_particles", "lost_particles");

  lost_part_tree->Branch(br_x_in_lab.c_str(), &x_in, x_in_lab.c_str() );
  lost_part_tree->Branch(br_theta_x_in_lab.c_str(), &theta_x_in, theta_x_in_lab.c_str() );
  lost_part_tree->Branch(br_y_in_lab.c_str(), &y_in, y_in_lab.c_str() );
  lost_part_tree->Branch(br_theta_y_in_lab.c_str(), &theta_y_in, theta_y_in_lab.c_str() );
  lost_part_tree->Branch(br_ksi_in_lab.c_str(), &ksi_in, ksi_in_lab.c_str() );
  lost_part_tree->Branch(br_s_in_lab.c_str(), &s_in, s_in_lab.c_str() );

  lost_part_tree->Branch(br_x_out_lab.c_str(), &x_out, x_out_lab.c_str() );
  lost_part_tree->Branch(br_theta_x_out_lab.c_str(), &theta_x_out, theta_x_out_lab.c_str() );
  lost_part_tree->Branch(br_y_out_lab.c_str(), &y_out, y_out_lab.c_str() );
  lost_part_tree->Branch(br_theta_y_out_lab.c_str(), &theta_y_out, theta_y_out_lab.c_str() );
  lost_part_tree->Branch(br_ksi_out_lab.c_str(), &ksi_out, ksi_out_lab.c_str() );
  lost_part_tree->Branch(br_s_out_lab.c_str(), &s_out, s_out_lab.c_str() );
  lost_part_tree->Branch(br_element_out_lab.c_str(), element_out, element_out_lab.c_str() );

  lost_part_tree->Write(NULL, TObject::kOverwrite);
  return lost_part_tree;
}


void MADParamGenerator::ClearWorkingFiles(const Parametisation_configuration &conf, std::string sample_file_name)
{
  std::string cmd;
  cmd = "rm -f ./part.in ./trackone ./trackloss ./" + sample_file_name;
  system(cmd.c_str());
}


void MADParamGenerator::DeleteLostParticlesFiles(const Parametisation_configuration &conf)
{
  std::string cmd;
  cmd = "rm -f ./" + conf.lost_particles_tree_filename + " ./" + conf.lost_particles_hist_filename;
  system(cmd.c_str());
}


void MADParamGenerator::DeleteApertureTestFiles(const Parametisation_configuration &conf)
{
  std::string cmd;
  cmd = "rm -f ./" + conf.samples_aperture_test_file_name;
  system(cmd.c_str());
}


int MADParamGenerator::BuildSample(const Parametisation_configuration &conf, std::string sample_file_name, bool recloss, bool compare_apert)
{
  ClearWorkingFiles(conf, sample_file_name);
  if(recloss && !compare_apert)
    DeleteLostParticlesFiles(conf);

  if(compare_apert)
    DeleteApertureTestFiles(conf);

  MADParamGenerator mad_conf_gen;

  std::vector<std::string> aperture_markers;
  for(unsigned int i=0; i<conf.inter_planes.size(); i++)
    aperture_markers.push_back(conf.inter_planes[i].to_marker_name);

  mad_conf_gen.GenerateMADConfFile(conf.base_mad_conf_file, conf.processed_mad_conf_file, conf.from_marker_name,
        conf.from_marker_s_pos, conf.define_from, conf.to_marker_name, conf.to_marker_s_pos,
        conf.define_to, conf.number_of_part_per_sample, (conf.aperture_limit||recloss||compare_apert), aperture_markers, conf.beam);
  int total_generated_particles = 0;
  int current_iteration_particles = 0;

  std::cout << std::endl << "Generating random samples, from " << conf.from_marker_name << " to " << conf.to_marker_name << ", file " <<
  sample_file_name << std::endl;
  std::cout << "Number of inter-planes: " << conf.inter_planes.size() << std::endl;

  do
  {
    mad_conf_gen.GenerateRandomSamples(conf.number_of_part_per_sample, conf.x_min, conf.x_max, conf.theta_x_min,
      conf.theta_x_max, conf.y_min, conf.y_max, conf.theta_y_min,
      conf.theta_y_max, conf.ksi_min, conf.ksi_max, "part.in");
    mad_conf_gen.RunMAD(conf.processed_mad_conf_file);
    current_iteration_particles = mad_conf_gen.AppendRootTree(sample_file_name, conf.destination_branch_prefix,
        conf.to_marker_name, recloss&&!compare_apert, conf.lost_particles_tree_filename, aperture_markers, compare_apert);

    if(!recloss || compare_apert)
    {
      total_generated_particles = GetNumberOfEntries(sample_file_name, conf.destination_branch_prefix);
      std::cout << "Total number of particles arrived at " << conf.to_marker_name << " " << total_generated_particles << " of "
        << conf.tot_entries_number << std::endl;
    }
    else
    {
      total_generated_particles = GetLostParticlesEntries(conf);
      std::cout << "Total number of particles lost before " << conf.to_marker_name << " " << total_generated_particles << " of "
        << conf.tot_entries_number << std::endl;
    }
  }
  while(total_generated_particles < conf.tot_entries_number && current_iteration_particles>5 );

  if(!recloss)
  {
    PrintTreeInfo(conf, sample_file_name);
  }

  return total_generated_particles;
}


void MADParamGenerator::PrintTreeInfo(const Parametisation_configuration &conf, std::string sample_file_name)
{
  TFile *f = TFile::Open(sample_file_name.c_str(), "read");
  if(!f->IsOpen())
    return;

  TTree *tree = (TTree*)f->Get("transport_samples");
  if(!tree)
    return;

  tree->Print();
  f->Close();
}


TTree *MADParamGenerator::GetSamplesTree(const Parametisation_configuration &conf, std::string sample_file_name)
{
  TFile *f = TFile::Open(sample_file_name.c_str(), "read");
  if(!f->IsOpen())
    return NULL;

  TTree *tree = (TTree*)f->Get("transport_samples");
  if(!tree)
    return NULL;

  return tree;
}


TTree *MADParamGenerator::GetAccelAcceptTree(TFile *f)
{
  if(!f || !f->IsOpen())
    return NULL;

  TTree *tree = (TTree*)f->Get("acc_acept_tree");
  if(!tree)
    return NULL;

  return tree;
}


void MADParamGenerator::WriteAccelAcceptTree(TFile *f, TTree *acc_acept_tree)
{
  if(!f->IsOpen())
    return;

  f->cd();
  if(!acc_acept_tree)
    return;

  acc_acept_tree->Write(NULL, TObject::kOverwrite);
  acc_acept_tree->Print();
}


int MADParamGenerator::GenerateTrainingData(const Parametisation_configuration &conf)
{
  std::cout<<std::endl;
  std::cout<<"=======================================" << std::endl;
  std::cout<<"==== Training data being generated ====" << std::endl;
  std::cout<<"=======================================" << std::endl;
  std::cout<<std::endl;
  return BuildSample(conf, conf.samples_train_root_file_name);
}


int MADParamGenerator::GenerateTestingData(const Parametisation_configuration &conf)
{
  std::cout<<std::endl;
  std::cout<<"======================================" << std::endl;
  std::cout<<"==== Testing data being generated ====" << std::endl;
  std::cout<<"======================================" << std::endl;
  std::cout<<std::endl;
  return BuildSample(conf, conf.samples_test_root_file_name);
}


int MADParamGenerator::GenerateLostParticleData(const Parametisation_configuration &conf)
{
  std::cout<<std::endl;
  std::cout<<"============================================" << std::endl;
  std::cout<<"==== Lost particle data being generated ====" << std::endl;
  std::cout<<"============================================" << std::endl;
  std::cout<<std::endl;
  return BuildSample(conf, conf.samples_test_root_file_name, true);
}


int MADParamGenerator::GenerateApertureTestingData(const Parametisation_configuration &conf)
{
  std::cout<<std::endl;
  std::cout<<"==================================================" << std::endl;
  std::cout<<"==== Aperture model test data being generated ====" << std::endl;
  std::cout<<"==================================================" << std::endl;
  std::cout<<std::endl;
  return BuildSample(conf, conf.samples_aperture_test_file_name, false, true);
}


void MADParamGenerator::OpenXMLConfigurationFile(std::string file_name)
{
  xml_parser.read(file_name);
  xml_parser.setFilename(file_name);
  xml_parser.read();
}


Parametisation_configuration MADParamGenerator::GetParamConfiguration(int id)
{
  Parametisation_configuration conf;
  conf.base_mad_conf_file = xml_parser.get<std::string>(id, std::string("base_mad_conf_file") );
  conf.processed_mad_conf_file = xml_parser.get<std::string>(id, "processed_mad_conf_file");
  conf.beam = xml_parser.get<std::string>(id, "beam");
  conf.nominal_beam_energy = xml_parser.get<double>(id, "nominal_beam_energy");
  conf.from_marker_name = xml_parser.get<std::string>(id, "from_marker_name");
  conf.from_marker_s_pos = xml_parser.get<double>(id, "from_marker_s_pos");
  conf.define_from = xml_parser.get<int>(id, "define_from");
  conf.to_marker_name = xml_parser.get<std::string>(id, "to_marker_name");
  conf.to_marker_s_pos = xml_parser.get<double>(id, "to_marker_s_pos");
  conf.define_to = xml_parser.get<int>(id, "define_to");
  conf.aperture_limit = xml_parser.get<int>(id, "aperture_limit");
  conf.tot_entries_number = xml_parser.get<int>(id, "tot_entries_number");

  conf.number_of_part_per_sample = xml_parser.get<int>(id, "number_of_part_per_sample");
  conf.x_min = xml_parser.get<double>(id, "x_min");
  conf.x_max = xml_parser.get<double>(id, "x_max");
  conf.theta_x_min = xml_parser.get<double>(id, "theta_x_min");
  conf.theta_x_max = xml_parser.get<double>(id, "theta_x_max");
  conf.y_min = xml_parser.get<double>(id, "y_min");
  conf.y_max = xml_parser.get<double>(id, "y_max");
  conf.theta_y_min = xml_parser.get<double>(id, "theta_y_min");
  conf.theta_y_max = xml_parser.get<double>(id, "theta_y_max");
  conf.ksi_min = xml_parser.get<double>(id, "ksi_min");
  conf.ksi_max = xml_parser.get<double>(id, "ksi_max");

  conf.samples_train_root_file_name = xml_parser.get<std::string>(id, "samples_train_root_file_name");
  conf.samples_test_root_file_name = xml_parser.get<std::string>(id, "samples_test_root_file_name");
  conf.samples_aperture_test_file_name = xml_parser.get<std::string>(id, "samples_aperture_test_file_name");
  conf.destination_branch_prefix = xml_parser.get<std::string>(id, "destination_branch_prefix");

  std::string pol_type = xml_parser.get<std::string>(id, "polynomials_type");
  if(pol_type == "kMonomials")
    conf.polynomials_type = TMultiDimFet::kMonomials;
  else if(pol_type == "kChebyshev")
    conf.polynomials_type = TMultiDimFet::kChebyshev;
  else if(pol_type == "kLegendre")
    conf.polynomials_type = TMultiDimFet::kLegendre;
  else
    conf.polynomials_type = TMultiDimFet::kMonomials;

  std::string sel_mode = xml_parser.get<std::string>(id, "terms_selelection_mode");
  if(sel_mode == "AUTOMATIC")
    conf.terms_selelection_mode = LHCOpticsApproximator::AUTOMATIC;
  else if(sel_mode == "PREDEFINED")
    conf.terms_selelection_mode = LHCOpticsApproximator::PREDEFINED;
  else
    conf.terms_selelection_mode = LHCOpticsApproximator::AUTOMATIC;

  conf.max_degree_x = xml_parser.get<int>(id, "max_degree_x");
  conf.max_degree_tx = xml_parser.get<int>(id, "max_degree_tx");
  conf.max_degree_y = xml_parser.get<int>(id, "max_degree_y");
  conf.max_degree_ty = xml_parser.get<int>(id, "max_degree_ty");
  conf.common_terms = xml_parser.get<int>(id, "common_terms");

  conf.precision_x = xml_parser.get<double>(id, "precision_x");
  conf.precision_tx = xml_parser.get<double>(id, "precision_tx");
  conf.precision_y = xml_parser.get<double>(id, "precision_y");
  conf.precision_ty = xml_parser.get<double>(id, "precision_ty");

  conf.approximation_error_histogram_file = xml_parser.get<std::string>(id, "approximation_error_histogram_file");

  conf.lost_particles_tree_filename = xml_parser.get<std::string>(id, "lost_particles_tree_filename");
  conf.lost_particles_hist_filename = xml_parser.get<std::string>(id, "lost_particles_hist_filename");

  conf.optics_parametrisation_file = xml_parser.get<std::string>(id, "optics_parametrisation_file");
  conf.optics_parametrisation_name = xml_parser.get<std::string>(id, "optics_parametrisation_name");

  std::vector<int> ids = xml_parser.getIds(id);
  for(unsigned int i=0; i<ids.size(); i++)
    conf.inter_planes.push_back( GetApertureConfiguration(id, ids[i]) );

  return conf;
}


Parametisation_aperture_configuration MADParamGenerator::GetApertureConfiguration(int param_id, int apreture_id)
{
  Parametisation_aperture_configuration conf;

  conf.to_marker_name = xml_parser.get<std::string>(param_id, apreture_id, std::string("to_marker_name") );

  std::string ap = xml_parser.get<std::string>(param_id, apreture_id, "ap_type");
  if(ap == "RECTELLIPSE")
    conf.ap_type = LHCApertureApproximator::RECTELLIPSE;
  else
    conf.ap_type = LHCApertureApproximator::RECTELLIPSE;

  conf.rect_rx = xml_parser.get<double>(param_id, apreture_id, "rect_rx");
  conf.rect_ry = xml_parser.get<double>(param_id, apreture_id, "rect_ry");
  conf.el_rx = xml_parser.get<double>(param_id, apreture_id, "el_rx");
  conf.el_ry = xml_parser.get<double>(param_id, apreture_id, "el_ry");

  return conf;
}


bool MADParamGenerator::CheckParamConfId(int id)
{
  std::vector<int> ids = xml_parser.getIds();
  bool val = false;
  for(unsigned int i=0; !val && i<ids.size(); i++)
    if(ids[i]==id)
      val = true;
  return val;
}

bool MADParamGenerator::CheckApertureConfId(int param_id, int apreture_id)
{
  std::vector<int> ids = xml_parser.getIds(param_id);
  bool val = false;
  for(unsigned int i=0; !val && i<ids.size(); i++)
    if(ids[i]==apreture_id)
      val = true;
  return val;
}


void MADParamGenerator::MakeParametrization(int id, bool generate_samples)
{
  if(!CheckParamConfId(id))
    return;

  Parametisation_configuration conf = this->GetParamConfiguration(id);

  if(generate_samples)
  {
    this->GenerateTrainingData(conf);
    this->GenerateTestingData(conf);
    this->GenerateApertureTestingData(conf);
  }

  std::string name = conf.optics_parametrisation_name;
  LHCOpticsApproximator approximator(name, name, conf.polynomials_type, conf.beam, conf.nominal_beam_energy);

  double prec[4];
  prec[0] = conf.precision_x;
  prec[1] = conf.precision_tx;
  prec[2] = conf.precision_y;
  prec[3] = conf.precision_ty;

  approximator.Train(this->GetSamplesTree(conf, conf.samples_train_root_file_name), conf.destination_branch_prefix, conf.terms_selelection_mode, conf.max_degree_x, conf.max_degree_tx, conf.max_degree_y, conf.max_degree_ty, conf.common_terms, prec);

  TFile *f = new TFile(conf.approximation_error_histogram_file.c_str(), "update");
  approximator.Test(this->GetSamplesTree(conf, conf.samples_test_root_file_name), f, conf.destination_branch_prefix, "");

  TrainAndAddApertures(conf, approximator, f);

  f->Close();
  delete f;

  if(conf.samples_aperture_test_file_name != "")
  {
    TFile *acc_accept_file = TFile::Open(conf.samples_aperture_test_file_name.c_str(), "update");
    TTree *new_acc_accept_tree = CreateAccelAcceptTree(acc_accept_file, "mad_param_filled_together");
    approximator.TestAperture(GetAccelAcceptTree(acc_accept_file), new_acc_accept_tree);
    WriteAccelAcceptTree(acc_accept_file, new_acc_accept_tree);
    acc_accept_file->Close();
  }

  TFile *approx_out = new TFile(conf.optics_parametrisation_file.c_str(), "update");
  approximator.Write(NULL, TObject::kOverwrite);
  approx_out->Close();
  delete approx_out;
}


void MADParamGenerator::TrainAndAddApertures(const Parametisation_configuration &conf, LHCOpticsApproximator &approximator, TFile *f_out)
{
  for(unsigned int i=0; i<conf.inter_planes.size(); i++)
  {
    std::string name = conf.from_marker_name + "_to_" + conf.inter_planes[i].to_marker_name;
    LHCOpticsApproximator aper_approx(name, name, conf.polynomials_type, conf.beam, conf.nominal_beam_energy);

    double prec[4];
    prec[0] = conf.precision_x;
    prec[1] = conf.precision_tx;
    prec[2] = conf.precision_y;
    prec[3] = conf.precision_ty;

    aper_approx.Train(this->GetSamplesTree(conf, conf.samples_train_root_file_name),
        conf.inter_planes[i].to_marker_name, conf.terms_selelection_mode, conf.max_degree_x,
        conf.max_degree_tx, conf.max_degree_y, conf.max_degree_ty, conf.common_terms, prec);

    aper_approx.Test(this->GetSamplesTree(conf, conf.samples_test_root_file_name), f_out,
        conf.inter_planes[i].to_marker_name, conf.optics_parametrisation_name);

    approximator.AddRectEllipseAperture(aper_approx, conf.inter_planes[i].rect_rx,
        conf.inter_planes[i].rect_ry, conf.inter_planes[i].el_rx, conf.inter_planes[i].el_ry);
  }
}


void MADParamGenerator::MakeAllParametrizations(bool generate_samples)
{
  std::vector<int> ids = xml_parser.getIds();

  for(unsigned int i=0; i<ids.size(); i++)
  {
    MakeParametrization(ids[i], generate_samples);
  }
}


void MADParamGenerator::IdentifyApertures(int id)
{
  if(!CheckParamConfId(id))
    return;

  Parametisation_configuration conf = this->GetParamConfiguration(id);
  this->GenerateLostParticleData(conf);
}


void MADParamGenerator::IdentifyAperturesForAll()
{
  std::vector<int> ids = xml_parser.getIds();

  for(unsigned int i=0; i<ids.size(); i++)
  {
    IdentifyApertures(ids[i]);
  }
}


std::ostream & operator<<(std::ostream &s, const Parametisation_configuration &c)
{
  s << "base_mad_conf_file " << c.base_mad_conf_file << std::endl;
  s << "processed_mad_conf_file " << c.processed_mad_conf_file << std::endl;
  s << "from_marker_name " << c.from_marker_name << std::endl;
  s << "from_marker_s_pos " << c.from_marker_s_pos << std::endl;
  s << "define_from " << c.define_from << std::endl;
  s << "to_marker_name " << c.to_marker_name << std::endl;
  s << "to_marker_s_pos " << c.to_marker_s_pos << std::endl;
  s << "define_to " << c.define_to << std::endl;
  s << "aperture_limit " << c.aperture_limit << std::endl;
  s << "tot_entries_number " << c.tot_entries_number << std::endl;

  s << "number_of_part_per_sample " << c.number_of_part_per_sample << std::endl;
  s << "x_min " << c.x_min << std::endl;
  s << "x_max " << c.x_max << std::endl;
  s << "theta_x_min " << c.theta_x_min << std::endl;
  s << "theta_x_max " << c.theta_x_max << std::endl;
  s << "y_min " << c.y_min << std::endl;
  s << "y_max " << c.y_max << std::endl;
  s << "theta_y_min " << c.theta_y_min << std::endl;
  s << "theta_y_max " << c.theta_y_max << std::endl;
  s << "ksi_min " << c.ksi_min << std::endl;
  s << "ksi_max " << c.ksi_max << std::endl;

  s << "samples_train_root_file_name " << c.samples_train_root_file_name << std::endl;
  s << "samples_test_root_file_name " << c.samples_test_root_file_name << std::endl;
  s << "destination_branch_prefix " << c.destination_branch_prefix << std::endl;

  s << "polynomials_type " << c.polynomials_type << std::endl;
  s << "terms_selelection_mode " << c.terms_selelection_mode << std::endl;
  s << "max_degree_x " << c.max_degree_x << std::endl;
  s << "max_degree_tx " << c.max_degree_tx << std::endl;
  s << "max_degree_y " << c.max_degree_y << std::endl;
  s << "max_degree_ty " << c.max_degree_ty << std::endl;
  s << "common_terms " << c.common_terms << std::endl;
  s << "approximation_error_histogram_file " << c.approximation_error_histogram_file << std::endl;

  s << "lost_particles_tree_filename " << c.lost_particles_tree_filename << std::endl;
  s << "lost_particles_hist_filename " << c.lost_particles_hist_filename << std::endl;

  s << "optics_parametrisation_file " << c.optics_parametrisation_file << std::endl;
  s << "inter_planes " << c.inter_planes.size() << std::endl;
  return s;
}


std::ostream & operator<<(std::ostream &s, const Parametisation_aperture_configuration &c)
{
  s << "to_marker_name " << c.to_marker_name << std::endl;
  s << "ap_type " << c.ap_type << std::endl;
  s << "rect_rx " << c.rect_rx << std::endl;
  s << "rect_ry " << c.rect_ry << std::endl;
  s << "el_rx " << c.el_rx << std::endl;
  s << "el_ry " << c.el_ry << std::endl;
  return s;
}
