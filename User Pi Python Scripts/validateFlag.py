import sys
import re

if(len(sys.argv) > 2):
        print "Parameter Error: too many parameters"
        sys.exit()
elif(len(sys.argv) < 2):
        print "Parameter Error: no flag parameter"
        sys.exit()

pattern = 'ctf{[a-zA-Z]{16}-\d{4}}'
flag = sys.argv[1]

regexTest = re.match(pattern, flag)

if(regexTest == None):
        print "Paramter Error: flag format is incorrect"
        exit()

flagLength = len(flag)
seq = flag[4:flagLength-1]
count = 0
for k in range(0, 16):
        count = count + ord(seq[k])
seqLength = len(seq)
final = int(seq[17:seqLength])
if(final == count):
        print "Flag:",flag,"is valid"
else:
        print "Flag:",flag,"is not valid"
