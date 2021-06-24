import math 
import numpy as np 
import matplotlib.pyplot as plt

# power_output_a = []
# power_output_b = []
# x_axis = []

# with open("sim.txt") as f:
#     lines = f.readlines()
#     lines = [ x.replace("\n", '').split(' ') for x in lines]
#     print(lines)
#     x_axis = [ (float(x[0])/(60*60)) for x in lines ]
#     power_output_a = [ float(x[1]) for x in lines ]
#     power_output_b = [ float(x[2]) for x in lines ]
    
start = 0
stop = 24
step = 0.001

change_period = 1
cahnge_factor_a = change_period/1
cahnge_factor_b = change_period/100
day_factor_a = 0.3
day_factor_b = 0.6
sunrise_factor_a = 0.5

sunrise_end = 7
sunrise_start = sunrise_end - change_period

sunset_end = 23
sunset_start = sunset_end - change_period

power_output_a = []
power_output_b = []
x_axis = []

power_min = 0
power_max = 950

for x in np.arange(start, stop, step):
    
   
    
    # # # generate sunrise
    # if(x >= sunrise_start and x <= sunrise_end):
    #     x_axis.append(x)
    #     pass
    #     x_local = (x - sunrise_start) 
    #     x_local_pow_a = change_period/2 - x_local
    #     power_output_a.append((power_max-power_min) * (sunrise_factor_a) * math.pow(math.e, (-(x_local_pow_a*x_local_pow_a)/cahnge_factor_a)))
        
    #     x_local_pow_b = x_local - change_period
    #     power_output_b.append((power_max-power_min) * day_factor_b * math.pow(math.e, (-(x_local_pow_b*x_local_pow_b)/cahnge_factor_b)))
        
    # generate sunset
    if(x >= sunset_start and x <= sunset_end):
        x_axis.append(x)
        pass
        x_local = (x - sunset_start) 
        x_local_pow_a = change_period/2 - x_local
        power_output_a.append((power_max-power_min) * (sunrise_factor_a) * math.pow(math.e, (-(x_local_pow_a*x_local_pow_a)/cahnge_factor_a)))
        
        x_local_pow_b = x_local
        power_output_b.append((power_max-power_min) * day_factor_b * math.pow(math.e, (-(x_local_pow_b*x_local_pow_b)/cahnge_factor_b)))

    # # generate evning
    # elif(x >= sunrise_start and x <= sunset_end):
    #     pass
    #     power_output_a.append((power_max-power_min)*day_factor_a)
    #     power_output_b.append((power_max-power_min)*day_factor_b)
    # # generate night
    # elif(True):
    #     pass
    #     power_output_a.append(power_min)
    #     power_output_b.append(power_min)

# # print(power_output_a)
sum1 = [x+y for x, y in zip(power_output_a, power_output_b)]
# plt.plot(x_axis, sum1)
plt.plot(x_axis, power_output_b, label="power_b")
plt.plot(x_axis, power_output_a, label="power_a")
plt.legend()
plt.savefig('test_graph_computed.png')