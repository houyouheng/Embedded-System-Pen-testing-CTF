import string
import random
file = open("../key/encryptionKey.txt","w")

flag = "ctf{"
value = 0
for i in range(16):
        newChar = random.choice(string.ascii_letters)
        flag = flag + newChar
        value = value + ord(newChar)
flag = flag + "-" + str(value) + "}"

print("key generated.")
file.write(flag)
file.close()
