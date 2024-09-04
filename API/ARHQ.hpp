#include "Utilities.hpp"

BEGIN_APIMODULE_NAMESPACE(ARHQ)

////////////////////////////////////////////////////////////////////////////////
// UTILITIES
////////////////////////////////////////////////////////////////////////////////

// convert direction to index
std::string dir_to_index(std::string dir){
  std::string index;
  if (dir=="X")
    index = "0";
  else if (dir=="Y")
    index = "1";
  else if (dir=="Z")
    index = "2";
  else if (dir=="T")
    index = "3";

  return index;
}

std::array<double, 4> twist0 = {0, 0, 0, 0};

// utility for gamma (Gamma::Algebra)
Gamma::Algebra GammaAlg_from_str(std::string gamma){
    Gamma::Algebra gamma_alg = strToVec<Gamma::Algebra>(gamma)[0];
    return gamma_alg;
}

// assign the RHQ parameters
template <typename TRHQ>
std::string assign_RHQInsertionI_II_par(TRHQ &Impr, std::string incipit, std::string q, std::string dir, std::string gamma, std::array<double,4> propTwist, std::string gauge){
    Impr.q = q;
    Impr.index = dir_to_index(dir);
    Impr.gamma = GammaAlg_from_str(gamma);
    Impr.gauge = gauge;
    if (propTwist!=twist0){
      Impr.propTwist = make_twist_par(propTwist);
    }

    std::string Impr_name = 
        "quarkImpr" + incipit + dir + gamma
        + "_" + remove_str(q, "quark_");
    return Impr_name;
}

// assign the RHQ parameters
template <typename TRHQ>
std::string assign_RHQInsertionIII_IV_par(TRHQ &Impr, std::string incipit, std::string q, std::string dir1, std::string gamma5, std::array<double,4> propTwist, std::string gauge){
    Impr.q = q;
    Impr.index1 = dir_to_index(dir1);
    Impr.gamma5 = GammaAlg_from_str(gamma5);
    Impr.gauge = gauge;
    if (propTwist!=twist0){
      Impr.propTwist = make_twist_par(propTwist);
    }

    std::string Impr_name = 
        "quarkImpr" + incipit + dir1 + gamma5 
        + "_" + remove_str(q, "quark_");
    return Impr_name;
}


////////////////////////////////////////////////////////////////////////////////
// Insertion Modules
////////////////////////////////////////////////////////////////////////////////

// RHQInsertionI
std::string make_RHQInsertionI(Application &application, std::string q, std::string dir, std::string gamma, std::array<double,4> propTwist=twist0, std::string gauge="gauge"){
    MRHQ::RHQInsertionI::Par Impr;
    std::string Impr_name = assign_RHQInsertionI_II_par(Impr, "I", q, dir, gamma, propTwist, gauge);

    application.createModule<MRHQ::RHQInsertionI>(Impr_name, Impr);
    return Impr_name;
}

// RHQInsertionII
std::string make_RHQInsertionII(Application &application, std::string q, std::string dir, std::string gamma, std::array<double,4> propTwist=twist0, std::string gauge="gauge"){
    MRHQ::RHQInsertionII::Par Impr;
    std::string Impr_name = assign_RHQInsertionI_II_par(Impr, "II", q, dir, gamma, propTwist, gauge);

    application.createModule<MRHQ::RHQInsertionII>(Impr_name, Impr);
    return Impr_name;
}

// RHQInsertionI
std::string make_RHQInsertionIII(Application &application, std::string q, std::string dir, std::string gamma5, std::array<double,4> propTwist=twist0, std::string gauge="gauge"){
    MRHQ::RHQInsertionIII::Par Impr;
    std::string Impr_name = assign_RHQInsertionIII_IV_par(Impr, "III", q, dir, gamma5, propTwist, gauge);

    application.createModule<MRHQ::RHQInsertionIII>(Impr_name, Impr);
    return Impr_name;
}


// RHQInsertionI
std::string make_RHQInsertionIV(Application &application, std::string q, std::string dir, std::string gamma5, std::array<double,4> propTwist=twist0, std::string gauge="gauge"){
    MRHQ::RHQInsertionIV::Par Impr;
    std::string Impr_name = assign_RHQInsertionIII_IV_par(Impr, "IV", q, dir, gamma5, propTwist, gauge);

    application.createModule<MRHQ::RHQInsertionIV>(Impr_name, Impr);
    return Impr_name;
}


////////////////////////////////////////////////////////////////////////////////
// Improvement as Seq sources
////////////////////////////////////////////////////////////////////////////////

