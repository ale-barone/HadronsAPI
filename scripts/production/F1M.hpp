namespace ENS{

  // lattice extent
  int LLAT = 48; // lattice L extent
  int TLAT = 96; // lattice T extent
  double AINV =  2.708; // inverse lattice spacing a^-1 = 1.7295(38) GeV



  // charm masses
  // std::vector<std::string> CMASS = {"0.204100", "0.232400", "0.279400"}; 
  std::vector<std::string> CMASS = {"0.232400", "0.279400"}; 

  // // tsnk position
  // std::vector<int> TSNKS = {30, 32, 34};
  // // t1 insertions
  // std::vector<int> T1INS = {9};

  // WIT ROME METHOD
  // tsnk position
  std::vector<int> TSNKS = {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
    28, 29, 30, 31, 32, 33, 34};
  std::vector<int> TSNKS_3PT = {30, 32, 34};
  // t1 insertions
  std::vector<int> T1INS = {8}; 

  // smearings
  // light
  std::vector<std::string> WIDTHS_l = {"10.18"}; //{"8.61", "10.18"}; //, "12.30"};
  // strange
  std::vector<std::string> WIDTHS_s = {"7.82", "9.40"}; //, "10.18"};


  // gauge field
  std::string GAUGE_NAME = "gauge";
  void make_gauge(Application &application){
    MIO::LoadNersc::Par gauge;
    gauge.file = "/mnt/lustre/tursafs1/home/dp391/shared/data/configs/dwf_2+1f/F1M/ckpoint_EODWF_lat";
    application.createModule<MIO::LoadNersc>(GAUGE_NAME, gauge);
  }

  // light action
  std::string make_action_l(Application &application){
    std::string action_name = "action_l";
    MAction::ScaledDWF::Par action_l;
    action_l.gauge = GAUGE_NAME;
    action_l.Ls = 12;
    action_l.M5 = 1.8;
    action_l.scale = 2;
    action_l.mass = 0.002144;
    action_l.boundary = "1 1 1 -1";
    action_l.twist = "0. 0. 0. 0.";   
    application.createModule<MAction::ScaledDWF>(action_name, action_l);
    return action_name;
  }
  // light solver
  std::string make_solver_l(Application &application){
    MSolver::RBPrecCG::Par solver;
    solver.action = "action_l";
    solver.maxIteration = 100000;
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
    action_s.Ls = 12;
    action_s.M5 = 1.8;
    action_s.scale = 2;
    action_s.mass = 0.022170;
    action_s.boundary = "1 1 1 -1";
    action_s.twist = "0. 0. 0. 0.";
    application.createModule<MAction::ScaledDWF>(action_name, action_s);
    return action_name;
  }
  // strange solver
  std::string make_solver_s(Application &application){
    MSolver::RBPrecCG::Par solver;
    solver.action = "action_s";
    solver.maxIteration = 100000;
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
    solver.maxIteration = 100000;
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
    action_b.mass = 2.787700;
    action_b.csw_r = 2.844100;
    action_b.csw_t = 2.844100;
    action_b.cF = 1.13142020726806e-310;
    action_b.clover_anisotropy.isAnisotropic = "true";
    action_b.clover_anisotropy.t_direction = 3;
    action_b.clover_anisotropy.xi_0 = 1;
    action_b.clover_anisotropy.nu = 1.227300;
    action_b.boundary = "1 1 1 -1";
    action_b.twist = "0. 0. 0. 0.";
    application.createModule<MAction::WilsonClover>(action_name, action_b);
    return action_name;
  }
  // bottom solver
  std::string make_solver_b(Application &application){
    MSolver::RBPrecCG::Par solver;
    solver.action = "action_b";
    solver.maxIteration = 100000;
    solver.residual = 1e-40;

    std::string solver_name = "solver_b";
    application.createModule<MSolver::RBPrecCG>(solver_name, solver);
    return solver_name;
  }

}