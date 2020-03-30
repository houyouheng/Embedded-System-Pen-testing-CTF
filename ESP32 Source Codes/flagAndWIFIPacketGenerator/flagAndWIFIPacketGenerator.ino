#include <string>
#include <iostream>
#include "WiFiConfig.h"
#include "Memory.h"
#include "Encryption.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include "mbedtls/aes.h"

char memory[MAX_MEMORY_SIZE][MAX_WORD_SIZE] = {
  { "" },
  { "" },
  { "" },
  { "" },
  { "" },
  { "" },
  { "" },
  { "" },
  { "" },
  { "" },
  { "" },
  { "" },
  { "" },
  { "" },
  { "" },
  { "" },
  { "" },
  { "" },
  { "" },
  { "" },
};

mbedtls_aes_context aes;
char encryptionKey[16];
boolean tempOutOFRange = false;

void setup() {
  pinMode (5, OUTPUT);
  Serial.begin(9600);

  Serial.println("\n-------Debug Info-----------");
  ConnectToWiFi();
  printWifiStatus();
  GenerateFakeTempDatas();
  debugAndTest();
  printMemory();
  Serial.println("------------------------------");

  delay(1000);

  xTaskCreate(
    taskOne,          /* Task function. */
    "TaskOne",        /* Strinwith name of task. */
    10000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    1,                /* Priority of the task. */
    NULL);
  xTaskCreate(
    taskTwo,          /* Task function. */
    "TaskTwo",        /* String with name of task. */
    10000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    1,                /* Priority of the task. */
    NULL);
}

void loop() {
  delay(1000);
  DetectCorruptTemp();
  if (tempOutOFRange == true) {
    blinkLed();
  }
}

void debugAndTest() {
  //-------------------------------------
  Serial.println("---------Testing------------");

  char* testString = "Hello World xxxx";
  unsigned char encryptBuffer[16];
  unsigned char decryptBuffer[16];
  unsigned char testDecryptBuffer[16];
  encrypt(testString, ENCRYPTION_KEY, encryptBuffer);
  Serial.print("Encrypted is: ");
  Serial.println((char*)encryptBuffer);

  Serial.print("Encrypted in Hex is:");
  for (int i = 0; i < strlen((char*)encryptBuffer); i++) {

    char str[3];
    sprintf(str, "%02x", (int)encryptBuffer[i]);
    Serial.print(str);
  }
  Serial.println("");

  char* testInput = "04b07222d6755ff18c705d2eafc3e510";
  unsigned char testInputInChar[16];
  hexToChar(testInput, (char*)testInputInChar);
  Serial.print("testInputInChar: ");
  for (int i = 0; i < 16; i++) {
    Serial.print((char)testInputInChar[i]);
  }
  Serial.println("");

  decrypt(testInputInChar, ENCRYPTION_KEY, testDecryptBuffer);
  Serial.print("Decrypted Test is: ");
  for (int i = 0; i < 16; i++) {
    Serial.print((char)testDecryptBuffer[i]);
  }
  Serial.println("");


  decrypt(encryptBuffer, encryptionKey, decryptBuffer);
  Serial.print("Deciphered good text is: ");
  for (int i = 0; i < 16; i++) {
    Serial.print((char)decryptBuffer[i]);
  }
  Serial.println("");

  Serial.println("---------Testing Done------------");
  //-------------------------------------
}

void GenerateFakeTempDatas() {
  for (int i = 0; i < MAX_MEMORY_SIZE; i++) {
    int randTemp = random(20, 30);
    sprintf(memory[i], "%d", randTemp);

  }
}

void DetectCorruptTemp() {
  for (int i = 0; i < MAX_MEMORY_SIZE; i++) {
    if (atoi(memory[i]) > 30) {
      tempOutOFRange = true;
    }
  }
}

void blinkLed() {
  digitalWrite (5, HIGH);
  delay(500);
  digitalWrite (5, LOW);
}

void ConnectToWiFi() {

  //  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());
  WiFi.begin(SSID, WiFiPassword);
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(SSID);
  Serial.print("Password: ");
  Serial.println(WiFiPassword);

  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);

    if ((++i % 16) == 0)
    {
      Serial.println(F(" still trying to connect"));
    }
  }
}

