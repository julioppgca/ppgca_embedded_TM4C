/*
 * eth_network.h
 *
 *  Created on: 24 de ago de 2017
 *      Author: juliosantos
 */

#ifndef PROJECT_INCLUDES_ETH_NETWORK_H_
#define PROJECT_INCLUDES_ETH_NETWORK_H_

/* Standard variables definitions */
#include <stdint.h>
#include <stdbool.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/cfg/global.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

/* TM4C tivaware RTOS lib */
#include <ti/drivers/GPIO.h>

/* NDK BSD support */
#include <sys/socket.h>

/* Tivaware Header files */
#include "inc/hw_types.h"
#include "inc/hw_nvic.h"  // NVIC direct write
#include "driverlib/systick.h" // Systick lib
#include "driverlib/rom_map.h" // ROM_EMAC call
#include "driverlib/rom.h"


#include <string.h>

#define TCPPORT             1000
#define TCPHANDLERSTACK     1024
#define TCPPACKETSIZE       256
#define NUMTCPWORKERS       3
#define SEND_PACKET_SIZE    100


// messages constants
#define HELP    "V1.1 - R = read rms value of AIN0 - F = read fft amplitude and angle"


extern char g_str_SendResult[];
extern char g_str_SendResult2[];

#endif /* PROJECT_INCLUDES_ETH_NETWORK_H_ */
