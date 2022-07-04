#include "Utilities.hpp"

BEGIN_APIMODULE_NAMESPACE(ARHQ)

////////////////////////////////////////////////////////////////////////////////
// IMPROVED SOURCES
////////////////////////////////////////////////////////////////////////////////

// utility for gamma (string)
std::string gamma_from_index(int index){
    std::string gamma;
    if (index>=0 && index <4){
        gamma = "Identity";
    }
    else if (index>=4 && index<8){
        gamma = "Gamma5";
    }
    else{
        HADRONS_ERROR(Argument, "direction must be either in [0,7]"); 
    }
    return gamma;
}

// utility for gamma (Gamma::Algebra)
Gamma::Algebra GammaAlg_from_str(std::string gamma){
    Gamma::Algebra gamma_alg = strToVec<Gamma::Algebra>(gamma)[0];
    return gamma_alg;
}

// utility for direction
int direction_from_index(int index){
    int direction;
    if (index>=0 && index <4){
        direction = direction;
    }
    else if (index>=4 && index<8){
        direction = index-4;
    }
    else{
        HADRONS_ERROR(Argument, "direction must be either in [0,7]"); 
    }
    return direction;
}


// assign the RHQ parameters
template <typename TRHQ>
void assign_RHQ_par(TRHQ &Impr, std::string q, int index){
    Impr.q = q;
    Impr.gauge = "gauge";
    Impr.index = direction_from_index(index);
    Impr.gamma5 = GammaAlg_from_str(gamma_from_index(index));
}

template <typename TRHQ>
void assign_RHQ_par(TRHQ &Impr, std::string q, int index, std::array<float, 4> twist){
    Impr.q = q;
    Impr.gauge = "gauge";
    Impr.index = direction_from_index(index);
    Impr.gamma5 = GammaAlg_from_str(gamma_from_index(index));
    Impr.propTwist = make_twist_par(twist);
}

// assign parameters for RHQSeq
template <typename TRHQ>
void assign_RHQSeq_par(TRHQ &Impr, std::string q, int t, std::array<float, 4> twist, int index){
    assign_RHQ_par(Impr, q, index);
    Impr.t = t;
    Impr.mom = make_twist_par(twist);
}

template <typename TRHQ>
void assign_RHQSeq_par(TRHQ &Impr, std::string q, int t, std::array<int, 4> mom, int index){
    assign_RHQ_par(Impr, q, index);
    Impr.t = t;
    Impr.mom = make_mom_par(mom);
}

// make the name for the RHQ module
template <typename TRHQ>
std::string make_RHQ_name(TRHQ Impr, std::string incipit){
    std::string gamma_name = "";
    Gamma::Algebra gamma5 = Impr.gamma5;
    if (gamma5==Gamma::Algebra::Gamma5)
    {   
        gamma_name = "Gamma5";
    }
    std::vector<std::string> directions = {"X", "Y", "Z", "T"};
    std::string Impr_name = 
        incipit + directions[Impr.index]
        + gamma_name + "_" + remove_str(Impr.q, "quark_");
    return Impr_name;
}

// make the name for the Seq RHQ module
template <typename TRHQ>
std::string make_RHQSeq_name(TRHQ Impr, std::string incipit){
    std::string gamma_name = "";
    Gamma::Algebra gamma5 = Impr.gamma5;
    if (gamma5==Gamma::Algebra::Gamma5)
    {   
        gamma_name = "Gamma5";
    }
    std::vector<std::string> directions = {"X", "Y", "Z", "T"};
    
    std::string mom_name = "";
    std::string mom_str = space_to_underscore(Impr.mom);
    if (mom_str != "0_0_0_0" || mom_str=="0.0_0.0_0.0_0.0")
        mom_name = "_mom_" + mom_str;

    std::string Impr_name = 
        incipit + directions[Impr.index] + gamma_name 
        + mom_name + "_" 
        + remove_str(Impr.q, "quark_");
    return Impr_name;
}

// make the name for the Seq RHQ module with twist
template <typename TRHQ>
std::string make_RHQSeqTwist_name(TRHQ Impr, std::string incipit){
    std::string gamma_name = "";
    Gamma::Algebra gamma5 = Impr.gamma5;
    if (gamma5==Gamma::Algebra::Gamma5)
    {   
        gamma_name = "Gamma5";
    }
    std::vector<std::string> directions = {"X", "Y", "Z", "T"};
    
    std::string mom_name = "";
    std::string mom_str = space_to_underscore(Impr.mom);     
    if (!(mom_str=="0_0_0_0" || mom_str=="0.0_0.0_0.0_0.0"))
        mom_name = "_tw_" + mom_str;

    std::string Impr_name = 
        incipit + directions[Impr.index] + gamma_name 
        + mom_name + "_" 
        + remove_str(Impr.q, "quark_");
    return Impr_name;
}

