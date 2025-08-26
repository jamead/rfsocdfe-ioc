TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = cpc
# cpc.dbd will be created and installed
DBD += cpc.dbd

# cpc.dbd will be made up from these files:
cpc_DBD += base.dbd

# Include dbd files from all support applications:
cpc_DBD += pscCore.dbd
cpc_DBD += readSendBinFile.dbd
cpc_DBD += readBinFile.dbd

# Add all the support libraries needed by this IOC
cpc_LIBS += pscCore

# Add your custom C source here:
cpc_SRCS += readSendBinFile.c  
cpc_SRCS += readBinFile.c 

# cpc_registerRecordDeviceDriver.cpp derives from cpc.dbd
cpc_SRCS += cpc_registerRecordDeviceDriver.cpp


# Build the main IOC entry point on workstation OSs.
cpc_SRCS_DEFAULT += cpcMain.cpp
cpc_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#cpc_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
cpc_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

