/*
 * adc_sample.h
 *
 *  Created on: 24 de ago de 2017
 *      Author: juliosantos
 */

#ifndef __SAMPLES_CONFIG_H__
#define __SAMPLES_CONFIG_H__

/* Standard variables definitions */
#include <stdint.h>
#include <stdbool.h>

/* XDCtools Header files */
#include <xdc/cfg/global.h>

/* Tivaware Header files */
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_adc.h"
#include "inc/hw_gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"
#include "driverlib/udma.h"
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"


#define DMA_BUFFER_SIZE             1024            // don't change!
#define CLK_FREQ                    120000000       // TM4C clock frequency
#define ADC_OVERSAMPLE_VALUE        64              // Over sample value 2..64 (power of 2 steps)
#define SAMPLE_FREQUENCY            60*512
#define ADC_OFFSET                  2048
#define ADC_SCALE                   3.3/4095


void adcPinConifig(void);
void TIMER_ADC_init(uint32_t sample_freq);
void ADC_init(void);
void DMA_init(void);
void ADC_Seq0_ISR(void);     //Interrupt service of ADC Sequencer 0
void ADC_Seq1_ISR(void);     //Interrupt service of ADC Sequencer 1

extern uint16_t g_uint16_adc0_ping[];
extern uint16_t g_uint16_adc0_pong[];

#endif // __SAMPLES_CONFIG_H__
