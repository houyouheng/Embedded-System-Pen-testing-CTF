from Crypto.Cipher import AES
key = ""
while(len(key) != 16):
    key = raw_input("Enter your Key: ") 
    if(len(key) == 16):
        break
    print("Error! Check your Key length")


data = ""
while(len(data) != 16):
    data = raw_input("Enter your data: ") 
    if(len(data) == 16):
        break
    print("Error! Check your data length")


# key = 'B7D4A5A5DB721CB5'
 
cipher = AES.new(key, AES.MODE_ECB)
msg =cipher.encrypt(data)

print("Your encrypted data is: " + msg.encode("hex"))