////////////////////////////////////////////////////////////////////////////////
// IMPROVED SEQUENTIAL SOURCES
////////////////////////////////////////////////////////////////////////////////

// RHQI_seq_source
std::string make_RHQI_seq_source(Application &application, std::string q, int t, std::array<float, 4> twist, int index){
    MRHQ::RHQSeqSourceI::Par ImprI;    
    assign_RHQSeq_par(ImprI, q, t, twist, index);
    std::string ImprI_name = make_RHQSeqTwist_name(ImprI, "source_seq_ImprI");
    application.createModule<MRHQ::RHQSeqSourceI>(ImprI_name, ImprI);
    return ImprI_name;
}

std::string make_RHQI_seq_source(Application &application, std::string q, int t, std::array<int, 4> mom, int index){
    MRHQ::RHQSeqSourceI::Par ImprI;    
    assign_RHQSeq_par(ImprI, q, t, mom, index);
    std::string ImprI_name = make_RHQSeq_name(ImprI, "source_seq_ImprI");
    application.createModule<MRHQ::RHQSeqSourceI>(ImprI_name, ImprI);
    return ImprI_name;
}

// RHQII_seq_source
std::string make_RHQII_seq_source(Application &application, std::string q, int t, std::array<float, 4> twist, int index){
    MRHQ::RHQSeqSourceII::Par Impr;    
    assign_RHQSeq_par(Impr, q, t, twist, index);
    Impr.momType = strToVec<OpIIMomType>("Twist")[0];
    std::string Impr_name = make_RHQSeqTwist_name(Impr, "source_seq_ImprII");
    application.createModule<MRHQ::RHQSeqSourceII>(Impr_name, Impr);
    return Impr_name;
}

std::string make_RHQII_seq_source(Application &application, std::string q, int t, std::array<int, 4> mom, int index){
    MRHQ::RHQSeqSourceII::Par Impr;    
    assign_RHQSeq_par(Impr, q, t, mom, index);
    Impr.momType = strToVec<OpIIMomType>("Sink")[0];
    std::string Impr_name = make_RHQSeq_name(Impr, "source_seq_ImprII");
    application.createModule<MRHQ::RHQSeqSourceII>(Impr_name, Impr);
    return Impr_name;
}

// RHQIII_seq_source
std::string make_RHQIII_seq_source(Application &application, std::string q, int t, std::array<float, 4> twist, int index){
    MRHQ::RHQSeqSourceIII::Par Impr;    
    assign_RHQSeq_par(Impr, q, t, twist, index); 
    std::string Impr_name = make_RHQSeqTwist_name(Impr, "source_seq_ImprIII");
    application.createModule<MRHQ::RHQSeqSourceIII>(Impr_name, Impr);
    return Impr_name;
}

std::string make_RHQIII_seq_source(Application &application, std::string q, int t, std::array<int, 4> mom, int index){
    MRHQ::RHQSeqSourceIII::Par Impr;    
    assign_RHQSeq_par(Impr, q, t, mom, index); 
    std::string Impr_name = make_RHQSeq_name(Impr, "source_seq_ImprIII");
    application.createModule<MRHQ::RHQSeqSourceIII>(Impr_name, Impr);
    return Impr_name;
}

// RHQIV_seq_source
std::string make_RHQIV_seq_source(Application &application, std::string q, int t, std::array<float, 4> twist, int index){
    MRHQ::RHQSeqSourceIV::Par Impr;    
    assign_RHQSeq_par(Impr, q, t, twist, index); 
    Impr.momType = strToVec<OpIVMomType>("Twist")[0];
    std::string Impr_name = make_RHQSeqTwist_name(Impr, "source_seq_ImprIV");
    application.createModule<MRHQ::RHQSeqSourceIV>(Impr_name, Impr);
    return Impr_name;
}

std::string make_RHQIV_seq_source(Application &application, std::string q, int t, std::array<int, 4> mom, int index){
    MRHQ::RHQSeqSourceIV::Par Impr;    
    assign_RHQSeq_par(Impr, q, t, mom, index); 
    Impr.momType = strToVec<OpIVMomType>("Sink")[0];
    std::string Impr_name = make_RHQSeq_name(Impr, "source_seq_ImprIV");
    application.createModule<MRHQ::RHQSeqSourceIV>(Impr_name, Impr);
    return Impr_name;
}

