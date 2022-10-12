# MK3 100 iterations 3 threads
import numpy as np
from matplotlib import pyplot as plt


num_threads = np.array([      1,      2,      3,      4,      5,      6,      7,      8,      9,     10,     11,     12])
Cube_51     = np.array([0.02319,0.02583,0.02489,0.02465,0.02568,0.02829,0.03031,0.03192,0.03353,0.03738,0.04634,0.04091])
Cube_101    = np.array([0.15828,0.15647,0.14328,0.13966,0.15086,0.14592,0.16437,0.14481,0.15613,0.14592,0.15300,0.15926])
Cube_301    = np.array([4.31497,3.30734,2.81639,2.70854,2.73161,2.85129,2.75831,2.75641,2.87974,2.78168,2.85953,2.89280])
Cube_501    = np.array([23.43954,13.61450,12.74339,12.61357,12.59366,12.56477,12.84383,13.40944,12.93605,13.19831,13.32984,13.37241])

plt.plot(num_threads, Cube_51/min(Cube_51), label="51")
plt.plot(num_threads, Cube_101/min(Cube_101), label="101")
plt.plot(num_threads, Cube_301/min(Cube_301), label="301")
plt.plot(num_threads, Cube_501/min(Cube_501), label="501")

plt.xlabel('Cube size [n]')
plt.ylabel('Solve time [s]')
plt.legend()
plt.grid(True)
plt.show()
