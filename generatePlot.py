import numpy as np
import pandas as pd
from matplotlib import pyplot as plt

df = pd.read_csv("./latencyLog2.csv")
df["log2array"] = np.log2(df["size"])

c = ['b', 'g', 'r', 'c', 'm', 'y', 'k', 'brown', 'darkslategray', 'navy', 'plum', 'pink']

dfx = df[df['cores'] == 1]
dfx = dfx[dfx['Sequential'] == 0]
dfx = dfx[dfx['modify'] == 0]
ax = dfx.plot(x='log2array', y='latency', c='r', title='Random Read Latency')
#plt.show()
for i in range(2,9):
	dfx = df[df['cores'] == i]
	dfx = dfx[dfx['Sequential'] == 0]
	dfx = dfx[dfx['modify'] == 0]

	dfx.plot(x='log2array', y='latency', c=c[i-1], ax=ax)

ax.legend([str(i) for i in range(1,12)], title='Thread Count')

dfx = df[df['cores'] == 1]
dfx = dfx[dfx['Sequential'] == 1]
dfx = dfx[dfx['modify'] == 0]
ax1 = dfx.plot(x='log2array', y='latency', c='r', title='Sequential Read Latency')
for i in range(2,9):
	dfx = df[df['cores'] == i]
	dfx = dfx[dfx['Sequential'] == 1]
	dfx = dfx[dfx['modify'] == 0]

	dfx.plot(x='log2array', y='latency', c=c[i-1], ax=ax1)
ax1.legend([str(i) for i in range(1,16)], title='Thread Count')
#plt.show()

dfx = df[df['cores'] == 1]
dfx = dfx[dfx['Sequential'] == 0]
dfx = dfx[dfx['modify'] == 1]
ax2 = dfx.plot(x='log2array', y='latency', c='r', title='Random Read/Write Latency')
for i in range(2,9):
	dfx = df[df['cores'] == i]
	dfx = dfx[dfx['Sequential'] == 0]
	dfx = dfx[dfx['modify'] == 1]

	dfx.plot(x='log2array', y='latency', c=c[i-1], ax=ax2)
ax2.legend([str(i) for i in range(1,13)], title='Thread Count')
#plt.show()

dfx = df[df['cores'] == 1]
dfx = dfx[dfx['Sequential'] == 1]
dfx = dfx[dfx['modify'] == 1]
ax3 = dfx.plot(x='log2array', y='latency', c='r', title='Sequential Read/Write Latency')
for i in range(2,9):
	dfx = df[df['cores'] == i]
	dfx = dfx[dfx['Sequential'] == 1]
	dfx = dfx[dfx['modify'] == 1]

	dfx.plot(x='log2array', y='latency', c=c[i-1], ax=ax3)
ax3.legend([str(i) for i in range(1,13)], title='Thread Count')

plt.show()
