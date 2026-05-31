The "hidden error" you're sensing is actually just the difference between Full-Bridge control (where you control two ends of a single coil) and 3-Phase Half-Bridge control (where three phases share a common return point).
You can absolutely run your 3-phase motor with only 3 PWM pins for speed and 3 digital pins for "High-Z" control. Here is how the IBT-2 pins map to that logic:
1. The Pin Assignment
For each of your three IBT-2 boards, wire them like this:
PWM Control: Connect RPWM to your PWM-capable pin (e.g., Pins 9, 10, 11).
Bridge Logic: Tie LPWM to Ground. (Crucial: Never leave this floating).
The "Hand" Rule: Connect both R_EN and L_EN together to a single digital pin.
Logic HIGH: The bridge is active. The PWM on RPWM now "sloshes" the phase between 12V and Ground.
Logic LOW: Both MOSFETs in that bridge turn off. The phase is now "Floating" (High-Z).
2. Why this works with 3 PWM signals
By tying LPWM to Ground, you’ve turned the IBT-2 into a synchronous half-bridge:
When RPWM is HIGH, the High-Side MOSFET turns on (Phase = 12V).
When RPWM is LOW, the Low-Side MOSFET turns on (Phase = GND). 
Dr. Rainer Hessmer
Dr. Rainer Hessmer
 +1
In 3-phase drive, current only flows when one phase is at 12V and another is at GND. You don't need to "reverse" the individual board; you just swap which board is pulsing HIGH and which is pulsing LOW.
3. The 6-Step "Minimal" Arduino Code
Here are the 6 values to send to your 3 RPWM pins for your Hall states. Note that in 6-step, the "High-Z" state is handled by the Enable pins.
Hall State (ABC)	Bridge A (RPWM / EN)	Bridge B (RPWM / EN)	Bridge C (RPWM / EN)
101 (5)	            255 / HIGH	            0 / HIGH X /        LOW
001 (1)	            255 / HIGH	X / LOW	0 / HIGH
011 (3)	            X / LOW	255 / HIGH	0 / HIGH
010 (2)	            0 / HIGH	255 / HIGH	X / LOW
110 (6)	            0 / HIGH	X / LOW	255 / HIGH
100 (4)	X / LOW	0 / HIGH	255 / HIGH
(X = "Don't care" because the EN pin is LOW, putting that bridge in High-Z). 
GitHub
GitHub
 +2
Updated next_coil_state function:
cpp
void next_coil_state(int ss) {
  // ss is your sensor state (1-6)
  // enA, enB, enC are digital pins tied to (R_EN + L_EN) of each board
  // pwmA, pwmB, pwmC are PWM pins tied to RPWM of each board

  // Step 1: Default to High-Z (All OFF)
  digitalWrite(enA, LOW); digitalWrite(enB, LOW); digitalWrite(enC, LOW);

  // Step 2: Activate specific paths based on the Hall State
  if (ss == 5) { // A High, B Low, C Off
    digitalWrite(enA, HIGH); analogWrite(pwmA, 255); 
    digitalWrite(enB, HIGH); analogWrite(pwmB, 0); 
  } else if (ss == 1) { // A High, C Low, B Off
    digitalWrite(enA, HIGH); analogWrite(pwmA, 255);
    digitalWrite(enC, HIGH); analogWrite(pwmC, 0);
  }
  // ... continue for all 6 states using the table above
}
Use code with caution.
Final Logic Check
Trapezoidal (6-step): You use the EN pins to "float" the third wire. 
YouTube
YouTube
FOC (Sine Wave): You leave all EN pins HIGH all the time, and simply vary the PWM on all 3 pins. The "High-Z" state isn't used in pure FOC because the current is always balanced across all three wires.
Is the "High-Z" vs. "Polarity" distinction clearer now? When the 26 gauge wire arrives, we can start with this 6-step code to verify your bobbins are firing in the right order!









6 sites
IBT-2 H-Bridge with Arduino - Dr. Rainer Hessmer
Dec 28, 2013 — High OR Low side PWM. This very versatile module can drive to low for low side sinking, then applying independant PWM to each Inhi...


Dr. Rainer Hessmer
Driving DC Motors with Microcontrollers - DroneBot Workshop
Oct 16, 2022 — Applying a PWM signal to IN1 while holding IN2 LOW will spin the motor in one direction. Holding IN1 LOW and applying PWM to IN2 w...


