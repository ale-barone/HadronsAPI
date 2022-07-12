#include "Utilities.hpp"

BEGIN_APIMODULE_NAMESPACE(AContraction)

////////////////////////////////////////////////////////////////////////////////
// assign parameters of the contraction
////////////////////////////////////////////////////////////////////////////////

template <typename TContraction>
void assign_contraction_par(TContraction &contraction,
                            std::string q1,
                            std::string q2,
                            std::array<std::string, 2> gammas,
                            std::string sink){
    contraction.q1 = q1;
    contraction.q2 = q2;
    contraction.sink = sink;
    // gammas
    std::string gamma_snk = gammas[0];
    std::string gamma_src = gammas[1];
    contraction.gammas = "(" + gamma_snk + " " + gamma_src + ")";
}

template <typename TContraction>
void assign_contraction_par(TContraction &contraction,
                            std::string q1,
                            std::string q2,
                            std::vector<std::array<std::string, 2>> gammas_list,
                            std::string sink){
    contraction.q1 = q1;
    contraction.q2 = q2;
    contraction.sink = sink;
    // gammas
    std::string gammas = "";
    for (int g=0; g<gammas_list.size(); g++){
        std::string gamma_snk = gammas_list[g][0];
        std::string gamma_src = gammas_list[g][1];
        gammas += "(" + gamma_snk + " " + gamma_src + ")";
    }
    contraction.gammas = gammas;
}

template <typename TContraction>
void assign_contraction_par(TContraction &contraction,
                            std::string q1,
                            std::string q2,
                            std::vector<std::string> gammas_snk_list,
                            std::string gamma_src,
                            std::string sink){
    contraction.q1 = q1;
    contraction.q2 = q2;
    contraction.sink = sink;
    // gammas
    std::string gammas = "";
    for (int g=0; g<gammas_snk_list.size(); g++){
        std::string gamma_snk = gammas_snk_list[g];
        gammas += "(" + gamma_snk + " " + gamma_src + ")";
    }
    contraction.gammas = gammas;    
}

// make name
template <typename TContraction>
std::string make_contraction_name(TContraction &contraction,
                                  std::string prefix,
                                  std::string gamma_snk){
    std::string snkmom = get_mom(contraction.sink);
    std::string contraction_name = prefix + "_"
                                   + "snkmom_" + snkmom + "_"
                                   + contraction.q1 + "_"
                                   + gamma_snk + "_" 
                                   + contraction.q2;
    return contraction_name;
}

////////////////////////////////////////////////////////////////////////////////
// generic contraction (internal)
////////////////////////////////////////////////////////////////////////////////

// general contraction
void make_contraction(Application &application,
                       std::string prefix,
                       std::string q1,
                       std::string q2,
                       std::array<std::string, 2> gammas,
                       std::string sink,
                       std::string folder_output="output"){

    MContraction::Meson::Par contraction;
    assign_contraction_par(contraction, q1, q2, gammas, sink);

    std::string gamma_snk = gammas[0];
    std::string contraction_name = make_contraction_name(contraction, prefix, gamma_snk);
    contraction.output = folder_output + "/" + contraction_name;
    application.createModule<MContraction::Meson>(contraction_name, contraction);
}

// general contraction for list of couples {gammas_snk, gamma_src}
void make_contraction(Application &application,
                       std::string prefix,
                       std::string q1,
                       std::string q2,
                       std::vector<std::array<std::string, 2>> gammas_list,
                       std::string sink,
                       std::string folder_output="output"){
    
    MContraction::Meson::Par contraction;
    assign_contraction_par(contraction, q1, q2, gammas_list, sink);
    std::string contraction_name = make_contraction_name(contraction, prefix, "Gammas");
    contraction.output = folder_output + "/" + contraction_name;
    application.createModule<MContraction::Meson>(contraction_name, contraction);
}

