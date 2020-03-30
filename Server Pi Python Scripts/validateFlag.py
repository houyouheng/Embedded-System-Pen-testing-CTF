import sys

flag = sys.argv[1]
flagLength = len(flag)
seq = flag[4:flagLength-1]
count = 0
for k in range(0, 10):
	count = count + ord(seq[k])
seqLength = len(seq)
final = int(seq[11:seqLength])
if(final == count):
	print "Flag:",flag,"is valid"
else:
	print "Flag:",flag,"is not valid"
	