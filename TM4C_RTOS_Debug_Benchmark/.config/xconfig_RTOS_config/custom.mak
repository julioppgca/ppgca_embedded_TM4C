## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,em4f linker.cmd package/cfg/RTOS_config_pem4f.oem4f

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/RTOS_config_pem4f.xdl
	$(SED) 's"^\"\(package/cfg/RTOS_config_pem4fcfg.cmd\)\"$""\"C:/Users/skelter_win/embedded2/ppgca_embedded_TM4C/TM4C_RTOS_Debug_Benchmark/.config/xconfig_RTOS_config/\1\""' package/cfg/RTOS_config_pem4f.xdl > $@
	-$(SETDATE) -r:max package/cfg/RTOS_config_pem4f.h compiler.opt compiler.opt.defs
