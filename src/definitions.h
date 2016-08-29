#ifndef DEFINE_H
#define DEFINE_H

#define DEVICE_ID 65
#define PROTOCOL_NUM 0x0100
#define BUILD_DATE 0x20150202

//reg 0:  Contains the device ID
#define deviceId *((uint16_t*)&commRegu8[0])
//reg 1:  Contains the supported protocol revision number
#define protocolRev *((uint16_t*)&commRegu8[2])
//reg 2:  32-bit build date: 0xYYYYMMDD.
#define buildDate *((uint32_t*)&commRegu8[4])
//reg 4:  Write to trigger a HB action
#define hb0 *((uint16_t*)&commRegu8[8])
//reg 5:  Read to verify HB action (hb0 is copied here on write).
#define hb1 *((uint16_t*)&commRegu8[10])
//reg 6:  Restart status: 0 = fresh
#define restartState *((uint16_t*)&commRegu8[12])
//reg 16:  Firing toggle ACT1
#define ACT1F_T *((uint8_t*)&commRegu8[32])
//reg 17:  Firing toggle ACT2
#define ACT2F_T *((uint8_t*)&commRegu8[34])
//reg 18:  Firing toggle ACT3
#define ACT3F_T *((uint8_t*)&commRegu8[36])
//reg 19:  Firing toggle ACT4
#define ACT4F_T *((uint8_t*)&commRegu8[38])
//reg 20:  Firing toggle ACT5
#define ACT5F_T *((uint8_t*)&commRegu8[40])
//reg 21:  Firing toggle ACT6
#define ACT6F_T *((uint8_t*)&commRegu8[42])
//reg 22:  Firing toggle ACT7
#define ACT7F_T *((uint8_t*)&commRegu8[44])
//reg 23:  Firing toggle ACT8
#define ACT8F_T *((uint8_t*)&commRegu8[46])
//reg 24:  Firing toggle ACT9
#define ACT9F_T *((uint8_t*)&commRegu8[48])
//reg 25:  Firing toggle ACT10
#define ACT10F_T *((uint8_t*)&commRegu8[50])
//reg 26:  Firign toggle ACT11
#define ACT11F_T *((uint8_t*)&commRegu8[52])
//reg 27:  Firign toggle ACT12
#define ACT12F_T *((uint8_t*)&commRegu8[54])
//reg 33:  PWM value for motor 1
#define HB1_PWM_Val *((int16_t*)&commRegu8[66])
//reg 34:  PWM value for motor 2
#define HB2_PWM_Val *((int16_t*)&commRegu8[68])
//reg 35:  current PWM value for motor 1
#define HB1_Current_PWM *((int16_t*)&commRegu8[70])
//reg 36:  current PWM value for motor 2
#define HB2_Current_PWM *((int16_t*)&commRegu8[72])
//reg 37:  1 if board is hard killed. 0 if board is not running
#define IS_KILLED *((uint8_t*)&commRegu8[74])
//reg 38:  Mission Status
#define MISSION_STATUS *((uint8_t*)&commRegu8[76])
//reg 39:  Mission LED control (Active-High)
#define MISSION_LED_CMD *((uint8_t*)&commRegu8[78])
//reg 40:  1 to enable board. 0 to kill
#define SOFTKILL *((uint8_t*)&commRegu8[80])
//reg 41:  1 to enable actuators. 0 to kill
#define KILL_ACT *((uint8_t*)&commRegu8[82])
//reg 42:  1 to enable motor 1. 0 to kill
#define KILL_HB1 *((uint8_t*)&commRegu8[84])
//reg 43:  1 to enable motor 2. 0 to kill
#define KILL_HB2 *((uint8_t*)&commRegu8[86])
//reg 44:  Actuator fuse status. 0 if short. 1 if open
#define ACT_FUSE_ST *((uint8_t*)&commRegu8[88])
//reg 45:  H-bridge 1 fuse status. 0 if short. 1 if open
#define HB1_FUSE_ST *((uint8_t*)&commRegu8[90])
//reg 46:  H-bridge 2 fuse status. 0 if short. 1 if open
#define HB2_FUSE_ST *((uint8_t*)&commRegu8[92])

#define CUSTOM_REG_NUM 31
#define NUM_READ_ONLY 7
#define READ_ONLY 35,36,37,38,44,45,46

#define VAR_STARTS 21,21,0,0,85,85,85,0,84,85,85,21

#define UART0_DEFAULT_BAUD_RATE 57600
#define UART1_DEFAULT_BAUD_RATE 57600
#define USART_PRIMARY 0

#define UART0_TX_BUFFER_SIZE SIZE128
#define UART0_RX_BUFFER_SIZE SIZE32
#define UART1_TX_BUFFER_SIZE SIZE0
#define UART1_RX_BUFFER_SIZE SIZE0

#define SERIAL_TIMEOUT WDTO_15MS
#endif