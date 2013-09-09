# Load sample
LoadNexus(Filename=r'/home/tianhl/workarea/CSNS_SANS_SIM/app/test/sample_sans.nxs',OutputWorkspace='sans_mc')
LoadInstrument(Filename=r'/home/tianhl/workarea/CSNS_SANS_SIM/app/reduce/CSNS_SANS_IDF_MC.xml',Workspace='sans_mc')
ConvertToHistogram(InputWorkspace='sans_mc', OutputWorkspace='sans_mc_hist')
# Detector
CropWorkspace(InputWorkspace='sans_mc_hist',OutputWorkspace='main_orig',StartWorkspaceIndex='5',EndWorkspaceIndex='6404')
ConvertUnits(InputWorkspace='main_orig',OutputWorkspace='main_wave',Target='Wavelength')
Rebin(InputWorkspace='main_wave',OutputWorkspace='main',Params='2.2,-0.05,5.5')
# Monitor
CropWorkspace(InputWorkspace='sans_mc_hist',OutputWorkspace='monitor_orig',StartWorkspaceIndex='1',EndWorkspaceIndex='1')
ConvertUnits(InputWorkspace='monitor_orig',OutputWorkspace='monitor_wave',Target='Wavelength')
RebinToWorkspace(WorkspaceToRebin='monitor_wave',WorkspaceToMatch='main',OutputWorkspace='Q_WAVE_conversion')
# Load can
LoadNexus(Filename=r'/home/tianhl/workarea/CSNS_SANS_SIM/app/test/can_sans.nxs',OutputWorkspace='sans_can')
LoadInstrument(Filename=r'/home/tianhl/workarea/CSNS_SANS_SIM/app/reduce/CSNS_SANS_IDF_MC.xml',Workspace='sans_can')
ConvertToHistogram(InputWorkspace='sans_can', OutputWorkspace='sans_can_hist')
# Can Detector
CropWorkspace(InputWorkspace='sans_can_hist',OutputWorkspace='can_orig',StartWorkspaceIndex='5',EndWorkspaceIndex='6404')
ConvertUnits(InputWorkspace='can_orig',OutputWorkspace='can_wave',Target='Wavelength')
Rebin(InputWorkspace='can_wave',OutputWorkspace='can',Params='2.2,-0.05,5.5')
# Can Monitor
CropWorkspace(InputWorkspace='sans_can_hist',OutputWorkspace='monitor_can_orig',StartWorkspaceIndex='1',EndWorkspaceIndex='1')
ConvertUnits(InputWorkspace='monitor_can_orig',OutputWorkspace='monitor_can_wave',Target='Wavelength')
RebinToWorkspace(WorkspaceToRebin='monitor_can_wave',WorkspaceToMatch='can',OutputWorkspace='Can_Q_WAVE_conversion')
# direct
LoadNexus(Filename=r'/home/tianhl/workarea/CSNS_SANS_SIM/app/test/directbeam.nxs',OutputWorkspace='direct_beam')
LoadInstrument(Filename=r'/home/tianhl/workarea/CSNS_SANS_SIM/app/reduce/CSNS_SANS_IDF_MC.xml',Workspace='direct_beam')
CropWorkspace(InputWorkspace='direct_beam',OutputWorkspace='direct_orig',StartWorkspaceIndex='2',EndWorkspaceIndex='4')
ConvertToHistogram(InputWorkspace='direct_orig', OutputWorkspace='direct_hist')
ConvertUnits(InputWorkspace='direct_hist',OutputWorkspace='direct_wave',Target='Wavelength')
Rebin(InputWorkspace='direct_wave',OutputWorkspace='direct_wave',Params='2.2,-0.05,5.5')
# Trans
CropWorkspace(InputWorkspace='sans_mc_hist',OutputWorkspace='trans_orig',StartWorkspaceIndex='2',EndWorkspaceIndex='4')
ConvertUnits(InputWorkspace='trans_orig',OutputWorkspace='trans_wave_tmp',Target='Wavelength')
InterpolatingRebin(InputWorkspace='trans_wave_tmp',OutputWorkspace='trans_wave',Params='2.2,-0.05,5.5')
CalculateTransmission(SampleRunWorkspace='trans_wave',DirectRunWorkspace='direct_wave',OutputWorkspace='transmission',IncidentBeamMonitor='4',TransmissionMonitor='5',RebinParams='2.2,-0.05,5.5',OutputUnfittedData='1')
Multiply(LHSWorkspace='transmission',RHSWorkspace='Q_WAVE_conversion',OutputWorkspace='Q_WAVE_conversion')
# Can Trans
CropWorkspace(InputWorkspace='sans_can_hist',OutputWorkspace='trans_can_orig',StartWorkspaceIndex='2',EndWorkspaceIndex='4')
ConvertUnits(InputWorkspace='trans_can_orig',OutputWorkspace='trans_can_wave_tmp',Target='Wavelength')
InterpolatingRebin(InputWorkspace='trans_can_wave_tmp',OutputWorkspace='trans_can_wave',Params='2.2,-0.05,5.5')
CalculateTransmission(SampleRunWorkspace='trans_can_wave',DirectRunWorkspace='direct_wave',OutputWorkspace='can_transmission',IncidentBeamMonitor='4',TransmissionMonitor='5',RebinParams='2.2,-0.05,5.5',OutputUnfittedData='1')
Multiply(LHSWorkspace='can_transmission',RHSWorkspace='Can_Q_WAVE_conversion',OutputWorkspace='Can_Q_WAVE_conversion')
# Q1D
Q1D(DetBankWorkspace='main',OutputWorkspace='main_Q1D',OutputBinning='0.009,0.002,0.285',WavelengthAdj='Q_WAVE_conversion', AccountForGravity='1')
Q1D(DetBankWorkspace='can',OutputWorkspace='can_Q1D',OutputBinning='0.009,0.002,0.285',WavelengthAdj='Can_Q_WAVE_conversion', AccountForGravity='1')
Minus(LHSWorkspace='main_Q1D',RHSWorkspace='can_Q1D',OutputWorkspace='Q1D')