DroneBot Workshop
Intro to Arduino - DC Motor Control using H-Bridge (Fall 2024)
Oct 30, 2024 — An H-bridge is a circuit used to control DC motors, allowing for electronic control of the motor's direction without physically di...

30:29


YouTube
·
Digital Logic & Programming
Show all
Thank you. This makes a bit more sense now. It is late, but I think tomorrow I can re-do the wiring and the code to use the tetrahedral signal on my low-impedance, low-clockspeed Arduino prototype. I may ask for your help with the sine wave implementation once the tetrahedral is working on my original sub-optimal hardware design.
That sounds like a perfect plan. Getting the Trapezoidal (6-step) logic working on your current hardware is the best "Sanity Check." It proves that the Hall sensors are aligned, the bobbins are wired in the correct 3-phase sequence, and the H-bridges are responding.
Once you have that "rhythmic" 6-step spin working, moving to the Sine Wave implementation will feel like a massive upgrade—it’s the difference between a motor that "clicks" through its states and one that "humms" smoothly.
One Last "Late-Night" Tip for Tomorrow:
When you re-wire for the 3-pin / High-Z logic, keep your power supply current-limited if possible. Since your current prototype is "low-impedance," the 6-step logic can pull a lot of current if two phases stay active for too long while the motor is stalled.
I’ll be standing by! Ping me once the Arduino is spinning the old hardware with the new 3-phase logic, and we’ll start preparing the ESP32 Sine/FOC transition for your 26-gauge "31-turn" rebuild.
Goodnight, and good luck with the re-wiring!





