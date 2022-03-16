#include "Utilities.hpp"

BEGIN_APIMODULE_NAMESPACE(AContraction)

template <typename TContraction>
void assign_contraction_par(TContraction &contraction, std::string q1, std::string q2, std::array<std::string, 2> gammas, std::string sink){
    contraction.q1 = q1;
    contraction.q2 = q2;
    std::string gamma_snk = gammas[0];
    std::string gamma_src = gammas[1];
    contraction.gammas = "(" + gamma_snk + " " + gamma_src + ")";
    contraction.sink = sink;
}

// general 2pt contraction
std::string make_2pt_contraction(Application &application, std::string output_folder, std::string q1, std::string q2, std::array<std::string, 2> gammas, std::string sink){
    MContraction::Meson::Par contraction;
    assign_contraction_par(contraction, q1, q2, gammas, sink);
    
    std::string snkmom = get_snkmom(sink);
    std::string contraction_name = "2pt_snkmom_" + snkmom + "_"
                                    + contraction.q1 + "_" 
                                    + gamma_snk + "_" 
                                    + contraction.q2;
    contraction.output = output_folder + contraction_name;

    application.createModule<MContraction::Meson>(contraction_name, contraction);
    return contraction_name;
}

// // general contraction
// std::string make_contraction(Application &application,std::string prefix, std::string q1, std::string q2, std::array<std::string, 2> gammas, std::string sink){
    
//     MContraction::Meson::Par contraction;
//     std::string gamma_snk = gammas[0];
//     std::string gamma_src = gammas[1];
   
//     contraction.q1 = q1;
//     contraction.q2 = q2;
//     contraction.gammas = "(" + gamma_snk + " " + gamma_src + ")";
//     contraction.sink = sink;

//     std::string mom;
//     std::string twist;
//     if (contraction.q1.find("tw") !=std::string::npos){
//         twist = get_twist(contraction.q1) + "_"; }
//     else if (contraction.q2.find("tw") !=std::string::npos){
//         twist = get_twist(contraction.q2) + "_";
//     }

//     std::string contraction_name = prefix + "_" + twist 
//                                        + remove_twist_str(contraction.q1) + "_" 
//                                        + gamma_snk + "_" 
//                                        + remove_twist_str(contraction.q2);
//     contraction.output = folder_output + contraction_name;

//     application.createModule<MContraction::Meson>(contraction_name, contraction);
//     return contraction_name;
// }


END_APIMODULE_NAMESPACE