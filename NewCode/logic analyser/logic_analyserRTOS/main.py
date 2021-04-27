
"""
values = [0, 0, 0, 0, 0, 0]
inactive = 0
last_values = "111111111111111"
file = open("test.csv")
switch = 0
for line in file:
    data = line.strip().split(',')
5000000
    time = int(data[0])-values[0]
    new_values = data[1]
    if new_values == 0:
        switch = 1



    values[0] = int(data[0])
    last_values = new_values
file.close()

time_activity = []
for i in range(1, len(values)):
    values[i] = (values[i]/values[0])*100
values = values[1:]
active = values[0]+values[1]
print(active)"""
from matplotlib import pyplot

file = open("1_2_3_m.txt", "r")
values = []
for line in file:
    values.append(float(line.strip("\n")))
val0 = values[0::3]
val1 = values[1::3]
val2 = values[2::3]

pyplot.axis([0, len(values)/3, 0, 600])
pyplot.hlines(100, 0, len(values), colors="red")
pyplot.hlines(200, 0, len(values), colors="red")
pyplot.hlines(300, 0, len(values), colors="red")
pyplot.hlines(400, 0, len(values), colors="red")
pyplot.plot(val0, color="blue")
pyplot.plot(val1, color="green")
pyplot.plot(val2, color="black")
pyplot.show()
file.close()
