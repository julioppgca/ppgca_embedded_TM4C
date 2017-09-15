#include "project_includes/adc_sample.h"

uint32_t udmaCtrlTable[DMA_BUFFER_SIZE]__attribute__((aligned(DMA_BUFFER_SIZE))); // uDMA control table variable

uint16_t g_uint16_adc0_ping[DMA_BUFFER_SIZE];
uint16_t g_uint16_adc0_pong[DMA_BUFFER_SIZE];
uint16_t g_uint16_adc1_ping[DMA_BUFFER_SIZE];
uint16_t g_uint16_adc1_pong[DMA_BUFFER_SIZE];


void adcPinConfig(void)
{
    // Enable Peripheral Clocks
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);

    // Configure the GPIO Pin Mux for PE3
    // for AIN0
    MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    // Configure the GPIO Pin Mux for PE2
    // for AIN1
    MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);
    // Configure the GPIO Pin Mux for PE1
    // for AIN2
    MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);
    // Configure the GPIO Pin Mux for PE0
    // for AIN3
    MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);
    // Configure the GPIO Pin Mux for PD7
    // for AIN4
    MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_7);
    // Configure the GPIO Pin Mux for PD6
    // for AIN5
    MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_6);
    // Configure the GPIO Pin Mux for PD5
    // for AIN6
    MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_5);
    // Configure the GPIO Pin Mux for PD4
    // for AIN7
    MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_4);
    // Configure the GPIO Pin Mux for PE5
    // for AIN8
    MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_5);
    // Configure the GPIO Pin Mux for PE4
    // for AIN9
    MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);
    // Configure the GPIO Pin Mux for PB4
    // for AIN10
    MAP_GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_4);
    // Configure the GPIO Pin Mux for PB5
    // for AIN11
    MAP_GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_5);
    // Configure the GPIO Pin Mux for PD3
    // for AIN12
    MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_3);
    // Configure the GPIO Pin Mux for PD2
    // for AIN13
    MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_2);
    // Configure the GPIO Pin Mux for PD1
    // for AIN14
    MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_1);
    // Configure the GPIO Pin Mux for PD0
    // for AIN15
    MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0);
    // Configure the GPIO Pin Mux for PK0
    // for AIN16
    MAP_GPIOPinTypeADC(GPIO_PORTK_BASE, GPIO_PIN_0);
    // Configure the GPIO Pin Mux for PK1
    // for AIN17
    MAP_GPIOPinTypeADC(GPIO_PORTK_BASE, GPIO_PIN_1);
    // Unlock the Port Pin and Set the Commit Bit
    HWREG(GPIO_PORTD_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE+GPIO_O_CR)   |= GPIO_PIN_7;
    HWREG(GPIO_PORTD_BASE+GPIO_O_LOCK) = 0x0;
    // Configure the GPIO Pin Mux for PK2
    // for AIN18
    MAP_GPIOPinTypeADC(GPIO_PORTK_BASE, GPIO_PIN_2);
    // Configure the GPIO Pin Mux for PK3
    // for AIN19
    MAP_GPIOPinTypeADC(GPIO_PORTK_BASE, GPIO_PIN_3);
}

void TIMER_ADC_init(uint32_t sample_freq)
{
    // Use Timer for both ADC0 and ADC1
    uint32_t ui32ClockFreq;

    // Set clock frequency
    ui32ClockFreq = SysCtlClockFreqSet(SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
    SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480,
    CLK_FREQ);

    // Enable timer peripheral clock
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);
    SysCtlPeripheralReset(SYSCTL_PERIPH_TIMER4);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER4));

    // Configure the timer
    TimerConfigure(TIMER4_BASE, TIMER_CFG_A_PERIODIC_UP);
    TimerLoadSet(TIMER4_BASE, TIMER_A, (ui32ClockFreq / sample_freq));

    // Enable timer to trigger ADC
    TimerControlTrigger(TIMER4_BASE, TIMER_A, true);

    // Enable event to trigger ADC
    TimerADCEventSet(TIMER4_BASE, TIMER_ADC_TIMEOUT_A);

    //enable timer
    TimerEnable(TIMER4_BASE, TIMER_A);
}

