#include <Hadrons/Application.hpp>
#include <Hadrons/Modules.hpp>
#include <API/API.hpp>
#include <API/Utilities.hpp>

using namespace Grid;
using namespace Hadrons;
using namespace API;

////////////////////////////////////////////////////////////////////////////////
// GENERAL SETUP
////////////////////////////////////////////////////////////////////////////////

// runID
// NB runid with the config number determine the Z2 source
std::string runid = "template";

// time slices
int tsrc = 0;
int T    = 64; // lattice temporal extent
int L    = 24; // lattice spatial extent
int delta_tsnk = 20; // tsnk-tsrc
int delta_t1   = 6;  // t1-tsrc
  
// compact if -> variable = (condition) ? expressionTrue : expressionFalse;
int tsnk = (delta_tsnk+tsrc<T) ? delta_tsnk+tsrc : (delta_tsnk+tsrc)-T;
int t1   = (delta_t1+tsrc<T)   ? delta_t1+tsrc   : (delta_t1+tsrc)-T;


// extra info for the contraction files
std::string extra_info = "";

// basename
std::string base = runid + "_tsrc" + std::to_string(tsrc);
// folder name
std::string folder_output = "output_" + base + "/";
// xml file name
std::string file = base + ".xml";


// GAMMAS //////////////////////////////////////////////////////////////////////

// for contractions
std::vector<std::string> Gammas = {
    "GammaX", "GammaY", "GammaZ", "GammaT",
    "GammaXGamma5", "GammaYGamma5", "GammaZGamma5", "GammaTGamma5" 
};
int NGammas = Gammas.size();

std::vector<std::array<std::string, 2>> RHQImpr = {
  {"X", "Identity"}, {"Y", "Identity"}, {"Z", "Identity"}, {"T", "Identity"}, 
  {"X", "Gamma5"}, {"Y", "Gamma5"}, {"Z", "Gamma5"}, {"T", "Gamma5"}, 
};
int NImpr = RHQImpr.size();

// MOMENTA ///////////////////////////////////////////////////////////////////
// usually either induced through twists or sink

// can be useful to keep mom0 separate
std::array<int, 4> mom0 = {0, 0, 0, 0};
std::array<int, 4> mom_p = {1, 1, 1, 0};
std::array<int, 4> mom_m = {-1, -1, -1, 0};

// twists
std::vector<std::array<double, 4>> twists = {
    {.0, .0, .0, .0},
    {1.0, 1.0, 1.0, .0},
    {-1.0, -1.0, -1.0, .0},
};
int Ntwists = twists.size();

// can be useful to keep twist0 separate
std::array<double, 4> twist0 = {0, 0, 0, 0};

////////////////////////////////////////////////////////////////////////////////
// CUSTOM FUNCTIONS (some templates can be found commented in the API)
////////////////////////////////////////////////////////////////////////////////

std::string make_action_c(Application &application, double mc, std::array<double, 4> twist){
    //name
    std::string name = "action_c_tw_" + make_twist_name(twist);
    // module
    MAction::ScaledDWF::Par action_c;
    action_c.gauge    = "gauge";
    action_c.Ls       = 12;
    action_c.M5       = 1.6;
    action_c.scale    = 2;
    action_c.boundary = "1 1 1 -1";
    action_c.twist    = make_twist_par(twist);
    action_c.mass     = mc;
    application.createModule<MAction::ScaledDWF>(name, action_c);
    return name;
}

