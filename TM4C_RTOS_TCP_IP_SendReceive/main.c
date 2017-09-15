/* Standard variables definitions */
#include <stdint.h>
#include <stdbool.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/cfg/global.h> // to get static definitions from RTOS.cfg file

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

/* Project Header files */
#include "project_includes/Board.h"
#include "project_includes/adc_sample.h"
#include "project_includes/eth_network.h"

/* CMSIS DSP math library */
#include <arm_math.h>

/* C standard libs */
#include <stdio.h>


/* Debug breakpoint value */
#define BREAK_POINT_VALUE   1   // Debug purpose only

/* global variables */
float32_t g_float32_ai0[SAMPLE_FRAME] = { };
float32_t g_float32_scope[SAMPLE_FRAME] = { };
float32_t rms_ai0 = 0;
float32_t g_float_ai0_mean=0;

float32_t fftResult[SAMPLE_FRAME] = { };
float32_t fftResultMag[SAMPLE_FRAME/2] = { };
float32_t maxValue=0;
uint32_t maxIndex=0;


/*
 *  ======== main ========
 */
int main(void)
{
    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initEMAC();

    System_printf("\nPeripheral initiated..." "\nStarting BIOS...\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}

/* initialize ADC/DMA data acquisition */
void initSamplesTask(void)
{
    DMA_init();
    ADC_init();
    TIMER_ADC_init(SAMPLE_FREQUENCY);
}

/* compute rms value  */
void signalProcessing_Task(void)
{
    uint16_t i;

    while (1)
    {
        // wait here until g_uint16_adc0_ping is being filled by DMA
        Semaphore_pend(s_adc0_ping_ready, BIOS_WAIT_FOREVER);
        // get the first part of the vector
        for(i=0;i<DMA_BUFFER_SIZE;i++)
            g_float32_ai0[i]=g_uint16_adc0_ping[i];

        // wait here until g_uint16_adc0_ping is being filled by DMA
        Semaphore_pend(s_adc0_pong_ready, BIOS_WAIT_FOREVER);
        // get the first part of the vector
        for(;i<2*DMA_BUFFER_SIZE;i++)
            g_float32_ai0[i]=g_uint16_adc0_pong[i-DMA_BUFFER_SIZE];

        /* use CMSIS DSP to get rms value */
        // calc mean value
        arm_mean_f32(g_float32_ai0, SAMPLE_FRAME, &g_float_ai0_mean);
        // apply adc offset
        arm_offset_f32(g_float32_ai0, -g_float_ai0_mean, g_float32_ai0, SAMPLE_FRAME);
        // apply adc scale TODO: Use sensor table to get value in ampere
        arm_scale_f32(g_float32_ai0, ADC_SCALE, g_float32_ai0, SAMPLE_FRAME);
        // get rms value
        arm_rms_f32(g_float32_ai0, SAMPLE_FRAME, &rms_ai0);

        // make a copy to graph view
        arm_copy_f32(g_float32_ai0, g_float32_scope, SAMPLE_FRAME);

        // find fft max bin with CMSIS DSP
        arm_rfft_fast_instance_f32 s;
        arm_rfft_fast_init_f32(&s, SAMPLE_FRAME);
        arm_rfft_fast_f32(&s, g_float32_ai0, fftResult, 0);
        arm_cmplx_mag_f32(fftResult,fftResultMag,SAMPLE_FRAME/2);
        arm_max_f32(fftResultMag, SAMPLE_FRAME/2, &maxValue, &maxIndex);

        // block access of g_str_SendResult while updating the value
        Semaphore_pend(s_critical_section, BIOS_WAIT_FOREVER);
            sprintf(g_str_SendResult,"AI0: %.6f Vrms",rms_ai0);
        Semaphore_post(s_critical_section); // release g_str_SendResult variable

        //TODO: get amplitude and angle (of the major frequency) from fftResult
        //      and send it over TCPIP when F command is received.

    }
}

/* heart beat from RTOS*/
void heartBeat_Task(void)
{
    while (1)
    {
        GPIO_toggle(Board_LED0);    // blink LED
        Task_sleep(500);            // goto to sleep
    }
}
