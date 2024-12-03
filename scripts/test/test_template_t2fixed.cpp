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
std::string runid = "testRHQTwist";

// time slices
int tsrc = 0;
int T    = 64; // lattice temporal extent
int L    = 24; // lattice spatial extent
int delta_tsnk = 20; // tsnk-tsrc
int delta_t1   = 6;  // t1-tsrc
int delta_t2   = 14;
  
// compact if -> variable = (condition) ? expressionTrue : expressionFalse;
int tsnk = (delta_tsnk+tsrc<T) ? delta_tsnk+tsrc : (delta_tsnk+tsrc)-T;
int t1   = (delta_t1+tsrc<T)   ? delta_t1+tsrc   : (delta_t1+tsrc)-T;
int t2   = (delta_t2+tsrc<T)   ? delta_t2+tsrc   : (delta_t2+tsrc)-T;


// extra info for the contraction files
std::string extra_info = "";

// basename
std::string base = runid + "_tsrc" + std::to_string(tsrc) + "_t2fixed";
// folder name
std::string folder_output = "../../data/output_" + base + "/";
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
std::array<int, 4> momp = {1, 1, 1, 0};
std::array<int, 4> momm = {-1, -1, -1, 0};

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
// CUSTOM FUNCTIONS
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

    // load gauge configuration
    MIO::LoadNersc::Par gauge;
    gauge.file = "/home/dp207/dp207/shared/data/configs/dwf_2+1f_Coulomb/C1M_Coulomb/ckpoint_lat_coulomb";
    application.createModule<MIO::LoadNersc>("gauge", gauge);
    
    ////////////////////////////////////////////////////////////////////////////
    // ACTIONS AND SOLVERS
    ////////////////////////////////////////////////////////////////////////////


    // bottom action
    MAction::WilsonClover::Par action_b;
    action_b.gauge = "gauge";
    action_b.mass  = 8.464000;
    action_b.csw_r = 5.760000;
    action_b.csw_t = 5.760000;
    action_b.cF    = 1.13142020726806e-310;
    action_b.clover_anisotropy.isAnisotropic = "true";
    action_b.clover_anisotropy.t_direction   = 3;
    action_b.clover_anisotropy.xi_0          = 1;
    action_b.clover_anisotropy.nu            = 2.999000;
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
    std::string solver_s = ASolver::make_solver_CG(application, "action_s", 8000, 1e-15);


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
    std::string sink_momp = ASink::make_sink(application, momp);
    std::string sink_momm = ASink::make_sink(application, momm);

    ////////////////////////////////////////////////////////////////////////////
    // PROPAGATORS AND IMPROVEMENTS
    ////////////////////////////////////////////////////////////////////////////

    // quark_s
    std::string quark_s = AFermion::make_propagator(application, "s", source, solver_s);
    // quark_b
    std::string quark_b = AFermion::make_propagator(application, "b", source, solver_b);

    // sequential source on s
    std::string source_seq_s = ASource::make_seq_source(application, quark_s, tsnk, "Gamma5", mom0);
    // sequential propagator for b_s
    std::string quark_b_s = AFermion::make_seq_propagator(application, "b", source_seq_s, solver_b);


    ////////////////////////////////////////////////////////////////////////////
    // 4pt CONTRACTIONS (T2 FIXED)  
    ////////////////////////////////////////////////////////////////////////////

    // sequential source for cbs
    std::vector<std::string> source_seq_b_s;
    // sequential propagator for cbs
    std::vector<std::vector<std::string>> quark_c_b_s;
    for (unsigned int g=0; g<NGammas; g++){
        // seq source
        source_seq_b_s.push_back(ASource::make_seq_source(application, quark_b_s, t2, Gammas[g], mom0));
        // propagator
        std::vector<std::string> quark_c_b_s_g;
        for (int tw=0; tw<Ntwists; tw++){ 
            quark_c_b_s_g.push_back(AFermion::make_seq_propagator(application, "c_LL", source_seq_b_s[g], solver_c[tw]));
        }
        quark_c_b_s.push_back(quark_c_b_s_g);
    }
    
    // 4pt contractions (t2 fixed)
    for (unsigned int g1=0; g1<NGammas; g1++){
      for (int tw=0; tw<Ntwists; tw++){
          // ALL CONTRACTIONS
          AContraction::make_4pt_contraction(application, quark_c_b_s[g1][tw], quark_b, {Gammas, {"Gamma5"}}, sink, folder_output);
      }
    }

    ////////////////////////////////////////////////////////////////////////////
    // IMPROVED TERMS     
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // t2 fixed - improvement on t2
    //////////////////////////////////////////////////////////////////////////// 

    // improve quark_cImpr#D_bs
    std::vector<std::vector<std::string>> quark_c_ImprI_b_s;
    std::vector<std::vector<std::string>> quark_c_ImprII_b_s;
    std::vector<std::vector<std::string>> quark_c_ImprIII_b_s;
    std::vector<std::vector<std::string>> quark_c_ImprIV_b_s;
    for (unsigned int i=0; i<NImpr; i++){
      std::string dir = RHQImpr[i][0];
      std::string gamma = RHQImpr[i][1];

      std::string source_seq_ImprI_b_s_i = ARHQ::make_RHQSeqSourceI(application, quark_b_s, t2, mom0, dir, gamma);
      std::string source_seq_ImprIII_b_s_i = ARHQ::make_RHQSeqSourceIII(application, quark_b_s, t2, mom0, dir, gamma);

      std::vector<std::string> source_seq_ImprII_b_s_i;
      std::vector<std::string> source_seq_ImprIV_b_s_i;
      for (int tw=0; tw<Ntwists; tw++){
        source_seq_ImprII_b_s_i.push_back(ARHQ::make_RHQSeqSourceII(application, quark_b_s, t2, twists[tw], dir, gamma, "Twist"));
        source_seq_ImprIV_b_s_i.push_back(ARHQ::make_RHQSeqSourceIV(application, quark_b_s, t2, twists[tw], dir, gamma, "Twist"));
      }

      // auxiliary quark for every index i
      std::vector<std::string> quark_c_ImprI_b_s_i;
      std::vector<std::string> quark_c_ImprII_b_s_i;
      std::vector<std::string> quark_c_ImprIII_b_s_i;
      std::vector<std::string> quark_c_ImprIV_b_s_i;
      for (int tw=0; tw<Ntwists; tw++){
        // quarks for every i
        quark_c_ImprI_b_s_i.push_back(AFermion::make_seq_propagator(application, "c", source_seq_ImprI_b_s_i, solver_c[tw]));
        quark_c_ImprII_b_s_i.push_back(AFermion::make_seq_propagator(application, "c", source_seq_ImprII_b_s_i[tw], solver_c[tw]));
        quark_c_ImprIII_b_s_i.push_back(AFermion::make_seq_propagator(application, "c", source_seq_ImprIII_b_s_i, solver_c[tw]));
        quark_c_ImprIV_b_s_i.push_back(AFermion::make_seq_propagator(application, "c", source_seq_ImprIV_b_s_i[tw], solver_c[tw]));        
      }
      // collect propagators into vector
      quark_c_ImprI_b_s.push_back(quark_c_ImprI_b_s_i);
      quark_c_ImprII_b_s.push_back(quark_c_ImprII_b_s_i);
      quark_c_ImprIII_b_s.push_back(quark_c_ImprIII_b_s_i);
      quark_c_ImprIV_b_s.push_back(quark_c_ImprIV_b_s_i);
    }

    ////////////////////////////////////////////////////////////////////////////
    // t2 fixed - improvement on t1
    //////////////////////////////////////////////////////////////////////////// 


    // improvement quark_Impr#D_b_s
    std::vector<std::vector<std::vector<std::string>>> quark_ImprI_c_b_s;
    std::vector<std::vector<std::vector<std::string>>> quark_ImprIII_c_b_s;

    for (unsigned int g=0; g<NGammas; g++){
      // auxiliary for g
      std::vector<std::vector<std::string>> quark_ImprI_c_b_s_g;
      std::vector<std::vector<std::string>> quark_ImprIII_c_b_s_g;

      for (int tw=0; tw<Ntwists; tw++){
        // auxiliary for tw
        std::vector<std::string> quark_ImprI_c_b_s_tw;
        std::vector<std::string> quark_ImprIII_c_b_s_tw;

        for (unsigned int i=0; i<NImpr; i++){
          std::string dir = RHQImpr[i][0];
          std::string gamma = RHQImpr[i][1];
          // quarks for every i
          quark_ImprI_c_b_s_tw.push_back(ARHQ::make_RHQInsertionI(application, quark_c_b_s[g][tw], dir, gamma, twists[tw]));
          quark_ImprIII_c_b_s_tw.push_back(ARHQ::make_RHQInsertionIII(application, quark_c_b_s[g][tw], dir, gamma, twists[tw]));
        }
        // push back
        quark_ImprI_c_b_s_g.push_back(quark_ImprI_c_b_s_tw);
        quark_ImprIII_c_b_s_g.push_back(quark_ImprIII_c_b_s_tw);
      }
      // push back
      quark_ImprI_c_b_s.push_back(quark_ImprI_c_b_s_g);
        quark_ImprIII_c_b_s.push_back(quark_ImprIII_c_b_s_g);
    }

    // improvement quark_Impr#D_b
    std::vector<std::string> quark_ImprII_b;
    std::vector<std::string> quark_ImprIV_b;
    for (int i=0; i<NImpr; i++){
      std::string dir = RHQImpr[i][0];
      std::string gamma = RHQImpr[i][1];
      quark_ImprII_b.push_back(ARHQ::make_RHQInsertionII(application, quark_b, dir, gamma));
      quark_ImprIV_b.push_back(ARHQ::make_RHQInsertionIV(application, quark_b, dir, gamma));
    }

    ////////////////////////////////////////////////////////////////////////////
    // IMPROVED CONTRACTIONS
    ////////////////////////////////////////////////////////////////////////////

    for (unsigned int g=0; g<NGammas; g++){
      for (unsigned int i=0; i<NImpr; i++){ 
        for (int tw=0; tw<Ntwists; tw++){
          // improvement on t1
          AContraction::make_4pt_contraction(application, quark_ImprI_c_b_s[g][tw][i], quark_b, {"Identity", "Gamma5"}, sink, folder_output, "");
          AContraction::make_4pt_contraction(application, quark_c_b_s[g][tw], quark_ImprII_b[i], {"Identity", "Gamma5"}, sink, folder_output, "");
          AContraction::make_4pt_contraction(application, quark_ImprIII_c_b_s[g][tw][i], quark_b, {"Identity", "Gamma5"}, sink, folder_output, "");
          AContraction::make_4pt_contraction(application, quark_c_b_s[g][tw], quark_ImprIV_b[i], {"Identity", "Gamma5"}, sink, folder_output, "");
        }
      }
    }

    for (unsigned int i=0; i<NImpr; i++){ 
      for (int tw=0; tw<Ntwists; tw++){
        // improvement on t2
        AContraction::make_4pt_contraction(application, quark_c_ImprI_b_s[i][tw], quark_b, {Gammas, {"Gamma5"}}, sink, folder_output);
        AContraction::make_4pt_contraction(application, quark_c_ImprII_b_s[i][tw], quark_b, {Gammas, {"Gamma5"}}, sink, folder_output);
        AContraction::make_4pt_contraction(application, quark_c_ImprIII_b_s[i][tw], quark_b, {Gammas, {"Gamma5"}}, sink, folder_output);
        AContraction::make_4pt_contraction(application, quark_c_ImprIV_b_s[i][tw], quark_b, {Gammas, {"Gamma5"}}, sink, folder_output);
      }
    }

    ////////////////////////////////////////////////////////////////////////////
    // CROSS-CHECK WITH SINK MOM
    ////////////////////////////////////////////////////////////////////////////

    // sequential propagator for quark_c_b_s
    std::vector<std::string> quark_c_b_s_momp;
    std::vector<std::string> quark_c_b_s_momm;

    // impr on t2
    std::vector<std::string> quark_c_ImprI_b_s_momp;
    std::vector<std::string> quark_c_ImprII_b_s_momp;
    std::vector<std::string> quark_c_ImprIII_b_s_momp;
    std::vector<std::string> quark_c_ImprIV_b_s_momp;

    std::vector<std::string> quark_c_ImprI_b_s_momm;
    std::vector<std::string> quark_c_ImprII_b_s_momm;
    std::vector<std::string> quark_c_ImprIII_b_s_momm;
    std::vector<std::string> quark_c_ImprIV_b_s_momm;
  
    for (unsigned int g=0; g<NGammas; g++){
      std::string source_seq_b_s_momp = ASource::make_seq_source(application, quark_b_s, t2, Gammas[g], momp);
      std::string source_seq_b_s_momm = ASource::make_seq_source(application, quark_b_s, t2, Gammas[g], momm);

      // seq source impr
      std::string dir = RHQImpr[g][0];
      std::string gamma = RHQImpr[g][1];

      std::string source_seq_ImprI_b_s_momp = ARHQ::make_RHQSeqSourceI(application, quark_b_s, t2, momp, dir, gamma);
      std::string source_seq_ImprII_b_s_momp = ARHQ::make_RHQSeqSourceII(application, quark_b_s, t2, momp, dir, gamma, "Sink");
      std::string source_seq_ImprIII_b_s_momp = ARHQ::make_RHQSeqSourceIII(application, quark_b_s, t2, momp, dir, gamma);
      std::string source_seq_ImprIV_b_s_momp = ARHQ::make_RHQSeqSourceIV(application, quark_b_s, t2, momp, dir, gamma, "Sink");

      std::string source_seq_ImprI_b_s_momm = ARHQ::make_RHQSeqSourceI(application, quark_b_s, t2, momm, dir, gamma);
      std::string source_seq_ImprII_b_s_momm = ARHQ::make_RHQSeqSourceII(application, quark_b_s, t2, momm, dir, gamma, "Sink");
      std::string source_seq_ImprIII_b_s_momm = ARHQ::make_RHQSeqSourceIII(application, quark_b_s, t2, momm, dir, gamma);
      std::string source_seq_ImprIV_b_s_momm = ARHQ::make_RHQSeqSourceIV(application, quark_b_s, t2, momm, dir, gamma, "Sink");

      quark_c_b_s_momp.push_back(AFermion::make_seq_propagator(application, "c", source_seq_b_s_momp, solver_c[0]));
      quark_c_b_s_momm.push_back(AFermion::make_seq_propagator(application, "c", source_seq_b_s_momm, solver_c[0]));

      quark_c_ImprI_b_s_momp.push_back(AFermion::make_seq_propagator(application, "c", source_seq_ImprI_b_s_momp, solver_c[0]));
      quark_c_ImprII_b_s_momp.push_back(AFermion::make_seq_propagator(application, "c", source_seq_ImprII_b_s_momp, solver_c[0]));
      quark_c_ImprIII_b_s_momp.push_back(AFermion::make_seq_propagator(application, "c", source_seq_ImprIII_b_s_momp, solver_c[0]));
      quark_c_ImprIV_b_s_momp.push_back(AFermion::make_seq_propagator(application, "c", source_seq_ImprIV_b_s_momp, solver_c[0]));

      quark_c_ImprI_b_s_momm.push_back(AFermion::make_seq_propagator(application, "c", source_seq_ImprI_b_s_momm, solver_c[0]));
      quark_c_ImprII_b_s_momm.push_back(AFermion::make_seq_propagator(application, "c", source_seq_ImprII_b_s_momm, solver_c[0]));
      quark_c_ImprIII_b_s_momm.push_back(AFermion::make_seq_propagator(application, "c", source_seq_ImprIII_b_s_momm, solver_c[0]));
      quark_c_ImprIV_b_s_momm.push_back(AFermion::make_seq_propagator(application, "c", source_seq_ImprIV_b_s_momm, solver_c[0]));

    }


    // impr on t1
    std::vector<std::vector<std::string>> quark_ImprI_c_b_s_momp;
    std::vector<std::vector<std::string>> quark_ImprIII_c_b_s_momp;
    std::vector<std::vector<std::string>> quark_ImprI_c_b_s_momm;
    std::vector<std::vector<std::string>> quark_ImprIII_c_b_s_momm;
    for (unsigned int g2=0; g2<NGammas; g2++){
      // impr on t1
      std::vector<std::string> quark_ImprI_c_b_s_momp_g2;
      std::vector<std::string> quark_ImprIII_c_b_s_momp_g2;
      std::vector<std::string> quark_ImprI_c_b_s_momm_g2;
      std::vector<std::string> quark_ImprIII_c_b_s_momm_g2;
      for (unsigned int i1=0; i1<NImpr; i1++){ 
        std::string dir = RHQImpr[i1][0];
        std::string gamma = RHQImpr[i1][1];

        quark_ImprI_c_b_s_momp_g2.push_back(ARHQ::make_RHQInsertionI(application, quark_c_b_s_momp[g2], dir, gamma));
        quark_ImprIII_c_b_s_momp_g2.push_back(ARHQ::make_RHQInsertionIII(application, quark_c_b_s_momp[g2], dir, gamma));

        quark_ImprI_c_b_s_momm_g2.push_back(ARHQ::make_RHQInsertionI(application, quark_c_b_s_momm[g2], dir, gamma));
        quark_ImprIII_c_b_s_momm_g2.push_back(ARHQ::make_RHQInsertionIII(application, quark_c_b_s_momm[g2], dir, gamma));
      }

      quark_ImprI_c_b_s_momp.push_back(quark_ImprI_c_b_s_momp_g2);
      quark_ImprIII_c_b_s_momp.push_back(quark_ImprIII_c_b_s_momp_g2);

      quark_ImprI_c_b_s_momm.push_back(quark_ImprI_c_b_s_momm_g2);
      quark_ImprIII_c_b_s_momm.push_back(quark_ImprIII_c_b_s_momm_g2);

    }

    // 4pt CONTRACTIONS
    // TREE LEVEL CONTRACTIONS
    for (unsigned int g2=0; g2<NGammas; g2++){
      AContraction::make_4pt_contraction(application, quark_c_b_s_momp[g2], quark_b, {Gammas, {"Gamma5"}}, sink_momp, folder_output);
      AContraction::make_4pt_contraction(application, quark_c_b_s_momp[g2], quark_b, {Gammas, {"Gamma5"}}, sink_momm, folder_output);
      AContraction::make_4pt_contraction(application, quark_c_b_s_momm[g2], quark_b, {Gammas, {"Gamma5"}}, sink_momp, folder_output);
      AContraction::make_4pt_contraction(application, quark_c_b_s_momm[g2], quark_b, {Gammas, {"Gamma5"}}, sink_momm, folder_output);
    }

    // CONTRACTIONS WITH IMPR ON T2
    for (unsigned int i2=0; i2<NImpr; i2++){
      AContraction::make_4pt_contraction(application, quark_c_ImprI_b_s_momp[i2], quark_b, {Gammas, {"Gamma5"}}, sink_momp, folder_output);
      AContraction::make_4pt_contraction(application, quark_c_ImprII_b_s_momp[i2], quark_b, {Gammas, {"Gamma5"}}, sink_momp, folder_output);
      AContraction::make_4pt_contraction(application, quark_c_ImprIII_b_s_momp[i2], quark_b, {Gammas, {"Gamma5"}}, sink_momp, folder_output);
      AContraction::make_4pt_contraction(application, quark_c_ImprIV_b_s_momp[i2], quark_b, {Gammas, {"Gamma5"}}, sink_momp, folder_output);

      AContraction::make_4pt_contraction(application, quark_c_ImprI_b_s_momp[i2], quark_b, {Gammas, {"Gamma5"}}, sink_momm, folder_output);
      AContraction::make_4pt_contraction(application, quark_c_ImprII_b_s_momp[i2], quark_b, {Gammas, {"Gamma5"}}, sink_momm, folder_output);
      AContraction::make_4pt_contraction(application, quark_c_ImprIII_b_s_momp[i2], quark_b, {Gammas, {"Gamma5"}}, sink_momm, folder_output);
      AContraction::make_4pt_contraction(application, quark_c_ImprIV_b_s_momp[i2], quark_b, {Gammas, {"Gamma5"}}, sink_momm, folder_output);

      AContraction::make_4pt_contraction(application, quark_c_ImprI_b_s_momm[i2], quark_b, {Gammas, {"Gamma5"}}, sink_momp, folder_output);
      AContraction::make_4pt_contraction(application, quark_c_ImprII_b_s_momm[i2], quark_b, {Gammas, {"Gamma5"}}, sink_momp, folder_output);
      AContraction::make_4pt_contraction(application, quark_c_ImprIII_b_s_momm[i2], quark_b, {Gammas, {"Gamma5"}}, sink_momp, folder_output);
      AContraction::make_4pt_contraction(application, quark_c_ImprIV_b_s_momm[i2], quark_b, {Gammas, {"Gamma5"}}, sink_momp, folder_output);

      AContraction::make_4pt_contraction(application, quark_c_ImprI_b_s_momm[i2], quark_b, {Gammas, {"Gamma5"}}, sink_momm, folder_output);
      AContraction::make_4pt_contraction(application, quark_c_ImprII_b_s_momm[i2], quark_b, {Gammas, {"Gamma5"}}, sink_momm, folder_output);
      AContraction::make_4pt_contraction(application, quark_c_ImprIII_b_s_momm[i2], quark_b, {Gammas, {"Gamma5"}}, sink_momm, folder_output);
      AContraction::make_4pt_contraction(application, quark_c_ImprIV_b_s_momm[i2], quark_b, {Gammas, {"Gamma5"}}, sink_momm, folder_output);
    }

    // CONTRACTIONS WITH IMPR ON T1
     for (unsigned int g2=0; g2<NGammas; g2++){
      for (unsigned int i=0; i<NImpr; i++){ 
        AContraction::make_4pt_contraction(application, quark_ImprI_c_b_s_momp[g2][i], quark_b, {"Identity", "Gamma5"}, sink_momp, folder_output, "");
        AContraction::make_4pt_contraction(application, quark_c_b_s_momp[g2], quark_ImprII_b[i], {"Identity", "Gamma5"}, sink_momp, folder_output, "");
        AContraction::make_4pt_contraction(application, quark_ImprIII_c_b_s_momp[g2][i], quark_b, {"Identity", "Gamma5"}, sink_momp, folder_output, "");
        AContraction::make_4pt_contraction(application, quark_c_b_s_momp[g2], quark_ImprIV_b[i], {"Identity", "Gamma5"}, sink_momp, folder_output, "");

        AContraction::make_4pt_contraction(application, quark_ImprI_c_b_s_momp[g2][i], quark_b, {"Identity", "Gamma5"}, sink_momm, folder_output, "");
        AContraction::make_4pt_contraction(application, quark_c_b_s_momp[g2], quark_ImprII_b[i], {"Identity", "Gamma5"}, sink_momm, folder_output, "");
        AContraction::make_4pt_contraction(application, quark_ImprIII_c_b_s_momp[g2][i], quark_b, {"Identity", "Gamma5"}, sink_momm, folder_output, "");
        AContraction::make_4pt_contraction(application, quark_c_b_s_momp[g2], quark_ImprIV_b[i], {"Identity", "Gamma5"}, sink_momm, folder_output, "");

        AContraction::make_4pt_contraction(application, quark_ImprI_c_b_s_momm[g2][i], quark_b, {"Identity", "Gamma5"}, sink_momp, folder_output, "");
        AContraction::make_4pt_contraction(application, quark_c_b_s_momm[g2], quark_ImprII_b[i], {"Identity", "Gamma5"}, sink_momp, folder_output, "");
        AContraction::make_4pt_contraction(application, quark_ImprIII_c_b_s_momm[g2][i], quark_b, {"Identity", "Gamma5"}, sink_momp, folder_output, "");
        AContraction::make_4pt_contraction(application, quark_c_b_s_momm[g2], quark_ImprIV_b[i], {"Identity", "Gamma5"}, sink_momp, folder_output, "");

        AContraction::make_4pt_contraction(application, quark_ImprI_c_b_s_momm[g2][i], quark_b, {"Identity", "Gamma5"}, sink_momm, folder_output, "");
        AContraction::make_4pt_contraction(application, quark_c_b_s_momm[g2], quark_ImprII_b[i], {"Identity", "Gamma5"}, sink_momm, folder_output, "");
        AContraction::make_4pt_contraction(application, quark_ImprIII_c_b_s_momm[g2][i], quark_b, {"Identity", "Gamma5"}, sink_momm, folder_output, "");
        AContraction::make_4pt_contraction(application, quark_c_b_s_momm[g2], quark_ImprIV_b[i], {"Identity", "Gamma5"}, sink_momm, folder_output, "");
      }
    }


    ////////////////////////////////////////////////////////////////////////////
    // END
    ////////////////////////////////////////////////////////////////////////////
    // save xml improvement
    application.saveParameterFile(file);
    
    // epilogue ////////////////////////////////////////////////////////////////
    LOG(Message) << "Grid is finalizing now" << std::endl;
    Grid_finalize();
    
    return EXIT_SUCCESS;
}
