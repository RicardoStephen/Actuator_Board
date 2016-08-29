#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "actuator.h"
#include "common.h"

#define HBT_PERIOD 250
#define SERIAL_PERIOD 10

#define motor1_period 1600  // period is 0.0125 ms times value
#define motor2_period 1600  // period is 0.0125 ms times value

volatile uint8_t hbt_count = HBT_PERIOD;
volatile uint8_t serial_count = SERIAL_PERIOD;

volatile int16_t HB1_PWM_Count = 0;
volatile int16_t HB2_PWM_Count = 0;

// Clears actuator channels
void softKillAct(void) {
  PORTA &= (1 << DISABLE);
  PORTB &= (1 << MOSI)|(1 << MISO)|(1 << SCK);
}

// Kills motor 1 by shorting it
void softKillHB1(void) {
  RCLR(HB1_MC1);
  RCLR(HB1_MC3);
  RCLR(HB1_MC2);
  RCLR(HB1_MC4);
}

// Kills motor 2 by shorting it
void softKillHB2(void) {
  RCLR(HB2_MC1);
  RCLR(HB2_MC3);
  RCLR(HB2_MC2);
  RCLR(HB2_MC4);
}

// Kills actuators and motors
void softKill(void) {
  softKillAct();
  softKillHB1();
  softKillHB2();
}

// Enable verification necessary for actuators and motors
uint8_t isEnabledGeneral(void) {
  return (~RGET(DISABLE) & ~SOFTKILL & serialActive());
}

// Checks ONLY kill status and fuse. See isEnabledGeneral()
uint8_t isEnabledAct(void) {
  return (~KILL_ACT & ~RGET(ACT_FUSE));
}

// Checks ONLY kill status and fuse. See isEnabledGeneral()
uint8_t isEnabledHB1(void) {
  return ~KILL_HB1 & ~RGET(HB1_FUSE);
}

// Checks ONLY kill status and fuse. See isEnabledGeneral()
uint8_t isEnabledHB2(void) {
  return ~KILL_HB2 & ~RGET(HB2_FUSE);
}

void pwmMotor1(void) {
  if(HB1_PWM_Count == motor1_period) {
    HB1_PWM_Count = 0;
    if(HB1_PWM_Val > HB1_Current_PWM+2)
      HB1_Current_PWM += 3;
    else if (HB1_PWM_Val < HB1_Current_PWM-2)
      HB1_Current_PWM -= 3;
    else
      HB1_Current_PWM = HB1_PWM_Val;
  }
  if(HB1_Current_PWM > 0) {
    if(((int16_t) HB1_PWM_Count) < HB1_Current_PWM) {
      RCLR(HB1_MC3);
      RCLR(HB1_MC2);
      RSET(HB1_MC4);
      RSET(HB1_MC1);
    } else {
      RCLR(HB1_MC1);
    }
  } else if (HB1_Current_PWM < 0) {
    if(((int16_t) HB1_PWM_Count) < -HB1_Current_PWM) {
      RCLR(HB1_MC1);
      RCLR(HB1_MC4);
      RSET(HB1_MC2);
      RSET(HB1_MC3);
    } else {
      RCLR(HB1_MC3);
    }
  } else {
    RCLR(HB1_MC1);
    RCLR(HB1_MC3);
    RSET(HB1_MC2);
    RSET(HB1_MC4);
  }
}

void pwmMotor2(void) {
  if(HB2_PWM_Count == motor2_period) {
    HB2_PWM_Count = 0;
    if(HB2_PWM_Val > HB2_Current_PWM+2)
      HB2_Current_PWM += 3;
    else if (HB2_PWM_Val < HB2_Current_PWM-2)
      HB2_Current_PWM -= 3;
    else
      HB2_Current_PWM = HB2_PWM_Val;
  }
  if(HB2_Current_PWM > 0) {
    if(((int16_t) HB2_PWM_Count) < HB2_Current_PWM) {
      RSET(HB2_MC3);
      RCLR(HB2_MC2);
      RSET(HB2_MC4);
      RCLR(HB2_MC1);
    } else {
      RSET(HB2_MC1);
    }
  } else if (HB2_Current_PWM < 0) {
    if(((int16_t) HB2_PWM_Count) < -HB2_Current_PWM) {
      RSET(HB2_MC1);
      RCLR(HB2_MC4);
      RSET(HB2_MC2);
      RCLR(HB2_MC3);
    } else {
      RSET(HB2_MC3);
    }
  } else {
    RSET(HB2_MC1);
    RSET(HB2_MC3);
    RSET(HB2_MC2);
    RSET(HB2_MC4);
  }
}