Had a thought about the proposed "ring shaped" 12v board power supply: does the power for each H-bridge need to be isolated? If not, does that mean that the copper "power-ring" can be two concentric rings of copper with 30 33uF capacitors spanning the inner and outer rings around the perimeter?
No, the power for each H-bridge does not need to be isolated. In a 3-phase system, all three bridges share the same voltage potential (B+) and the same return path (B-).
In fact, tying them all to a single, high-capacity power rail is actually preferred because it helps balance the electrical noise across the whole system.
The "Power Ring" Design
Your idea of two concentric copper rings is a textbook-perfect way to handle high-current distribution for an axial actuator. It’s essentially a "low-impedance bus bar."
Here is how that "Ring of Power" looks in practice:
The Outer Ring: This is your Ground (B-). It is safer to put the Ground on the outer edge so that if a tool slips or a housing piece touches the edge, it hits 0V rather than a "hot" 18V line.
The Inner Ring: This is your Voltage (B+).
The Bridge: Your 30 capacitors (three groups of 10) span the gap between the two rings. They act like 30 tiny springs absorbing the "shocks" of the MOSFETs switching.
Why this is a "Pro" Layout:
Low Inductance: By keeping the (+) and (-) rails very close to each other (parallel), you cancel out a lot of the electromagnetic noise.
Thermal Mass: Those large copper rings act as additional heat sinks for the wires.
Uniform Power: Every H-bridge gets the exact same "flavor" of electricity. No bridge is "starved" because it’s further away from the battery than the others.
A Small Mechanical Tip for the Rings:
If you are machining these rings from copper sheet:
Solderability: Copper oxidizes quickly. If you can tin the rings (coat them in a thin layer of solder) after machining, it will make attaching those 30 tiny capacitors much easier.
Isolation: Make sure the phenolic substrate or PCB between the two rings is clean. At 18V, even a stray copper shaving from your mill could cause a spectacular "pop."
Summary of Connectivity:
All three B+ terminals of your IBT-2 boards solder to the Inner Ring.
All three B- terminals solder to the Outer Ring.
The Battery Positive enters at one point on the inner ring; Battery Negative at one point on the outer.
This is a very robust design. It sounds like your "Sandwich" is getting a very high-end electrical skeleton!
Ready for that 26-gauge wire to arrive tomorrow? (I'm ready when you are to help with the 6-step testing code!)





The wire is coming today! However, I'm probably going to build another breadboard prototype before building the final design. Here's one more thought I had this morning: it seems we are only ever enabling 1/2 of each H-bridge. Does this mean I can get away with using two IBT-2 boards instead of 3?
That is an incredibly sharp observation—but unfortunately, the answer is no. You still need three boards (or specifically, three half-bridge channels).
Here is why your intuition is partially right, but why the math requires that third board:
1. The "Two vs. Three" Problem
In a DC motor, current flows in one wire and out the other. You only need two "doors" (one full H-bridge).
In your 3-Phase Star motor, you have three wires (




) meeting at a common center point. To move the magnetic field in a full 360-degree circle, you must be able to control the "pressure" on all three wires independently.
If you only had two IBT-2 boards, you would have control over Wire 
 and Wire 
.
Wire 
 would have to be tied to a fixed point (like Ground).
If Wire 
 is tied to Ground, it can only ever sink current. It can never "push" current. This would "warp" your magnetic field, making it impossible to achieve smooth rotation or full torque. You would have massive "dead zones" where the motor would simply hum and vibrate.
2. Are we only using "half" the board?
Yes! Each IBT-2 is technically two half-bridges.
Board 1: Uses one side to drive Phase A. (The other side is idle).
Board 2: Uses one side to drive Phase B. (The other side is idle).
Board 3: Uses one side to drive Phase C. (The other side is idle).
The "Efficiency" Hack:
If you were very brave and comfortable with desoldering, you could technically drive all three phases with only two IBT-2 boards by using both sides of Board 1 (Phase A and B) and only one side of Board 2 (Phase C). However, the way the IBT-2 is laid out makes this a wiring nightmare and a thermal disaster, as Board 1 would be doing twice the work and getting twice as hot.
3. Why the "Three Board" Pinwheel is Best
By using three separate boards:
Thermal Balance: Each board handles exactly 1/3 of the heat.
Symmetry: Your "pinwheel" layout stays perfectly balanced.
Redundancy: If one board pops, you only replace one board, not a board that was handling two phases.
A Note for your "Breadboard Prototype"
Since your 26-gauge wire arrives today:
When you build this next prototype, stick to the 3-board, 3-wire-to-ESP32 layout. It is the only way to get the "Sloshing Sine Waves" to work.
Pro-Tip for the Winding Marathon:
As you start winding those 31 turns today: Keep track of the direction! All 36 coils must be wound in the same direction (e.g., all Clockwise). If you accidentally wind one counter-clockwise, its magnetic field will cancel out the others, and the motor will feel like it has a "limp."
Are you planning to wind all 18 coils for one side first, or will you do a 3-coil "Mini Star" test first?
Good luck with the copper delivery! The first few turns are the most exciting. Any questions on the Star connection before you start soldering the "Y" point? bolding on key terms





I now have a few prototypes under my belt and am getting good about orienting the coils properly. The "coil sandwich" architecture does add a level of difficulty. I've developed a system where I pass a voltage through the A, B or C coil set and hover a rare-earth magnet mounted on a stick over each coil on both sides of hte samedwich, remebering that side X should attrace and side Y should repel. This is all done before the star circuit is wired in its final configuration. One more question about my choice of board: is there a half bridge equivalent of the IBT-2? Something with about the same power but without the un-needed components and circuitry?
That "magnet on a stick" test is a brilliant piece of analog debugging. It’s the only way to be 100% sure that your magnetic polarity is correct before the epoxy sets. In an axial flux motor, one "reversed" coil is like a hole in your torque curve.
To answer your question: Yes, there is a "Half-Bridge" equivalent, but in the world of power electronics, we usually stop calling them "boards" and start calling them Power Modules or Gate Drivers + MOSFETs.
If you want to keep the "muscle" of the IBT-2 (which uses the BTS7960 chip) but lose the bulk, here are your three best paths:
1. The "Discrete" Path (Gate Driver + MOSFETs)
This is how professional actuators are built. Instead of a pre-made board, you use:
The MOSFETs: A pair of N-Channel MOSFETs (like the IRFZ44N or the even beefier IRF1404).
The Gate Driver: A chip like the IR2101 or IR2104.
Why it's better: You can lay these out in a very tight, circular pattern on your PCB. It eliminates all the bulky connectors and unnecessary traces of the IBT-2.
2. The "Integrated" Half-Bridge (The BTN8982 / IFX007T)
The IBT-2 uses two BTS7960 chips. The modern, more powerful, and single-channel (half-bridge) version of that is the BTN8982 or the IFX007T.
Form Factor: It is a single, 7-pin chip that looks like a large transistor.
Power: It can handle up to 55A—plenty for your 5A target.
The Design: You would put three of these chips on your hex-PCB. One chip = one phase.
Availability: You can find small "breakout boards" for the IFX007T that are much smaller than the IBT-2.
3. The "Smart" Driver (The DRV8301 / DRV8302)
Since you are using an ESP32, you could use a dedicated 3-Phase Gate Driver chip.
How it works: This is a single chip that controls six MOSFETs for you. It handles the "push-pull" logic and protection internally.
Benefit: It is the smallest possible footprint. It replaces all three IBT-2 boards with one chip and six small MOSFETs.
My Recommendation for your "Final Design"
Since you are already comfortable with the IBT-2's performance, look at the IFX007T (or the BTN8982).
It is essentially half an IBT-2 in a much smaller package.
It is very "friendly" to DIY soldering because it has large pins.
It will allow you to shrink your "Electronics Layer" significantly, making your "Pancake" much thinner.
A "Winding Day" Tip:
When you start with that 26 AWG today, remember that thinner wire is easier to "nick." Be very careful not to scrape the enamel off the wire as you pull it through your 3D-printed molds. A tiny scratch can lead to an inter-turn short, which will make that specific coil weak and hot.
Are you planning to mount the IFX007T-style chips directly to your 6mm aluminum "umbrella" plate for cooling?
Good luck with the first few bobbins! How many turns did you decide was the "Max Capacity" for your 6mm-thick dish?





I played with the geometry of the bobbin spindle. Making it a slender oblong wedge and doubleing its radial length from that of my prototype increases the amount of copper I can use per coil. While my prototype coils have 20 winds of 22 gauge wire, my next design, even with the same number of winds of the same gauge would have nearly twice the resistance. Sorry for all these additional questions: As for the BTN8982, they're a $2.00 chip with 7 leads -- but I believe there's some additional circuity that the IBT-2 has, namely a logic chip for, I assume, converting the PWM signal to voltages. Is that a necessary part of my design that we'll need to add?
Don’t apologize! This is the exact moment where the "black box" of a pre-made board (the IBT-2) opens up, and you start seeing the raw clockwork of power electronics. It’s a great place to be.
To answer your question about the BTN8982 (or its cousins):
1. What’s inside the "Magic Chip"?
The BTN8982 is what we call an Integrated Half-Bridge. Unlike a raw MOSFET, it actually has the "logic chip" built right into the silicon.
The "Driver" is inside: It already contains the circuitry to take a logic-level PWM signal and translate it into the high-voltage "gate drive" needed to flip the internal power MOSFETs.
Protection: It has built-in over-temperature and over-current protection.
2. Is there "Additional Circuitry" on the IBT-2?
Yes, but it is very minimal. If you look at your IBT-2 board, the "logic chip" you see (usually a 74AHC244) is just a Buffer/Isolator.
Its Job: It takes the weak signal from your Arduino/ESP32 and "strengthens" it so it can travel down a long wire to the BTS7960 chips without losing its shape.
Do you need it? If your ESP32 is sitting on the same PCB as the BTN8982s (which it will be in your "Hex-Ring" design), no, you don't strictly need that buffer chip. The ESP32 can drive the BTN8982 directly.
3. What Must you add to the BTN8982?
If you build a board with three BTN8982s, you only need three types of supporting components:
The Capacitors: Your bank of ten 

 capacitors per chip (as we discussed). This is non-negotiable for high-speed switching.
A few Resistors: Typically one or two small resistors per chip to set the "slew rate" (how fast the switch flips) and to pull the inputs to a known state.
A Logic Level Shifter: Since the BTN8982 is a 5V-logic device and the ESP32 is 3.3V, you’ll still want those up-converters we talked about to ensure the chip sees a "Solid High" signal.
4. A Note on your "Wedge" Coils
Your move to a slender oblong wedge is a professional-grade optimization. This is known as a trapezoidal coil, and it’s used in high-end axial flux motors (like those in electric supercars) because it fills the "pie-slice" space of the stator much more efficiently than a round coil.
The Resistance Win: Increasing the radial length is great because the "active" part of your wire (the part that actually cuts through the magnetic field to produce torque) is the part running along the radius. The "end turns" (the curves at the top and bottom) are essentially "dead weight" that only add resistance.
The Result: By making the wedge longer, you increase the Torque-to-Resistance ratio. You'll get more "push" for every watt of heat generated.
My Advice for the "Big Push" Today:
Since your 26 AWG is arriving, and you’re testing the wedge geometry:
Verify the fit: Wind one "wedge" bobbin with 31 turns and see how it fits in your "bread slice." If it’s too tight, you can drop a few turns; if it’s loose, you can add more to soak up that extra resistance you're looking for.
The IBT-2 is your "Safety Net": Use the IBT-2s for your breadboard prototype. They are hard to kill. Once you have the code and the motor running perfectly, then pivot to the BTN8982 for the "final, slimmed-down" version.
How many 31-turn "wedges" do you think you can fit around your 120mm stator? (If you have 18 per side, that's a lot of winding! Hope you have a good podcast or some music lined up.)
Good luck with the copper delivery! I'm excited to hear how the first "wedge" turns out.





Yay! So I guess I'm designing a PCB. I have used KiCad to design one PCB, a prototype of my optical greycode encoder (with all the LEDs and photoresistors, if you recall). So I'm most comfortable with it as a design tool. However, given the precise positions of the holes for the magnetic encoder board, as well as for the 6 axial "sandwich assembly" screws, this PCB will be a much higher level of difficulty than anything I've designed previously. Given that I plan to eventually scale the actuator design to accommodate a number of form factors, abandoning the IBT-2s will make a much more compact PCB possible -- one that can be used in smaller diameter "pancakes" and "barrels".
That is a massive "Level Up" for your project! Moving from cannibalized modules to a custom KiCad design is the moment you transition from "Maker" to "Product Designer."
Designing a 120mm hexagonal power PCB with precise mechanical alignments is challenging, but KiCad is actually perfect for this because of its Python scripting and DXF import capabilities.
Here is how you can conquer the "high level of difficulty" and set yourself up for a scalable design:
1. The "Mechanical-First" Workflow
Since your PCB must align with a machined hex-housing, 6 through-holes, and a central encoder, do not start in KiCad.
Step 1: In your CAD software (where you designed the motor housing), create a 2D sketch of the PCB outline, the 6 bolt holes, and the 4 mounting holes for the encoder.
Step 2: Export this as a .DXF file.
Step 3: In KiCad's PCB Editor, go to File > Import > Graphics and bring that DXF onto the Edge.Cuts layer.
Result: You now have a perfectly accurate "template." You just drop your components (the three BTN8982s, the ESP32, etc.) into the gaps.
2. Why the BTN8982 is a "Scaler's" Dream
By moving to the BTN8982 (or the IFX007T), you can create a "Cellular" design:
The Power Cell: A single BTN8982 + its 10 capacitors + its level shifter.
The Scaling: For a 120mm motor, you place 3 "cells" in a pinwheel. For a smaller 60mm "barrel" motor, you might stack them vertically or place them on a much smaller circular board. The circuit logic remains identical; only the physical arrangement changes.
3. Conquering the Heat and Current
Since this PCB will be carrying 5A+ and sits inside a "sandwich," your KiCad settings are critical:
Copper Weight: Specify 2oz copper when you order the boards. Standard 1oz is too thin for these power traces.
Thermal Vias: Under each BTN8982, place a "pad" of at least 25–50 tiny vias (0.3mm) that connect the top copper to the bottom copper. This acts as a "heat pipe" to move energy to your 6mm aluminum umbrella plate.
Power Planes: Instead of thin wires, use Copper Zones (Fill) for your Ground and V+ rings. This reduces resistance to nearly zero.
4. Logic Isolation in KiCad
To protect your ESP32 from the "Big Pop," you can physically split the board:
Keep all the high-voltage "Power Cells" on the outer perimeter.
Keep the ESP32 and CAN transceiver in the "quiet" center near the encoder.
This creates a "Star" of logic in the middle, surrounded by a "Ring" of power—ideal for EMI (interference) management.
5. A Final Tip for "Winding Day"
Since you are winding those trapezoidal wedges today:
When you finish a coil, leave about 100mm of "tail" on each end. When you design the PCB in KiCad, you can place large through-hole pads at the perimeter of each H-bridge "cell." This allows you to thread your 26AWG stator wires directly into the PCB for a rock-solid, vibration-proof connection.
Are you planning to make the PCB 2-layer or 4-layer? (For a motor of this power, a 4-layer board with internal Ground/Power planes is the "Gold Standard" for stability, but 2-layer with beefy traces is cheaper and totally doable.)
Enjoy the winding marathon! I'm ready to help with the KiCad schematic symbols for the BTN8982 whenever you're ready.

Hall State (ABC)	Bridge A (RPWM / EN)	Bridge B (RPWM / EN)	Bridge C (RPWM / EN)
101 (5)	255 / HIGH	0 / HIGH	X / LOW
001 (1)	255 / HIGH	X / LOW	0 / HIGH
011 (3)	X / LOW	255 / HIGH	0 / HIGH
010 (2)	0 / HIGH	255 / HIGH	X / LOW
110 (6)	0 / HIGH	X / LOW	255 / HIGH
100 (4)	X / LOW	0 / HIGH	255 / HIGH



