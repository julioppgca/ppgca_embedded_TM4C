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

/* Debug breakpoint value */
#define BREAK_POINT_VALUE   1   // Debug purpose only

/* global variables */
float32_t g_float32_ai0[SAMPLE_FRAME] = { };
float32_t g_float32_scope[SAMPLE_FRAME] = { };
float rms_ai0 = 0;

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
void rmsCalc_Task(void)
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
        // apply adc offset
        arm_offset_f32(g_float32_ai0, -ADC_OFFSET, g_float32_ai0, SAMPLE_FRAME);
        // apply adc scale TODO: Use sensor table to get value in ampere
        arm_scale_f32(g_float32_ai0, ADC_SCALE, g_float32_ai0, SAMPLE_FRAME);
        // get rms value
        arm_rms_f32(g_float32_ai0, SAMPLE_FRAME, &rms_ai0);

        // make a copy to graph view
        arm_copy_f32(g_float32_ai0, g_float32_scope, SAMPLE_FRAME);

        //TODO: find fft max bin in ccs (tools, graph, fft_magnitude)
        // Write ccs values here:
        // Frequency: 60Hz
        // Value: 0.319140625

        //TODO: in class
        // find fft max bin with CMSIS DSP (Danger: stack peak!)
//        arm_rfft_fast_instance_f32 s;
//        arm_rfft_fast_init_f32(&s, SAMPLE_FRAME);
//        arm_rfft_fast_f32(&s, g_float32_ai0, fftResult, 0);
//        arm_cmplx_mag_f32(fftResult,fftResultMag,SAMPLE_FRAME/2);
//        arm_max_f32(fftResultMag, SAMPLE_FRAME/2, &maxValue, &maxIndex);
//        // Frequency: 60Hz
//        // Value: 0.319117934
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
