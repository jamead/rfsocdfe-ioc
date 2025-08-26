#!../../bin/linux-x86_64/cpc

#- You may have to change cpc to something else
#- everywhere it appears in this file

< envPaths

epicsEnvSet("IOCNAME", "lab")

# PSC IP address
epicsEnvSet("CPC_IP", "10.0.142.116"); 


cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/cpc.dbd"
cpc_registerRecordDeviceDriver pdbbase



## Load record instances
dbLoadRecords("db/control.db","P=$(IOCNAME), NO=1")
dbLoadRecords("db/lstats.db","P=$(IOCNAME), NO=1")
dbLoadRecords("db/brdstats.db","P=$(IOCNAME), NO=1")
dbLoadRecords("db/fpgabin.db","P=$(IOCNAME), NO=1")
dbLoadRecords("db/sfpdb_control.db","P=$(IOCNAME), NO=1")





var(PSCDebug, 5)	#5 full debug

#CPC Create the PSC
createPSC("cpc1", $(CPC_IP), 3000, 0)
setPSCSendBlockSize("cpc1", 80000, 80000)


cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncxxx,"user=mead"
