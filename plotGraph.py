import matplotlib.pyplot as plt
import csv

x = []
y = []

with open('durations.txt','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=' ')
    for row in plots:
        x.append(int(row[1]))
        y.append(int(row[0]))

plt.plot(x,y)
plt.xlabel('threads')
plt.ylabel('duration')
plt.title('Runtime duration as a function of number of threads')
plt.show()
