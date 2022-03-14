#include "Utilities.hpp"

BEGIN_API_NAMESPACE
BEGIN_APIMODULE_NAMESPACE("Fermion")

// assign the MFermion::GaugeProp parameters
template <typename TFermion>
void assign_propagator_par(TFermion &quark, std::string source, std::string solver){
    quark.source = source;
    quark.solver = solver;
}

// // make the name for the propagator
// template <typename TFermioin>
// std::string make_propagator_name(TFermion quark, std::string incipit){

// }

// PROPAGATOR
std::string make_propagator(Application &application, std::string name, std::string source, std::string solver){
    MFermion::GaugeProp::Par quark;

    quark.source = source;
    quark.solver = solver;

    std::string twist = get_twist(solver);
    std::string name_prop;
    if (twist.empty())
        name_prop = "quark_" + name;
    else
        name_prop = "quark_" + name + "_" + twist;
    application.createModule<MFermion::GaugeProp>(name_prop, quark);
    return name_prop;
}

// SEQUENTIAL PROPAGATOR
std::string make_seq_propagator(Application &application, std::string name, std::string seq_source, std::string solver){
    MFermion::GaugeProp::Par quark;

    quark.source = seq_source;
    quark.solver = solver;

    std::string twist;
    if (get_twist(solver).empty())
        twist = "";
    else
        twist = "_" + get_twist(solver);
    
    std::string name_prop = "quark_" + name + "_" + remove_twist_str(remove_str(remove_str(seq_source, "source_seq_"), "sm_")) + twist;
    application.createModule<MFermion::GaugeProp>(name_prop, quark);
    return name_prop;
}


END_APIMODULE_NAMESPACE