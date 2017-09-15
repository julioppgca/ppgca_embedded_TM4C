/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-B06
 */

#include <xdc/std.h>

#include <ti/sysbios/family/arm/m3/Hwi.h>
extern const ti_sysbios_family_arm_m3_Hwi_Handle ADC_Seq0_ISR_Handle;

#include <ti/sysbios/family/arm/m3/Hwi.h>
extern const ti_sysbios_family_arm_m3_Hwi_Handle ADC_Seq1_ISR_Handle;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle initSamplesTask_Handle;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle s_adc0_pong_ready;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle signalProcessing_Task_Handle;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle s_adc0_ping_ready;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle s_adc1_ping_ready;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle s_adc1_pong_ready;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle heartBeat_Task_Handle;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle s_critical_section;

extern int xdc_runtime_Startup__EXECFXN__C;

extern int xdc_runtime_Startup__RESETFXN__C;

