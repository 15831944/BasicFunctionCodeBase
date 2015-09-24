## Add here the options you want to have for compilation, for each 
## group of device driver.
## default template is -c2 -O0
## extra options are 
## -G    : enable debug information
## -Dxxx : specific define for a group of drivers
## type st20icc -Z for all options
DVD_MAKE_VERSION := 2
include $(DVD_MAKE)/generic.mak
C_CC_GLOBAL  = -DNG_CPU_ST20C2 -DNG_COMP_ST20CC -DNG_RTOS -DNGOS_ISR_SUPPORTED 
C_CC_STIP    = -c2 -G -align-to 1 -O2 $(C_CC_GLOBAL)
CC           = st20icc -I$(subst $(BAD_SLASH),$(GOOD_SLASH),../../include) -I$(subst $(BAD_SLASH),$(GOOD_SLASH),../../include/nexgen)
ML           = st20libr
LK           = st20link os20ilc3.lib os20intc1.lib
## CP           = copy >NUL: 2>NUL:
CP           = copy
DEL          = del /F /Q /S >NUL: 2>NUL:
ECHO         = echo
##################
## Dependencies ##
##################
all : sntp.tco
$(ML) -F sntp.lbb -o sntp.lib
$(CP) sntp.lib $(subst $(BAD_SLASH),$(GOOD_SLASH),../../libs)
sntp.tco : sntp.c sntp.h 
$(CC) sntp.c         -o sntp.tco  $(C_CC_STIP) 
purge :
@echo Cleaning...
-@$(DEL) *.tco
-@$(DEL) *.lib
-@$(DEL) *.dbg
-@$(DEL) *.lku
-@$(DEL) *.map
-@$(DEL) *.bak
-@$(DEL) *.hex
-@$(DEL) *.bin
