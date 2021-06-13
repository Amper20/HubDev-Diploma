import math 
import numpy as np 
import matplotlib.pyplot as plt

power_output_a = []
power_output_b = []
x_axis = []

with open("sim.txt") as f:
    lines = f.readlines()
    lines = [ x.replace("\n", '').split(' ') for x in lines]
    print(lines)
    x_axis = [ (float(x[0])/(60*60)) for x in lines ]
    power_output_a = [ float(x[1]) for x in lines ]
    power_output_b = [ float(x[2]) for x in lines ]
# start = 0
# stop = 24
# step = 0.001

# change_period = 0.5
# cahnge_factor = change_period/10
# day_factor_a = 0.2
# day_factor_b = 0.9

# sunrise_end = 7
# sunrise_start = sunrise_end - change_period

# sunset_end = 23
# sunset_start = sunset_end - change_period

# power_output_a = []
# power_output_b = []
# x_axis = []

# power_min = 0
# power_max = 1023

# for x in np.arange(start, stop, step):
    
#     x_axis.append(x)
    
#     # generate sunrise
#     if(x >= sunrise_start and x <= sunrise_end):
#         pass
#         x_local = (x - sunrise_start) 
#         x_local_pow_a = change_period/2 - x_local
#         power_output_a.append((power_max-power_min) * math.pow(math.e, (-(x_local_pow_a*x_local_pow_a)/cahnge_factor)))
        
#         x_local_pow_b = x_local - change_period
#         power_output_b.append((power_max-power_min) * day_factor_b * math.pow(math.e, (-(x_local_pow_b*x_local_pow_b)/cahnge_factor)))
        
#     # generate sunset
#     elif(x >= sunset_start and x <= sunset_end):
#         pass
#         x_local = (x - sunset_start) 
#         x_local_pow_a = change_period/2 - x_local
#         power_output_a.append((power_max-power_min) * math.pow(math.e, (-(x_local_pow_a*x_local_pow_a)/cahnge_factor)))
        
#         x_local_pow_b = x_local
#         power_output_b.append((power_max-power_min) * day_factor_b * math.pow(math.e, (-(x_local_pow_b*x_local_pow_b)/cahnge_factor)))

#     # generate evning
#     elif(x >= sunrise_start and x <= sunset_end):
#         pass
#         power_output_a.append((power_max-power_min)*day_factor_a)
#         power_output_b.append((power_max-power_min)*day_factor_b)
#     # generate night
#     elif(True):
#         pass
#         power_output_a.append(power_min)
#         power_output_b.append(power_min)

# # print(power_output_a)
plt.plot(x_axis, power_output_a)
plt.plot(x_axis, power_output_b)
plt.savefig('test_graph_computed.png')