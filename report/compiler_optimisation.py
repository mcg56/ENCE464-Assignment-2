# MK3 100 iterations 3 threads

from matplotlib import pyplot as plt


cube_size = [7, 51, 101, 151, 201, 251, 301]#, 401, 501]
O_zero  = [0.00946,0.10709,0.44868,1.38037,3.17062,6.01178,10.35129]#,24.42437,47.46733]
O_one   = [0.00953,0.04386,0.20040,0.58402,1.28309,2.45980,4.81897]#,11.91114,23.23211]
O_two   = [0.00889,0.04199,0.20223,0.58328,1.28859,2.46629,4.44303]#,11.94420,23.25869]
O_three = [0.00870,0.02268,0.16198,0.53158,1.15723,2.29396,4.29559]#,12.53149,24.02194]
O_fast  = [0.00845,0.02256,0.16421,0.52456,1.16242,2.29763,4.28558]#,12.72092,24.42916]
O_s     = [0.00866,0.03437,0.19800,0.56940,1.26963,2.45285,4.45872]#,11.97594,23.21310]
O_g     = [0.00834,0.04276,0.19930,0.67470,1.27856,2.44025,4.45774]#,12.01298,23.33818]

plt.plot(cube_size, O_zero, label="-O0")
plt.plot(cube_size, O_one, label="-O1")
# plt.plot(cube_size, O_two, label="-O2")
plt.plot(cube_size, O_three, label="-O3")
plt.plot(cube_size, O_fast, label="-Ofast")
# plt.plot(cube_size, O_s, label="-Os")
# plt.plot(cube_size, O_g, label="-Og")
plt.xlabel('Cube size [n]')
plt.ylabel('Solve time [s]')
plt.legend()
plt.grid(True)
plt.show()
