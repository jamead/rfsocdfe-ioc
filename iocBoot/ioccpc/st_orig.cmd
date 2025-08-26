#!//home/mead/epics/pscdrv/bin/linux-x86_64/pscdemo 
epicsEnvSet("TOP","/home/mead/epics/pscdrv")
epicsEnvSet("BPMDIR","$(TOP)")
epicsEnvSet("CHP_DBDIR","/home/mead/epics/chpioc")


epicsEnvSet("IOCNAME", "LN-BI")



epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST", "YES")
#epicsEnvSet("EPICS_CA_ADDR_LIST", "10.0.142.20")
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES", "16000000")


## You may have to change psc to something else
## everywhere it appears in this file

## Register all support components
dbLoadDatabase("$(BPMDIR)/dbd/pscdemo.dbd",0,0)
pscdemo_registerRecordDeviceDriver(pdbbase) 

# BPM IP address
epicsEnvSet("BPM1_IP", "10.0.142.10");  #4009


## Load record instances
epicsEnvSet("ALEN", "10000");   # ADC length
epicsEnvSet("TLEN", "10000");   # TbT Length

########## use template

### PVs for first LINAC ZBPM:
dbLoadRecords("$(CHP_DBDIR)/chp.db", "P=$(IOCNAME), NO=1, ADC_WFM_LEN=$(ALEN), TBT_WFM_LEN=$(TLEN), SAVG_N=100, SAVG_NSAM=100")

#####################################################
var(PSCDebug, 5)	#5 full debug

#bpm1 Create the PSC
createPSC("chp_tx_1", $(BPM1_IP), 7, 0)
#createPSC("chp_wfm_rx_1", $(BPM1_IP), 20, 0)
createPSC("chp_stat_rx_1", $(BPM1_IP), 600, 0)

###########
iocInit
###########

#epicsThreadSleep 1

#dbpf $(IOCNAME){BPM:11}Loc:Machine-SP, 5