////////////////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    // initialise Grid /////////////////////////////////////////////////////////
    Grid_init(&argc, &argv);
    
    // initialise application //////////////////////////////////////////////////

    Application            application;
    Application::GlobalPar globalPar;
    LOG(Message) << "======== XML generation with tsrc=" + std::to_string(tsrc) +" ========" << std::endl;
    
    // global initialisation
    // global parameters
    globalPar.trajCounter.start = 1000;
    globalPar.trajCounter.end   = 1020;
    globalPar.trajCounter.step  = 20;
    globalPar.runId = runid;
    application.setPar(globalPar);   

    // create modules //////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // GAUGE
    ////////////////////////////////////////////////////////////////////////////

    // // load gauge configuration
    // MIO::LoadNersc::Par gauge;
    // gauge.file = "/path/to/config";
    // application.createModule<MIO::LoadNersc>("gauge", gauge);
    
    // unit gauge
    application.createModule<MGauge::Unit>("gauge");

    ////////////////////////////////////////////////////////////////////////////
    // ACTIONS AND SOLVERS
    ////////////////////////////////////////////////////////////////////////////


    // bottom action
    MAction::WilsonClover::Par action_b;
    action_b.gauge = "gauge";
    action_b.mass  = 7.47;
    action_b.csw_r = 4.92;
    action_b.csw_t = 4.92;
    action_b.clover_anisotropy.isAnisotropic = "true";
    action_b.clover_anisotropy.t_direction   = 3;
    action_b.clover_anisotropy.xi_0          = 1;
    action_b.clover_anisotropy.nu            = 2.93;
    action_b.boundary = "1 1 1 -1";
    action_b.twist    = "0. 0. 0. 0.";
    application.createModule<MAction::WilsonClover>("action_b", action_b);

    // bottom solver
    std::string solver_b = ASolver::make_solver_CG(application, "action_b", 1000, 1e-45);


    // strange action
    MAction::DWF::Par action_s;
    action_s.gauge    = "gauge";
    action_s.mass     = 0.3;
    action_s.Ls       = 16;
    action_s.M5       = 1.8;
    action_s.boundary = "1 1 1 -1";
    action_s.twist    = "0. 0. 0. 0.";    
    application.createModule<MAction::DWF>("action_s", action_s);

    // strange solver
    std::string solver_s = ASolver::make_solver_CG(application, "action_s", 8000, 1e-5);


    // ACTION and SOLVER for quark c (twist applied)
    std::vector<std::string> solver_c;
    for (int tw=0; tw<Ntwists; tw++){
        std::string action_c = make_action_c(application, 0.35, twists[tw]);
        solver_c.push_back(ASolver::make_solver_CG(application, action_c, 8000, 1e-20));
    }


    ////////////////////////////////////////////////////////////////////////////
    // SOURCE AND SINK
    ////////////////////////////////////////////////////////////////////////////

    // source
    std::string source = ASource::make_z2_source(application, tsrc, tsrc);
    // // smeared source
    // std::string source_sm = make_sm_source(application, source); 

    // sink
    std::string sink = ASink::make_sink(application, mom0);
    // std::string sink_p = ASink::make_sink(application, mom_p);
    // std::string sink_m = ASink::make_sink(application, mom_m);


    ////////////////////////////////////////////////////////////////////////////
    // PROPAGATORS AND IMPROVEMENTS 
    ////////////////////////////////////////////////////////////////////////////

   
    // QUARK_B_S ///////////////////////////////////////////////////////////////
    // quark_s
    std::string quark_s = AFermion::make_propagator(application, "s", source, solver_s);
    // sequential source on s
    std::string source_seq_s = ASource::make_seq_source(application, quark_s, tsnk, "Gamma5", mom0);
    // sequential propagator for b_s
    std::string quark_b_s = AFermion::make_seq_propagator(application, "b", source_seq_s, solver_b);


    // QUARK_C_B ///////////////////////////////////////////////////////////////
    // quark_b 
    std::string quark_b = AFermion::make_propagator(application, "b", source, solver_b);
    // sequential source for quark_c_b
    std::vector<std::string> source_seq_b;
    // sequential propagator for quark_c_b
    std::vector<std::vector<std::string>> quark_c_b;
    for (unsigned int g=0; g<NGammas; g++){
        // seq source
        source_seq_b.push_back(ASource::make_seq_source(application, quark_b, t1, Gammas[g], mom0));
        // propagator
        std::vector<std::string> quark_c_b_g;
        for (int tw=0; tw<Ntwists; tw++){ 
            quark_c_b_g.push_back(AFermion::make_seq_propagator(application, "c", source_seq_b[g], solver_c[tw]));
        }
        quark_c_b.push_back(quark_c_b_g);
    }

    // IMPROVEMENT ON t2 ///////////////////////////////////////////////////////
    
    // IMPR I/III
    std::vector<std::string> quarkImprI_b_s;
    std::vector<std::string> quarkImprIII_b_s;
    for (int i=0; i<NImpr; i++){
      std::string dir = RHQImpr[i][0];
      std::string gamma = RHQImpr[i][1];
      quarkImprI_b_s.push_back(ARHQ::make_RHQInsertionI(application, quark_b_s, dir, gamma));
      quarkImprIII_b_s.push_back(ARHQ::make_RHQInsertionIII(application, quark_b_s, dir, gamma));
    }
   
    // IMPR II/IV (final vector has structure: 
    // quarkImprII/IV_c_b[i][g][tw] with
    //   - i=improvement
    //   - g=gamma in t1
    //   - tw=twist
    std::vector<std::vector<std::vector<std::string>>> quarkImprII_c_b;
    std::vector<std::vector<std::vector<std::string>>> quarkImprIV_c_b;
    for (int i=0; i<NImpr; i++){
      std::string dir = RHQImpr[i][0];
      std::string gamma = RHQImpr[i][1];

      std::vector<std::vector<std::string>> quarkImprII_c_b_i;
      std::vector<std::vector<std::string>> quarkImprIV_c_b_i;
      for (unsigned int g=0; g<NGammas; g++){
        std::vector<std::string> quarkImprII_c_b_g;
        std::vector<std::string> quarkImprIV_c_b_g;
        for (int tw=0; tw<Ntwists; tw++){
          quarkImprII_c_b_g.push_back(ARHQ::make_RHQInsertionII(application, quark_c_b[g][tw], dir, gamma, twists[tw]));
          quarkImprIV_c_b_g.push_back(ARHQ::make_RHQInsertionIV(application, quark_c_b[g][tw], dir, gamma, twists[tw]));
        }
        quarkImprII_c_b_i.push_back(quarkImprII_c_b_g);
        quarkImprIV_c_b_i.push_back(quarkImprIV_c_b_g);
      }
      quarkImprII_c_b.push_back(quarkImprII_c_b_i);
      quarkImprIV_c_b.push_back(quarkImprIV_c_b_i);
    }

    // IMPROVEMENT ON t1 ///////////////////////////////////////////////////////

    // sequential sources
    std::vector<std::string> source_seq_ImprI_b;
    std::vector<std::vector<std::string>> source_seq_ImprII_b;
    std::vector<std::string> source_seq_ImprIII_b;
    std::vector<std::vector<std::string>> source_seq_ImprIV_b;
    for (int i=0; i<NImpr; i++){
      std::string dir = RHQImpr[i][0];
      std::string gamma = RHQImpr[i][1];
    
      source_seq_ImprI_b.push_back(ARHQ::make_RHQSeqSourceI(application, quark_b, t1, mom0, dir, gamma));
      source_seq_ImprIII_b.push_back(ARHQ::make_RHQSeqSourceIII(application, quark_b, t1, mom0, dir, gamma));

      std::vector<std::string> source_seq_ImprII_b_i;
      std::vector<std::string> source_seq_ImprIV_b_i;
      for (int tw=0; tw<Ntwists; tw++){
        source_seq_ImprII_b_i.push_back(ARHQ::make_RHQSeqSourceII(application, quark_b, t1, twists[tw], dir, gamma, "Twist"));
        source_seq_ImprIV_b_i.push_back(ARHQ::make_RHQSeqSourceIV(application, quark_b, t1, twists[tw], dir, gamma, "Twist"));
      }
      source_seq_ImprII_b.push_back(source_seq_ImprII_b_i);
      source_seq_ImprIV_b.push_back(source_seq_ImprIV_b_i);
    }

    // IMPR propagators
    std::vector<std::vector<std::string>> quark_c_ImprI_b;
    std::vector<std::vector<std::string>> quark_c_ImprII_b;
    std::vector<std::vector<std::string>> quark_c_ImprIII_b;
    std::vector<std::vector<std::string>> quark_c_ImprIV_b;
    for (int i=0; i<NImpr; i++){
      std::vector<std::string> quark_c_ImprI_b_i;
      std::vector<std::string> quark_c_ImprII_b_i;
      std::vector<std::string> quark_c_ImprIII_b_i;
      std::vector<std::string> quark_c_ImprIV_b_i;
      for (int tw=0; tw<Ntwists; tw++){ 
        quark_c_ImprI_b_i.push_back(AFermion::make_seq_propagator(application, "c", source_seq_ImprI_b[i], solver_c[tw]));
        quark_c_ImprII_b_i.push_back(AFermion::make_seq_propagator(application, "c", source_seq_ImprII_b[i][tw], solver_c[tw]));
        quark_c_ImprIII_b_i.push_back(AFermion::make_seq_propagator(application, "c", source_seq_ImprIII_b[i], solver_c[tw]));
        quark_c_ImprIV_b_i.push_back(AFermion::make_seq_propagator(application, "c", source_seq_ImprIV_b[i][tw], solver_c[tw]));
      }
      quark_c_ImprI_b.push_back(quark_c_ImprI_b_i);
      quark_c_ImprII_b.push_back(quark_c_ImprII_b_i);
      quark_c_ImprIII_b.push_back(quark_c_ImprIII_b_i);
      quark_c_ImprIV_b.push_back(quark_c_ImprIV_b_i);
    }

    ////////////////////////////////////////////////////////////////////////////
    // 4PT CONTRACTIONS
    ////////////////////////////////////////////////////////////////////////////
    
    // TREE LEVEL CONTRACTIONS
    for (unsigned int g1=0; g1<NGammas; g1++){
      for (int tw=0; tw<Ntwists; tw++){
        AContraction::make_4pt_contraction(application, quark_b_s, quark_c_b[g1][tw], {Gammas, {"Gamma5"}}, sink, folder_output, extra_info);
      }
    }

    // CONTRACTIONS with IMPR on t2
    for (unsigned int i2=0; i2<NImpr; i2++){
      for (unsigned int g1=0; g1<NGammas; g1++){
        for (int tw=0; tw<Ntwists; tw++){
          AContraction::make_4pt_contraction(application, quarkImprI_b_s[i2], quark_c_b[g1][tw], {"Identity", "Gamma5"}, sink, folder_output, extra_info);
          AContraction::make_4pt_contraction(application, quark_b_s, quarkImprII_c_b[i2][g1][tw], {"Identity", "Gamma5"}, sink, folder_output, extra_info);
          AContraction::make_4pt_contraction(application, quarkImprIII_b_s[i2], quark_c_b[g1][tw], {"Identity", "Gamma5"}, sink, folder_output, extra_info);
          AContraction::make_4pt_contraction(application, quark_b_s, quarkImprIV_c_b[i2][g1][tw], {"Identity", "Gamma5"}, sink, folder_output, extra_info);
        }
      }
    }

    // CONTRACTIONS with IMPR on t1
    for (unsigned int i2=0; i2<NImpr; i2++){
      for (int tw=0; tw<Ntwists; tw++){
        AContraction::make_4pt_contraction(application, quark_b_s, quark_c_ImprI_b[i2][tw], {Gammas, {"Gamma5"}}, sink, folder_output, extra_info);
        AContraction::make_4pt_contraction(application, quark_b_s, quark_c_ImprII_b[i2][tw], {Gammas, {"Gamma5"}}, sink, folder_output, extra_info);
        AContraction::make_4pt_contraction(application, quark_b_s, quark_c_ImprIII_b[i2][tw], {Gammas, {"Gamma5"}}, sink, folder_output, extra_info);
        AContraction::make_4pt_contraction(application, quark_b_s, quark_c_ImprIV_b[i2][tw], {Gammas, {"Gamma5"}}, sink, folder_output, extra_info);
      }
    }
    
    ////////////////////////////////////////////////////////////////////////////
    // END
    ////////////////////////////////////////////////////////////////////////////

    // save xml improvement
    application.saveParameterFile(file);

    // // execution ///////////////////////////////////////////////////////////////
    // try
    // {
    //     application.run();
    // }
    // catch (const std::exception& e)
    // {
    //     Exceptions::abort(e);
    // }
    
    // epilogue ////////////////////////////////////////////////////////////////
    LOG(Message) << "Grid is finalizing now" << std::endl;
    Grid_finalize();
    
    return EXIT_SUCCESS;
}