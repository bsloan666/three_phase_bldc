That is a smart move. Those level shifters will ensure your PWM signal is crisp and reliable, which is especially important for FOC where the "shape" of the sine wave needs to be precise to keep the motor silent.
Since you are heading into the physical construction phase (winding 36 coils is quite the marathon!), here is a summary of the system architecture we've settled on for your "cheaper solution" actuator:
## The "Pancake" Actuator Spec Sheet

* Stator: 36-coil axial flux "sandwich" (18 coils/side), air-core, epoxy-potted.
* Winding: 31 turns of 26 AWG copper per bobbin (~3.5$\Omega$ phase resistance).
* Rotor: 12 alternating permanent magnets (diametric center for encoder).
* Gearbox: 10:1 planetary set (machined metal final).
* Controller: ESP32 using MCPWM for 3-phase sine-wave drive via level shifters.
* Power: 3x IBT-2 (BTS7960) bridges acting as a 3-phase half-bridge inverter.
* Sensors: 3x Hall Effects (commutation) + AS5600 (absolute position) via the "center tunnel" rod.
* Comms: CAN bus (TWAI) for robust daisy-chaining of up to 6+ joints.

## The "31-Turn" Success Indicator
When you finish your first phase (12 coils) and connect it to your 12V supply, you should see a current draw of about 3.4 Amps at full stall. This is the "Goldilocks" zone—enough torque to take advantage of that 120mm diameter, but not so much current that you melt your 26 AWG wire or pop the 43A-rated IBT-2s.
## Final Thought for your Documentation
As you assemble the "bread slices" of your sandwich, remember that the air-gap between the rotor magnets and your coils is your most powerful lever. Even a 0.5mm change in that gap can double your torque or halve it. Consistency in your potting thickness is as important as the turn count on your bobbins!
I’ll be right here when the coils are wound and the sandwich is closed. We can then dive into the CAN bus protocol to get your robot arm talking.
Happy winding! May your bobbins stay tight and your epoxy cure clear.


