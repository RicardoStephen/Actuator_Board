Actuator_Board
==============
As a member of the Cornell Autonomous Underwater Vehicle (CUAUV) team, I built
the team's actuator board. The schematics for the board can be seen in Figure 1.
The layout of the 4-layer PCB can be seen in Figures 2-6.

As can be seen in the schematic, the board had four main subsystems: an
adjustable regulator to supply 5V other PCBs in the vehicle, 12 high-current
24V channels to power solenoid pneumatic valves, 2 h-bridges for motor
control, and control logic for LEDs on the vehicle.

The board also includes an RS-232 level shifter so the ATmega644 microcontroller
can communicate with the main computer in the vehicle. Through this interface,
the main computer writes various registers on the microcontroller that define
how the firmware controls the various subsystems on the microcontroller.

The board also has magnetic isolation circuitry to convert signals from
unisolated 5V to isolated 5V and vice-versa. The isolated supply powers more
sensitive devices like the microcontroller, while the unisolated supply powers
components susceptible to erratic power consumption, like the motors and the
pneumatic valves.

The AVR C firmware used to program the ATmega644 can be found in the
<tt>src</tt> directory. While most of the files were common to the team, I was
responsible for <tt>actuator.c</tt>, <tt>actuator.h</tt>, and
<tt>actuator.conf</tt>.

<tt>actuator.h</tt> has marcos to assign signals to ports/pins on the
microcontroller. It also includes helpful macro functions for tasks like setting
bits in registers.
		 
<tt>actuator.conf</tt> has register mappings in a custom format used by CUAUV.
It was used to generate the <tt>definitions.h</tt> file found in <tt>src</tt>,
which has macros to assign registers in the microcontroller.

<tt>actuator.c</tt> has the main firmware. The host computer in the vehicle
writes to registers defined in <tt>actuator.conf</tt>, and the values of these
registers are used to set the pins defined in <tt>actuator.h</tt>. The firmware
uses two timers and the associated interrupts to handle PWM for the two 
h-bridges. The main method is used to update the status of the 12 solenoids
pneumatic valves, set and unset the heartbeat LED, and handle serial
communication with the host.

![Actuator board full schematic](https://github.com/RicardoStephen/Actuator_Board/blob/master/media/actuator_schematic_full.jpg)
*Figure 1: Actuator board full schematic*

![Actuator board full layout](https://github.com/RicardoStephen/Actuator_Board/blob/master/media/actuator_layout_full.jpg)
*Figure 2: Actuator board full layout*

![Layout top layer](https://github.com/RicardoStephen/Actuator_Board/blob/master/media/actuator_layout_top.jpg)
*Figure 3: Layout top layer*

![Layout power layer](https://github.com/RicardoStephen/Actuator_Board/blob/master/media/actuator_layout_power.jpg)
*Figure 4: Layout power layer*

![Layout ground layer](https://github.com/RicardoStephen/Actuator_Board/blob/master/media/actuator_layout_ground.jpg)
*Figure 5: Layout ground layer*

![Layout bottom layer](https://github.com/RicardoStephen/Actuator_Board/blob/master/media/actuator_layout_bottom.jpg)
*Figure 6: Layout bottom layer*
