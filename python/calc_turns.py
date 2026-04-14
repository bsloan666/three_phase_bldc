import argparse

def calculate_motor_specs(wire_gauge, perimeter_mm, num_coils, target_res, voltage, target_rpm):
    # Resistance in Ohms per 1000ft for common AWG
    awg_data = {18: 6.385, 20: 10.15, 22: 16.14, 24: 25.67, 26: 40.81}
    
    # 1. Basic Wire Calculations
    res_per_ft = awg_data.get(wire_gauge, 16.14) / 1000
    total_ft = target_res / res_per_ft
    ft_per_coil = total_ft / (num_coils / 3)  # 3-phase star assumption
    turns = (ft_per_coil * 304.8) / perimeter_mm
    
    # 2. Back-EMF and Speed (Approximate for Air-Core)
    # Note: K_v is roughly proportional to 1/turns. 
    # This is a simplified estimation for an air-core axial flux.
    # In reality, magnet strength (Tesla) plays a huge role.
    est_kv = 10000 / (int(turns) * (num_coils/36)) # Heuristic for 120mm air-core
    no_load_speed = est_kv * voltage
    
    print(f"--- 120mm Axial Flux Design Specs ---")
    print(f"Wire Gauge:      {wire_gauge} AWG")
    print(f"Target Phase R:  {target_res} Ohms")
    print(f"Total Wire:      {total_ft:.2f} ft")
    print(f"Turns per Coil:  {int(turns)}")
    print(f"--- Performance (at {voltage}V) ---")
    print(f"Est. Kv:         {int(est_kv)} RPM/V")
    print(f"No-load Speed:   {int(no_load_speed)} RPM")
    
    if no_load_speed < target_rpm:
        print(f"WARNING: Voltage too low for {target_rpm} RPM. Increase V or reduce turns.")
    else:
        print(f"Speed Check:     Target {target_rpm} RPM is achievable.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-w", "--wire-gauge", type=int, default=22)
    parser.add_argument("-p", "--perimeter", type=float, required=True, help="Mean perimeter of coil in mm")
    parser.add_argument("-n", "--num-coils", type=int, default=36)
    parser.add_argument("-r", "--resistance", type=float, default=3.5)
    parser.add_argument("-v", "--voltage", type=float, default=12.0)
    parser.add_argument("-s", "--target-rpm", type=float, default=1000.0)
    
    args = parser.parse_args()
    calculate_motor_specs(args.wire_gauge, args.perimeter, args.num_coils, 
                          args.resistance, args.voltage, args.target_rpm)

