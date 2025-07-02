#include <Hadrons/Application.hpp>
#include <Hadrons/Modules.hpp>
#include <API/API.hpp>
// include <../../../install/SOURCE/AleHadrons/API/ARHQ.hpp>
#include <API/Utilities.hpp>
#include <string>
#include <template/@ensID@.hpp>


using namespace Grid;
using namespace Hadrons;
using namespace API;

////////////////////////////////////////////////////////////////////////////////
// GENERAL SETUP (FOR EACH ENSEMBLE)
////////////////////////////////////////////////////////////////////////////////

// ENS ID
std::string ENSID = "@ensID@";
// TSRC
int TSRC = @tsrc@;
std::string tsrc_to_string(int tsrc){
  std::string out = std::to_string(tsrc);
  if (tsrc<10)
    out = "0" + out;
  return out;
}
std::string TSRC_STR = tsrc_to_string(TSRC);

// run ID
std::string RUNID = ENSID; // do we need to coordinate with Tobi?
// file name
std::string OUTPUT_FILE =  "inclusive_btoc_" + ENSID + "_tsrc" + TSRC_STR + ".template.xml";
// extra info
std::string extra_info = "";
// folder name
std::string folder_output = "../../data/" + ENSID + "/output_inclusive_btoc/tsrc" + TSRC_STR;


// GLOBAL VARIABLE ENSEMBLE SPECIFIC THAT HAS TO BE DEFINED IN  <template/@ensID@.hpp>
int LLAT = ENS::LLAT;
int TLAT = ENS::TLAT;
double AINV = ENS::AINV;

// Current insertion times
std::vector<int> TSNKS = ENS::TSNKS;
std::vector<int> T1INS = ENS::T1INS;

// charm masses
std::vector<std::string> CMASS = ENS::CMASS;

// Spectator quarks
std::vector<std::string> SPECTATORS = {"l", "s"};

// Smearings
// Light
std::vector<std::string> WIDTHS_l = ENS::WIDTHS_l;
// Strange
std::vector<std::string> WIDTHS_s = ENS::WIDTHS_s;


////////////////////////////////////////////////////////////////////////////////
// MOMENTA / TWISTS
////////////////////////////////////////////////////////////////////////////////

// zero momentum for sink
std::array<int, 4> MOM0 = {0, 0, 0, 0};

// zero twists
std::array<double, 4> TW0 = {.0, .0, .0, .0};

// list of physical 3-momentum q^2 we want to induce in GeV^2
std::vector<double> Q2_VECTOR = {0, 0.33, 0.66, 1, 2, 3, 4, 5};

// functions that make the twist automatically for each ensemble with
// q = 2pi/L * (tw, tw, tw)
// q^2 = (2pi)^2 / L^2 * 3*tw^2
// tw = L / (2pi) * sqrt(q^2/3)
std::array<double, 4> make_twist_array(double q2){
  double q2_lattice_units = q2 / std::pow(AINV, 2);
  double tw = LLAT / (2*M_PI) * sqrt(q2_lattice_units/3);
  // round the tw to the 3rd digit
  tw = std::round(tw * 1000.0) / 1000.0;
  std::array<double, 4> out = {tw, tw, tw, 0}; 
  return out;
}

std::vector<std::array<double, 4>> make_twists(){
  std::vector<std::array<double, 4>> out;
  for (double q2 : Q2_VECTOR){
    out.push_back(make_twist_array(q2));
  }
  return out;
}

// twists
std::vector<std::array<double, 4>> TWISTS = make_twists();
// number of twists
int NTWISTS = TWISTS.size();
// small twists (for 3pt) - made to be same size as twists
int NTWISTS_small = 4; // for Q2_VECTOR = {0, 0.33, 0.66, 1}

////////////////////////////////////////////////////////////////////////////////
// GAMMAS INSERTION AND IMPROVEMENTS
////////////////////////////////////////////////////////////////////////////////

// Gammas at src-snk for 2pt contractions
std::vector<std::array<std::string, 2>> GAMMAS_2PT = {
  // axial-pseudoscalar
  {"Gamma5", "Gamma5"},
  {"Gamma5", "GammaTGamma5"},
  {"GammaTGamma5", "Gamma5"},
  {"GammaTGamma5", "GammaTGamma5"},
  // vector
  {"GammaX", "GammaX"},
  {"GammaY", "GammaY"},
  {"GammaZ", "GammaZ"},
  {"GammaT", "GammaT"},
};

