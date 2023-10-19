#include "Utilities.hpp"

BEGIN_APIMODULE_NAMESPACE(AFermion)

// assign the MFermion::GaugeProp parameters
template <typename TFermion>
void assign_propagator_par(TFermion &quark, std::string source, std::string solver){
    quark.source = source;
    quark.solver = solver;
}



std::string make_propagator_name(std::string name, std::string source, std::string solver){
    
    std::string twist = get_twist(solver);
    std::string twist_name = "";
    if (!twist.empty() && !(twist=="0_0_0_0" || twist=="0.0_0.0_0.0_0.0"))
        twist_name = "_tw_" + twist;

    std::string mom = get_mom(source);
    std::string mom_name = "";
    if (!mom.empty() && !(twist=="0_0_0_0"))
        mom_name = "_mom_" + mom;
    
    std::string quark_source = "";
    if (has_string(source, "_seq"))
        // quark_source = "_" + remove_twist_str(remove_str(remove_str(source, "source_seq_"), "sm_"));
        quark_source = "_" + remove_twist_str(remove_str(remove_str(remove_str(source, "source_"), "seq_"), "sm_"));
    
    std::string name_prop = 
          "quark_" + name 
        + quark_source
        + twist_name;// + mom_name;
    return name_prop;
}

// PROPAGATOR
std::string make_propagator(Application &application, std::string name, std::string source, std::string solver){
    MFermion::GaugeProp::Par quark;
    quark.source = source;
    quark.solver = solver;

    std::string name_prop = make_propagator_name(name, source, solver);
    application.createModule<MFermion::GaugeProp>(name_prop, quark);
    return name_prop;
}

// SEQUENTIAL PROPAGATOR
std::string make_seq_propagator(Application &application, std::string name, std::string seq_source, std::string solver){
    return make_propagator(application, name, seq_source, solver);
}


END_APIMODULE_NAMESPACE