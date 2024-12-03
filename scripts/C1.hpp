namespace ENS{

  // lattice extent
  int LLAT = 24; // lattice L extent
  int TLAT = 64; // lattice T extent



  // charm masses
  std::vector<double> CMASS = {0.3, 0.35, 0.4}; 

  // tsnk position
  std::vector<int> TSNKS = {18, 20, 22};
  // t1 insertions
  std::vector<int> T1INS = {6};

  // smearings
  // light
  std::vector<double> WIDTHS_l = {5.5, 6.5, 7.86};
  // strange
  std::vector<double> WIDTHS_s = {5.0, 6.0, 6.5};



  // std::vector<std::array<double, 4>> TWISTS = {
  //     {.0, .0, .0, .0},
  //     {-0.63, -0.63, -0.63, .0},
  //     {-0.89, -0.89, -0.89, .0},
  //     {-1.10, -1.10, -1.10, .0},
  //     {-1.55, -1.55, -1.55, .0},
  //     {-1.90, -1.90, -1.90, .0},
  //     {-2.19, -2.19, -2.19, .0},
  //     {-2.45, -2.45, -2.45, .0},
  //     {-2.69, -2.69, -2.69, .0},
  //     {-2.90, -2.90, -2.90, .0}
  // };

  // gauge field
  std::string GAUGE_NAME = "gauge";
  void make_gauge(Application &application){
    MIO::LoadNersc::Par gauge;
    gauge.file = "/home/dp207/dp207/shared/data/configs/dwf_2+1f/C1/ckpoint_lat";
    application.createModule<MIO::LoadNersc>(GAUGE_NAME, gauge);
  }

  // do we need eigenvalues??
  // light action
  std::string make_action_l(Application &application){
    std::string action_name = "action_l";
    MAction::DWF::Par action_l;
    action_l.gauge = GAUGE_NAME;
    action_l.Ls = 16;
    action_l.M5 = 1.8;
    action_l.boundary = "1 1 1 -1";
    action_l.twist = "0. 0. 0. 0.";
    action_l.mass = 0.005;
    application.createModule<MAction::DWF>(action_name, action_l);
    return action_name;
  }

  // strange action
  std::string make_action_s(Application &application){
    std::string action_name = "action_s";
    MAction::DWF::Par action_s;
    action_s.gauge = GAUGE_NAME;
    action_s.Ls = 16;
    action_s.M5 = 1.8;
    action_s.boundary = "1 1 1 -1";
    action_s.twist = "0. 0. 0. 0.";
    action_s.mass = 0.0362;
    application.createModule<MAction::DWF>(action_name, action_s);
    return action_name;
  }


  // charm action
  std::string make_action_c(Application &application, double mass, std::array<double, 4> twist){
      std::string action_name = "action_c_m" + double_to_string(mass) + "_tw_" + make_twist_name(twist);
      MAction::ScaledDWF::Par action_c;
      action_c.gauge = GAUGE_NAME;
      action_c.Ls = 12;
      action_c.M5 = 1.6;
      action_c.scale = 2;
      action_c.boundary = "1 1 1 -1";
      action_c.twist = make_twist_par(twist);
      action_c.mass = mass;
      application.createModule<MAction::ScaledDWF>(action_name, action_c);
      return action_name;
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

}