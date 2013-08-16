# Load Monta Carlo data from NeXus file. 
LoadNexus(Filename=r'/home/tianhl/workarea/CSNS_SANS_SIM/app/test.nxs',OutputWorkspace='67093_sans')
# Load instrument definition file into mantid
LoadInstrument(Filename=r'/home/tianhl/workarea/CSNS_SANS_SIM/app/CSNS_SANS_IDF_MC.xml',Workspace='67093_sans')
## Crop det data into workspace, and deal with it
#CropWorkspace(InputWorkspace='67093_sans',OutputWorkspace='67093main_orig',StartWorkspaceIndex='2',EndWorkspaceIndex='15627')
#ConvertUnits(InputWorkspace='67093main_orig',OutputWorkspace='67093main_wave',Target='Wavelength')
##Rebin(InputWorkspace='67093main_wave',OutputWorkspace='67093main',Params='2.2,-0.035,10')
#Rebin(InputWorkspace='67093main_wave',OutputWorkspace='67093main',Params='2.2,0.156,10')
## Crop monitor data into workspace, and deal with it
#CropWorkspace(InputWorkspace='67093_sans',OutputWorkspace='Monitor_orig',StartWorkspaceIndex='1',EndWorkspaceIndex='1')
#ConvertUnits(InputWorkspace='Monitor_orig',OutputWorkspace='Monitor_wave',Target='Wavelength')
##Rebin(InputWorkspace='Monitor_wave',OutputWorkspace='Monitor',Params='2.2,-0.035,10')
#Rebin(InputWorkspace='Monitor_wave',OutputWorkspace='Monitor',Params='2.2,0.156,10')
#RebinToWorkspace(WorkspaceToRebin='Monitor',WorkspaceToMatch='67093main',OutputWorkspace='Q_WAVE_conversion')
## Convert to Q values, and adjust by wave correction
#Q1D(DetBankWorkspace='67093main',OutputWorkspace='67093main_Q1D',OutputBinning='0.009,0.002,0.285',WavelengthAdj='Q_WAVE_conversion')
#Q1D(DetBankWorkspace='67093main',OutputWorkspace='67093main_Q1D_orig',OutputBinning='0.009,0.002,0.285')



