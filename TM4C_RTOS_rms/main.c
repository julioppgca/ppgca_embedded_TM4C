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

/* math standard C library */
#include <math.h>

#define BREAK_POINT_VALUE   4   // Debug purpose only

int32_t g_int32_ai0[2 * DMA_BUFFER_SIZE] = { };
float rms_ai0 = 0;

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

void initSamplesTask(void)
{
    DMA_init();
    ADC_init();
    TIMER_ADC_init(SAMPLE_FREQUENCY);
}

/* compute rms value  */
void rmsCalc_Task(void)
{
    float rms;
    uint16_t i;
    uint16_t x = 0;

    while (1)
    {
        // wait here until g_uint16_adc0_ping is being filled by DMA
        Semaphore_pend(s_adc0_ping_ready, BIOS_WAIT_FOREVER);
        // new rms value calculation
        rms = 0;
        // process first part
        for (i = 0; i < DMA_BUFFER_SIZE; i++)
        {
            g_int32_ai0[i] = (int16_t) g_uint16_adc0_ping[i] - ADC_OFFSET;
            g_int32_ai0[i] *= g_int32_ai0[i];
            rms += g_int32_ai0[i];
        }

        // wait here until g_uint16_adc0_pong is being filled by DMA
        Semaphore_pend(s_adc0_pong_ready, BIOS_WAIT_FOREVER);
        // process second part
        for (; i < 2 * DMA_BUFFER_SIZE; i++)
        {
            g_int32_ai0[i] = (int16_t) g_uint16_adc0_ping[i] - ADC_OFFSET;
            g_int32_ai0[i] *= g_int32_ai0[i];
            rms += g_int32_ai0[i];
        }
        rms /= 2 * DMA_BUFFER_SIZE;
        rms_ai0 = sqrt(rms) * ADC_SCALE;

        // debug purpose only
        if (++x > BREAK_POINT_VALUE)
        {
            x = 0; // insert a break point here
        }
    }
}

/* heart beat from RTOS*/
void heartBeat_Task(void)
{
    while (1)
    {
        GPIO_toggle(Board_LED0);
        Task_sleep(500);
    }
}
