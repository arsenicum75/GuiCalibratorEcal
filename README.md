# GuiCalibratorEcal
GUI application for calibration of the electromagnetic calorimeter ECal of the HADES experiment

To run: 
  in ROOT command line run 
  .x run_gui_calibrator_ecal.cc(sector, cell)

  or run directly from bash (you need installed ROOT):
  root -l run_gui_calibrator_ecal.cc\(sector\,cell\)

You need to have 4 files with data: 
  Ag123Ag_High_gen3_v2.root - file with tree containing high field data
  Ag123Ag_Low_gen3_v2.root  - file with tree containing low field data
  EventListsHigh.root       - file with eventlists corresponding to each module at high field
  EventListsLow.root        - file with eventlists corresponding to each module at low field
  
The eventlists can be produced from trees by macro make_event_list.C

The structure of the tree is the following:
{sector, cell, cell2, tof, theta, phi, richQa, emc_mq, mom, beta, charge, mm_qa, energy, maxenergy, time, time2, size, emc_theta, emc_phi, emc_path, candpart_length, emc_length, beamtime, TOT, TOT2, dthetadphi_qa, dtime, chi2, raw_time, raw_time2}

