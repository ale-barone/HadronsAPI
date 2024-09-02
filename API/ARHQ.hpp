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
        "quarkImpr" + incipit + "_" + dir + "_"
        + gamma + "_" + remove_str(q, "quark_");
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
        "quarkImpr" + incipit + "_" + dir1 + "_"
        + gamma5 + "_" + remove_str(q, "quark_");
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
// IMPROVED SOURCES
////////////////////////////////////////////////////////////////////////////////

END_APIMODULE_NAMESPACE