void updateOutputs(void) {
  if (isEnabledGeneral()) {
    if (isEnabledAct()) {
      SETIF(ACT1, ACT1F_T);
      SETIF(ACT2, ACT2F_T);
      SETIF(ACT3, ACT3F_T);
      SETIF(ACT4, ACT4F_T);
      SETIF(ACT5, ACT5F_T);
      SETIF(ACT6, ACT6F_T);
      SETIF(ACT7, ACT7F_T);
      SETIF(ACT8, ACT8F_T);
      SETIF(ACT9, ACT9F_T);
      SETIF(ACT10, ACT10F_T);
      SETIF(ACT11, ACT11F_T);
      SETIF(ACT12, ACT12F_T);
    } else {
      softKillAct();
    }
    if (isEnabledHB1()) {
      pwmMotor1();
    } else {
      softKillHB1();
    }
    if (isEnabledHB2()) {
      // pwmMotor2();
      pwmServo2();
    } else {
      softKillHB2();
    }
  } else {
    softKill();
  }
}

void updateStatus(void) {
  MISSION_STATUS = (RGET(MISSION)) ? 1 : 0;

  SETIF(MISSION_LED, MISSION_LED_CMD);
  IS_KILLED = RGET(DISABLE);
  HB1_FUSE_ST = RGET(HB1_FUSE);
  HB2_FUSE_ST = RGET(HB2_FUSE);
  ACT_FUSE_ST = RGET(ACT_FUSE);

}

void initRegisters(void) {
  ACT1F_T = 0;
  ACT2F_T = 0;
  ACT3F_T = 0;
  ACT4F_T = 0;
  ACT5F_T = 0;
  ACT6F_T = 0;
  ACT7F_T = 0;
  ACT8F_T = 0;
  ACT9F_T = 0;
  ACT10F_T = 0;
  ACT11F_T = 0;
  ACT12F_T = 0;

  HB1_PWM_Val = 0;
  HB2_PWM_Val = 0;

  HB2_Current_PWM = 0;
  HB2_Current_PWM = 0;

  // Killed on startup
  SOFTKILL = 1;
  KILL_ACT = 1;
  KILL_HB1 = 1;
  KILL_HB2 = 1;

}

void setup(void) {
  // PA initialization
  uint8_t disable_temp = (1 << DISABLE);
  DDR(DISABLE) = ~disable_temp;

  // PB initialization
  DDR(ACT5) |= ~((1 << MOSI)|(1 << MISO)|(1<<SCK));
  softKill();

  // PC initialization
  DDR(MISSION_LED) = (1 << MISSION_LED)|(1 << HB1_MC2)|(1 << HB1_MC3)|(1 << HB1_MC4);
  uint8_t portc_temp = (1 << MISSION_LED)|(1 << HB1_MC2)|(1 << HB1_MC3)|(1 << HB1_MC4);
  PORT(MISSION_LED) = ~portc_temp;

  // PD initialization
  DDR(HBT) = (1 << HBT)|(1 << HB1_MC1)|(1 << HB2_MC1)|(1 << HB2_MC2)|(1 << HB2_MC3)|(1 << HB2_MC4);
  PORTD &= ~((1 << HBT)|(1 << HB1_MC1)|(1 << HB2_MC1)|(1 << HB2_MC2)|(1 << HB2_MC3)|(1 << HB2_MC4));

  // Setup 1 ms interrupt timer
  TCCR0A = (1<<WGM01); // CTC mode
  TCCR0B = (1<<CS02); // 256x prescaler
  OCR0A = 78; // Timer0 TOP value for interrupt
  TIMSK0 = (1<<OCIE0A); // Interrupt on overflow. 20MHz/256/78 = 1.002kHz

  // Setup for PWM timer
  TCCR2A = (1<<WGM21);// CTC mode
  //TCCR2B = (1<<CS21)|(1<<CS20); // 32x prescalar
  //OCR2A = 2; // Timer2 TOP value for interrupt
  TCCR2B = (1<<CS20); // 1x prescalar
  OCR2A = 250; // Timer0 TOP value for interrupt
  TIMSK2 = (1<<OCIE2A); // Interrupt on overflow. 20MHz/250 = 80kHz 
  
  // Initialize host computer I/O
  uart0Init();
  packetHandlerInit();

  // Enable Interrupt
  sei();
}

int main(void) {
  setup();
  initRegisters();
  serialSetTimeout(1500);
 
  while(1) {
    updateStatus();
    if(hbt_count == 0) {
      TOGGLE(HBT);
      hbt_count = HBT_PERIOD;
    }
    if(serial_count == 0) {
      serial_count = SERIAL_PERIOD;
      handleSerialRx();
      hb1 = hb0; // Send hbt to host computer
    }
    updateOutputs();
  }

}

ISR(TIMER0_COMPA_vect) {
  if(hbt_count > 0) hbt_count--;
  if(serial_count > 0) serial_count--;
}

ISR(TIMER2_COMPA_vect){
  if(HB1_PWM_Count < motor1_period)
    HB1_PWM_Count++;
  if(HB2_PWM_Count < motor2_period)
    HB2_PWM_Count++;
}