// assign parameters for RHQSeq
template <typename TRHQ>
void assign_RHQSeq_I_II_par(TRHQ &Impr, std::string q, int t, std::array<int, 4> mom, std::string dir, std::string gamma, std::string gauge){
    // assign_RHQ_par(Impr, q, index);
    Impr.q = q;
    Impr.gauge = gauge;
    Impr.index = dir_to_index(dir);
    Impr.gamma = GammaAlg_from_str(gamma);
    
    Impr.t = t;
    Impr.mom = make_mom_par(mom);
}

template <typename TRHQ>
void assign_RHQSeq_III_IV_par(TRHQ &Impr, std::string q, int t, std::array<int, 4> mom, std::string dir, std::string gamma5, std::string gauge){
    // assign_RHQ_par(Impr, q, index);
    Impr.q = q;
    Impr.gauge = gauge;
    Impr.index = dir_to_index(dir);
    Impr.gamma5 = GammaAlg_from_str(gamma5);
    
    Impr.t = t;
    Impr.mom = make_mom_par(mom);
}

// make the name for the Seq RHQ module
template <typename TRHQ>
std::string make_RHQSeq_I_II_name(TRHQ Impr, std::string incipit, std::string gamma){
    std::vector<std::string> directions = {"X", "Y", "Z", "T"};
    
    std::string mom_name = "";
    std::string mom_str = space_to_underscore(Impr.mom);
    if (mom_str != "0_0_0_0" || mom_str=="0.0_0.0_0.0_0.0")
        mom_name = "_mom_" + mom_str;

    std::string Impr_name = 
        incipit + directions[std::stoi(Impr.index)] + gamma 
        + mom_name + "_" 
        + remove_str(Impr.q, "quark_");
    return Impr_name;
}

// make the name for the Seq RHQ module
template <typename TRHQ>
std::string make_RHQSeq_III_IV_name(TRHQ Impr, std::string incipit, std::string gamma5){
    std::vector<std::string> directions = {"X", "Y", "Z", "T"};
    
    std::string mom_name = "";
    std::string mom_str = space_to_underscore(Impr.mom);
    if (mom_str != "0_0_0_0" || mom_str=="0.0_0.0_0.0_0.0")
        mom_name = "_mom_" + mom_str;

    std::string Impr_name = 
        incipit + directions[std::stoi(Impr.index)] + gamma5
        + mom_name + "_" 
        + remove_str(Impr.q, "quark_");
    return Impr_name;
}

////////////////////////////////////////////////////////////////////////////////
// Sequential Modules
////////////////////////////////////////////////////////////////////////////////


std::string make_RHQSeqSourceI(Application &application, std::string q, int t, std::array<int,4> mom, std::string dir, std::string gamma, std::string gauge="gauge"){
    MRHQ::RHQSeqSourceI::Par ImprI;    
    assign_RHQSeq_I_II_par(ImprI, q, t, mom, dir, gamma, gauge);
    std::string ImprI_name = make_RHQSeq_I_II_name(ImprI, "source_seq_ImprI", gamma);
    application.createModule<MRHQ::RHQSeqSourceI>(ImprI_name, ImprI);
    return ImprI_name;
}

std::string make_RHQSeqSourceII(Application &application, std::string q, int t, std::array<int,4> mom, std::string dir, std::string gamma, std::string gauge="gauge"){
    MRHQ::RHQSeqSourceII::Par ImprII;    
    assign_RHQSeq_I_II_par(ImprII, q, t, mom, dir, gamma, gauge);
    std::string ImprII_name = make_RHQSeq_I_II_name(ImprII, "source_seq_ImprII", gamma);
    application.createModule<MRHQ::RHQSeqSourceII>(ImprII_name, ImprII);
    return ImprII_name;
}

std::string make_RHQSeqSourceIII(Application &application, std::string q, int t, std::array<int,4> mom, std::string dir, std::string gamma5, std::string gauge="gauge"){
    MRHQ::RHQSeqSourceIII::Par ImprIII;    
    assign_RHQSeq_III_IV_par(ImprIII, q, t, mom, dir, gamma5, gauge);
    std::string ImprIII_name = make_RHQSeq_III_IV_name(ImprIII, "source_seq_ImprIII", gamma5);
    application.createModule<MRHQ::RHQSeqSourceIII>(ImprIII_name, ImprIII);
    return ImprIII_name;
}

std::string make_RHQSeqSourceIV(Application &application, std::string q, int t, std::array<int,4> mom, std::string dir, std::string gamma5, std::string gauge="gauge"){
    MRHQ::RHQSeqSourceIV::Par ImprIV;    
    assign_RHQSeq_III_IV_par(ImprIV, q, t, mom, dir, gamma5, gauge);
    std::string ImprIV_name = make_RHQSeq_III_IV_name(ImprIV, "source_seq_ImprIV", gamma5);
    application.createModule<MRHQ::RHQSeqSourceIV>(ImprIV_name, ImprIV);
    return ImprIV_name;
}

END_APIMODULE_NAMESPACE