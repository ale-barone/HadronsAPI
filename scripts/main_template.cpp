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
std::string runid = "runID";

// time slices
int tsrc = 0;
int T    = 64; // lattice temporal extent
int L    = 24; // lattice spatial extent
int delta_tsnk = 20; // tsnk-tsrc
  
// compact if -> variable = (condition) ? expressionTrue : expressionFalse;
int tsnk = (delta_tsnk+tsrc<T) ? delta_tsnk+tsrc : (delta_tsnk+tsrc)-T;

// extra info for the contraction files
std::string extra_info = "L" + std::to_string(L);

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

// MOMENTA ///////////////////////////////////////////////////////////////////
// usually either induced through twists or sink

// momenta 
std::vector<std::array<int, 4>> mom = {
    {0, 0, 0, 0},
    {1, 1, 1, 0},
};
int Nmom = mom.size();
// can be useful to keep mom0 separate
std::array<int, 4> mom0 = {0, 0, 0, 0};

// twists
std::vector<std::array<float, 4>> twists = {
    {.0, .0, .0, .0},
    {1.0, 1.0, 1.0, .0},
};
int Ntwists = twists.size();
// can be useful to keep twist0 separate
std::array<float, 4> twist0 = {0, 0, 0, 0};


////////////////////////////////////////////////////////////////////////////////
// CUSTOM FUNCTIONS (some templates can be found commented in the API)
////////////////////////////////////////////////////////////////////////////////

// // smeared source (example)
// std::string make_sm_source(Application &application, std::string source){
//     std::string source_sm_name = "source_sm" + remove_str(source, "source");
//     MSource::JacobiSmear::Par source_sm;
//     source_sm.gauge      = "gauge";
//     source_sm.width      = ;
//     source_sm.iterations = ;
//     source_sm.orthog     = ;
//     source_sm.source     = source;
//     application.createModule<MSource::JacobiSmear>(source_sm_name, source_sm);
//     return source_sm_name;
// }

// // smeared propagator (example)
// std::string make_sm_propagator(Application &application, std::string name, std::string source){
//     MSource::JacobiSmear::Par source_sm;
//     source_sm.gauge      = "gauge";
//     source_sm.width      = ;
//     source_sm.iterations = ;
//     source_sm.orthog     = ;
//     source_sm.source     = source;

//     std::string twist = get_twist(source);
//     std::string twist_name = "";
//     std::string mom = get_mom(source);
//     std::string mom_name = "";
    
//     if (!twist.empty())
//         twist_name = "_tw_" + twist;
//     if (!mom.empty())
//         mom_name = "_mom_" + mom;
    
//     std::string source_sm_name = 
//           "quark_" + name 
//         + twist_name + mom_name;

