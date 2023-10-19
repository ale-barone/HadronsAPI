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
                            std::array<std::vector<std::string>, 2> gammas_snk_src_list,
                            std::string sink){
    contraction.q1 = q1;
    contraction.q2 = q2;
    contraction.sink = sink;  
    // gammas
    std::vector<std::string> gammas_snk_list = gammas_snk_src_list[0];
    std::vector<std::string> gammas_src_list = gammas_snk_src_list[1]; 
    std::string gammas = "";
    for (int gsnk=0; gsnk<gammas_snk_list.size(); gsnk++){
        std::string gamma_snk = gammas_snk_list[gsnk];
        for (int gsrc=0; gsrc<gammas_src_list.size(); gsrc++){
            std::string gamma_src = gammas_src_list[gsrc];
            gammas += "(" + gamma_snk + " " + gamma_src + ")";
        }
    }
    contraction.gammas = gammas;    
}

template <typename TContraction>
void assign_contraction_par_pair(TContraction &contraction,
                            std::string q1,
                            std::string q2,
                            std::array<std::vector<std::string>, 2> gammas_snk_src_list,
                            std::string sink){
    contraction.q1 = q1;
    contraction.q2 = q2;
    contraction.sink = sink;
    std::vector<std::string> gammas_snk_list = gammas_snk_src_list[0];
    std::vector<std::string> gammas_src_list = gammas_snk_src_list[1]; 
    assert(gammas_snk_list.size()==gammas_src_list.size());
    // gammas
    std::string gammas = "";
    for (int g=0; g<gammas_snk_list.size(); g++){
        std::string gamma_snk = gammas_snk_list[g];
        std::string gamma_src = gammas_src_list[g];
        gammas += "(" + gamma_snk + " " + gamma_src + ")";
    }
    contraction.gammas = gammas;    
}



////////////////////////////////////////////////////////////////////////////////
// contraction name
////////////////////////////////////////////////////////////////////////////////

// make name
template <typename TContraction>
std::string make_contraction_name(TContraction &contraction,
                                  std::string prefix,
                                  std::string gamma_snk,
                                  std::string gamma_src,
                                  std::string extra_info=""){
    std::string snkmom = get_mom(contraction.sink);
    std::string contraction_name = prefix + "_"
                                   + "snkmom_" + snkmom + "_"
                                   + gamma_snk + "_" 
                                   + contraction.q1 + "_"
                                   + gamma_src + "_" 
                                   + contraction.q2;
    if (extra_info != ""){
        contraction_name = contraction_name + "_" + extra_info;
    }

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
                       std::string folder_output="output",
                       std::string extra_info=""){

    MContraction::Meson::Par contraction;
    assign_contraction_par(contraction, q1, q2, gammas, sink);

    std::string gamma_snk = gammas[0];
    std::string gamma_src = gammas[1];
    std::string contraction_name = make_contraction_name(contraction, prefix, gamma_snk, gamma_src, extra_info);
    contraction.output = folder_output + "/" + contraction_name;
    application.createModule<MContraction::Meson>(contraction_name, contraction);
}

// general contraction for list of gammas_src
void make_contraction(Application &application,
                          std::string prefix,
                          std::string q1,
                          std::string q2,
                          std::array<std::vector<std::string>, 2> gammas_snk_src_list,
                          std::string sink,
                          std::string folder_output,
                          std::string extra_info=""){

    // create contraction
    MContraction::Meson::Par contraction;
    assign_contraction_par(contraction, q1, q2, gammas_snk_src_list, sink);

    std::string gamma_snk_name = "Gammas";
    std::string gamma_src_name = "Gammas";
    if (gammas_snk_src_list[0].size()==1)
        gamma_snk_name = gammas_snk_src_list[0][0];
    if (gammas_snk_src_list[1].size()==1)
        gamma_src_name = gammas_snk_src_list[1][0];
    std::string contraction_name = make_contraction_name(contraction, prefix, gamma_snk_name, gamma_src_name, extra_info);
    contraction.output = folder_output + "/" + contraction_name;
    application.createModule<MContraction::Meson>(contraction_name, contraction);
}

