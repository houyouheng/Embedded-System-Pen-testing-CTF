from Crypto.Cipher import AES

key = ""

while(len(key) != 16):
    key = raw_input("Enter your Encrypted Key: ") 
    if(len(key) == 16):
        break
    print("Error! Check your Encrypted Key length")


data = raw_input("Enter your Encrypted Data: ") 
if(len(data) != 32):
    print("Error! Check your Encrypted Data length")
    exit()

# key = 'B7D4A5A5DB721CB5'
 
cipher = AES.new(key, AES.MODE_ECB)
msg =cipher.decrypt(data.decode('hex'))

print("Your decrypted data is: " + msg.decode("UTF-8"))
