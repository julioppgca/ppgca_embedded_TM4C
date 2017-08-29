# invoke SourceDir generated makefile for RTOS_config.pem4f
RTOS_config.pem4f: .libraries,RTOS_config.pem4f
.libraries,RTOS_config.pem4f: package/cfg/RTOS_config_pem4f.xdl
	$(MAKE) -f /Users/juliosantos/ppgca_embedded/TM4C_RTOS_rms/src/makefile.libs

clean::
	$(MAKE) -f /Users/juliosantos/ppgca_embedded/TM4C_RTOS_rms/src/makefile.libs clean

