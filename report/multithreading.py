# MK3 100 iterations 3 threads

from matplotlib import pyplot as plt


cube_size                = [      7,     51,    101,    151,    201,    251,    301]
O_nothreading            = [0.00410,0.02610,0.22000,0.60329,1.39033,2.40635,4.24304]
O_threading_every_time   = [0.02372,0.03965,0.15846,0.43068,0.97252,1.88077,3.53645]
O_threading_once         = [0.01027,0.02547,0.14841,0.43874,0.91584,1.77697,3.12364]


plt.plot(cube_size, O_nothreading, label="No threading")
plt.plot(cube_size, O_threading_every_time, label="Threading")
plt.plot(cube_size, O_threading_once, label="Threading with sync")

plt.xlabel('Cube size [n]')
plt.ylabel('Solve time [s]')
plt.legend()
plt.grid(True)
plt.show()
