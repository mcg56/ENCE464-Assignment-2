from matplotlib import pyplot as plt
import numpy as np

labels = ["K,J,I","K,I,J",'I,J,K']
time = [0.10170, 0.21761, 0.32129]

d1_refs = [7915785, 13472335, 14153652]
d1_cache_misses = [988508, 2188738, 4696034]
d1_cache_misses_percent = [12.5, 16.2, 33.2]


ll_refs = [990059, 2190261, 4679556]
ll_cache_misses = [75021, 74999, 74998]
ll_cache_misses_percent = [0.3, 0.2, 0.2]

per = [12.5,16.2,33.2,0.2,0.3,0.2]
x = np.arange(len(labels))
width = 0.45

fig, ax = plt.subplots()

rects1 = ax.bar(x - 3*width/4, d1_refs, width/2, label='D1 cache references')
rects2 = ax.bar(x - width/4, d1_cache_misses, width/2, label='D1 cache misses (%)')
rects3 = ax.bar(x + width/4, ll_refs, width/2, label='LL cache references')
rects4 = ax.bar(x + 3*width/4, ll_cache_misses, width/2, label='LL cache misses (%)')



ax.set_ylabel('Count')
ax.set_xlabel('Loop order')
ax.set_xticks(x, labels)
ax.legend()

# ax.bar_label(rects1, padding=3)
# ax.bar_label(rects2, padding=3)
# ax.bar_label(rects3, padding=3)
# ax.bar_label(rects4, padding=3)

iter_array = [0,1,2,6,7,8]
for i in iter_array:
    bar = ax.patches[i]
    value = bar.get_height()
    text = f'{value}'
    text_x = bar.get_x() + bar.get_width() / 2
    text_y = bar.get_y() + value
    ax.text(text_x, text_y+100000, text, ha='center',size=9)

iter_array_1 = [3,4,5,9,10,11]
j = 0
for i in iter_array_1:
    bar = ax.patches[i]
    value = bar.get_height()
    perc = per[j]
    text = f'({perc}%)\n {value}'
    text_x = bar.get_x() + bar.get_width() / 2
    text_y = bar.get_y() + value
    ax.text(text_x, text_y+100000, text, ha='center',size=9)
    j+=1

# for bar in ax.patches:
#     if (i == 5):
#         value = bar.get_height()
#         text = f'{value}'
#         text_x = bar.get_x() + bar.get_width() / 2
#         text_y = bar.get_y() + value
#         ax.text(text_x, text_y+100000, text, ha='center',size=9)
#     i+=1
fig.tight_layout()

plt.show()