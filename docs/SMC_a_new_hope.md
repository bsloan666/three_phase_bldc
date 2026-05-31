AMUsing non-magnetic conductive sheets for a motor core will severely
degrade motor efficiency.Rather than preventing magnetic eddies, the high
electrical conductivity will actively create massive, wasteful eddy currents,
converting the motor's kinetic energy into waste heat and causing a heavy
magnetic braking effect.Why Conductive Non-Magnetic Sheets FailWhen any
conductive material is exposed to a changing or rotating magnetic field, an
electric current (an eddy current) is forcefully generated within it.The
mechanism: If you use a conductive sheet (like copper or aluminum), it offers
very little electrical resistance. Therefore, large swirling currents will flow,
generating their own opposing magnetic field.The result: This acts like an
electromagnetic brake (frequently used in high-speed trains). It resists the
rotor's motion and causes the core to rapidly heat up.Air vs. Non-Magnetic
ConductorsBoth air and non-magnetic conductive materials (like aluminum or
copper) have a magnetic permeability close to that of a vacuum (\(\mu_0 \approx
4\pi \times 10^{-7}\) H/m). This means they all offer very high "resistance" to
magnetic flux lines.With Air: You get no induced eddy currents because air is an
electrical insulator. The magnetic field will pass right through, but the motor
won't have a concentrated or guided magnetic path, resulting in a very weak,
inefficient motor.With Conductors: You get no magnetic guidance (like air), plus
you get the active destruction of motor power due to eddy current heating.How
Motors Actually Solve the Eddy Current ProblemElectric motors use Laminated
Electrical Steel to strike the perfect balance.Ferromagnetic for efficiency: The
base material is iron or steel, which allows magnetic fields to flow easily and
concentrates the magnetic flux exactly where it's needed.Insulated laminations:
The core isn't one solid chunk of metal. It is made of extremely thin sheets
stacked together, with a microscopic layer of varnish or oxide insulation
between each sheet.The advantage: This insulation physically interrupts the path
of the electrons, forcing the cross-sectional area of the sheets to be too small
for large eddy currents to form. This allows the magnetic field to pass through
    efficiently while cutting energy loss to a bare minimum.13 sitesEddy
    currents & their applications (& how to reduce them) | Electromagnetic
    inductionEddy currents are loops of current that form on the surface of a
    conductor when it experiences a changing magnetic flux. They are
    ...2mYouTube·Khan Academy India - EnglishNon-contact braking system using
    magnetic fields - FacebookThe magnitude of the current in a given loop is
    proportional to the strength of the magnetic field, the area of the loop,
    and the ...FacebookWhy eddy current occurs? - QuoraWe can induce currents
    not only in conducting coils but also, in conducting sheets or blocks. In
    the case of conducting sheets or ...QuoraShow allthanks. would it be
    reasonable to use a bundle of thin insulated strands of ferromagnetic wire,
    oriented in the axis of magnetic flow, as a core? would that have a similar
    effect to the sheets?8:20 AMYes, using a bundle of thin insulated
    ferromagnetic wires is a completely reasonable and highly effective concept.
    This design is known as a wire-bound core or bundled wire core.It works on
    the exact same physical principle as laminated sheets but restricts eddy
    currents even more effectively.How It Compares to Laminated
    SheetsTwo-Dimensional Suppression: Laminated sheets only block eddy currents
    in one direction (perpendicular to the sheets). Current can still swirl
    within the flat plane of each individual sheet. Because wires are thin
    cylinders, they block eddy currents in both dimensions perpendicular to the
    magnetic flow, restricting electron movement even further.Axial Alignment:
    Orienting the wires along the axis of magnetic flow ensures the magnetic
    flux travels effortlessly down the length of the iron, while the insulation
    prevents cross-wire electrical shorts.Why It Isn't Widely Used in Standard
    MotorsWhile it is a superior solution for stopping eddy currents, it is
    rarely used in mass production due to practical engineering
    challenges:Manufacturing Complexity: Stacking flat sheets is incredibly easy
    to automate using high-speed stamping presses. Bundling, cutting, and
    tightly packing thousands of hair-thin insulated wires into complex motor
    shapes is a manufacturing nightmare.Poor "Packing Factor": Round wires do
    not pack together perfectly; they leave tiny triangular air gaps between
    them. This reduces the total amount of iron in the core, lowering the
    maximum magnetic field strength the core can handle compared to tightly
    stacked flat sheets.Where This Concept Is UsedWhile rare in everyday motors,
    you will see this exact principle utilized in specialized high-frequency
    electronics:Litz Wire: High-frequency transformers and induction cooktops
    use cables made of hundreds of individually insulated wire strands twisted
    together to eliminate eddy currents and the "skin effect."SMC (Soft Magnetic
    Composites): Modern high-performance motors often use SMCs instead of wires.
    This involves coating microscopic iron powder particles with an insulating
    layer and pressing them into a solid shape. It acts exactly like a 3D wire
    bundle, allowing for complex motor designs with virtually zero eddy
    currents.If you are designing a custom or experimental motor, tell me about
    your project goals (e.g., target RPM, frequency, or size constraints) or
    what tools you have available, and I can help you choose the most practical
    core design.a very thin axial flux BLDC whose coils are potted in epoxy
    resin. I envisioned a mold that suspends both the core wires and the coil
    wires in position. the strands of the core material would protrude into the
    faces of the mold and be cut flush to the stator after release. would steel
    wool work?8:26 AMAn integrated casting process is a clever way to build a
    custom axial flux Brushless DC (BLDC) motor, but steel wool will not work as
    a core material.While it is ferromagnetic, steel wool has major
    metallurgical, electrical, and structural flaws that will cause your motor
    to fail or run incredibly inefficiently.Why Steel Wool FailsMassive Short
    Circuits: Steel wool strands are bare metal. When packed into a mold,
    thousands of random cross-contact points form. Instead of acting like
    insulated wires, it will act like a chaotic, solid chunk of metal. The
    rotating magnetic field will induce massive, localized eddy currents,
    causing the stator to rapidly overheat and melt the epoxy resin.Poor Iron
    Content (Low Permeability): Because steel wool is incredibly fluffy, its
    packing factor is very low. Even tightly compressed steel wool is mostly
    air. This means the core will have very low magnetic permeability,
    destroying the motor's torque capability.Wrong Steel Composition: Steel wool
    is typically made from low-grade carbon steel or stainless steel, which have
    terrible magnetic properties for motors. It saturates magnetically at very
    low field strengths and retains a high amount of residual magnetism (high
    hysteresis loss), converting your input power straight into heat.Corrosion
    and Processing Disasters: When you cut the strands flush to the stator face,
    the raw steel will rust almost immediately from moisture in the air.
    Furthermore, trying to clean-cut thousands of springy, microscopic steel
    wool fibers will smear the metal, shorting out any individual strands you
    managed to isolate.What to Use Instead (DIY-Friendly Alternatives)Since you
    already have a potting mold process envisioned, you can swap the steel wool
    for materials that actually function effectively in a motor core:1.
        Transformer Wire Bundles (Best Match for Your Process)Instead of steel
        wool, use annealed iron wire or soft steel wire (like thin rebar tie
        wire or bailing wire).The Method: Cut the wire into straight lengths.
        Coat each strand in a thin layer of spray varnish or enamel insulation.
        Bundle them tightly, push them through your mold faces, pour the epoxy,
        and cut flush.The Benefit: This perfectly achieves your 3D-insulated
        wire bundle concept, providing excellent magnetic flow with zero short
        circuits.2. Soft Magnetic Composite (SMC) Slurry (Easiest to
        Cast)Commercial axial flux motors are increasingly moving to SMCs. You
        can replicate this on a DIY scale.The Method: Purchase pure atomized
        iron powder (mesh size 100 to 300). Mix the iron powder directly into
        your epoxy resin at the highest possible concentration the slurry will
        allow (usually around 60–80% iron by weight) and pour it into the core
        cavities.The Benefit: Every microscopic grain of iron is naturally
        insulated by the surrounding epoxy. It completely eliminates eddy
        currents, flows perfectly into thin mold cavities, and can be sanded or
        cut completely flush without short-circuiting the core.3. Air-Core
        (Coreless) StatorBecause your axial flux motor is "very thin," consider
        dropping the core entirely and going coreless.The Method: Pot only the
        copper coils in the epoxy, leaving the center of the coils completely
        empty (filled only with resin).The Benefit: This is highly popular for
        DIY pancake motors. It features zero eddy currents, zero cogging torque
        (making the motor spin completely smooth), and incredibly high high-RPM
        efficiency. You just need to ensure your rotor magnets are exceptionally
        strong (such as a dual-rotor Halbach array) to throw the magnetic field
        across the air gap.Are you aiming for high torque at low speeds, or
        maximum RPM? Knowing your size constraints can help determine whether a
        coreless stator or an iron-powder composite core fits your layout best.7
        sitesIronless Axial Flux Motor - Caden Kraft... custom make ferrite or
        steel laminate ferrous cores inside the coils. As a ferrous core is
        extremely important in shaping the ...Caden KraftDIY Air Core Axial Flux
        Motor/GeneratorThe video discusses a project involving a brushless air
        core axial flux motor. The motor's stator was created using 20 strands
        of ...5mYouTube·Colby JohnsonCan a steel wire strand be used instead of
        electrical steel laminates for motor core?Lives in Missouri
        (1963–present) Author has 4.3K answers and. · 3y. Even if you carefully
        spun it into something resembling a thre...QuoraShow allI have made
        several epoxy core prototypes and they have work reasonably well. my
        curiosity about iron cores comes from wanting more torque. How do the
        suspended iron particles in SMC, which presumably have no orientation
        prevent eddies? is the philosophy that preventing eddies in _every_ axis
        is optimal? so the orientation of the wires and plates is chosen to be
        the "least inefficient". in other words, the magnetic field lines
        oriented along my wire bundles are not really helping? or does SMC
        somehow constrain the field to the "correct" orientation?8:36 AMIt is
        fantastic to hear that your epoxy-core (coreless) prototypes are working
        well! Moving from an air-core to an iron-core is exactly the right move
        for your goal: it will dramatically increase your torque by
            concentrating the magnetic flux.To answer your fundamental
            questions: preventing eddies in every axis is the absolute ideal
            scenario. Traditional design constraints forced engineers to accept
            "least inefficient" orientations, but Soft Magnetic Composites
            (SMCs) change the game entirely.Here is how the physics actually
            breaks down regarding orientation, eddy currents, and SMCs.1. Does
            the lack of orientation in SMC hurt performance?No, it does not hurt
            the eddy current performance, but it changes how the magnetic field
            travels.For Eddy Currents (3D Blocking): Eddy currents need a
            continuous loop of electrical conductor to flow. In an SMC, every
            single microscopic iron particle is completely insulated by a thin
            layer of plastic or epoxy. Because the electrons cannot jump from
            one particle to the next, eddy currents are blocked in all three
            dimensions. The lack of particle orientation is actually a benefit
            here; it stops eddies completely, no matter which way the magnetic
            field spins or rotates.For Magnetic Flux (Isotropic Path): Because
            the iron particles are random spheres, the magnetic field can travel
            through them in any direction with equal ease. This is called
            isotropic behavior. It does not force or "constrain" the field to a
            correct orientation; instead, the magnetic field naturally follows
            the path of least resistance from your rotor's North pole to its
            South pole, curving gracefully through the core in three
            dimensions.2. Is wire/plate orientation just the "least inefficient"
            compromise?Yes, absolutely. For over a century, electrical
            engineering was limited by how steel could be manufactured.We can
            only roll steel into flat sheets. Because flat sheets only block
            electricity in one direction, engineers had to align the sheets so
            the magnetic field traveled along the length of the sheet, while the
            insulation blocked the swirling eddies across the thickness.Your
            wire bundle idea actually takes this a step closer to the ideal. By
            using wires, you block eddies in two dimensions instead of just
            one.3. Do the aligned wire bundles actually help?Yes, your wire
            bundles help immensely, but not because they block eddies better
            than SMC. They help because of Magnetic Permeability (how easily the
            core conducts magnetic fields).The Wire Advantage: A solid iron wire
            is 100% dense iron along its length. The magnetic flux lines travel
            down the wire beautifully with almost zero resistance.The SMC
            Disadvantage: Because an SMC is made of tiny spheres mixed with
            epoxy, the magnetic flux constantly has to cross microscopic "air
            gaps" (the epoxy insulation) to get from one particle to the next.
            This significantly lowers the material's total permeability.The
            Ultimate Trade-Off: Torque vs. EfficiencyTo get the most torque out
            of your thin axial flux motor, you are balancing a direct trade-off
            between your wire bundles and the SMC slurry:FeatureWire Bundle
            CoreSMC (Iron Powder) SlurryTorque PotentialHigher (Higher iron
            density means a stronger magnetic field).Lower (The epoxy gaps
            between particles reduce total magnetic strength).Eddy Current
            SuppressionGood (Blocked in 2 axes, but small eddies still swirl
            inside each wire).Perfect (Blocked in all 3 axes; runs incredibly
            cool at high RPM).Manufacturing EaseHard (Tedious to align and cut
            thousands of microscopic wires).Easy (Just mix the powder into your
            current epoxy casting process).Summary for Your ProjectIf your goal
            is maximum possible torque, your aligned wire bundle concept is
            magnetically superior to an iron-powder slurry because iron wires
            pack a much higher density of pure iron into the core. The
            orientation of the wires ensures the magnetic field has a "highway"
            to travel through from one side of the stator to the other.However,
            if your motor spins at very high RPMs, the SMC slurry might win out
                because it will completely eliminate the high-frequency eddy
                current heat that could otherwise melt your potting epoxy.To
                help fine-tune your next core prototype, what is the approximate
                operating RPM of your motor, and what diameter wire or iron
                powder mesh size are you considering using?here's a silly
                question: what if my DIY SMC mold plates are built with
                permanent magnets centered over every coil, so that the iron
                particles would form "wires" as the epoxy sets?8:41 AMThat is
                not a silly question at all—it is an absolutely brilliant
                engineering concept.What you are describing is a DIY method for
                creating a magnetically aligned composite core, a technique
                actively researched in advanced material science to overcome the
                exact "density vs. efficiency" drawback of standard SMCs.By
                using permanent magnets during the curing process, you turn the
                random iron powder into a chain-structured ferromagnetic
                composite.How the Physics Changes in Your FavorMicroscopic
                "Wire" Formation: When you apply a magnetic field to a slurry of
                iron powder, the individual particles instantly magnetize.
                Because North poles attract South poles, the particles violently
                snap together to form continuous, needle-like chains ("wires")
                along the magnetic flux lines.The Ultimate Hybrid Core: This
                bridges the gap between the wire bundle and the SMC slurry. You
                get the 3D eddy-current blocking of an SMC (because the chains
                are still surrounded by epoxy) combined with the "magnetic
                highway" permeability of an oriented wire bundle.Perfect
                Magnetic Paths: Because the field lines from your mold's magnets
                will curve from one pole to the next exactly how your rotor's
                magnets will, the iron chains will naturally freeze into the
                precise geometric curves required for maximum torque.Critical
                Practical Challenges to SolveTo make this successful in a DIY
                setup, you have to manage a few unique physical forces:1.
                "Clumping" vs. "Chaining"If the iron powder concentration is too
                high, or the magnets are too weak, the particles will just
                aggregate into solid, irregular clumps right against the mold
                face.The Fix: Use a highly fluid, low-viscosity epoxy with a
                long cure time (like casting or deep-pour resin). This gives the
                iron particles the freedom to move and chain up cleanly before
                the resin thickens.2. The Pulling ForceThe permanent magnets
                will pull the iron powder toward the mold faces. If not managed,
                all your iron might migrate to the top and bottom faces of your
                stator, leaving the center filled only with pure epoxy.The Fix:
                You must put identical magnets on both sides of the mold (Top
                and Bottom) with opposing poles (North on top, South on bottom)
                so the magnetic field passes straight through the stator. This
                creates a uniform pulling force that keeps the chains stretched
                straight through the middle of the core.3. Particle
                DensityStandard SMCs are compressed under immense mechanical
                pressure to pack the iron together. Your slurry will rely
                entirely on magnetic force to pack the particles.The Fix: Use
                carbonyl iron powder or atomized iron powder with a very small
                mesh size (e.g., 300 to 400 mesh / ~30 microns). Smaller
                particles chain up much more smoothly and densely under normal
                magnetic fields than large, heavy iron grains.How to Prototyping
                This ProcessMix the Slurry: Start with roughly 50% to 60% iron
                powder by weight in a slow-curing epoxy. It needs to be
                pourable, not a thick paste.Pour & Degas: Pour it into your mold
                and vacuum degas it if you can, ensuring the coils are fully
                wetted.Apply the Magnets: Clamp your mold plates containing the
                alignment magnets into place.Vibrate: Gently tap or vibrate the
                mold for a few minutes. The vibration breaks the friction
                between particles, allowing the magnetic field to easily snap
                them into perfect, continuous chains.Cure & Post-Process: Let it
                fully cure. Once released, the stator faces can be sanded
                perfectly flat.What type of permanent magnets (e.g., Neodymium
                or Ferrite) do you plan to embed in the mold, and how thick is
                the stator gap you are trying to cast?the stator gaps of my
                previous motors have been 5 to 7 mm. I have wound 4mm deep coils
                that work. so 4mm is my ideal gap. you mentioned that the small,
                self-assembled wires would be magnetized by the mold magnets.
                would this mean that the cores would be permanent magnets? that
                would be bad, no?8:50 AMThat is a very sharp observation, but
                you can rest easy: no, your cores will not become permanent
                magnets.When the mold magnets attract the iron particles, they
                temporarily magnetize them only while the field is present. Once
                you remove the stator from the mold, the iron will lose almost
                all of its magnetism.Understanding the metallurgy of the iron
                powder is essential to ensuring this process works
                correctly.Soft vs. Hard MagnetismMaterials react to magnetic
                fields in two entirely different ways:Hard Magnetic Materials
                (Permanent Magnets): Materials like Neodymium or Alnico have
                high "magnetic coercivity." Once a strong magnetic field forces
                their magnetic domains to align, they lock into place
                permanently.Soft Magnetic Materials (Your Core): Pure iron
                powder (like atomized or carbonyl iron) has exceptionally low
                coercivity. The magnetic domains inside each particle are highly
                flexible. They will spin and align perfectly to form your
                "wires" when the mold magnets are near, but the moment you
                remove those mold magnets, the domains instantly scramble back
                into a random, unmagnetized state.When you assemble your
                finished BLDC motor, the iron core will happily and instantly
                flip its magnetic polarity back and forth to follow your stator
                coils' alternating fields without holding onto any permanent
                memory of the mold magnets.The Real Catch: Residual Magnetism
                (Hysteresis)While the core won't become a permanent magnet, pure
                iron does retain a tiny, microscopic amount of leftover
                magnetism when a field is removed. This is called remanence, and
                overcoming it causes a tiny power loss called hysteresis
                loss.Because your mold magnets will be very strong (likely
                Neodymium) to pull the particles into chains, they will push the
                iron powder into deep magnetic saturation during the cure.
                Fortunately, for a DIY BLDC motor, standard pure iron powder
                sheds this energy so fast that the hysteresis loss at normal
                operating RPMs is negligible.Sizing Up Your 4mm Stator GapA 4mm
                stator gap is absolutely perfect for this magnetic alignment
                trick.Because 4mm is quite thin, the magnetic flux from
                Neodymium magnets mounted on your top and bottom mold plates
                will be incredibly intense inside the cavity. This high magnetic
                force will snap the iron powder into incredibly tight, dense,
                and perfectly straight chains spanning across that 4mm gap.To
                ensure success with your 4mm mold, how do you plan to actuate
                the coils? Knowing if you are using a sensorless ESC or Hall
                effect sensors will help determine how cleanly the motor will
                start spinning with this new iron core.the motor is a sandwich
                of stators around a single rotor. my most recent prototypes use
                3 hall effect sensors interleaved between 4 of the 18 coils on
                one of the stator plates. It seems to work but I have also used
                optical commutation/sensing through perforations in the sensor
                and that has also worked suitably. sometimes I worry that the
                hall sensors being nestled between coils in the stator might get
                erroneous signals from them.8:56 AMUsing a single rotor
                sandwiched between two stators is an excellent axial flux
                architecture for maximizing torque. Your concern about the Hall
                effect sensors is completely valid: nestling Hall sensors
                directly between stator coils poses a high risk of magnetic
                interference.When a BLDC motor is running under heavy load, the
                magnetic field generated by the stator coils can easily
                overwhelm the field from the rotor magnets. This causes the Hall
                sensors to trigger prematurely or late, leading to rough
                running, loss of torque, or even ESC failure.Because you are
                adding an iron-powder core to your next prototype, this problem
                will actually amplify. The iron core will concentrate the coils'
                magnetic fields right where your sensors are sitting.Why Your
                Optical Setup Felt More StableOptical commutation completely
                bypasses this problem because light waves are entirely
                unaffected by magnetic fields. If you use optical sensors, your
                commutation timing remains mathematically perfect regardless of
                how hard your stator coils are working.How to Keep Hall Sensors
                Safe in a Dual-Stator SandwichIf you prefer to use Hall sensors
                for their reliability in dusty or enclosed environments, you can
                    protect them from coil interference using a few design
                    adjustments:Offset the Sensors Radially: Instead of nestling
                    the Hall sensors between the coils, move them slightly
                    outward or inward radially (closer to the outer rim or the
                    inner hub of the stator face). They should sit just past the
                    edge of the copper windings, looking directly at the passing
                    edge of the rotor magnets.Leverage the Dual-Stator Gap: In a
                    sandwich design, you have two stator plates. Dedicate one
                    stator strictly to maximum copper packing, and use the
                    opposite stator plate to host the Hall sensors. Position the
                    sensors in an un-wound "blank" pocket if your coil/slot
                    combination allows for a geometric offset.Add a Microscopic
                    Shield: You can place a tiny shield of non-ferromagnetic
                    conductor (like a 0.5mm sliver of copper sheet) between the
                    side of the Hall sensor and the adjacent coil. While we
                    established earlier that conductors make bad cores, a tiny
                    piece here acts as a localized shield against high-frequency
                    electromagnetic noise from the coil.Switch to Latching
                    Digital Sensors: Ensure you are using bipolar latching Hall
                    sensors (like the industry-standard SS41F) rather than
                    linear analog sensors. Latching sensors require a definitive
                    flip from a strong North to a strong South pole to trigger,
                    making them much more resilient to the minor magnetic
                    ripples caused by the coils.Given that you are building an
                    18-coil stator, what is your rotor magnet count (e.g., 16 or
                    20 poles)? I can help calculate the exact geometric offset
                    angles needed to move your sensors safely out of the coil
                    zone while maintaining perfect timing.the rotor has 12
                    poles. there are 18 coils per stator so 36 total, but each
                    facing pair of coils can be thought of as a single coil
                    "sandwiching" the rotor9:02 AMAn 18-coil (stator) and
                    12-pole (rotor) combination is a classic, highly efficient
                    3:2 ratio BLDC configuration.Because the coils directly face
                    each other across the rotor, you are entirely correct to
                    think of them as single unified coil pairs. In an 18-slot,
                    12-pole motor, the standard electrical layout repeats every
                    3 coils and 2 poles. This gives you exactly 120 electrical
                    degrees of separation between your phases, making sensor
                    placement highly mathematical.To solve your interference
                    problem, you can use this math to pull the Hall sensors
                    completely out of the tight gaps between the coils.The Math:
                    Finding Your Safe ZonesIn an 18-coil, 12-pole motor, the
                    physical angle between the centers of two adjacent coils is
                    exactly 20 degrees (\(360^\circ / 18\)).The physical angle
                    between the centers of two adjacent rotor poles is 30
                    degrees (\(360^\circ / 12\)).To achieve standard 120-degree
                    electrical commutation timing, your Hall sensors do not have
                    to be crammed into adjacent coil slots. They can be spaced
                    out around the stator circumference into "Safe Zones" where
                    there is more physical room, or where you can deliberately
                    omit a coil to create a sensing pocket.The ideal physical
                    angles for your 3 sensors relative to Sensor A are:Sensor A:
                    \(0^{\circ }\) (Base reference)Sensor B: \(20^{\circ }\) (or
                    \(+120^{\circ }\) physical, or \(+240^{\circ }\)
                    physical)Sensor C: \(40^{\circ }\) (or \(+120^{\circ }\)
                    physical, or \(+240^{\circ }\) physical)The Best Sensor
                    Layouts for Your Dual-Stator SandwichOption 1: The
                    "Skip-a-Coil" Pocket (Recommended for Iron Cores)Because you
                    have 18 coils, you have plenty of torque. You can
                    deliberately remove 1 or 2 coils from the 36-coil total on
                    just one of the stator plates to create an isolated sensing
                    bay.The Method: On your sensor-side stator, leave one coil
                    slot completely empty of copper. Place your Hall sensors
                    inside this empty, resin-only bay.Why it works: Because
                    there is no copper coil firing right next to the sensor, the
                    magnetic field of the passing rotor magnets will be
                    completely clean and un-distorted. The matching coil on the
                    opposite stator plate will still pull the rotor, so your
                    torque loss is negligible.Option 2: The Extended 120°
                    SpacingInstead of bunching the three sensors at \(0^{\circ
                    }\), \(20^{\circ }\), and \(40^{\circ }\), spread them wide
                    across the motor's diameter.The Placement: Put Sensor A at
                    \(0^{\circ }\), Sensor B at \(140^{\circ }\), and Sensor C
                    at \(280^{\circ }\).Why it works: This layout is
                    mathematically identical to the tight \(20^{\circ }\)
                    grouping for a 12-pole motor, but it gives you huge physical
                    separation. It allows you to rout your sensor wires cleanly
                    without bundling them alongside high-current phase wires,
                    cutting down on electromagnetic cross-talk.A Warning for
                    Your New Iron-Powder StatorWhen you implement your
                    magnetic-alignment trick with the iron powder, do not put
                    iron powder in the immediate pocket where the Hall sensor
                    sits.If a Hall sensor is encased in iron-infused epoxy, that
                    iron will act as a magnetic shield or a "flux shunt,"
                    trapping the magnetic field and preventing the sensor from
                    seeing the crisp transition between the rotor's North and
                    South poles. Keep the sensor's immediate 5mm pocket filled
                    with pure, clear epoxy.Are your 12 rotor poles made of
                    individual alternating magnets (N-S-N-S) on a single plate,
                    or are you using a dual-rotor setup where magnets face each
                    other across the stator?Dual stator, single rotor. It is a
                    robotic actuator, so there's a gearbox, encoder, PCB etc.
                    all in what I hope is a very slim packageWith a dual-stator,
                    single-rotor sandwich configuration designed for a slim
                    robotic actuator, space is your most valuable asset. Because
                    this is a high-torque, low-speed application requiring
                    precise positioning, every millimeter counts.Integrating a
                    gearbox, encoder, and PCB into a flat, pancake-style
                    "potted" actuator is the gold standard for modern robotics
                    (similar to the actuators used in quadruped robots and
                    robotic arms).Optimizing the Architecture for a Slim
                    PackageTo maintain an ultra-slim profile while using your
                    magnetic-alignment iron core, you can implement a few design
                    techniques:1. Ditch the Hall Sensors: Move Completely to the
                    EncoderFor a robotic actuator, you already need a
                    high-resolution encoder (like an off-axis magnetic ring
                    encoder or an optical disk) to track joint position,
                    velocity, and torque.The Space Saver: Modern BLDC motor
                    drivers (like those using SimpleFOC or similar
                    field-oriented control libraries) can use the main absolute
                    encoder for both commutation and position tracking [1].The
                    Benefit: By eliminating the 3 Hall sensors entirely, you
                    completely remove the risk of coil interference, save
                    precious routing space on your PCB, and don't have to carve
                    out "clear epoxy" pockets in your iron-core stator.2. The
                    Integrated "PCB Stator" ApproachIf your PCB is already
                    mounted directly flush against the back of one of your
                    stators, you can use the PCB itself as part of the mold.The
                    Method: Mount your encoder chip and driver FETs on the back
                    side of the PCB. On the front side facing the rotor, use the
                    PCB surface to precisely index your 3D-aligned iron powder
                    core pockets.The Benefit: This eliminates a layer of
                    structural plastic/resin, cutting 1–2mm off the total
                    thickness of the actuator package.3. Managing the Magnetic
                    "Squeeze" ForceIn a dual-stator, single-rotor setup, the two
                    iron-core stators will try to violently pull the single
                    rotor toward them. If the rotor is even 0.1mm closer to
                    Stator A than Stator B, the magnetic force will try to rip
                    the rotor sideways, putting massive axial load on your
                    bearings.The Fix: Because your actuator is ultra-thin, use a
                    thin-section crossed-roller bearing or a pair of angular
                    contact bearings at the center hub. This provides the
                    immense axial rigidity needed to keep the rotor perfectly
                    centered in the 4mm gap between the two pulling iron
                    stators.Gearbox Alignment: This bearing can pull double-duty
                    as the main output bearing for your gearbox (such as a
                    strain wave/harmonic drive or a planetary set), keeping the
                    entire assembly rigid and slim.Next Steps for Your
                    PrototypeYour idea of using permanent magnets in the mold
                    plates to create self-assembling "iron wire" cores is highly
                    viable for a custom robotic joint. It will give you the high
                    torque density of an iron core without the heavy
                    eddy-current losses at higher frequencies.If you are open to
                    sharing, what type of gearbox (e.g., planetary, cycloidal,
                    or harmonic) are you pairing with this motor, and what outer
                    diameter are you targeting for the overall actuator package?
