#define UDP_TX_PACKET_MAX_SIZE 32
#define READ_REQUEST 0
#define WRITE_REQUEST 1

/*Different WiFi SSIDs used for debugging purposes*/
//const char *SSID = "RHIT-OPEN";
const char *SSID = "MyPi";
//const char *SSID = "Zhou Tesla 2.4";

/*Password field for the wifi*/
const char *WiFiPassword = "";
//const char *WiFiPassword = "ppxia4396";


IPAddress local_ip(192, 168 ,42 ,22 );
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
const int BUFFERSIZE = 128;
