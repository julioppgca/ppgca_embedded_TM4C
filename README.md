# Using TM4C MCUs with TI-RTOS
This repository contains the source files used in Embedded System 2 course. 
As soon as new examples become available, they will appear in the root directory with a brief description.

## File structure:
 ### Docs: 
  Contains useful reference files and further readings.
 
 ### TM4C_RTOS_Template 
  A model with pre-configured Ethernet server module and a data acquisition system with uDMA peripheral.

 ### TM4C_RTOS_rms
  Using Semaphores to manipulate inter threaded variables. 
  Calculating rms value from a sampled signal at 'ain0' channel.
  
  ### TM4C_RTOS_CMSIS_DSP
   Using arm CMSIS DSP library with TM4C and TI-RTOS.
   Calculating rms value and fft transform.
   
  ### TM4C_RTOS_TCP_IP_SendReceive
   Receive 'R' command from TCP/IP Socket and respond with AIN0 rms value.
  
## Sensor Data
 Please get your sensor calibration values from the link below:
 > https://tinyurl.com/SensorCalibrationTable

 