void ADC_init(void)
{
    // configure ADC pins
    adcPinConfig();

    /* ----------- ADC0 Initialization-----------*/

    // Enable ADC peripheral clock
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_ADC0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));

    // Configure the ADC to use PLL at 480 MHz divided by 15 to get an ADC
    // clock of 32 MHz, 2MSPS.
    ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_SRC_PLL | ADC_CLOCK_RATE_FULL, 15);

    // Use Hardware Oversample
    ADCHardwareOversampleConfigure(ADC0_BASE, ADC_OVERSAMPLE_VALUE);

    // Disable before configuring
    ADCSequenceDisable(ADC0_BASE, 0);

    // Configure ADC0 sequencer 0:
    // Triggered by timer, highest priority (0)
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_TIMER, 0);

    // Configure ADC0 sequencer 0 step 0:
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0,
    ADC_CTL_CH0 | ADC_CTL_END | ADC_CTL_IE);

    // Analog reference is internal
    ADCReferenceSet(ADC0_BASE, ADC_REF_INT);

    //  Enable ADC interrupt
    ADCIntEnableEx(ADC0_BASE, ADC_INT_DMA_SS0);
    IntEnable(INT_ADC0SS0);

    // Enable ADC to use uDMA
    ADCSequenceDMAEnable(ADC0_BASE, 0);

    // Enable ADC
    ADCSequenceEnable(ADC0_BASE, 0);

    /* ----------- ADC1 Initialization-----------*/

    // Enable ADC 1 peripheral clock
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    SysCtlPeripheralReset(SYSCTL_PERIPH_ADC1);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC1));

    // Configure the ADC to use PLL at 480 MHz divided by 15 to get an ADC
    // clock of 32 MHz, 2MSPS.
    ADCClockConfigSet(ADC1_BASE, ADC_CLOCK_SRC_PLL | ADC_CLOCK_RATE_FULL, 15);

    // Use Hardware Oversample
    ADCHardwareOversampleConfigure(ADC1_BASE, ADC_OVERSAMPLE_VALUE);

    // Disable before configuring
    ADCSequenceDisable(ADC1_BASE, 0);

    // Configure ADC0 sequencer 0:
    // Triggered by timer, second priority (1)
    ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_TIMER, 1);

    // Configure ADC0 sequencer 0 step 0:
    ADCSequenceStepConfigure(ADC1_BASE, 0, 0,
    ADC_CTL_CH1 | ADC_CTL_END | ADC_CTL_IE);

    // Analog reference is internal
    ADCReferenceSet(ADC1_BASE, ADC_REF_INT);

    //  Enable ADC interrupt
    ADCIntEnableEx(ADC1_BASE, ADC_INT_DMA_SS0);
    IntEnable(INT_ADC1SS0);

    // Enable ADC to use uDMA
    ADCSequenceDMAEnable(ADC1_BASE, 0);

    // Enable ADC
    ADCSequenceEnable(ADC1_BASE, 0);

}

void DMA_init(void)
{
    /* Enable uDMA clock */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UDMA));

    /* Disable uDMA before configure*/
    uDMADisable();

    /* Enable uDMA */
    uDMAEnable();

    // Use channel 24 for ADC1, must replace all 'UDMA_CHANNEL_ADC0' by 'UDMA_CH24_ADC1_0'
    uDMAChannelAssign(UDMA_CH24_ADC1_0);

    /* Set the control table for uDMA */
    uDMAControlBaseSet(udmaCtrlTable);

    /* Disable unneeded attributes of the uDMA channels */
    uDMAChannelAttributeDisable(UDMA_CHANNEL_ADC0, UDMA_ATTR_ALL);

    /* Disable unneeded attributes of the uDMA channels -- ADC1 */
    uDMAChannelAttributeDisable(UDMA_CH24_ADC1_0, UDMA_ATTR_ALL);

    // Only allow burst transfers ADC0
    uDMAChannelAttributeEnable(UDMA_CHANNEL_ADC0, UDMA_ATTR_USEBURST);

    // Only allow burst transfers ADC1
    uDMAChannelAttributeEnable(UDMA_CH24_ADC1_0, UDMA_ATTR_USEBURST);

    // Channel A udma control set
    uDMAChannelControlSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
                          UDMA_SIZE_16 |
                          UDMA_SRC_INC_NONE |
                          UDMA_DST_INC_16 |
                          UDMA_ARB_8);

    uDMAChannelControlSet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT,
                          UDMA_SIZE_16 |
                          UDMA_SRC_INC_NONE |
                          UDMA_DST_INC_16 |
                          UDMA_ARB_8);

    // Channel A transfer set for ADC0
    uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
                           UDMA_MODE_PINGPONG,
                           (void *) (ADC0_BASE + ADC_O_SSFIFO0),
                           g_uint16_adc0_ping,
                           DMA_BUFFER_SIZE);

    uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT,
                           UDMA_MODE_PINGPONG,
                           (void *) (ADC0_BASE + ADC_O_SSFIFO0),
                           g_uint16_adc0_pong,
                           DMA_BUFFER_SIZE);

    // Channel A udma control set for ADC1
   uDMAChannelControlSet(UDMA_CH24_ADC1_0 | UDMA_PRI_SELECT,
                      UDMA_SIZE_16 |
                      UDMA_SRC_INC_NONE |
                      UDMA_DST_INC_16 |
                      UDMA_ARB_8);

   uDMAChannelControlSet(UDMA_CH24_ADC1_0 | UDMA_ALT_SELECT,
                      UDMA_SIZE_16 |
                      UDMA_SRC_INC_NONE |
                      UDMA_DST_INC_16 |
                      UDMA_ARB_8);

   // Channel A transfer set for ADC1
   uDMAChannelTransferSet(UDMA_CH24_ADC1_0 | UDMA_PRI_SELECT,
                       UDMA_MODE_PINGPONG,
                       (void *) (ADC1_BASE + ADC_O_SSFIFO0),
                       g_uint16_adc1_ping,
                       DMA_BUFFER_SIZE);

   uDMAChannelTransferSet(UDMA_CH24_ADC1_0 | UDMA_ALT_SELECT,
                       UDMA_MODE_PINGPONG,
                       (void *) (ADC1_BASE + ADC_O_SSFIFO0),
                       g_uint16_adc1_pong,
                       DMA_BUFFER_SIZE);

    /* Enable the channels */
    // Channel for ADC0
    uDMAChannelEnable(UDMA_CHANNEL_ADC0);

    // Channel for ADC1
    uDMAChannelEnable(UDMA_CH24_ADC1_0);

}

