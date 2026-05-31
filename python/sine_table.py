import math


array_size = 60 


head = f"  const uint8_t saddle_table[{array_size}] = "
head += "{"

print(head)

outstr = ""
incr = (math.pi * 2)/array_size

for i in range(array_size):
    outstr += f"{int(math.sin(incr * i) * 128) + 128}, "
    if i and not (i % 16):
        print(f"    {outstr}")
        outstr = ""

print(f"    {outstr}")


print("  };")