// general contraction for list of gammas_snk
void make_contraction(Application &application,
                          std::string prefix,
                          std::string q1,
                          std::string q2,
                          std::vector<std::string> gammas_snk_list,
                          std::string gamma_src,
                          std::string sink,
                          std::string folder_output){

    // create contraction
    MContraction::Meson::Par contraction;
    assign_contraction_par(contraction, q1, q2, gammas_snk_list, gamma_src, sink);
    std::string contraction_name = make_contraction_name(contraction, prefix, "Gammas");
    contraction.output = folder_output + "/" + contraction_name;
    application.createModule<MContraction::Meson>(contraction_name, contraction);
}

////////////////////////////////////////////////////////////////////////////////
// 2pt contraction
////////////////////////////////////////////////////////////////////////////////

// general 2pt contraction
void make_2pt_contraction(Application &application,
                          std::string q1,
                          std::string q2,
                          std::array<std::string, 2> gammas,
                          std::string sink,
                          std::string folder_output="output"){

    make_contraction(application, "2pt", q1, q2, gammas, sink, folder_output + "/2pt");
}

// general 2pt contraction for list of couples {gammas_snk, gamma_src}
void make_2pt_contraction(Application &application,
                          std::string q1,
                          std::string q2,
                          std::vector<std::array<std::string, 2>> gammas_list,
                          std::string sink,
                          std::string folder_output="output"){
    
    make_contraction(application, "2pt", q1, q2, gammas_list, sink, folder_output + "/2pt");
}

// general 2pt contraction for list of gammas_snk
void make_2pt_contraction(Application &application,
                          std::string q1,
                          std::string q2,
                          std::vector<std::string> gammas_snk_list,
                          std::string gamma_src,
                          std::string sink,
                          std::string folder_output){

    make_contraction(application, "3pt", q1, q2, gammas_snk_list, gamma_src, sink, folder_output + "/2pt");
}


////////////////////////////////////////////////////////////////////////////////
// 3pt contraction
////////////////////////////////////////////////////////////////////////////////

// general 3pt contraction
void make_3pt_contraction(Application &application,
                          std::string q1,
                          std::string q2,
                          std::array<std::string, 2> gammas,
                          std::string sink,
                          std::string folder_output="output"){

    make_contraction(application, "3pt", q1, q2, gammas, sink, folder_output + "/3pt");
}

// general 3pt contraction for list of couples {gammas_snk, gamma_src}
void make_3pt_contraction(Application &application,
                          std::string q1,
                          std::string q2,
                          std::vector<std::array<std::string, 2>> gammas_list,
                          std::string sink,
                          std::string folder_output="output"){
    
    make_contraction(application, "3pt", q1, q2, gammas_list, sink, folder_output + "/3pt");
}

// general 3pt contraction for list of gammas_snk
void make_3pt_contraction(Application &application,
                          std::string q1,
                          std::string q2,
                          std::vector<std::string> gammas_snk_list,
                          std::string gamma_src,
                          std::string sink,
                          std::string folder_output){

    make_contraction(application, "3pt", q1, q2, gammas_snk_list, gamma_src, sink, folder_output + "/3pt");
}


////////////////////////////////////////////////////////////////////////////////
// 4pt contraction
////////////////////////////////////////////////////////////////////////////////

// general 4pt contraction
void make_4pt_contraction(Application &application,
                          std::string q1,
                          std::string q2,
                          std::array<std::string, 2> gammas,
                          std::string sink,
                          std::string folder_output="output"){

    make_contraction(application, "4pt", q1, q2, gammas, sink, folder_output + "/4pt");
}

// general 4pt contraction for list of couples {gammas_snk, gamma_src}
void make_4pt_contraction(Application &application,
                          std::string q1,
                          std::string q2,
                          std::vector<std::array<std::string, 2>> gammas_list,
                          std::string sink,
                          std::string folder_output="output"){
    
    make_contraction(application, "4pt", q1, q2, gammas_list, sink, folder_output + "/4pt");
}

// general 4pt contraction for list of gammas_snk
void make_4pt_contraction(Application &application,
                          std::string q1,
                          std::string q2,
                          std::vector<std::string> gammas_snk_list,
                          std::string gamma_src,
                          std::string sink,
                          std::string folder_output){

    make_contraction(application, "4pt", q1, q2, gammas_snk_list, gamma_src, sink, folder_output + "/4pt");
}

END_APIMODULE_NAMESPACE