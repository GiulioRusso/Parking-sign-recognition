import csv
import numpy as np
import matplotlib.pyplot as plt
import os

filename = "/Users/giuliorusso/Documents/Istruzione/Università/Magistrale/Advanced Image Analysis/Progetto/parking-performance/label-scores.csv"

precision = []
recall = []
lines = []

TP_i = 0
FP_i = 0
precision_i = 0.0
recall_i = 0.0
# total_signs = 160 if not skip occluded and oof, or 145 if skip occluded and oof
total_signs = 160

with open(filename) as csv_file:
	csv_reader = csv.reader(csv_file, delimiter=',')
	for row in csv_reader:
		lines.append(np.array([int(row[0]), float(row[1])]))

lines = np.array(lines)
lines = np.flip(lines[np.argsort(lines[:, 1])])

# print("lines.shape =", lines.shape)
# print("lines: ", lines)
# print("lines.shape[0]: ", lines.shape[0])

num_lines = lines.shape[0]

for i in range(num_lines):

	# print("\n\n LINE ", i, ": ", lines[i])
	# print("is_TP: ", lines[i][1])
	# print("score: ", lines[i][0])

	if lines[i][1] == 1:
		TP_i += 1
	else:
		FP_i += 1

	# print(lines[i][1])
	# print("TP_i:", TP_i)
	# print("FP_i:", FP_i)

	precision_i = TP_i / (TP_i+FP_i)
	recall_i = TP_i / total_signs

	# print("Precision ", i, ": ", precision_i)
	# print("Recall ", i, ": ", recall_i)

	precision.append(precision_i)
	recall.append(recall_i)

# print("precision list: ", precision)
# print("recall list: ", recall)

fig, ax = plt.subplots()
ax.xaxis.set_ticks(np.arange(0.0, 1.01, 0.1))
ax.yaxis.set_ticks(np.arange(0.0, 1.01, 0.1))
ax.set(xlabel='Recall', ylabel='Precision', xlim=(0.0, 1.01), ylim=(0.0, 1.01))
ax.fill_between(np.array(recall), np.array(precision), 0, facecolor="orange")

AUPR = np.trapz(np.array(precision), np.array(recall)) * 100

mylabel = "Precision-Recall, AUPR = "+ str(AUPR.__round__(2)) + "%"
ax.plot(recall, precision, "r", label=mylabel)
fig.legend(loc="upper center")
fig.savefig(os.path.join("/Users/giuliorusso/Documents/Istruzione/Università/Magistrale/Advanced Image Analysis/Progetto/parking-performance/", "precision-recall.png"))

print("AUPR = ", AUPR)