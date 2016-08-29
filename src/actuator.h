#ifndef _actuator_h_
#define _actuator_h_

#include <avr/io.h>

// Miscellaneous definitions
#define DISABLE PA7
#define DISABLE_PORT A
#define HB1_FUSE PC3
#define HB1_FUSE_PORT C
#define HB2_FUSE PC4
#define HB2_FUSE_PORT C
#define ACT_FUSE PC5
#define ACT_FUSE_PORT C
#define MISSION PC6
#define MISSION_PORT C
#define MISSION_LED PC7
#define MISSION_LED_PORT C
#define HBT PD2
#define HBT_PORT D
#define MOSI PB5
#define MISO PB6
#define SCK PB7
#define RX PD0
#define TX PD1

// Actuator definitions
#define ACT1 PB4
#define ACT1_PORT B
#define ACT2 PB3
#define ACT2_PORT B
#define ACT3 PB2
#define ACT3_PORT B
#define ACT4 PB1
#define ACT4_PORT B
#define ACT5 PB0
#define ACT5_PORT B
#define ACT6 PA0
#define ACT6_PORT A
#define ACT7 PA1
#define ACT7_PORT A
#define ACT8 PA2
#define ACT8_PORT A
#define ACT9 PA3
#define ACT9_PORT A
#define ACT10 PA4
#define ACT10_PORT A
#define ACT11 PA5
#define ACT11_PORT A
#define ACT12 PA6
#define ACT12_PORT A

// DC motors 1
#define HB1_MC1 PD7
#define HB1_MC1_PORT D
#define HB1_MC2 PC0
#define HB1_MC2_PORT C
#define HB1_MC3 PC1
#define HB1_MC3_PORT C
#define HB1_MC4 PC2
#define HB1_MC4_PORT C

// DC motor 2
#define HB2_MC1 PD3
#define HB2_MC1_PORT D
#define HB2_MC2 PD4
#define HB2_MC2_PORT D
#define HB2_MC3 PD5
#define HB2_MC3_PORT D
#define HB2_MC4 PD6
#define HB2_MC4_PORT D

// Helper macros
#define _TOKENPASTE(x,y) x##y
#define TOKENPASTE(x,y) _TOKENPASTE(x,y)

// Register macros
#define _PORT(port) _TOKENPASTE(PORT,port)
#define PORT(pin) _PORT(pin##_PORT) 
#define _PIN(port) _TOKENPASTE(PIN, port)
#define PIN(pin) _PIN(pin##_PORT)
#define _DDR(port) _TOKENPASTE(DDR,port)
#define DDR(pin) _DDR(pin##_PORT)

// Pin macros
#define _RSET(port, pin) {port |= (1 << (pin));}
#define RSET(pin) _RSET(_PORT(pin##_PORT),pin)
#define _RCLR(port, pin) {port &= ~(1 << (pin));}
#define RCLR(pin) _RCLR(_PORT(pin##_PORT),pin)
// #define TOGGLE(pin) {_PORT(pin##_PORT) ^= (1 << (pin));} // Takes 3 cycles
#define TOGGLE(pin) {_PIN(pin##_PORT) |= (1 << (pin));} // Takes 2 cycles
#define SETIF(pin,expr) { if(expr) _RSET(_PORT(pin##_PORT),pin) else _RCLR(_PORT(pin##_PORT),pin)}
#define RGET(pin) (_PIN(pin##_PORT) & (1 << (pin)) ? 1 : 0)

#endif // Closes _actuator_h_
