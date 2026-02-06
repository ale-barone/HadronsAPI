namespace ENS{

  // lattice extent
  int LLAT = 24; // lattice L extent
  int TLAT = 64; // lattice T extent
  double AINV = 1.7295; // inverse lattice spacing a^-1 = 1.7295(38) GeV



  // charm masses
  //std::vector<double> CMASS = {0.30, 0.35, 0.40}; 
  // std::vector<std::string> CMASS = {"0.30", "0.35", "0.40"}; 
  std::vector<std::string> CMASS = {"0.35"}; 

  // tsnk position
  // std::vector<int> TSNKS = {18, 20, 22};
  // t1 insertions
  // std::vector<int> T1INS = {4, 5, 6};

  // FOR ROME
  std::vector<int> TSNKS = {8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
  // t1 insertions
  std::vector<int> T1INS = {4, 5, 6};

  // smearings
  // light
  std::vector<std::string> WIDTHS_l = {"5.50", "6.50", "7.86"};
  // strange
  std::vector<std::string> WIDTHS_s = {"5.00", "6.00", "6.50"};


  // gauge field
  std::string GAUGE_NAME = "gauge";
  void make_gauge(Application &application){
    MIO::LoadNersc::Par gauge;
    gauge.file = "/mnt/lustre/tursafs1/home/dp391/shared/data/configs/dwf_2+1f/C1M/ckpoint_C1M_lat";
    application.createModule<MIO::LoadNersc>(GAUGE_NAME, gauge);
  }

  // light action
  std::string make_action_l(Application &application){
    std::string action_name = "action_l";
    MAction::ScaledDWF::Par action_l;
    action_l.gauge = GAUGE_NAME;
    action_l.Ls = 24;
    action_l.M5 = 1.8;
    action_l.scale = 2;
    action_l.mass = 0.005;
    action_l.boundary = "1 1 1 -1";
    action_l.twist = "0. 0. 0. 0.";   
    application.createModule<MAction::ScaledDWF>(action_name, action_l);
    return action_name;
  }
  // light solver
  std::string make_solver_l(Application &application){
    MSolver::RBPrecCG::Par solver;
    solver.action = "action_l";
    solver.maxIteration = 8000;
    solver.residual = 1e-8;

    std::string solver_name = "solver_l";
    application.createModule<MSolver::RBPrecCG>(solver_name, solver);
    return solver_name;
  }


  // strange action
  std::string make_action_s(Application &application){
    std::string action_name = "action_s";
    MAction::ScaledDWF::Par action_s;
    action_s.gauge = GAUGE_NAME;
    action_s.Ls = 24;
    action_s.M5 = 1.8;
    action_s.scale = 2;
    action_s.mass = 0.0362;
    action_s.boundary = "1 1 1 -1";
    action_s.twist = "0. 0. 0. 0.";
    application.createModule<MAction::ScaledDWF>(action_name, action_s);
    return action_name;
  }
  // strange solver
  std::string make_solver_s(Application &application){
    MSolver::RBPrecCG::Par solver;
    solver.action = "action_s";
    solver.maxIteration = 8000;
    solver.residual = 1e-8;

    std::string solver_name = "solver_s";
    application.createModule<MSolver::RBPrecCG>(solver_name, solver);
    return solver_name;
  }


  // charm action
  std::string make_action_c(Application &application, std::string mass, std::array<double, 4> twist){
      std::string action_name = "action_c_m" + mass + "_tw_" + make_twist_name(twist);
      MAction::ScaledDWF::Par action_c;
      action_c.gauge = GAUGE_NAME;
      action_c.Ls = 12;
      action_c.M5 = 1.6;
      action_c.scale = 2;
      action_c.mass = std::stod(mass);
      action_c.boundary = "1 1 1 -1";
      action_c.twist = make_twist_par(twist);
      application.createModule<MAction::ScaledDWF>(action_name, action_c);
      return action_name;
  }
  // charm solver
  std::string make_solver_c(Application &application, std::string action){
    MSolver::RBPrecCG::Par solver;
    solver.action = action;
    solver.maxIteration = 8000;
    solver.residual = 1e-12;

    std::string solver_name = "solver_" + remove_str(action, "action_");
    application.createModule<MSolver::RBPrecCG>(solver_name, solver);
    return solver_name;
  }

  // bottom action
  std::string make_action_b(Application &application){
    std::string action_name = "action_b";
    MAction::WilsonClover::Par action_b;
    action_b.gauge = GAUGE_NAME;
    action_b.mass = 8.464000;
    action_b.csw_r = 5.760000;
    action_b.csw_t = 5.760000;
    action_b.cF = 1.13142020726806e-310;
    action_b.clover_anisotropy.isAnisotropic = "true";
    action_b.clover_anisotropy.t_direction = 3;
    action_b.clover_anisotropy.xi_0 = 1;
    action_b.clover_anisotropy.nu = 2.999000;
    action_b.boundary = "1 1 1 -1";
    action_b.twist = "0. 0. 0. 0.";
    application.createModule<MAction::WilsonClover>(action_name, action_b);
    return action_name;
  }
  // bottom solver
  std::string make_solver_b(Application &application){
    MSolver::RBPrecCG::Par solver;
    solver.action = "action_b";
    solver.maxIteration = 8000;
    solver.residual = 1e-40;

    std::string solver_name = "solver_b";
    application.createModule<MSolver::RBPrecCG>(solver_name, solver);
    return solver_name;
  }

}