#include "Utilities.hpp"

// just to collect the usual actions and parameters

BEGIN_APIMODULE_NAMESPACE(AAction)

std::string make_action_name(std::string quark, std::array<float, 4> twist){
    std::string twist_name = make_twist_name(twist);
    std::string action_name = "action_" + quark;
    if (twist!=std::array<float, 4>{0, 0, 0, 0})
        action_name += "_tw_" + twist_name;
    return action_name;
}

// action_b
// std::string make_action_b(Application &application, std::array<float, 4> twist = {0, 0, 0, 0}){
//     std::string action_b_name = make_action_name("b", twist);
//     MAction::WilsonClover::Par action_b;
//     action_b.gauge = "gauge";
//     action_b.mass  = ;
//     action_b.csw_r = ;
//     action_b.csw_t = ;
//     action_b.clover_anisotropy.isAnisotropic = "true";
//     action_b.clover_anisotropy.t_direction   = ;
//     action_b.clover_anisotropy.xi_0          = ;
//     action_b.clover_anisotropy.nu            = ;
//     action_b.boundary = "1 1 1 -1";
//     action_b.twist    = make_twist_par(twist);
//     application.createModule<MAction::WilsonClover>(action_b_name, action_b);
//     return action_b_name;
// }

// action_c
std::string make_action_c(Application &application, double mass, std::array<float, 4> twist = {0., 0., 0., 0.}){
    // mass = 0.35, 0.40
    std::string action_c_name = make_action_name("c", twist);
    MAction::ScaledDWF::Par action_c;
    action_c.gauge    = "gauge";
    action_c.Ls       = 12;
    action_c.M5       = 1.6;
    action_c.scale    = 2;
    action_c.boundary = "1 1 1 -1";
    action_c.twist    = make_twist_par(twist);
    action_c.mass = mass;
    application.createModule<MAction::ScaledDWF>(action_c_name, action_c);
    return action_c_name;
}

template <typename TDWF>
void assign_dwf_par(TDWF &action_dwf, std::array<float, 4> twist){
    action_dwf.gauge    = "gauge";
    action_dwf.Ls       = 16;
    action_dwf.M5       = 1.8;
    action_dwf.boundary = "1 1 1 -1";
    action_dwf.twist    = make_twist_par(twist);
}

std::string make_action_l(Application &application, double mass, std::array<float, 4> twist = {0., 0., 0., 0.}){
    std::string action_l_name = make_action_name("l", twist);
    MAction::DWF::Par action_l;
    assign_dwf_par(action_l, twist);

    action_l.mass = mass;
    application.createModule<MAction::DWF>(action_l_name, action_l);
    return action_l_name;
}

std::string make_action_s(Application &application, double mass, std::array<float, 4> twist = {0., 0., 0., 0.}){
    // usual mass is 0.03224
    std::string action_s_name = make_action_name("s", twist);
    MAction::DWF::Par action_s;
    assign_dwf_par(action_s, twist);

    action_s.mass = mass;
    application.createModule<MAction::DWF>(action_s_name, action_s);
    return action_s_name;
}

END_APIMODULE_NAMESPACE