LoadNexus(Filename=r'/home/tianhl/workarea/CSNS_SANS_SIM/app/test/sample_sans.nxs',OutputWorkspace='sans_mc')
LoadInstrument(Filename=r'/home/tianhl/workarea/CSNS_SANS_SIM/app/reduce/CSNS_SANS_IDF_MC.xml',Workspace='sans_mc')
ConvertToHistogram(InputWorkspace='sans_mc', OutputWorkspace='sans_mc_hist')
# Detector
CropWorkspace(InputWorkspace='sans_mc_hist',OutputWorkspace='main_orig',StartWorkspaceIndex='2',EndWorkspaceIndex='6401')
ConvertUnits(InputWorkspace='main_orig',OutputWorkspace='main_wave',Target='Wavelength')
Rebin(InputWorkspace='main_wave',OutputWorkspace='main',Params='2.2,0.156,10')
# Monitor
CropWorkspace(InputWorkspace='sans_mc_hist',OutputWorkspace='monitor_orig',StartWorkspaceIndex='1',EndWorkspaceIndex='1')
ConvertUnits(InputWorkspace='monitor_orig',OutputWorkspace='monitor_wave',Target='Wavelength')
RebinToWorkspace(WorkspaceToRebin='monitor_wave',WorkspaceToMatch='main',OutputWorkspace='Q_WAVE_conversion')
# Q1D
Q1D(DetBankWorkspace='main',OutputWorkspace='main_Q1D',OutputBinning='0.009,0.002,0.285',WavelengthAdj='Q_WAVE_conversion', AccountForGravity='1')
