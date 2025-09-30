namespace ENS{

  // lattice extent
  int LLAT = 32; // lattice L extent
  int TLAT = 64; // lattice T extent
  double AINV = 2.3586; // inverse lattice spacing a GeV



  // charm masses
  //std::vector<double> CMASS = {0.30, 0.35, 0.40}; 
  std::vector<std::string> CMASS = {"0.23504", "0.26848", "0.3288"}; 

  // tsnk position
  std::vector<int> TSNKS = {25, 27, 30};
  // t1 insertions
  std::vector<int> T1INS = {8};

  // smearings
  // light
  std::vector<std::string> WIDTHS_l = {"7.50", "8.86", "10.72"};
  // strange
  std::vector<std::string> WIDTHS_s = {"6.82", "8.18", "8.86"};



  // gauge field
  std::string GAUGE_NAME = "gauge";
  std::string GAUGEF_NAME = "gaugef";
  void make_gauge(Application &application){
    MIO::LoadNersc::Par gauge;
    gauge.file = "/mnt/lustre/tursafs1/home/dp392/dp392/shared/projects/exclusive/raw/M1M/confs/ckpoint_M1M_lat";
    application.createModule<MIO::LoadNersc>(GAUGE_NAME, gauge);

    MUtilities::GaugeSinglePrecisionCast::Par gaugef;
    gaugef.field = GAUGE_NAME;
    application.createModule<MUtilities::GaugeSinglePrecisionCast>(GAUGEF_NAME, gaugef);
  }

  // light action
  std::string make_action_l(Application &application){
    std::string action_name = "action_l";
    MAction::ScaledDWF::Par action_l;
    action_l.gauge = GAUGE_NAME;
    action_l.Ls = 12;
    action_l.M5 = 1.8;
    action_l.scale = 2;
    action_l.mass = 0.004;
    action_l.boundary = "1 1 1 -1";
    action_l.twist = "0. 0. 0. 0.";   
    application.createModule<MAction::ScaledDWF>(action_name, action_l);

    std::string actionf_l_name = "actionf_l";
    MAction::ScaledDWFF::Par actionf_l;
    actionf_l.gauge = GAUGEF_NAME;
    actionf_l.Ls = 12;
    actionf_l.M5 = 1.8;
    actionf_l.scale = 2;
    actionf_l.mass = 0.004;
    actionf_l.boundary = "1 1 1 -1";
    actionf_l.twist = "0. 0. 0. 0.";   
    application.createModule<MAction::ScaledDWFF>(actionf_l_name, actionf_l);

    return action_name;
  }
  //light solver
  std::string make_solver_l(Application &application){
    MSolver::MixedPrecisionRBPrecCG::Par solver;
    solver.innerAction = "actionf_l";
    solver.outerAction = "action_l";
    solver.maxInnerIteration = 100000;
    solver.maxOuterIteration = 100000;
    solver.residual = 1e-8;

    std::string solver_name = "solver_l";
    application.createModule<MSolver::MixedPrecisionRBPrecCG>(solver_name, solver);
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
    action_s.mass = 0.02539;
    action_s.boundary = "1 1 1 -1";
    action_s.twist = "0. 0. 0. 0.";
    application.createModule<MAction::ScaledDWF>(action_name, action_s);

    std::string actionf_s_name = "actionf_s";
    MAction::ScaledDWFF::Par actionf_s;
    actionf_s.gauge = GAUGEF_NAME;
    actionf_s.Ls = 12;
    actionf_s.M5 = 1.8;
    actionf_s.scale = 2;
    actionf_s.mass = 0.02539;
    actionf_s.boundary = "1 1 1 -1";
    actionf_s.twist = "0. 0. 0. 0.";   
    application.createModule<MAction::ScaledDWFF>(actionf_s_name, actionf_s);

    return action_name;
  }

  // strange solver
  std::string make_solver_s(Application &application){
    MSolver::MixedPrecisionRBPrecCG::Par solver;
    solver.innerAction = "actionf_s";
    solver.outerAction = "action_s";
    solver.maxInnerIteration = 100000;
    solver.maxOuterIteration = 100000;
    solver.residual = 1e-8;

    std::string solver_name = "solver_s";
    application.createModule<MSolver::MixedPrecisionRBPrecCG>(solver_name, solver);
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

    std::string actionf_name = "actionf_c_m" + mass + "_tw_" + make_twist_name(twist);
    MAction::ScaledDWFF::Par actionf_c;
    actionf_c.gauge = GAUGEF_NAME;
    actionf_c.Ls = 12;
    actionf_c.M5 = 1.6;
    actionf_c.scale = 2;
    actionf_c.mass = std::stod(mass);
    actionf_c.boundary = "1 1 1 -1";
    actionf_c.twist = make_twist_par(twist);
    application.createModule<MAction::ScaledDWFF>(actionf_name, actionf_c);
    
    return action_name;
  }
  
  // charm solver
  std::string make_solver_c(Application &application, std::string action){
    MSolver::MixedPrecisionRBPrecCG::Par solver;
    solver.innerAction = "actionf_" + remove_str(action, "action_");
    solver.outerAction = action;
    solver.maxInnerIteration = 100000;
    solver.maxOuterIteration = 100000;
    solver.residual = 1e-12;

    std::string solver_name = "solver_" + remove_str(action, "action_");
    application.createModule<MSolver::MixedPrecisionRBPrecCG>(solver_name, solver);
    return solver_name;
  }



  // bottom action
  std::string make_action_b(Application &application){
    std::string action_name = "action_b";
    MAction::WilsonClover::Par action_b;
    action_b.gauge = GAUGE_NAME;
    action_b.mass = 3.277000;
    action_b.csw_r = 2.640000;
    action_b.csw_t = 2.640000;
    action_b.cF = 1.13142020726806e-310;
    action_b.clover_anisotropy.isAnisotropic = "true";
    action_b.clover_anisotropy.t_direction = 3;
    action_b.clover_anisotropy.xi_0 = 1;
    action_b.clover_anisotropy.nu = 2.186000;
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