void printMemory() {
  for (int i = 0; i < MAX_MEMORY_SIZE; i++) {
    char newSendString[298];
    sprintf(newSendString, "%d --------------- %s", i, memory[i]);
    Serial.println(newSendString);
  }
}

void sendReadResponse(char* message, IPAddress serverIP, int port, int readLength) {
  WiFiUDP Udp;
  Udp.begin(8000);

  //  char address_buffer[3];
  //  address_buffer[0] = message[0];
  //  address_buffer[1] = message[1];
  //  address_buffer[2] = '\0';
  //  int address = atoi(address_buffer);

  int address = (message[0] - '0') * 10 + (message[1] - '0');

  char readResponseMessage[MAX_WORD_SIZE] = "";
  for (int i = 0; i < readLength; i++) {
    sprintf(readResponseMessage, "%s%s", readResponseMessage , memory + address + i);
  }

  Udp.beginPacket(serverIP, port);
  Udp.write((uint8_t*)readResponseMessage, strlen(readResponseMessage));
  Udp.endPacket();
}

void sendWriteResponse(char* message, IPAddress serverIP, int port) {
  WiFiUDP Udp;
  Udp.begin(8000);

  char address_buffer[3];
  address_buffer[0] = message[0];
  address_buffer[1] = message[1];
  address_buffer[2] = '\0';
  int address = atoi(address_buffer);

  int dataLength = strlen(message) - 3;
  char data_buffer[dataLength];
  for (int i = 0; i < dataLength; i++) {
    data_buffer[i] = message[i + 3];
  }
  strcpy(memory[address], data_buffer);

  Udp.beginPacket(serverIP, port);
  char ackBuffer[50] = "Acknowledging Write";
  Udp.write((uint8_t*)ackBuffer, strlen(ackBuffer));
  Udp.endPacket();
}

void sendUnknownResponse(char* message, IPAddress serverIP, int port) {
  WiFiUDP Udp;
  Udp.begin(8000);
  Udp.beginPacket(serverIP, port);
  char ackBuffer[50] = "Unknown Command";
  Udp.write((uint8_t*)ackBuffer, strlen(ackBuffer));
  Udp.endPacket();
}

void sendHelloWorld(IPAddress serverIP, int port) {
  WiFiUDP Udp;
  Udp.begin(8000);

  Udp.beginPacket(serverIP, port);
  char testBuffer[50] = "Hello World";
  Udp.write((uint8_t*)testBuffer, strlen(testBuffer));
  Udp.endPacket();
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void taskTwo( void * parameter ) {
  WiFiUDP Udp;
  Udp.begin(8000);
  while (true) {
    int packetSize = Udp.parsePacket();

    if (Udp.available()) {
      char readBuffer[packetSize] = "";
      char packetBuffer[32] = "";

      Udp.read(readBuffer, packetSize);
      //            Serial.print("readBuffer: ");
      //            Serial.print(readBuffer);

      unsigned char hexToCharBuffer[16];
      hexToChar(readBuffer, (char*)hexToCharBuffer);
      //            Serial.print("hexToCharBuffer: ");
      //            for (int i = 0; i < 16; i++) {
      //              Serial.print((char)hexToCharBuffer[i]);
      //            }
      //            Serial.println("");

      decrypt(hexToCharBuffer, encryptionKey, (unsigned char*)packetBuffer);
                  Serial.print("Decrypted is: ");
                  for (int i = 0; i < 16; i++) {
                    Serial.print((char)packetBuffer[i]);
                  }
                  Serial.println("");

      int readOrWrite = packetBuffer[2] - '0';

      //      Serial.print("readOrWrite: ");
      //      Serial.println(readOrWrite);

      switch (readOrWrite) {
        case READ_REQUEST: {
            int readLength = packetBuffer[3] - '0';
            int address = (packetBuffer[0] - '0') * 10 + (packetBuffer[1] - '0');
            if (address + readLength < 0 || address + readLength > 19) {
              sendUnknownResponse(packetBuffer, Udp.remoteIP(), Udp.remotePort());
            } else {
              sendReadResponse(packetBuffer, Udp.remoteIP(), Udp.remotePort(), readLength);
            }
          }
          break;
        case WRITE_REQUEST: {
            sendWriteResponse(packetBuffer, Udp.remoteIP(), Udp.remotePort());
          }
          break;
        default: {
            sendUnknownResponse(packetBuffer, Udp.remoteIP(), Udp.remotePort());
          }
          break;
      }
    }
  }
  vTaskDelete( NULL );
}

void encrypt(const char* message, const char* key, unsigned char * outputBuffer) {
  //--------------------XOR Encrypt--------------------------------------
  //  int string_size = strlen(message);
  //  char * output = new char[string_size + 1];
  //  strcpy(output, message);
  //  for (int i = 0; i < string_size; i++)
  //    output[i] = message[i] ^ key[i % (sizeof(key) / sizeof(char))];
  //  return output;
  //--------------------XOR Encrypt--------------------------------------

  //-----------------------AES-128bits-----------------------------------
  //  int string_size = strlen(message);
  //  Serial.println(string_size);
  //  unsigned char * output = new unsigned char[string_size + 1];
  mbedtls_aes_init( &aes );
  mbedtls_aes_setkey_enc( &aes, (const unsigned char*)key, strlen(key) * 8 );
  mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, (const unsigned char*)message, outputBuffer);
  mbedtls_aes_free( &aes );
  //-----------------------AES-128bits-----------------------------------
}

