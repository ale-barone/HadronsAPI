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


std::string ENSID = "@ensID@";
int TSRC = @tsrc@;


// run ID
std::string RUNID = ENSID; // do we need to coordinate with Tobi?
// file name
std::string OUTPUT_FILE =  "inclusive_btoc_" + ENSID + "_tsrc" + std::to_string(TSRC) + ".template.xml";
// extra info
std::string extra_info = "";
// folder name
std::string folder_output = "../../data/" + ENSID + "/output_inclusive_btoc/tsrc" + std::to_string(TSRC);


// GLOBAL VARIABLE ENSEMBLE SPECIFIC THAT HAS TO BE DEFINE IN  <template/@ensID@.hpp>

int LLAT = ENS::LLAT;
int TLAT = ENS::TLAT;
double AINV = ENS::AINV;

// Current insertion times
std::vector<int> TSNKS = ENS::TSNKS;
std::vector<int> T1INS = ENS::T1INS;
// charm masses
std::vector<double> CMASS = ENS::CMASS;


// Spectator quarks
std::vector<std::string> SPECTATORS = {"l", "s"};

// Smearings
// Light
std::vector<double> WIDTHS_l = ENS::WIDTHS_l;
// Strange
std::vector<double> WIDTHS_s = ENS::WIDTHS_s;


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
  double tw = LLAT / (2*M_PI) * sqrt(q2_lattice_units/3) ;
  std::cout<< LLAT << " MPI=" <<M_PI << " tw=" << tw << std::endl;
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
int NTWISTS_small = 4; // for Q2_VECOTR = {0, 0.33, 0.66, 1}



////////////////////////////////////////////////////////////////////////////////
// GAMMAS INSERTION AND IMPROVEMENTS
////////////////////////////////////////////////////////////////////////////////

// Gammas for 2pt (D(s) and D(s)*)
std::vector<std::string> GAMMAS_Ds = {"Gamma5", "GammaX", "GammaY", "GammaZ"};

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

// combinations for improvement terms
std::vector<std::array<std::string, 2>> RHQImpr = {
  {"X", "Identity"},
  {"Y", "Identity"},
  {"Z", "Identity"},
  {"T", "Identity"},
  {"X", "Gamma5"},
  {"Y", "Gamma5"},
  {"Z", "Gamma5"},
  {"T", "Gamma5"},
};
int NImpr = RHQImpr.size();


////////////////////////////////////////////////////////////////////////////////
// CUSTOM FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

