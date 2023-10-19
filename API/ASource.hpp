#include "Utilities.hpp"


BEGIN_APIMODULE_NAMESPACE(ASource)

// point
std::string make_pt_source(Application &application, std::string pos){
    std::string source_name = "source_pt_" + space_to_underscore(pos);
    MSource::Point::Par source;
    source.position = pos;
    application.createModule<MSource::Point>(source_name, source);
    return source_name;
}

// z2 source
std::string make_z2_source(Application &application, int tA, int tB){
    std::string source_z2_name = "source_z2";
    MSource::Z2::Par source_z2;
    source_z2.tA = tA;
    source_z2.tB = tB;
    application.createModule<MSource::Z2>(source_z2_name, source_z2);
    return source_z2_name;
}

// // smeared source (just template)
// std::string make_sm_source(Application &application, std::string source){
//     std::string source_sm_name = "source_sm" + remove_str(source, "source");
//     MSource::JacobiSmear::Par source_sm;
//     source_sm.gauge      = "gauge";
//     source_sm.width      = 7.86;
//     source_sm.iterations = 100;
//     source_sm.orthog     = 3;
//     source_sm.source     = source;
//     application.createModule<MSource::JacobiSmear>(source_sm_name, source_sm);
//     return source_sm_name;
// }

// // smeared propagator (just template)
// std::string make_sm_propagator(Application &application, std::string name, std::string source){
//     MSource::JacobiSmear::Par source_sm;
//     source_sm.gauge      = "gauge";
//     source_sm.width      = 7.86;
//     source_sm.iterations = 100;
//     source_sm.orthog     = 3;
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


// SEQ SOURCE
std::string make_seq_source(Application &application, std::string q, int t, std::string gamma, std::array<int, 4> mom){
    MSource::SeqGamma::Par source_seq;

    Gamma::Algebra gamma_alg = strToVec<Gamma::Algebra>(gamma)[0];
    std::string mom_par = make_mom_par(mom);
    std::string mom_str = make_mom_name(mom);

    source_seq.q     = q;
    source_seq.tA    = t;
    source_seq.tB    = t;
    source_seq.gamma = gamma_alg;
    source_seq.mom   = mom_par;

    std::string mom_name = "";
    if (mom_str != "0_0_0_0")
        mom_name = "_mom_" + mom_str;

    std::string source_seq_name = "source_seq_"
                                  + gamma 
                                  + mom_name + "_"
                                  + remove_str(source_seq.q, "quark_");
    application.createModule<MSource::SeqGamma>(source_seq_name, source_seq);
    return source_seq_name;
}

END_APIMODULE_NAMESPACE