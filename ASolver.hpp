#include "Utilities.hpp"


BEGIN_APIMODULE_NAMESPACE(ASolver)


std::string make_solver_CG(Application &application, std::string action, int maxIter, double res){
    MSolver::RBPrecCG::Par solver;
    solver.action       = action;
    solver.maxIteration = maxIter;
    solver.residual     = res;

    std::string quark_name = remove_str(action, "action_");
    std::string solver_name = "solver_" + quark_name;
    application.createModule<MSolver::RBPrecCG>(solver_name, solver);
    return solver_name;
}

END_APIMODULE_NAMESPACE