// general contraction for list of gammas_snk_src
void make_contraction_pair(Application &application,
                          std::string prefix,
                          std::string q1,
                          std::string q2,
                          std::array<std::vector<std::string>, 2> gammas_snk_src_list,
                          std::string sink,
                          std::string folder_output,
                          std::string extra_info=""){

    // create contraction
    MContraction::Meson::Par contraction;
    assign_contraction_par_pair(contraction, q1, q2, gammas_snk_src_list, sink);
    std::string contraction_name = make_contraction_name(contraction, prefix, "Gammas", "Gammas", extra_info);
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
                          std::string folder_output,
                          std::string extra_info){

    make_contraction(application, "2pt", q1, q2, gammas, sink, folder_output + "/2pt", extra_info);
}

void make_2pt_contraction(Application &application,
                          std::string q1,
                          std::string q2,
                          std::array<std::vector<std::string>, 2> gammas_snk_src_list,
                          std::string sink,
                          std::string folder_output,
                          std::string extra_info=""){

    make_contraction(application, "2pt", q1, q2, gammas_snk_src_list, sink, folder_output + "/2pt", extra_info);
}

void make_2pt_contraction_pair(Application &application,
                          std::string q1,
                          std::string q2,
                          std::array<std::vector<std::string>, 2> gammas_snk_src_list,
                          std::string sink,
                          std::string folder_output,
                          std::string extra_info=""){

    make_contraction_pair(application, "2pt", q1, q2, gammas_snk_src_list, sink, folder_output + "/2pt", extra_info);
}



// ////////////////////////////////////////////////////////////////////////////////
// // 3pt contraction
// ////////////////////////////////////////////////////////////////////////////////

// general 3pt contraction
void make_3pt_contraction(Application &application,
                          std::string q1,
                          std::string q2,
                          std::array<std::string, 2> gammas,
                          std::string sink,
                          std::string folder_output,
                          std::string extra_info){

    make_contraction(application, "3pt", q1, q2, gammas, sink, folder_output + "/3pt", extra_info);
}

// general 3pt contraction for list of gammas_src
void make_3pt_contraction(Application &application,
                          std::string q1,
                          std::string q2,
                          std::array<std::vector<std::string>, 2> gammas_snk_src_list,
                          std::string sink,
                          std::string folder_output,
                          std::string extra_info){

    make_contraction(application, "3pt", q1, q2, gammas_snk_src_list, sink, folder_output + "/3pt", extra_info);
}

// general 3pt contraction for list of gammas_src
void make_3pt_contraction_pair(Application &application,
                          std::string q1,
                          std::string q2,
                          std::array<std::vector<std::string>, 2> gammas_snk_src_list,
                          std::string sink,
                          std::string folder_output,
                          std::string extra_info){

    make_contraction_pair(application, "3pt", q1, q2, gammas_snk_src_list, sink, folder_output + "/3pt", extra_info);
}


// ////////////////////////////////////////////////////////////////////////////////
// // 4pt contraction
// ////////////////////////////////////////////////////////////////////////////////

// general 4pt contraction
void make_4pt_contraction(Application &application,
                          std::string q1,
                          std::string q2,
                          std::array<std::string, 2> gammas,
                          std::string sink,
                          std::string folder_output,
                          std::string extra_info){

    make_contraction(application, "4pt", q1, q2, gammas, sink, folder_output + "/4pt", extra_info);
}

// general 4pt contraction for list of gammas_src
void make_4pt_contraction(Application &application,
                          std::string q1,
                          std::string q2,
                          std::array<std::vector<std::string>, 2> gammas_snk_src_list,
                          std::string sink,
                          std::string folder_output,
                          std::string extra_info=""){

    make_contraction(application, "4pt", q1, q2, gammas_snk_src_list, sink, folder_output + "/4pt", extra_info);
}

// general 4pt contraction for list of gammas_src
void make_4pt_contraction_pair(Application &application,
                          std::string q1,
                          std::string q2,
                          std::array<std::vector<std::string>, 2> gammas_snk_src_list,
                          std::string sink,
                          std::string folder_output,
                          std::string extra_info=""){

    make_contraction_pair(application, "4pt", q1, q2, gammas_snk_src_list, sink, folder_output + "/4pt", extra_info);
}

END_APIMODULE_NAMESPACE