// Gammas for Ds (in 3pt)
std::vector<std::string> GAMMAS_Ds = {
  "Gamma5",
  "GammaTGamma5",
  "GammaT",
  "GammaX",
  "GammaY",
  "GammaZ"
};

// gammas for current insertion in 4pt
std::vector<std::string> GAMMAS = {
  "GammaX",
  "GammaY",
  "GammaZ",
  "GammaT",
  "GammaXGamma5",
  "GammaYGamma5",
  "GammaZGamma5",
  "GammaTGamma5"
};
// number of gammas
int NGAMMAS = GAMMAS.size();


////////////////////////////////////////////////////////////////////////////////
// CUSTOM FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

// to avoid recomputing the same smeared source in case we want to use the same
// smearing for light and strange
std::vector<std::string> concatenate_widths(std::vector<std::string> v1, std::vector<std::string> v2){
  std::vector<std::string> out = v1;
  out.insert(out.end(), v2.begin(), v2.end());
  std::sort(out.begin(), out.end());
  out.erase(std::unique(out.begin(), out.end()), out.end());
  return out;
}

// shif time insertion (tsnk or t1) according to source position TSRC
int shift_tins(int tins){
  // compact if -> variable = (condition) ? expressionTrue : expressionFalse;
  int out = (tins + TSRC < TLAT) ? tins + TSRC : (tins + TSRC) - TLAT;
  return out;
}

// smeared source (just template)
std::string make_sm_source(Application &application, std::string source, std::string width){
  std::string source_sm_name =
      "source_sm" + remove_str(source, "source") + "_w" + width;

  MSource::JacobiSmear::Par source_sm;
  source_sm.gauge = "gauge";
  source_sm.width = std::stod(width);
  source_sm.iterations = 150;
  source_sm.orthog = 3;
  source_sm.source = source;
  application.createModule<MSource::JacobiSmear>(source_sm_name, source_sm);
  return source_sm_name;
}


// smeared propagator (just template)
std::string make_sm_propagator(Application &application, std::string name, std::string source, std::string width){
  MSource::JacobiSmear::Par source_sm;
  source_sm.gauge = "gauge";
  source_sm.width = std::stod(width);
  source_sm.iterations = 150;
  source_sm.orthog = 3;
  source_sm.source = source;

  std::string twist = get_twist(source);
  std::string twist_name = "";
  std::string mom = get_mom(source);
  std::string mom_name = "";

  if (!twist.empty())
    twist_name = "_tw_" + twist;
  if (!mom.empty())
    mom_name = "_mom_" + mom;

  std::string source_sm_name =
      "quark_" + name + "_w" + width + twist_name + mom_name;

  application.createModule<MSource::JacobiSmear>(source_sm_name, source_sm);
  return source_sm_name;
}

// folder structure for easier mapping (especially in 4pt functions)
//  cm
//  |_twist
//    |_tree
//    |_impr  
// each of tree/impr will contain all configs

std::string make_folder_structure_4pt_tree(std::string cm, std::array<double, 4> twist){
  std::string out = folder_output + "/4pt/cm" + cm + "/tw_" + make_twist_name(twist) + "/tree";
  return out;
}
std::string make_folder_structure_4pt_impr(std::string cm, std::array<double, 4> twist){
  std::string out = folder_output + "/4pt/cm" + cm + "/tw_" + make_twist_name(twist) + "/impr";
  return out;
}