//     application.createModule<MSource::JacobiSmear>(source_sm_name, source_sm);
//     return source_sm_name;
// }

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

    // light action
    std::string action_l = AAction::make_action_l(application, 0.005);

    // strange action
    MAction::DWF::Par action_s;
    action_s.gauge    = "gauge";
    action_s.mass     = 0.3;
    action_s.Ls       = 16;
    action_s.M5       = 1.8;
    action_s.boundary = "1 1 1 -1";
    action_s.twist    = "0. 0. 0. 0.";    
    application.createModule<MAction::DWF>("action_s", action_s);

    // light solver
    std::string solver_l = ASolver::make_solver_CG(application, "action_l", 8000, 1e-5);

    // strange solver
    std::string solver_s = ASolver::make_solver_CG(application, "action_s", 8000, 1e-5);


    // EXAMPLE OF ACTION AND SOLVER WITH TWISTS
    // std::vector<std::string> solver_q;
    // for (int tw=0; tw<Ntwists; tw++){
    //     std::string action_q = AAction::make_action_c(application, 0.35, twists[tw]);
    //     solver_q.push_back(ASolver::make_solver_CG(application, action_q, 8000, 1e-15));
    // }

    ////////////////////////////////////////////////////////////////////////////
    // SOURCE AND SINK
    ////////////////////////////////////////////////////////////////////////////

    // source
    std::string source = ASource::make_z2_source(application, tsrc, tsrc);
    // // smeared source
    // std::string source_sm = make_sm_source(application, source); 

    // // sink
    // std::string sink0 = ASink::make_sink(application, mom0);
    // sink for multiple momenta
    std::vector<std::string> sink;
    for (int m=0; m<Nmom; m++){
        sink.push_back(ASink::make_sink(application, mom[m]));
    }

    ////////////////////////////////////////////////////////////////////////////
    // PROPAGATORS AND 2PT CONTRACTIONS
    ////////////////////////////////////////////////////////////////////////////

    // // EXAMPLES OF PROPAGATORS WITH DIFFERENT SMEARINGS
    // std::string quark_q_LL = AFermion::make_propagator(application, "q_LL", source_z2, solver_q);
    // std::string quark_q_LS = AFermion::make_propagator(application, "q_LS", source_sm, solver_q);
    // std::string quark_q_SL = make_sm_propagator(application, "q_SL", quark_q_LL);
    // std::string quark_q_SS = make_sm_propagator(application, "q_SS", quark_q_LS);

    // // EXAMPLE OF PROPAGATORS WITH TWISTS
    // std::vector<std::string> quark_q_LL;
    // for (int tw=0; tw<Ntwists; t++){
    //     quark_q_LL.push_back(AFermion::make_propagator(application, "q_LL", source_z2, solver_q[tw]););
    // }


    // quark_l_LL
    std::string quark_l_LL = AFermion::make_propagator(application, "l_LL", source, solver_l);
    // quark_s_LL
    std::string quark_s_LL = AFermion::make_propagator(application, "s_LL", source, solver_s);
    
    // contraction
    for (int m=0; m<Nmom; m++){
        AContraction::make_2pt_contraction(application, quark_l_LL, quark_l_LL, {"Gamma5", "Gamma5"}, sink[m], folder_output, extra_info);
        AContraction::make_2pt_contraction(application, quark_l_LL, quark_s_LL, {"Gamma5", "Gamma5"}, sink[m], folder_output, extra_info);
    }
    
    ////////////////////////////////////////////////////////////////////////////
    // PROPAGATORS AND 3PT CONTRACTIONS
    ////////////////////////////////////////////////////////////////////////////

    // sequential source on s_LL
    std::string source_seq_s_LL = ASource::make_seq_source(application, quark_s_LL, tsnk, "Gamma5", mom0);
    // sequential propagator for l_LL_s_LL
    std::string quark_l_LL_s_LL = AFermion::make_seq_propagator(application, "l_LL", source_seq_s_LL, solver_l);

    // // sequential source on s_LL smeared
    // std::string source_seq_sm_s_LL = make_sm_source(application, source_seq_s_LL);
    // // sequential propagator for l_LS_s_LL
    // std::string quark_l_LS_s_LL = AFermion::make_seq_propagator(application, "l_LS", source_seq_sm_s_LL, solver_b);


    // contraction
    for (int m=0; m<Nmom; m++){
        AContraction::make_3pt_contraction(application, quark_l_LL, quark_l_LL_s_LL, {Gammas, {"Gamma5"}}, sink[m], folder_output, extra_info);  
    }
  
    // // contraction with twisted propagator
    // std::vector<std::string> quark_c_LS_s_LL;
    // for (int tw=0; tw<Ntwists; tw++){
    //     quark_q_LS_l_LL.push_back(AFermion::make_seq_propagator(application, "q_LS", source_seq_sm_l_LL, solver_q[tw]));
    //     // Bs->Ds
    //     AContraction::make_3pt_contraction(application, quark_q_LS_l_LL[tw], quark_l_LL, {Gammas, {"Gamma5"}}, sink0, folder_output, extra_info);  
    // }

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