////////////////////////////////////////////////////////////////////////////////
// IMPROVED OPERATORS
////////////////////////////////////////////////////////////////////////////////

// RHQI
std::string make_RHQI(Application &application, std::string q, int index){
    MRHQ::RHQInsertionI::Par Impr;
    assign_RHQ_par(Impr, q, index);
    std::string Impr_name = make_RHQ_name(Impr, "quark_ImprI");
    application.createModule<MRHQ::RHQInsertionI>(Impr_name, Impr);
    return Impr_name;
}

std::string make_RHQI(Application &application, std::string q, int index, std::array<float, 4> twist){
    MRHQ::RHQInsertionI::Par Impr;
    assign_RHQ_par(Impr, q, index, twist);
    std::string Impr_name = make_RHQ_name(Impr, "quark_ImprI");
    application.createModule<MRHQ::RHQInsertionI>(Impr_name, Impr);
    return Impr_name;
}

// RHQII
std::string make_RHQII(Application &application, std::string q, int index){
    MRHQ::RHQInsertionII::Par Impr;
    assign_RHQ_par(Impr, q, index);
    std::string Impr_name = make_RHQ_name(Impr, "quark_ImprII");
    application.createModule<MRHQ::RHQInsertionII>(Impr_name, Impr);
    return Impr_name;
}

std::string make_RHQII(Application &application, std::string q, int index, std::array<float, 4> twist){
    MRHQ::RHQInsertionII::Par Impr;
    assign_RHQ_par(Impr, q, index, twist);
    std::string Impr_name = make_RHQ_name(Impr, "quark_ImprII");
    application.createModule<MRHQ::RHQInsertionII>(Impr_name, Impr);
    return Impr_name;
}

// RHQIII
std::string make_RHQIII(Application &application, std::string q, int index, std::string flag="LeftRight"){
    MRHQ::RHQInsertionIII::Par Impr;
    assign_RHQ_par(Impr, q, index);
    Impr.flag = strToVec<OpIIIFlag>(flag)[0];

    std::string flag_name = ""; 
    if (flag=="Chroma"){
        flag_name = "_flagChroma";
    }
    std::string Impr_name = make_RHQ_name(Impr, "quark_ImprIII") + flag_name;
    application.createModule<MRHQ::RHQInsertionIII>(Impr_name, Impr);
    return Impr_name;
}

std::string make_RHQIII(Application &application, std::string q, int index, std::array<float, 4> twist, std::string flag="LeftRight"){
    MRHQ::RHQInsertionIII::Par Impr;
    assign_RHQ_par(Impr, q, index, twist);
    Impr.flag = strToVec<OpIIIFlag>(flag)[0];

    std::string flag_name = ""; 
    if (flag=="Chroma"){
        flag_name = "_flagChroma";
    }
    std::string Impr_name = make_RHQ_name(Impr, "quark_ImprIII") + flag_name;
    application.createModule<MRHQ::RHQInsertionIII>(Impr_name, Impr);
    return Impr_name;
}

// RHQIV
std::string make_RHQIV(Application &application, std::string q, int index, std::string flag="LeftRight"){
    MRHQ::RHQInsertionIV::Par Impr;
    assign_RHQ_par(Impr, q, index);
    Impr.flag = strToVec<OpIVFlag>(flag)[0];

    std::string flag_name = ""; 
    if (flag=="Chroma"){
        flag_name = "_flagChroma";
    }
    std::string Impr_name = make_RHQ_name(Impr, "quark_ImprIV") + flag_name;
    application.createModule<MRHQ::RHQInsertionIV>(Impr_name, Impr);
    return Impr_name;
}

std::string make_RHQIV(Application &application, std::string q, int index, std::array<float, 4> twist, std::string flag="LeftRight"){
    MRHQ::RHQInsertionIV::Par Impr;
    assign_RHQ_par(Impr, q, index, twist);
    Impr.flag = strToVec<OpIVFlag>(flag)[0];

    std::string flag_name = ""; 
    if (flag=="Chroma"){
        flag_name = "_flagChroma";
    }
    std::string Impr_name = make_RHQ_name(Impr, "quark_ImprIV") + flag_name;
    application.createModule<MRHQ::RHQInsertionIV>(Impr_name, Impr);
    return Impr_name;
}

END_APIMODULE_NAMESPACE