////////////////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  // initialise Grid ///////////////////////////////////////////////////////////
  Grid_init(&argc, &argv);

  // initialise application ////////////////////////////////////////////////////

  Application application;
  Application::GlobalPar globalPar;
  LOG(Message) << "======== XML generation with TSRC=" + TSRC_STR + " for ENS=" + ENSID + "========" << std::endl;

  // global initialisation
  // global parameters
  // globalPar.trajCounter.start = ;
  // globalPar.trajCounter.end   = ;
  // globalPar.trajCounter.step  = 20;
  globalPar.runId = RUNID;
  globalPar.scheduler.schedulerType = "naive";
  application.setPar(globalPar);

  // create modules ////////////////////////////////////////////////////////////
  

  //////////////////////////////////////////////////////////////////////////////
  // ACTIONS AND SOLVERS
  //////////////////////////////////////////////////////////////////////////////
  LOG(Message) << "Making actions and solvers" << std::endl;

  // gauge
  ENS::make_gauge(application);

  // light action
  std::string action_l = ENS::make_action_l(application);
  std::string solver_l = ENS::make_solver_l(application);

  // strange
  std::string action_s = ENS::make_action_s(application);
  std::string solver_s = ENS::make_solver_s(application);

  // bottom
  std::string action_b = ENS::make_action_b(application);
  std::string solver_b = ENS::make_solver_b(application);


  // charm, 2D vector  with structure [cm][tw]
  std::vector<std::vector<std::string>> solver_c_cm_tw;
  for (int cm=0; cm<CMASS.size(); cm++){
    std::vector<std::string> solver_c_tw;
    for (int tw=0; tw<NTWISTS; tw++){
      std::string action_c = ENS::make_action_c(application, CMASS[cm], TWISTS[tw]);
      solver_c_tw.push_back(ENS::make_solver_c(application, action_c));
    }
    solver_c_cm_tw.push_back(solver_c_tw);
  }

  //////////////////////////////////////////////////////////////////////////////
  // SOURCE AND SINK
  //////////////////////////////////////////////////////////////////////////////

  LOG(Message) << "Making sources and sinks" << std::endl;

  // sink
  std::string sink = ASink::make_sink(application, MOM0);

  // z2 source
  std::string source_z2 = "Z2_t" + TSRC_STR + "_p+0_+0_+0";
  MSource::Z2::Par source_z2_module;
  source_z2_module.tA = TSRC;
  source_z2_module.tB = TSRC;
  application.createModule<MSource::Z2>(source_z2, source_z2_module);


  //============================================================================
  // Loop over smearing types and widths for both s and l
  //============================================================================

  // smeared sources for s
  std::vector<std::string> sources_sm_l;
  std::vector<std::string> sources_sm_s;
  for (std::string width : concatenate_widths(WIDTHS_l, WIDTHS_s)){
    std::string source_sm_w = make_sm_source(application, source_z2, width); 
    if (std::find(WIDTHS_l.begin(), WIDTHS_l.end(), width) != WIDTHS_l.end())
      sources_sm_l.push_back(source_sm_w);
    if (std::find(WIDTHS_s.begin(), WIDTHS_s.end(), width) != WIDTHS_s.end())
      sources_sm_s.push_back(source_sm_w);
  }

  //////////////////////////////////////////////////////////////////////////////
  // PROPAGATORS 
  //////////////////////////////////////////////////////////////////////////////

  // quark_b
  std::string quark_b = AFermion::make_propagator(application, "b", source_z2, solver_b);

  //========================================================================//
  // Loop over stange and light quark SPECTATORS
  //========================================================================//

  std::vector<std::vector<std::vector<std::string>>> quark_b_sNl_SS_tsnk_w;
  for (std::string spectator_quark : SPECTATORS)
  {
    std::string solver_spec;
    std::vector<std::string> sources_sm_spec;
    std::vector<std::string> widths;
    if (spectator_quark == "l")
    {
      solver_spec = solver_l;
      sources_sm_spec = sources_sm_l;
      widths = WIDTHS_l;
    }
    else if (spectator_quark == "s")
    {
      solver_spec = solver_s;
      sources_sm_spec = sources_sm_s;
      widths = WIDTHS_s;
    }



    //========================================================================//
    // Loop over smearing types and widths
    //========================================================================//

    std::string quark_spec = AFermion::make_propagator(application, spectator_quark, source_z2, solver_spec);

    std::vector<std::string> quark_spec_LS_sm;
    std::vector<std::string> quark_spec_SL_sm;
    std::vector<std::string> quark_spec_SS_sm;
    for (int w=0; w<widths.size(); w++){
      quark_spec_LS_sm.push_back(AFermion::make_propagator(application, spectator_quark + "_LS_w" + widths[w], sources_sm_spec[w], solver_spec));
      quark_spec_SL_sm.push_back(make_sm_propagator(application, spectator_quark + "_SL", quark_spec, widths[w]));
      quark_spec_SS_sm.push_back(make_sm_propagator(application, spectator_quark + "_SS", quark_spec_LS_sm[w], widths[w]));
      // 2pt smeared
      if (spectator_quark == "s"){
        if (widths[w]!="5.00" & widths[w]!="6.00" & widths[w]!="6.50"){
          AContraction::make_2pt_contraction(application, quark_b, quark_spec_LS_sm[w], GAMMAS_2PT, sink, folder_output, extra_info);
          AContraction::make_2pt_contraction(application, quark_b, quark_spec_SL_sm[w], GAMMAS_2PT, sink, folder_output, extra_info);
          AContraction::make_2pt_contraction(application, quark_b, quark_spec_SS_sm[w], GAMMAS_2PT, sink, folder_output, extra_info);
        }
      }
    }

    //========================================================================//
    // Loop over sink-source sperations
    //========================================================================//
    std::vector<std::vector<std::string>> quark_b_spec_SS_tsnk_w;
    for (int i=0; i<TSNKS.size(); i++)
    {
      int tsnk = shift_tins(TSNKS[i]);
      // sequential sources on top of quark_spec
      // smeared
      std::vector<std::string> source_seq_spec_SS_w;
      for (int w=0; w<widths.size(); w++){
        source_seq_spec_SS_w.push_back(ASource::make_seq_source(application, quark_spec_SS_sm[w], tsnk, "Gamma5", MOM0));
      }

      //========================================================================//
      // Loop over smearing types and widths
      //========================================================================//
      std::vector<std::string> quark_b_spec_SS_w;
      for (int w=0; w<widths.size(); w++){
        quark_b_spec_SS_w.push_back(AFermion::make_seq_propagator(application, "b", source_seq_spec_SS_w[w], solver_b));
      }
      quark_b_spec_SS_tsnk_w.push_back(quark_b_spec_SS_w);
    }
    quark_b_sNl_SS_tsnk_w.push_back(quark_b_spec_SS_tsnk_w);
  }


  ////////////////////////////////////////////////////////////////////////////
  // 4pt CONTRACTIONS (T1 FIXED)
  ////////////////////////////////////////////////////////////////////////////

  // ========================================================================//
  // Tree level
  // ========================================================================//

  LOG(Message) << "Generating 4pt functions tree level" << std::endl;

  // source seq_b
  std::vector<std::vector<std::string>> source_seq_b;
  for (int t1_dummy : T1INS){
    int t1 = shift_tins(t1_dummy);
    std::vector<std::string> source_seq_b_t1;
    for (unsigned int g=0; g<NGAMMAS; g++){
      source_seq_b_t1.push_back(ASource::make_seq_source(application, quark_b, t1, GAMMAS[g], MOM0));
    }
    source_seq_b.push_back(source_seq_b_t1);
  }
  
  //for (int t1ins=0; t1ins<T1INS.size(); t1ins++){
  for (int cm=0; cm<CMASS.size(); cm++){
    for (int tw=0; tw<NTWISTS; tw++){
      for (int t1ins=0; t1ins<T1INS.size(); t1ins++){
        for (unsigned int g=0; g<NGAMMAS; g++){
          std::string quark_c_b_cm = AFermion::make_seq_propagator(application, "c_m" + CMASS[cm], source_seq_b[t1ins][g], solver_c_cm_tw[cm][tw]);

          for (int spec=0; spec<SPECTATORS.size(); spec++){
            // pick widths
            std::vector<std::string> widths;
            if (SPECTATORS[spec] == "l")
              widths = WIDTHS_l;
            else if (SPECTATORS[spec] == "s")
              widths = WIDTHS_s;
            for (int ts=0; ts<TSNKS.size(); ts++){
              for (int w=0; w<widths.size(); w++){
                std::string folder_4pt_tree = make_folder_structure_4pt_tree(CMASS[cm], TWISTS[tw]);
                AContraction::make_4pt_contraction(application, quark_b_sNl_SS_tsnk_w[spec][ts][w], quark_c_b_cm, {GAMMAS, {"Gamma5"}}, sink, folder_4pt_tree, extra_info);
                }
              }
          }
        }
      }
    }
  }


  ////////////////////////////////////////////////////////////////////////////
  // END
  ////////////////////////////////////////////////////////////////////////////
  // save xml
  application.saveParameterFile(OUTPUT_FILE);


  // // epilogue ////////////////////////////////////////////////////////////////
  LOG(Message) << "Grid is finalizing now" << std::endl;
  Grid_finalize();

  return EXIT_SUCCESS;
}