// interrupt vector = 30 (datasheet table 2-9 pg.: 116)
void ADC_Seq0_ISR(void)
{
    uint32_t modePrimary;
    uint32_t modeAlternate;

    // Clear the interrupt
    ADCIntClearEx(ADC0_BASE, ADC_INT_DMA_SS0);

    // Get the mode statuses of primary and alternate control structures
    modePrimary = uDMAChannelModeGet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT);
    modeAlternate = uDMAChannelModeGet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT);

    // Reload the control structures
    if ((modePrimary == UDMA_MODE_STOP) && (modeAlternate != UDMA_MODE_STOP))
    {
        // Need to reload primary control structure
        uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
        UDMA_MODE_PINGPONG,
                               (void *) (ADC0_BASE + ADC_O_SSFIFO0),
                               g_uint16_adc0_ping,
                               DMA_BUFFER_SIZE);
        // free to process g_uint16_adc0_ping
        Semaphore_post(s_adc0_ping_ready);

    }
    else if ((modePrimary != UDMA_MODE_STOP) && (modeAlternate == UDMA_MODE_STOP))
    {
        // Need to reload alternate control structure
        uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT,
        UDMA_MODE_PINGPONG,
                               (void *) (ADC0_BASE + ADC_O_SSFIFO0),
                               g_uint16_adc0_pong,
                               DMA_BUFFER_SIZE);
        // free to process g_uint16_adc0_pong
        Semaphore_post(s_adc0_pong_ready);
    }
}

// interrupt vector = 62 (datasheet table 2-9 pg.: 117)
void ADC_Seq1_ISR(void)
{
    uint32_t modePrimary;
    uint32_t modeAlternate;

    // Clear the interrupt
    ADCIntClearEx(ADC1_BASE, ADC_INT_DMA_SS0);


    // Get the mode statuses of primary and alternate control structures
    modePrimary = uDMAChannelModeGet(UDMA_CH24_ADC1_0 | UDMA_PRI_SELECT);
    modeAlternate = uDMAChannelModeGet(UDMA_CH24_ADC1_0 | UDMA_ALT_SELECT);

    // Reload the control structures
    if ((modePrimary == UDMA_MODE_STOP) && (modeAlternate != UDMA_MODE_STOP))
    {
        // Need to reload primary control structure
        uDMAChannelTransferSet(UDMA_CH24_ADC1_0 | UDMA_PRI_SELECT,
                               UDMA_MODE_PINGPONG,
                               (void *) (ADC1_BASE + ADC_O_SSFIFO0),
                               g_uint16_adc1_ping,
                               DMA_BUFFER_SIZE);
        Semaphore_post(s_adc1_ping_ready);
    }
    else if ((modePrimary != UDMA_MODE_STOP) && (modeAlternate == UDMA_MODE_STOP))
    {
        // Need to reload alternate control structure
        uDMAChannelTransferSet(UDMA_CH24_ADC1_0 | UDMA_ALT_SELECT,
                               UDMA_MODE_PINGPONG,
                               (void *) (ADC1_BASE + ADC_O_SSFIFO0),
                               g_uint16_adc1_pong,
                               DMA_BUFFER_SIZE);
        Semaphore_post(s_adc1_pong_ready);
    }
}
