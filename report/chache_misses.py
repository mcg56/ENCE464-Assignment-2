from matplotlib import pyplot as plt
import numpy as np

labels = ["i,j,k","j,i,k",'i,k,j']
misses = [100,120,200]
data = [20,30,40]

x = np.arange(len(labels))
width = 0.35 

fig, ax = plt.subplots()
rects1 = ax.bar(x - width/2, misses, width, label='Misses')
rects2 = ax.bar(x + width/2, data, width, label='Data')

ax.set_ylabel('Count')
ax.set_xlabel('Loop Cofiguration')
ax.set_xticks(x, labels)
ax.legend()

ax.bar_label(rects1, padding=3)
ax.bar_label(rects2, padding=3)

fig.tight_layout()

plt.show()