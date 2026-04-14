Ransanx Controller Document 
===========================

How to set the tachometer: 
--------------------------
1. Select "1" for mode 
2. Set the frequency division value to the motor "pole pair number" 
3. If the pole pair number is 2, set the frequency division value to 6 (2*3=6) 

Special instructions for external signal input:
-----------------------------------------------
1. When external PWM signal input is connected from the PG2 ports of the mainboard 
    (the built-in potentiometer on the board is adjusted to the minimum, and the short 
    interface on the board is short-circuited for wiring as shown in the figure above), 
    the amplitude is 2.5-5V, and the frequency is 50HZ-20KHZ 
2. When external analog 0-5V input is connected, input from the 2 wiring terminals of 
    the mainboard (the built-in potentiometer on the board is adjusted to the minimum, 
    and the wiring is shown in the dotted line in the figure above) 
3. When the external potentiometer is used for speed adjustment, connect the 
    corresponding 3 wiring terminals of the mainboard (the potentiometer on the board 
    is adjusted to the minimum, and the wiring is shown in the potentiometer diagram in 
    the figure above) 
4. Forward, reverse and stop are connected to ground (0 potential) 
    through a switch, and low level is effective, and the external MCU can also give a 
    low level signal. The brake is connected through a switch connected to 5V high level 
    valid. The front row of wiring terminals are all weak current signals directly input 
    to the MCU main control. They must not touch or come into contact with high voltage 
    and strong current. 

Connections
-----------
1. MAMBMC phase line output connects to the motor; 
2. 5VGND The motherboard comes with a 5V power supply (the external power supply current 
    does not exceed 50MA) 
3. VCCGND main power supply (external DC power supply) 
4. SC speed pulse signal output (only the interface is reserved, no technical support) 
5. DIR direction control forward and reverse control interface (low level is valid and 
    can be connected to an external switch on the ground) 
6. STOP stop control interface (low level is valid and can be connected to an external 
    switch on the ground) 
7. BRAKE brake control brake control interface (high level is valid and can be connected 
    to an external 5V switch) 
8. Speed ​​control speed control signal input (the integrated potentiometer on 
    the board can also be connected to an external 0-5V analog PWM dual signal input for 
    speed control) 
9. HaHbHc+5VGND Hall signal power supply input interface. Generally, motors with Hall 
    have corresponding 5 wires ; 


Note: 
-----
1. Since the forward and reverse rotation is a hard commutation without delay and the brake 
    will have a great impact, these two functions cannot be operated when running at high 
    voltage and full speed. Forced operation may damage the power tube and chip. The speed 
    control throttle must be reduced to below 50%. 
2. The maximum voltage (60V) and maximum power (350W) marked on this module are limit 
    parameters under specific conditions. In actual application, a margin should be left for 
    use

Details
-------
If you are not clear about the definition of the motor phase line and the Hall line, and it 
cannot work properly after being connected, you can blindly connect according to the 
following method and then slowly change the Hall line sequence. First, connect the three 
phase lines of the motor to the phase line interface of the driver board at will. There are 
five Hall lines on the motor. Three Hall signal lines are connected to the three Hall signal 
ports on the driver board at will. The other two Hall power supply lines must be connected 
correctly (the motherboard will provide 5V power supply. One of the Hall lead wires we send 
is red 5V and the other is black ground wire. You must find a way to connect it to the motor 
Hall power supply line. 

Remember!!!) After checking that everything is connected correctly, power on for low voltage 
(7-12V) and low current (1-3A) power-on debugging (if conditions permit, use constant DC power 
supply or low power power supply plus current limiting resistor, etc.), turn on the 
potentiometer on the board to adjust the speed throttle to about 20%. If the motor does not 
rotate or there are other abnormal phenomena, immediately turn off the power, and then change 
the order of the three phase lines of the motor (because the phase line interface on our driver 
board is a terminal, it is more convenient to connect and remove the wires, so I use the motor 
phase line to change the line sequence. If the motor phase line is fixed, the principle of 
changing the three Hall signal line sequence is the same. It depends on personal preference). 
Until it can rotate smoothly after power is turned on, it means that the wiring is correct. 

There are 6 different combinations of line sequence change. I will list them below: 
If the order of the currently connected phase lines is defined as ABC, it can be changed to 
ACB BAC BCA CAB CBA. Each time these combinations are changed, the machine is tested once. 
Only 1 of these 6 combinations is correct.

When the wiring is incorrect and the operation is abnormal, do not test the machine with high 
current and high voltage, otherwise there is a risk of damaging the driver board. After the 
three motor phase lines are swapped, if the line sequence is correct, the motor will run as 
smooth as silk after power on, start smoothly at low speed, and have high torque; if the three 
motor phase lines are not connected in the right order, generally there are 

1. After power on, the motor cannot start normally and has no response, or it only shakes when 
    starting and cannot rotate normally. 
2. There is a slight shake, it is difficult to start, it cannot turn, and it makes a slight 
    sound. Sometimes it needs to be manually turned to turn. 
3. The motor can turn in one direction, but not in the other direction, and there is some slight 
    sound. 
4. The motor starts with a large shake and is weak, there is a sound, and the current is 
    obviously too large. The power tube is obviously heated and serious
    
