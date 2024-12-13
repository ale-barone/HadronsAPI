namespace ENS{

  // lattice extent
  int LLAT = 24; // lattice L extent
  int TLAT = 64; // lattice T extent
  double AINV = 1.7295; // inverse lattice spacing a^-1 = 1.7295(38) GeV



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


  // gauge field
  std::string GAUGE_NAME = "gauge";
  void make_gauge(Application &application){
    MIO::LoadNersc::Par gauge;
    gauge.file = "/mnt/lustre/tursafs1/home/dp207/dp207/shared/paboyle/C1M/ckpoint_C1M_lat";
    application.createModule<MIO::LoadNersc>(GAUGE_NAME, gauge);
  }

  // do we need eigenvalues??
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

  // strange action
  std::string make_action_s(Application &application){
    std::string action_name = "action_s";
    MAction::ScaledDWF::Par action_s;
    action_s.gauge = GAUGE_NAME;
    action_s.Ls = 24;
    action_s.M5 = 1.8;
    action_s.mass = 0.0362;
    action_s.boundary = "1 1 1 -1";
    action_s.twist = "0. 0. 0. 0.";
    application.createModule<MAction::ScaledDWF>(action_name, action_s);
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
      action_c.mass = mass;
      action_c.boundary = "1 1 1 -1";
      action_c.twist = make_twist_par(twist);
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