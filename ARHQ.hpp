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
void assign_RHQ_par(TRHQ &Impr, std::string q,int index){
    Impr.q = q;
    Impr.gauge = "gauge";
    Impr.index = direction_from_index(index);
    Impr.gamma5 = GammaAlg_from_str(gamma_from_index(index));
}

// assign parameters for RHQSeq
template <typename TRHQ>
void assign_RHQSeq_par(TRHQ &Impr, std::string q, int t, std::array<float, 4> twist, int index){
    assign_RHQ_par(Impr, q, index);
    Impr.t = t;
    Impr.mom = make_twist_par(twist);
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

// make the name for the RHQSeq module
template <typename TRHQ>
std::string make_RHQSeq_name(TRHQ Impr, std::string incipit){
    std::string tw_name = space_to_underscore(Impr.mom);
    std::string Impr_name = make_RHQ_name(Impr, incipit) + "_tw_" + tw_name;
    return Impr_name;
}

////////////////////////////////////////////////////////////////////////////////
// IMPROVED SEQUENTIAL SOURCES
////////////////////////////////////////////////////////////////////////////////

// RHQI_seq_source
std::string make_RHQI_seq_source(Application &application, std::string q, int t, std::array<float, 4> twist, int index){
    MRHQ::RHQSeqSourceI::Par ImprI;    
    assign_RHQSeq_par(ImprI, q, t, twist, index);
    std::string ImprI_name = make_RHQSeq_name(ImprI, "source_seq_ImprI");
    application.createModule<MRHQ::RHQSeqSourceI>(ImprI_name, ImprI);
    return ImprI_name;
}

// RHQII_seq_source
std::string make_RHQII_seq_source(Application &application, std::string q, int t, std::array<float, 4> twist, int index){
    MRHQ::RHQSeqSourceII::Par Impr;    
    assign_RHQSeq_par(Impr, q, t, twist, index);
    Impr.momType = strToVec<OpIIMomType>("Twist")[0];
    std::string Impr_name = make_RHQSeq_name(Impr, "source_seq_ImprII");
    application.createModule<MRHQ::RHQSeqSourceII>(Impr_name, Impr);
    return Impr_name;
}

// RHQIII_seq_source
std::string make_RHQIII_seq_source(Application &application, std::string q, int t, std::array<float, 4> twist, int index){
    MRHQ::RHQSeqSourceIII::Par Impr;    
    assign_RHQSeq_par(Impr, q, t, twist, index); 
    std::string Impr_name = make_RHQSeq_name(Impr, "source_seq_ImprIII");
    application.createModule<MRHQ::RHQSeqSourceIII>(Impr_name, Impr);
    return Impr_name;
}

// RHQIV_seq_source
std::string make_RHQIV_seq_source(Application &application, std::string q, int t, std::array<float, 4> twist, int index){
    MRHQ::RHQSeqSourceIV::Par Impr;    
    assign_RHQSeq_par(Impr, q, t, twist, index); 
    Impr.momType = strToVec<OpIVMomType>("Twist")[0];
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

// RHQII
std::string make_RHQII(Application &application, std::string q, int index){
    MRHQ::RHQInsertionII::Par Impr;
    assign_RHQ_par(Impr, q, index);
    std::string Impr_name = make_RHQ_name(Impr, "quark_ImprII");
    application.createModule<MRHQ::RHQInsertionII>(Impr_name, Impr);
    return Impr_name;
}

// RHQIII
std::string make_RHQIII(Application &application, std::string q, int index){
    MRHQ::RHQInsertionIII::Par Impr;
    assign_RHQ_par(Impr, q, index);
    Impr.flag = strToVec<OpIIIFlag>("LeftRight")[0];
    std::string Impr_name = make_RHQ_name(Impr, "quark_ImprIII");
    application.createModule<MRHQ::RHQInsertionIII>(Impr_name, Impr);
    return Impr_name;
}

// RHQIV
std::string make_RHQIV(Application &application, std::string q, int index){
    MRHQ::RHQInsertionIV::Par Impr;
    assign_RHQ_par(Impr, q, index);
    Impr.flag = strToVec<OpIVFlag>("LeftRight")[0];
    std::string Impr_name = make_RHQ_name(Impr, "quark_ImprIV");
    application.createModule<MRHQ::RHQInsertionIV>(Impr_name, Impr);
    return Impr_name;
}

END_APIMODULE_NAMESPACE