// to avoid recomputing the same smeared source in case we want to use the same
// smearing for light and strange
std::vector<double> concatenate_widths(std::vector<double> v1, std::vector<double> v2){
  std::vector<double> out = v1;
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
std::string make_sm_source(Application &application, std::string source, double width){
  std::string source_sm_name =
      "source_sm" + remove_str(source, "source") + "_w" + double_to_string(width);

  MSource::JacobiSmear::Par source_sm;
  source_sm.gauge = "gauge";
  source_sm.width = width;
  source_sm.iterations = 150;
  source_sm.orthog = 3;
  source_sm.source = source;
  application.createModule<MSource::JacobiSmear>(source_sm_name, source_sm);
  return source_sm_name;
}


// smeared propagator (just template)
std::string make_sm_propagator(Application &application, std::string name, std::string source, double width){
  MSource::JacobiSmear::Par source_sm;
  source_sm.gauge = "gauge";
  source_sm.width = width;
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
      "quark_" + name + "_w" + double_to_string(width)+ twist_name + mom_name;

  application.createModule<MSource::JacobiSmear>(source_sm_name, source_sm);
  return source_sm_name;
}

// folder structure for easier mapping (especially in 4pt functions)
//  cm
//  |_twist
//    |_tree
//    |_impr  
// each of tree/impr will contain all configs

std::string make_folder_structure_4pt_tree(double cm, std::array<double, 4> twist){
  std::string out = folder_output + "/4pt/cm" + double_to_string(cm) + "/tw" + make_twist_name(twist) + "/tree";
  return out;
}
std::string make_folder_structure_4pt_impr(double cm, std::array<double, 4> twist){
  std::string out = folder_output + "/4pt/cm" + double_to_string(cm) + "/tw" + make_twist_name(twist) + "/impr";
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
  LOG(Message) << "======== XML generation with TSRC=" + std::to_string(TSRC) + " for ENS=" + ENSID + "========" << std::endl;

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
  std::string solver_l = ASolver::make_solver_CG(application, action_l, 8000, 1e-8);

  // strange
  std::string action_s = ENS::make_action_s(application);
  std::string solver_s = ASolver::make_solver_CG(application, action_s, 8000, 1e-8);

  // bottom
  std::string action_b = ENS::make_action_b(application);
  std::string solver_b = ASolver::make_solver_CG(application, action_b, 1000, 1e-40);


  // charm, 2D vector  with structure [cm][tw]
  std::vector<std::vector<std::string>> solver_c_cm_tw;
  for (int cm=0; cm<CMASS.size(); cm++){
    LOG(Message) << cm << " " << CMASS[cm] << std::endl;
    LOG(Message) << CMASS << std::endl;
    std::vector<std::string> solver_c_tw;
    for (int tw=0; tw<NTWISTS; tw++){
      LOG(Message) << tw << " " << TWISTS[tw][0] << std::endl;
      std::string action_c = ENS::make_action_c(application, CMASS[cm], TWISTS[tw]);
      solver_c_tw.push_back(ASolver::make_solver_CG(application, action_c, 8000, 1e-12));
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
  std::string source_z2 = "Z2_t0" + std::to_string(TSRC) + "_p+0_+0_+0";
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
  for (double width : concatenate_widths(WIDTHS_l, WIDTHS_s)){
    std::string source_sm_w = make_sm_source(application, source_z2, width); 
    if (std::find(WIDTHS_l.begin(), WIDTHS_l.end(), width) != WIDTHS_l.end())
      sources_sm_l.push_back(source_sm_w);
    if (std::find(WIDTHS_s.begin(), WIDTHS_s.end(), width) != WIDTHS_s.end())
      sources_sm_s.push_back(source_sm_w);
  }

  //////////////////////////////////////////////////////////////////////////////
  // PROPAGATORS AND 2PT CONTRACTIONS
  //////////////////////////////////////////////////////////////////////////////
  LOG(Message) << "Making 2pt contractions" << std::endl;

  // quark_b
  std::string quark_b = AFermion::make_propagator(application, "b", source_z2, solver_b);

  //========================================================================//
  // Loop over stange and light quark SPECTATORS
  //========================================================================//

  
  std::vector<std::string> quark_sNl;
  std::vector<std::vector<std::vector<std::string>>> quark_b_sNl_SL_tsnk_w;
  std::vector<std::vector<std::vector<std::string>>> quark_b_sNl_SS_tsnk_w;
  std::vector<std::vector<std::string>> source_seq_sNl;
  for (std::string spectator_quark : SPECTATORS)
  {
    std::string solver_spec;
    std::vector<std::string> sources_sm_spec;
    std::vector<double> widths;
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

    // quark_spec
    std::string quark_spec = AFermion::make_propagator(application, spectator_quark, source_z2, solver_spec);
    quark_sNl.push_back(quark_spec);
    //========================================================================//
    // Loop over smearing types and widths
    //========================================================================//
    std::vector<std::string> quark_spec_LS_sm;
    std::vector<std::string> quark_spec_SL_sm;
    std::vector<std::string> quark_spec_SS_sm;
    for (int w=0; w<widths.size(); w++)
    {
      quark_spec_LS_sm.push_back(AFermion::make_propagator(application, spectator_quark + "_LS_w" + double_to_string(widths[w]), sources_sm_spec[w], solver_spec));
      quark_spec_SL_sm.push_back(make_sm_propagator(application, spectator_quark + "_SL", quark_spec, widths[w]));
      quark_spec_SS_sm.push_back(make_sm_propagator(application, spectator_quark + "_SS", quark_spec_LS_sm[w], widths[w]));
    }

    //========================================================================//
    // B(s) for all smearing for all combinations
    //========================================================================//

    // unsmeared
    AContraction::make_2pt_contraction(application, quark_b, quark_spec, {"Gamma5", "Gamma5"}, sink, folder_output, extra_info);
    // smeared
    for (int w = 0; w < widths.size(); ++w){
      AContraction::make_2pt_contraction(application, quark_b, quark_spec_LS_sm[w], {"Gamma5", "Gamma5"}, sink, folder_output, extra_info);
      AContraction::make_2pt_contraction(application, quark_b, quark_spec_SL_sm[w], {"Gamma5", "Gamma5"}, sink, folder_output, extra_info);
      AContraction::make_2pt_contraction(application, quark_b, quark_spec_SS_sm[w], {"Gamma5", "Gamma5"}, sink, folder_output, extra_info);
    }

    //========================================================================//
    // Loop over sink-source sperations
    //========================================================================//
    std::vector<std::vector<std::string>> quark_b_spec_SL_tsnk_w;
    std::vector<std::vector<std::string>> quark_b_spec_SS_tsnk_w;
    std::vector<std::string> quark_b_spec_tsnk;
    std::vector<std::string> source_seq_spec_tsnk;
    for (int i=0; i<TSNKS.size(); i++)
    {
      int tsnk = shift_tins(TSNKS[i]);

      // sequential sources on top of quark_spec
      // unsmeared
      std::string source_seq_spec = ASource::make_seq_source(application, quark_spec, tsnk, "Gamma5", MOM0);
      source_seq_spec_tsnk.push_back(source_seq_spec);
      // smeared
      std::vector<std::string> source_seq_spec_LS_w;
      std::vector<std::string> source_seq_spec_SL_w;
      std::vector<std::string> source_seq_spec_SS_w;
      for (int w=0; w<widths.size(); w++){
        source_seq_spec_LS_w.push_back(ASource::make_seq_source(application, quark_spec_LS_sm[w], tsnk, "Gamma5", MOM0));
        source_seq_spec_SL_w.push_back(ASource::make_seq_source(application, quark_spec_SL_sm[w], tsnk, "Gamma5", MOM0));
        source_seq_spec_SS_w.push_back(ASource::make_seq_source(application, quark_spec_SS_sm[w], tsnk, "Gamma5", MOM0));
      }


      // sequential propagator quark_b_spec
      // unsmeared
      std::string quark_b_spec = AFermion::make_seq_propagator(application, "b", source_seq_spec, solver_b);
      // B(s)->B(s) unsmeared only, (we need only GammaT(Gamma5) for ZV(A) but we contract everything)
      AContraction::make_3pt_contraction(application, quark_b, quark_b_spec, {GAMMAS, {"Gamma5"}}, sink, folder_output, extra_info);
      //========================================================================//
      // Loop over smearing types and widths
      //========================================================================//
      std::vector<std::string> quark_b_spec_SS_w;
      std::vector<std::string> quark_b_spec_SL_w;
      for (int w=0; w<widths.size(); w++){
        quark_b_spec_SS_w.push_back(AFermion::make_seq_propagator(application, "b", source_seq_spec_SS_w[w], solver_b));
        quark_b_spec_SL_w.push_back(AFermion::make_seq_propagator(application, "b", source_seq_spec_SL_w[w], solver_b));
      }
      quark_b_spec_SS_tsnk_w.push_back(quark_b_spec_SS_w);
      quark_b_spec_SL_tsnk_w.push_back(quark_b_spec_SL_w);
    }
    source_seq_sNl.push_back(source_seq_spec_tsnk);
    quark_b_sNl_SS_tsnk_w.push_back(quark_b_spec_SS_tsnk_w);
    quark_b_sNl_SL_tsnk_w.push_back(quark_b_spec_SL_tsnk_w);
  }

  //////////////////////////////////////////////////////////////////////////////
  // CHARM LOOP FOR 2PT/3PT
  //////////////////////////////////////////////////////////////////////////////

  LOG(Message) << "Making 2pt/3pt contractions for charms" << std::endl;

  for (int cm=0; cm<CMASS.size(); cm++){
    for (int tw=0; tw<NTWISTS_small; tw++){
      std::string quark_c_cm_tw = AFermion::make_propagator(application, "c_m" + double_to_string(CMASS[cm]), source_z2, solver_c_cm_tw[cm][tw]);
      
      // contractions
      for (int spec=0; spec<SPECTATORS.size(); spec++){
        // 2pt D(s)(*)
        AContraction::make_2pt_contraction(application, quark_c_cm_tw, quark_sNl[spec], {GAMMAS_Ds, GAMMAS_Ds}, sink, folder_output, extra_info);
        // 3pt
        // Ds->Ds  
        if (TWISTS[tw]==TW0){
          for (int i = 0; i < TSNKS.size(); ++i){
            int tsnk = shift_tins(TSNKS[i]);
            std::string quark_c_spec_cm_TW0 = AFermion::make_seq_propagator(application, "c_m" + double_to_string(CMASS[cm]), source_seq_sNl[spec][i], solver_c_cm_tw[cm][tw]);
            AContraction::make_3pt_contraction(application, quark_c_spec_cm_TW0, quark_c_cm_tw, {GAMMAS, {"Gamma5"}}, sink, folder_output, extra_info);
          }
        }
        
        // Bs->Ds(*)
        std::vector<double> widths;
        if (SPECTATORS[spec] == "l")
          widths = WIDTHS_l;
        else if (SPECTATORS[spec] == "s")
          widths = WIDTHS_s;
        
        for (int i = 0; i < TSNKS.size(); ++i){
          for (int w=0; w<widths.size(); w++){
            AContraction::make_3pt_contraction(application, quark_b_sNl_SL_tsnk_w[spec][i][w], quark_c_cm_tw, {GAMMAS, GAMMAS_Ds}, sink, folder_output, extra_info);
          }
        }      
      }
    }
    // Ds->Ds for tw=0
    //quark_c_spec_Ds_cm_TW0 = AFermion::make_seq_propagator(application, "c_cm" + std::to_string(CMASS[cm]).substr(0, 4), source_seq_spec, solver_c_cm_tw[cm][0]);
  }

  ////////////////////////////////////////////////////////////////////////////
  // 4pt CONTRACTIONS (T1 FIXED)
  ////////////////////////////////////////////////////////////////////////////

  // ========================================================================//
  // Tree level
  // ========================================================================//

  LOG(Message) << "Generating 4pt functions tree level+impr on t2" << std::endl;

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

  // improvements on t1
  std::vector<std::vector<std::vector<std::vector<std::string>>>> quarkImprI_b_spec_tsnk_w(NImpr);
  std::vector<std::vector<std::vector<std::vector<std::string>>>> quarkImprIII_b_spec_tsnk_w(NImpr);

  for (int i = 0; i < NImpr; i++) // Loop over indices/gammas of improvement
  {
    // Create vectors for each level
    std::vector<std::vector<std::vector<std::string>>> quarkImprI_spec_tsnk_w(SPECTATORS.size());
    std::vector<std::vector<std::vector<std::string>>> quarkImprIII_spec_tsnk_w(SPECTATORS.size());

    // select direction and gamma for improvement
    std::string dir = RHQImpr[i][0];
    std::string gamma = RHQImpr[i][1];
    //========================================================================//
    // Loop over spectator quarks
    //========================================================================//
    for (int q = 0; q < SPECTATORS.size(); ++q)
    {
      // Initialize the third dimension
      std::vector<std::vector<std::string>> quarkImprI_tsnk_w(TSNKS.size());
      std::vector<std::vector<std::string>> quarkImprIII_tsnk_w(TSNKS.size());

      //========================================================================//
      // Loop over time separations
      //========================================================================//
      for (int tsnk = 0; tsnk < TSNKS.size(); ++tsnk)
      {
        // Initialize the innermost dimension
        std::vector<std::string> quarkImprI_w(WIDTHS_l.size());
        std::vector<std::string> quarkImprIII_w(WIDTHS_l.size());

        //========================================================================//
        // Loop over SS smearing on strange/light and all widths
        //========================================================================//
        for (int w = 0; w < WIDTHS_l.size(); ++w)
        {
          // Call the functions and store the results in the innermost vectors
          quarkImprI_w[w] = ARHQ::make_RHQInsertionI(application, quark_b_sNl_SS_tsnk_w[q][tsnk][w], dir, gamma);
          quarkImprIII_w[w] = ARHQ::make_RHQInsertionIII(application, quark_b_sNl_SS_tsnk_w[q][tsnk][w], dir, gamma);
        }

        // Store the innermost vectors in the next level up
        quarkImprI_tsnk_w[tsnk] = quarkImprI_w;
        quarkImprIII_tsnk_w[tsnk] = quarkImprIII_w;
      }

      // Store these vectors into the quark spectator level
      quarkImprI_spec_tsnk_w[q] = quarkImprI_tsnk_w;
      quarkImprIII_spec_tsnk_w[q] = quarkImprIII_tsnk_w;
    }

    // Finally, store the entire structure into the outermost vectors
    quarkImprI_b_spec_tsnk_w[i] = quarkImprI_spec_tsnk_w;
    quarkImprIII_b_spec_tsnk_w[i] = quarkImprIII_spec_tsnk_w;
  }

  //for (int t1ins=0; t1ins<T1INS.size(); t1ins++){
  
  for (int cm=0; cm<CMASS.size(); cm++){
    for (int tw=0; tw<NTWISTS; tw++){
      for (int t1ins=0; t1ins<T1INS.size(); t1ins++){
        for (unsigned int g=0; g<NGAMMAS; g++){
          std::string quark_c_b_cm = AFermion::make_seq_propagator(application, "c_m" + double_to_string(CMASS[cm]), source_seq_b[t1ins][g], solver_c_cm_tw[cm][tw]);
          
          std::vector<std::string> quarkImprII_c_b_i;
          std::vector<std::string> quarkImprIV_c_b_i;
          for (int i=0; i<NImpr; i++){
            std::string dir = RHQImpr[i][0];
            std::string gamma = RHQImpr[i][1];
            quarkImprII_c_b_i.push_back(ARHQ::make_RHQInsertionII(application, quark_c_b_cm, dir, gamma, TWISTS[tw]));
            quarkImprIV_c_b_i.push_back(ARHQ::make_RHQInsertionIV(application, quark_c_b_cm, dir, gamma, TWISTS[tw]));
          }
          for (int spec=0; spec<SPECTATORS.size(); spec++){
            // pick widths
            std::vector<double> widths;
            if (SPECTATORS[spec] == "l")
              widths = WIDTHS_l;
            else if (SPECTATORS[spec] == "s")
              widths = WIDTHS_s;
            for (int ts=0; ts < TSNKS.size(); ts++){
              for (int w=0; w<widths.size(); w++){
                std::string folder_4pt_tree = make_folder_structure_4pt_tree(CMASS[cm], TWISTS[tw]);
                AContraction::make_4pt_contraction(application, quark_b_sNl_SS_tsnk_w[spec][ts][w], quark_c_b_cm, {GAMMAS, {"Gamma5"}}, sink, folder_4pt_tree, extra_info);
                
                std::string folder_4pt_impr = make_folder_structure_4pt_impr(CMASS[cm], TWISTS[tw]);
                for (int i=0; i<NImpr; i++){
                  AContraction::make_4pt_contraction(application, quarkImprI_b_spec_tsnk_w[i][spec][ts][w], quark_c_b_cm, {"Identity", "Gamma5"}, sink, folder_4pt_impr, extra_info);
                  AContraction::make_4pt_contraction(application, quarkImprIII_b_spec_tsnk_w[i][spec][ts][w], quark_c_b_cm, {"Identity", "Gamma5"}, sink, folder_4pt_impr, extra_info);
                  AContraction::make_4pt_contraction(application, quark_b_sNl_SS_tsnk_w[spec][ts][w], quarkImprII_c_b_i[i], {"Identity", "Gamma5"}, sink, folder_4pt_impr, extra_info);
                  AContraction::make_4pt_contraction(application, quark_b_sNl_SS_tsnk_w[spec][ts][w], quarkImprIV_c_b_i[i], {"Identity", "Gamma5"}, sink, folder_4pt_impr, extra_info);
                }
              }
            }
          }
        }
      }
    }
  }

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  // IMPROVEMENT ON t1
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

  LOG(Message) << "Generating 4pt functions with impr on t1" << std::endl;

  for (int imp=0; imp<NImpr; imp++){
    std::string dir = RHQImpr[imp][0];
    std::string gamma = RHQImpr[imp][1];

    // loop over t1 insertions
    for (int t1_dummy = 0; t1_dummy < T1INS.size(); t1_dummy++){
      int t1 = shift_tins(T1INS[t1_dummy]);

      std::string source_seq_ImprI_b_imp_t1 = ARHQ::make_RHQSeqSourceI(application, quark_b, t1, MOM0, dir, gamma);
      std::string source_seq_ImprIII_b_imp_t1 = ARHQ::make_RHQSeqSourceIII(application, quark_b, t1, MOM0, dir, gamma);

      // Loop over twists as these need to be included for II and IV improvements
      for (int tw=0; tw<NTWISTS; tw++){
        std::string source_seq_ImprII_b_imp_t1_tw = ARHQ::make_RHQSeqSourceII(application, quark_b, t1, TWISTS[tw], dir, gamma, "Twist");
        std::string source_seq_ImprIV_b_imp_t1_tw = ARHQ::make_RHQSeqSourceIV(application, quark_b, t1, TWISTS[tw], dir, gamma, "Twist");
        for (int cm=0; cm<CMASS.size(); cm++){      
          std::string quark_c_ImprI_b_cm = AFermion::make_seq_propagator(application, "c_m" + double_to_string(CMASS[cm]) +"_t1"+std::to_string(t1), source_seq_ImprI_b_imp_t1, solver_c_cm_tw[cm][tw]);          
          std::string quark_c_ImprII_b_cm = AFermion::make_seq_propagator(application, "c_m" + double_to_string(CMASS[cm])+"_t1"+std::to_string(t1), source_seq_ImprII_b_imp_t1_tw, solver_c_cm_tw[cm][tw]);
          std::string quark_c_ImprIII_b_cm = AFermion::make_seq_propagator(application, "c_m" + double_to_string(CMASS[cm])+"_t1"+std::to_string(t1), source_seq_ImprIII_b_imp_t1, solver_c_cm_tw[cm][tw]);
          std::string quark_c_ImprIV_b_cm = AFermion::make_seq_propagator(application, "c_m" + double_to_string(CMASS[cm])+"_t1"+std::to_string(t1), source_seq_ImprIV_b_imp_t1_tw, solver_c_cm_tw[cm][tw]);

          std::string folder_4pt_impr = make_folder_structure_4pt_impr(CMASS[cm], TWISTS[tw]);
          for (int spec=0; spec<SPECTATORS.size(); spec++){
            // pick widths
            std::vector<double> widths;
            if (SPECTATORS[spec] == "l")
              widths = WIDTHS_l;
            else if (SPECTATORS[spec] == "s")
              widths = WIDTHS_s;
            for (int ts=0; ts < TSNKS.size(); ts++){
              for (int w=0; w<widths.size(); w++){
                AContraction::make_4pt_contraction(application, quark_b_sNl_SS_tsnk_w[spec][ts][w], quark_c_ImprI_b_cm, {GAMMAS, {"Gamma5"}}, sink, folder_4pt_impr, extra_info);
                AContraction::make_4pt_contraction(application, quark_b_sNl_SS_tsnk_w[spec][ts][w], quark_c_ImprII_b_cm, {GAMMAS, {"Gamma5"}}, sink, folder_4pt_impr, extra_info);
                AContraction::make_4pt_contraction(application, quark_b_sNl_SS_tsnk_w[spec][ts][w], quark_c_ImprIII_b_cm, {GAMMAS, {"Gamma5"}}, sink, folder_4pt_impr, extra_info);
                AContraction::make_4pt_contraction(application, quark_b_sNl_SS_tsnk_w[spec][ts][w], quark_c_ImprIV_b_cm, {GAMMAS, {"Gamma5"}}, sink, folder_4pt_impr, extra_info);
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