void decrypt(unsigned char* message, const char* key, unsigned char * outputBuffer) {

  //-----------------------AES-128bits-----------------------------------

  //  int string_size = strlen(message);
  //  unsigned char * output = new unsigned char[string_size + 1];
  mbedtls_aes_init( &aes );
  mbedtls_aes_setkey_enc( &aes, (const unsigned char*)key, strlen(key) * 8 );
  //  Serial.println("Test???");
  mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, (const unsigned char*)message, outputBuffer);
  mbedtls_aes_free( &aes );
  //-----------------------AES-128bits-----------------------------------
}

void hexToChar(char* hex, char* output) {

  byte hexLength = strlen(hex);

  char returnMessage[hexLength / 2];

  byte i = 0;
  for (i = 0; i < hexLength; i += 2) {
    returnMessage[i / 2] = getByteAt(hex, i);
  }

  memcpy(output, returnMessage, sizeof(returnMessage));
}

char getByteAt(char* hex, byte i) {
  return getHexVal(hex[i]) * 16 + getHexVal(hex[i + 1]);
}

byte getHexVal(char cx) {
  if (cx <= '0') {
    return 0;
  }
  if (cx <= '9') {
    return cx - '0';
  }
  if (cx >= 'a' && cx <= 'f') {
    return cx - 'a' + 10;
  }
  if (cx >= 'A' && cx <= 'F') {
    return cx - 'A' + 10;
  }
  return 0;
}

void taskOne( void * parameter ) {
  std::string stringToSend = "";
  char* headerToSend = "ctf{";

  int checksum = 0;

  for (int i = 4; i < 20; i++) {
    int randomNum = random(0, 26);
    char c = 'a' + (char)randomNum;
    checksum += 97 + randomNum;
    stringToSend += c;
  }

//  stringToSend += "      ";

  memcpy(encryptionKey, stringToSend.c_str(), sizeof(encryptionKey));
//  Serial.print("encryptionKey is: ");
//  Serial.println(encryptionKey);

  unsigned char stringToSend1[16];
  encrypt(stringToSend.c_str(), ENCRYPTION_KEY, stringToSend1);

  char stringToSend1Hex[32] = "";

  for (int i = 0; i < 16; i++) {
    sprintf(stringToSend1Hex, "%s%02x", stringToSend1Hex, (int)stringToSend1[i]);
  }

  while (true) {
    Serial.print(headerToSend);
    Serial.print(stringToSend1Hex);
    Serial.print("-");
    Serial.print(checksum);
    Serial.println("}");

    delay(2000);
  }

  vTaskDelete( NULL );
}
