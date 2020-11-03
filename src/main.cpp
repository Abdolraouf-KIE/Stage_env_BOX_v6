//jyhy
// Status:      Fully operational 
// Version:     v2.2 (change of phone number is possible) 
// commment:     to add 
//                  a)Change of phone number
//                  b)Change of mode of operation
// String("{") + String(ID)+ String(": [{\"ts\": ") +String(intMQTTdateTime)+ String(", \"values\": {\"TNB\":") + String(TNB) + String(", \"ELCB\":") + String(ELCB) +String(",\"Colour\":\"Red\",\"Light\":0, \"AmberFlashing\":1}}]}")
// 
// 
// 
// 
#include <Arduino.h>
// #include <TimeLib.h>
#define ESP32_RTOS  // Uncomment this line if you want to use the code with freertos (only works on the ESP32)
#define lightseq
byte checkAmberStatus2();
void checkPinStatus();
unsigned long entry;
bool flashingStatus=0;
//for getting epochtime
const char* UnixTime_topic= "datetimeonly";
String MQTTdateTime="";
unsigned long int intMQTTdateTime=0;
unsigned long Zeromillis=0;
/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-sim800l-send-text-messages-sms/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

/***************HTTP ID******************/
String ID;
String SMS_TARGET =String("+60183855039");
String ReportMode= "SMS"; //determines whether use SMS or MQTT. 1 means MQTT.
#include <EEPROM.h>
#include <WiFi.h>
// #include <ESP8266WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <EEPROMRead&Write.h> //see header in src. Has two dunctions to read and write from EEPROM
#include <DYNAWebpage.h>
/*********************************************************************/

//dont forget uncomment this 
// #define SMS
#define MQTT

// Your phone number to send SMS: + (plus sign) and country code, for Portugal +351, followed by phone number
// SMS_TARGET Example for Portugal +351XXXXXXXXX
//#define SMS_TARGET  "+60109243524"
// String SMS_TARGET =String("+60183855039");

#define MASA_TUNGGU 720 //tunggu kalau tkde change utk GREEN DAN RED.. report fault
#define Yellow_Time 720 //time in seconds after multiplied by 5 so 720 means 3600s.
String last_ioRead="";
//############################# JGN UBAH LEPAS LINE NI ###########################################################
#define USERMQTT "2sa34dd5" // Put your Username
#define PASSMQTT "2sa34dd5" // Put your Password
#define MQTT_CLIENT_NAME "" // MQTT client Name, please enter your own 8-12 alphanumeric character ASCII string; 
char mqttBroker[]  = "broker.react.net.my";

char payload[100];
// Saving the parameters inside AllAlarms
String AllAlarms;

String last_AllAlarms=String("");
// char topic[150];

String topic= "v1/gateway/sms";
String debugTopic= "v1/gateway/smsserial" ;

String Heartbeat;
unsigned long previousMillis=0;
unsigned long currentMillis;
unsigned long interval=600000; //interval for heartbeat

// SIM card PIN (leave empty, if not defined)
const char simPIN[]   = "";
// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb

#include <Wire.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
// #include <Time.h>

// TTGO T-Call pins
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22

//Digital IO Pin Sensing
#define Input1               15 //change from 12 to 15
#define Input2               33 //change from 13 to 33
#define Input3               12 //change from 14 to 12
#define Input4               13 //change from 15 to 13
#define Input5               14// change from 33 to 14

//status byte
#define   RED_OK              0x00
#define   RED_FAULTY          0x01  
#define   GREEN_OK            0x02 
#define   GREEN_FAULTY        0x03 
#define   AMBER_OK            0x04 
#define   AMBER_FAULTY        0x05
#define   LIGHT_OK            0x06 
#define   LIGHT_FAULTY        0x07

int A1State = 0;         // current state of the button
int A2State = 0;         // current state of the button
int A3State = 0;         // current state of the button
int A4State = 0;         // current state of the button
int A5State = 0;         // current state of the button

int lastA1State = 1;     // previous state of the button
int lastA2State = 1;     // previous state of the button
//int lastA3State = 1;     // previous state of the button
//int lastA4State = 1;     // previous state of the button
//int lastA5State = 1;     // previous state of the button


bool blink_flag =false;
bool fault_flag = false;
int count =0;

bool sysStateOK =false;
bool lastSysStateOK = false;


// Set serial for debug console (to Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to SIM800 module)
#define SerialAT  Serial1

/****************************************
 * Setting SIM network configuration (U Mobile/maxis/Diji).
 ****************************************/
// Range to attempt to autobaud
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

// Your GPRS credentials, if any
const char apn[] = "my3g";  //must test one of the three given sets of settings by maxis: https://apn.how/malaysia/maxis
const char gprsUser[] = "vodafone";
const char gprsPass[] = "";

//topic assginmment was removed here as it was not being recognized as an assignemnt
//instead it thought it was a decleration.

/****************************************/

// Define the serial console for debug prints, if needed
//#define DUMP_AT_COMMANDS

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00

bool setPowerBoostKeepOn(int en){
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_SYS_CTL0);
  if (en) {
    Wire.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
  } else {
    Wire.write(0x35); // 0x37 is default reg value
  }
  return Wire.endTransmission() == 0;
}

/****************************************
 * Auxiliar Functions
 ****************************************/

// TinyGsm modem(SerialAT);
TinyGsmClientSecure ubidots(modem);
PubSubClient client(ubidots);

// void callback(char* topic, byte* payload, unsigned int length) {
//   char p[length + 1];
//   memcpy(p, payload, length);
//   p[length] = NULL;
//   String message(p);
//   Serial.write(payload, length);
//   Serial.println(topic);
// }

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // sendMQTT(String("Attempting MQTT connection..."),debugTopic);
    
    // Attemp to connect
    if (client.connect(MQTT_CLIENT_NAME, USERMQTT, PASSMQTT)) {
      Serial.println("Connected");
      // sendMQTT(String("Connected"),debugTopic);
      // client.subscribe(ID_topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 0.5 seconds");
      // Wait 2 seconds before retrying
      delay(500);
    }
  }
}

//send MQTT function
void sendMQTT(String MQTTMessage, String MQTTtopic){
  char MQTTpayload[500];
  char arraytopic[300];

  //Making the syntax for json as per Ameen's equest:
  // MQTTMessage= String("{\"") + String(ID) + String("\":[{\"values\":{\"") +String(MQTTMessage) +String("\": ") + String(state) + String("}}]}");
  /*no need as will do this befoe hand*/

  // converting Strings to char array
      // sprintf(MQTTpayload, "%s", MQTTMessage); // This doesnt work as we have String class
      // sprintf(arraytopic, "%s", MQTTtopic);
    MQTTMessage.toCharArray(MQTTpayload, 500);
    MQTTtopic.toCharArray(arraytopic, 300);

  if(client.publish(arraytopic, MQTTpayload)){
    SerialMon.print("\nSENDMQTT: MQTT Topic: ");
    SerialMon.println(arraytopic);
    SerialMon.print("          MQTT Message: ");
    SerialMon.println(MQTTpayload);
  }
  else{
    SerialMon.println("INFO: MQTT failed to send");
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  // changing to String
  char p[len + 1];
  memcpy(p, payload, len);
  p[len] = NULL;
  String message(p);
  SerialMon.println(String("###MessageString= ") + String(message));
  MQTTdateTime=message;
}
/****************************************/

void setup() {
  SerialMon.begin(115200);
  // Set console baud rate
  // SerialMon.begin(115200);
  // Begin and read from EEPROM the ID of device
  SerialMon.println("\nInput values for below signals (0 means there is signal):");
  SerialMon.println("TNB|ELCB|RED|YELLOW|GREEN");
  EEPROM.begin(512);
  if (EEPROM.read(10) == 0xFF)
  {
    ID=String ("No Name");
    SerialMon.println(ID);
  }else
  {
    //reading the ID name from EEPROM starting from adress 10 till '\0'
    ID= read_String(10);
    SerialMon.println(ID);
  }

//EEPROM for ReportMode
  if (EEPROM.read(150) == 0xFF)
  {
    ReportMode=String ("SMS");
    SerialMon.println(ReportMode);
  }else
  {
    //reading the ID name from EEPROM starting from adress 150 till '\0'
    ReportMode= read_String(150);
    SerialMon.println(ReportMode);
  }

  // eeprom for Phone    
  EEPROM.begin(512);
  if (EEPROM.read(100) == 0xFF)
  {
    SMS_TARGET=String ("No Phone");
    SerialMon.println(SMS_TARGET);
  }else
  {
    //reading the ID name from EEPROM starting from adress 100 till '\0'
    SMS_TARGET= read_String(100);
    SerialMon.println(SMS_TARGET);
  }

  /****************************************
 *  Setting up the WIFI AP and handeling the change of ID from webpage
 ****************************************/
  
  // WiFi.mode(WIFI_AP);
  WiFi.softAP(wifiName, wifiPass);              
  // WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  // WiFi.softAP(wifiName, wifiPass);
 SerialMon.print("Connected to ");
 SerialMon.println(String(wifiName) + String("   PASS: ") + String(wifiPass));
 SerialMon.print("Soft-AP IP address = ");
  SerialMon.println(WiFi.softAPIP());

 // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

 // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputMessage2;
    String inputMessage3;

    String inputParam;
    String inputParam2;
    String inputParam3;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage> and that of input 2
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1; 
  //assigning the new ID into the EEPROM at address 10 (ID) if the vars are different
      if (inputMessage != ID)
      {
        ID=inputMessage;
        SerialMon.print("ID set to: ");
        SerialMon.println(ID);
        writeString(10, inputMessage);
      }
     }//end of param 1

     if (request->hasParam(PARAM_INPUT_2)){
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam2 = PARAM_INPUT_2;
      // and 100 (SMS_TARGET) 
      if (inputMessage2 != SMS_TARGET)
      {
        SMS_TARGET=inputMessage2;
        SerialMon.print("SMS_TARGET set to: ");
        SerialMon.println(SMS_TARGET);
        writeString(100, SMS_TARGET);
      }
     }//end of PARAM2
     if (request->hasParam(PARAM_INPUT_3)){
      inputMessage3 = request->getParam(PARAM_INPUT_3)->value();
      inputParam3 = PARAM_INPUT_3;
      // address 200 (ReportMode) 
      if (inputMessage3 != ReportMode)
      {
        ReportMode=inputMessage3;
        SerialMon.print("ReportMode set to: ");
        SerialMon.println(ReportMode);
        writeString(150, ReportMode);
      }
     }/*end of param 3*/else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
      request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + ID + " and for input field SMS_TARGET with value " + SMS_TARGET + "ReportMode=" + ReportMode +
                                     "\n ESP is restarted to allow the changes to happen... Wait a bit then go back to homepage." +
                                     "<br><a href=\"/\">Return to Home Page</a>");

       ESP.restart();
  // //assigning the new ID into the EEPROM at address 10
  // writeString(10, inputMessage);
  });

  server.onNotFound(notFound);
  server.begin();
  /****************************************
 *  WIFI AP and webpage DONE
 ****************************************/

  // Keep power when running from battery
  Wire.begin(I2C_SDA, I2C_SCL);
  bool isOk = setPowerBoostKeepOn(1);
  SerialMon.println(String("INFO: IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));

  // Set modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  pinMode(Input1, INPUT_PULLUP);
  pinMode(Input2, INPUT_PULLUP);
  pinMode(Input3, INPUT_PULLUP);
  pinMode(Input4, INPUT_PULLUP);
  pinMode(Input5, INPUT_PULLUP);
  
  // Set GSM module baud rate and UART pins
  SerialAT.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  // Restart SIM800 module, it takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("INFO: Init Modem...");
  modem.restart();
  // modem.init();
  // use modem.init() if you don't need the complete restart

  // Unlock your SIM card with a PIN if needed
  if (strlen(simPIN) && modem.getSimStatus() != 3 ) {
    modem.simUnlock(simPIN);
  }

  delay(10000); //delay 10s
  if(ReportMode==String("SMS")){
    SerialMon.println("INFO: Try to Send SMS");
    // To send an SMS, call modem.sendSMS(SMS_TARGET, smsMessage)
    String smsMessage = String(ID) + "\nHELLO WORLD!";
    if(modem.sendSMS(SMS_TARGET, smsMessage)){
      SerialMon.print("SENDSMS: SMS Text: ");
      SerialMon.println(smsMessage);
    }
    else{
      SerialMon.println("INFO: SMS failed to send");
    }
  }//end of SMS_test
  /****************************************
 * GPM network setup
 ****************************************/
    if(ReportMode==String("MQTT")){
      SerialMon.println(" success");

      if (modem.isNetworkConnected()) {
      SerialMon.println("Network connected");
      }
      else{
        SerialMon.println("Network Not Connected");
      }
    
      SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork(600000L)) {
      // SerialMon.println(TinyGsmSim800::getRegistrationStatus());
      SerialMon.println(" fail");
      delay(10000);
      return;
    }
    
    SerialMon.println(" success");

      if (modem.isNetworkConnected()) {
      SerialMon.println("Network connected");
      }

    // GPRS connection parameters are usually set after network registration
      SerialMon.print(F("Connecting to "));
      SerialMon.print(apn);
      if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        SerialMon.println(" fail");
        delay(10000);
        return;
      }
      SerialMon.println(" success");

    if (modem.isGprsConnected()) {
      SerialMon.println("GPRS connected");
    }

    // MQTT Broker setup
    client.setBufferSize(400);
    Serial.print("client.getBufferSize() ");
    Serial.println(client.getBufferSize());
    client.setServer(mqttBroker, 8883);
    client.setCallback(mqttCallback); 

      // test MQTT upon setup and get unix time
    if (!client.connected()) {
    reconnect();
    }
    SerialMon.print("Attempting to connect subscribe: ");
    Serial.println(client.subscribe(UnixTime_topic));
    
    while(MQTTdateTime==""){
      client.loop();
    }
    Zeromillis=millis();
    client.unsubscribe(UnixTime_topic);
    delay(2000);
    Serial.print("MQTTdateTime: ");
    Serial.println(MQTTdateTime);

    //convert to int
    for(int i=0; i<10; i++)
    {
      intMQTTdateTime = intMQTTdateTime * 10 + ( MQTTdateTime[i] - '0' );
    }
    SerialMon.print("\nintMQTTdateTime: ");
    SerialMon.println(intMQTTdateTime);
    //Malaysian time needs to add 8 hours
    intMQTTdateTime = intMQTTdateTime + 28800;
    SerialMon.print("\nt_unix_date2:(MY) ");
    // printf("\nDate2: %4d-%02d-%02d %02d:%02d:%02d\n", year(intMQTTdateTime), month(intMQTTdateTime), day(intMQTTdateTime), hour(intMQTTdateTime), minute(intMQTTdateTime), second(intMQTTdateTime));
    
    //sending test MQTT message
    if (!client.connected()) {
    reconnect();
    }
    sendMQTT(String (String("TestMQTT ts:")+ String(intMQTTdateTime)), topic);

  }//sending MQTT
/****************************************/

} //end of setup


void read_io() {
  A1State = digitalRead(Input1);
  A2State = digitalRead(Input2);
  A3State = digitalRead(Input3);
  A4State = digitalRead(Input4);
  A5State = digitalRead(Input5);   
  
  String read_ioString="";
  // String read_ioString= String("\nInput values for below signals (0 means there is signal):");
  // read_ioString = read_ioString + String("\nTNB|ELCB|RED|YELLOW|GREEN");
  SerialMon.print("|");
  read_ioString = read_ioString + String("|");
  SerialMon.print("IO State :");
  // read_ioString = read_ioString + String("IO State :");
  SerialMon.print(A1State);       //TNB
  read_ioString = read_ioString + String(A1State);
  SerialMon.print("|");
  read_ioString = read_ioString + String("|");
  SerialMon.print(A2State);       //ELCB
  read_ioString = read_ioString + String(A2State);
  SerialMon.print("|");
  read_ioString = read_ioString + String("|");
  SerialMon.print(A3State);       //RED
  read_ioString = read_ioString + String(A3State);
  SerialMon.print("|");
  read_ioString = read_ioString + String("|");
  SerialMon.print(A4State);       //Yellow
  read_ioString = read_ioString + String(A4State);
  SerialMon.print("|");
  read_ioString = read_ioString + String("|");
  SerialMon.print(A5State);       //GREEN
  read_ioString = read_ioString + String(A5State);
  SerialMon.print("|");
  read_ioString = read_ioString + String("|");
  // SerialMon.print(read_ioString);
  if (read_ioString!=last_ioRead)
  {
    // String("{" + ID + ": [{\"ts\": " + String(ts) + "values\": {\"TNB\":0, "ELCB":0,"Colour":"Red","Light":0, "AmberFlashing":"false"}}]}
    reconnect();
    // sendMQTT(read_ioString, debugTopic);
    last_ioRead=read_ioString;
    intMQTTdateTime= (intMQTTdateTime + (unsigned long)(millis() - Zeromillis)/1000);
    read_ioString=String("{\"") + String(ID)+ String("\": [{\"ts\": ") +String(intMQTTdateTime)+ String(", \"values\": {\"TNB\":") + String("0") + String(", \"ELCB\":") + String("0") +String(",\"Colour\":\"-1\",\"Light\":-1, \"AmberFlashing\":1, \"smsserial\": \"")+String(read_ioString)+String("\"}}]}");
    // read_ioString=String("{") + String(ID)+ String(": [{\"ts\": ") +String(intMQTTdateTime)+ String(", \"values\": {\"TNB\":") + String(TNB) + String(", \"ELCB\":") + String(ELCB) +String(",\"Colour\":\"Red\",\"Light\":0, \"AmberFlashing\":1}}]}");
    sendMQTT(read_ioString,topic);
  }
  read_ioString=String("");
}

void sendSms(String smsMessage){
  
  if(modem.sendSMS(SMS_TARGET, smsMessage)){
    SerialMon.print("SENDSMS: SMS Text: ");
    SerialMon.println(smsMessage);
  }
  else{
    SerialMon.println("INFO: SMS failed to send");
  }
}

byte checkRedStatus(){
  SerialMon.println("04 FUNCTION: checkRedStatus");

  long timerStart=0;
  long timerNow=0;
  byte loop_byte = RED_FAULTY;

  timerStart = millis();

  for (;;) {
          SerialMon.print("FOR LOOP: checkRedStatus");
          // sendMQTT("FOR LOOP: checkRedStatus", debugTopic);
          read_io();
          if (A4State == HIGH && A5State == HIGH && A3State == LOW){
                SerialMon.println("RED LIGHT OK");
                // sendMQTT("RED LIGHT OK", debugTopic);
                loop_byte = RED_OK;
                if (!client.connected()) {
                  reconnect();
                }
                // sendMQTT("Heartbeat: ID:"+String(ID)+" TNB:0, ELCB:0, Colour: Red", topic);
                intMQTTdateTime= (intMQTTdateTime + (unsigned long)(millis() - Zeromillis)/1000);
                sendMQTT(String("{\"") + String(ID)+ String("\": [{\"ts\": ") +String(intMQTTdateTime)+ String(", \"values\": {\"TNB\":") + String("0") + String(", \"ELCB\":") + String("0") +String(",\"Colour\":\"Red\",\"Light\":-1, \"AmberFlashing\":1, \"smsserial\": \"")+String("-1")+String("\"}}]}"),topic);
                flashingStatus=false;
                break;
          }
          
          if (A4State == LOW && A5State == HIGH && A3State == LOW){
                SerialMon.println("RED LIGHT OK");
                // sendMQTT("RED LIGHT OK", debugTopic);
                loop_byte = RED_OK;
                if (!client.connected()) {
                  reconnect();
                }
                // sendMQTT("Heartbeat: ID:"+String(ID)+" TNB:0, ELCB:0, Colour: Red", topic);
                intMQTTdateTime= (intMQTTdateTime + (unsigned long)(millis() - Zeromillis)/1000);
                sendMQTT(String("{\"") + String(ID)+ String("\": [{\"ts\": ") +String(intMQTTdateTime)+ String(", \"values\": {\"TNB\":") + String("0") + String(", \"ELCB\":") + String("0") +String(",\"Colour\":\"Red\",\"Light\":-1, \"AmberFlashing\":1, \"smsserial\": \"")+String("-1")+String("\"}}]}"),topic);
                flashingStatus=false;
                break;
          }
          
          else{
              SerialMon.print("WRONG STATE: RED");
              SerialMon.print("|");
              // sendMQTT("RED LIGHT OK", debugTopic);
              // check for Amber flashing
              checkAmberStatus2();
          }
          
          SerialMon.print("Timer: ");
          timerNow = millis();
          SerialMon.println((timerNow-timerStart)/1000);
          
          if ( (timerNow-timerStart)/1000> MASA_TUNGGU  ){ //300 saat
            loop_byte=RED_FAULTY;
            SerialMon.println("RED_FAULTY: Break");
            break;
        }
        delay(1000);
     } //for loop ending
  return loop_byte;
}


byte checkGreenStatus(){
  SerialMon.println("05 FUNCTION: checkGreenStatus");

  long timerStart=0;
  long timerNow=0;
  byte loop_byte = GREEN_FAULTY;
  
  timerStart = millis();

        for(;;){
              SerialMon.print("FOR LOOP: checkGreenStatus");
              read_io();
              //true only if change state
              if (A3State == HIGH && A4State == HIGH && A5State == LOW ){
                    SerialMon.println("GREEN LIGHT OK");
                    loop_byte = GREEN_OK;
                    if (!client.connected()) {
                      reconnect();
                    }
                    // sendMQTT("Heartbeat: ID:"+String(ID)+" TNB:0, ELCB:0, Colour: Green", topic);
                  intMQTTdateTime= (intMQTTdateTime + (unsigned long)(millis() - Zeromillis)/1000);
                  sendMQTT(String("{\"") + String(ID)+ String("\": [{\"ts\": ") +String(intMQTTdateTime)+ String(", \"values\": {\"TNB\":") + String("0") + String(", \"ELCB\":") + String("0") +String(",\"Colour\":\"Green\",\"Light\":-1, \"AmberFlashing\":1, \"smsserial\": \"")+String("-1")+String("\"}}]}"),topic);
                    flashingStatus=false;
                    break;
                }
              if (A3State == LOW && A4State == HIGH && A5State == LOW ){
                    SerialMon.println("GREEN LIGHT OK");
                    loop_byte = GREEN_OK;
                    if (!client.connected()) {
                      reconnect();
                    }
                    // sendMQTT("Heartbeat: ID:"+String(ID)+" TNB:0, ELCB:0, Colour: Green", topic);
                    intMQTTdateTime= (intMQTTdateTime + (unsigned long)(millis() - Zeromillis)/1000);
                    sendMQTT(String("{\"") + String(ID)+ String("\": [{\"ts\": ") +String(intMQTTdateTime)+ String(", \"values\": {\"TNB\":") + String("0") + String(", \"ELCB\":") + String("0") +String(",\"Colour\":\"Green\",\"Light\":-1, \"AmberFlashing\":1, \"smsserial\": \"")+String("-1")+String("\"}}]}"),topic);
                    flashingStatus=false;
                    break;
               }
              else{
                  SerialMon.print("WRONG STATE: GREEN");
                  SerialMon.print("|");
                  //check for Amber status
                  checkAmberStatus2();
                }
              
              SerialMon.print("GREEN Timer: ");
              timerNow = millis();
              SerialMon.println((timerNow-timerStart)/1000);
              
              if ( (timerNow-timerStart)/1000> MASA_TUNGGU ){ //300 saat
                loop_byte=GREEN_FAULTY;
                SerialMon.println("GREEN_FAULTY: Break");
                break;
            }
            delay(1000);
        }
  return loop_byte;
}

byte checkAmberStatus2(){
  SerialMon.println("XX FUNCTION: checkAmber Flash Status");
  if (!client.connected()) {
      reconnect();
    }
  long timerStart=0;
  long timerNow=0;
  byte loop_byte = AMBER_FAULTY;

//get epoch time every 10 minutes
  if ((unsigned long)(millis()-Zeromillis)>interval)
  {
    intMQTTdateTime=0;
    MQTTdateTime="";
    // getEpochTime(intMQTTdateTime);
    if (!client.connected()) {
    reconnect();
    }
    SerialMon.print("Attempting to connect subscribe: ");
    Serial.println(client.subscribe(UnixTime_topic));
    
    while(MQTTdateTime==""){
      client.loop();
    }
    Zeromillis=millis();
    client.unsubscribe(UnixTime_topic);
    delay(2000);
    Serial.print("MQTTdateTime: ");
    Serial.println(MQTTdateTime);

    //convert to int
    for(int i=0; i<10; i++)
    {
      intMQTTdateTime = intMQTTdateTime * 10 + ( MQTTdateTime[i] - '0' );
    }
    SerialMon.print("\nintMQTTdateTime: ");
    SerialMon.println(intMQTTdateTime);
    //Malaysian time needs to add 8 hours
    intMQTTdateTime = intMQTTdateTime + 28800;
    SerialMon.print("\nt_unix_date2:(MY) ");
    // printf("\nDate2: %4d-%02d-%02d %02d:%02d:%02d\n", year(intMQTTdateTime), month(intMQTTdateTime), day(intMQTTdateTime), hour(intMQTTdateTime), minute(intMQTTdateTime), second(intMQTTdateTime));
    //************************************************************************************
  }

  timerStart = millis();

        for(;;){
          SerialMon.print("FOR LOOP: checkAmberStatus");
          read_io();
          //true only if change state
          if (A3State == HIGH && A5State == HIGH && A4State == LOW && !flashingStatus){
                SerialMon.println("AMBER LIGHT OK");
                // count++;
                // loop_byte = AMBER_OK;

                if (count==2)
                {
                 if (!client.connected()) {
                 reconnect();
                  }
                // sendMQTT("Heartbeat: ID:"+String(ID)+" TNB:0, ELCB:0, AmberFlashing: true", topic);
                intMQTTdateTime= (intMQTTdateTime + (unsigned long)(millis() - Zeromillis)/1000);
                sendMQTT(String("{\"") + String(ID)+ String("\": [{\"ts\": ") +String(intMQTTdateTime)+ String(", \"values\": {\"TNB\":") + String("0") + String(", \"ELCB\":") + String("0") +String(",\"Colour\":\"-1\",\"Light\":-1, \"AmberFlashing\":0, \"smsserial\": \"")+String("-1")+String("\"}}]}"),topic);
                flashingStatus=true;
                }
                count++;
                delay(2000);
                checkAmberStatus2();
                loop_byte = AMBER_OK;
                break;
            }
         if (A3State == HIGH && A5State == LOW && A4State == LOW && !flashingStatus){
                SerialMon.println("AMBER LIGHT OK");
                loop_byte = AMBER_OK;
                if (count==2)
                {
                 if (!client.connected()) {
                 reconnect();
                  }
                // sendMQTT("Heartbeat: ID:"+String(ID)+" TNB:0, ELCB:0, AmberFlashing: true", topic);
                intMQTTdateTime= (intMQTTdateTime + (unsigned long)(millis() - Zeromillis)/1000);
                sendMQTT(String("{\"") + String(ID)+ String("\": [{\"ts\": ") +String(intMQTTdateTime)+ String(", \"values\": {\"TNB\":") + String("0") + String(", \"ELCB\":") + String("0") +String(",\"Colour\":\"-1\",\"Light\":-1, \"AmberFlashing\":0, \"smsserial\": \"")+String("-1")+String("\"}}]}"),topic);
                flashingStatus=true;
                }
                count++;
                delay(2000);                
                checkAmberStatus2();
                break;
            }
          else{
              SerialMon.print("WRONG STATE: yellow");
              SerialMon.print("|");
            }
          
          SerialMon.print("yellow Timer: ");
          timerNow = millis();
          SerialMon.println((timerNow-timerStart)/1000);
          
          if ( (timerNow-timerStart)/1000> 2  ){ //only 10 saat utk amber
            loop_byte=AMBER_FAULTY;
            SerialMon.println("yellow_FAULTY: Break");
            count=0; 
            //lets check for TNB and ELCB
/*******************************************************************************************/
            SerialMon.print("02 FUNCTION: check TNB & ELCB");
            SerialMon.println("");
              if (A1State != lastA1State) {
                if (A1State == HIGH){
                if(ReportMode==String("SMS")){
                }

                if(ReportMode==String("MQTT")){
                //   AllAlarms = AllAlarms+ String("\"TNB\":") + String(1) +String(" ,");
                //   SerialMon.print("TNB check, AllAlarms: ");
                //   SerialMon.println(AllAlarms);
                  count=0;
                  checkPinStatus();

                }
                }
                else{
                if(ReportMode==String("SMS")){
                }
                  
                if(ReportMode==String("MQTT")){
                //   AllAlarms = AllAlarms+ String("\"TNB\":") + String(0) +String(" ,");
                //   SerialMon.print("TNB check, AllAlarms: ");
                //   SerialMon.println(AllAlarms);
                }
                }
              }
              else if(ReportMode==String("MQTT"))
              {
                if (A1State == HIGH){
                //   AllAlarms = AllAlarms+ String("\"TNB\":") + String(1) +String(" ,");
                //   SerialMon.print("TNB check, AllAlarms: ");
                //   SerialMon.println(AllAlarms);
                count=0;
                checkPinStatus();
                }
                else{
                //   AllAlarms = AllAlarms+ String("\"TNB\":") + String(0) +String(" ,");
                //   SerialMon.print("TNB check, AllAlarms: ");
                //   SerialMon.println(AllAlarms);
                }
              }
              
              lastA1State = A1State;

              if (A1State == LOW /*&& A2State != lastA2State*/){
                if (A2State == HIGH){
                if(ReportMode==String("SMS")){
                //   if (A2State != lastA2State)
                //   {
                //   SerialMon.println("TRYSENDSMS: ELCB=TRIP");
                //   myString = String(ID) + "\nELCB=TRIP";
                //   sendSms(myString);
                //   }
                }
                if(ReportMode==String("MQTT")){
                //   AllAlarms = AllAlarms+ String("\"ELCB\":") + String(1) +String(" ,");
                //   SerialMon.print("ELCB check, AllAlarms: ");
                //   SerialMon.println(AllAlarms);
                count=0;
                checkPinStatus();
                }
                }
                else{
                if(ReportMode==String("SMS")){
                //  if (A2State != lastA2State)
                //  {
                //   SerialMon.println("TRYSENDSMS: ELCB=OK");
                //   myString = String(ID) + "\nELCB=OK";
                //   sendSms(myString);
                //  }
                }
                if(ReportMode==String("MQTT")){
                //   AllAlarms = AllAlarms+ String("\"ELCB\":") + String(0) +String(" ,");
                //   SerialMon.print("ELCB check, AllAlarms: ");
                //   SerialMon.print(AllAlarms);
                }
                }
              }
              
              
              else if(ReportMode==String("MQTT"))
              {
                if (A2State == HIGH){
                //   AllAlarms = AllAlarms+ String("\"ELCB\":") + String(1) +String(" ,");
                //   SerialMon.print("ELCB check, AllAlarms: ");
                //   SerialMon.println(AllAlarms);
                count=0;
                checkPinStatus();
                }
                else{
                //   AllAlarms = AllAlarms+ String("\"ELCB\":") + String(0) +String(" ,");
                //   SerialMon.print("ELCB check, AllAlarms: ");
                //   SerialMon.print(AllAlarms);
              }
              }
  /*******************************************************************************************/
            break;
        }
        delay(1000);
      
      }//foor loop
      return loop_byte;
}


byte checkAmberStatus(){
  SerialMon.println("06 FUNCTION: checkAmberStatus");

  long timerStart=0;
  long timerNow=0;
  byte loop_byte = AMBER_FAULTY;

  timerStart = millis();

        for(;;){
          SerialMon.print("FOR LOOP: checkAmberStatus");
          read_io();
          //true only if change state
          if (A3State == HIGH && A5State == HIGH && A4State == LOW ){
                SerialMon.println("AMBER LIGHT OK");
                // count++;
                // loop_byte = AMBER_OK;
                if (!client.connected()) {
                 reconnect();
                }
                // sendMQTT("Heartbeat: ID:"+String(ID)+" TNB:0, ELCB:0, Colour: yellow", topic);
                intMQTTdateTime= (intMQTTdateTime + (unsigned long)(millis() - Zeromillis)/1000);
                sendMQTT(String("{\"") + String(ID)+ String("\": [{\"ts\": ") +String(intMQTTdateTime)+ String(", \"values\": {\"TNB\":") + String("0") + String(", \"ELCB\":") + String("0") +String(",\"Colour\":\"Yellow\",\"Light\":-1, \"AmberFlashing\":1, \"smsserial\": \"")+String("-1")+String("\"}}]}"),topic);
                delay(10000);
                checkAmberStatus2();
                loop_byte = AMBER_OK;
                break;
            }
         if (A3State == HIGH && A5State == LOW && A4State == LOW ){
                SerialMon.println("AMBER LIGHT OK");
                count++;
                loop_byte = AMBER_OK;
                if (!client.connected()) {
                 reconnect();
                }
                // sendMQTT("Heartbeat: ID:"+String(ID)+" TNB:0, ELCB:0, Colour: yellow", topic);
                intMQTTdateTime= (intMQTTdateTime + (unsigned long)(millis() - Zeromillis)/1000);
                sendMQTT(String("{\"") + String(ID)+ String("\": [{\"ts\": ") +String(intMQTTdateTime)+ String(", \"values\": {\"TNB\":") + String("0") + String(", \"ELCB\":") + String("0") +String(",\"Colour\":\"Yellow\",\"Light\":-1, \"AmberFlashing\":1, \"smsserial\": \"")+String("-1")+String("\"}}]}"),topic);
                checkAmberStatus2();
                break; //asdsa
            }
          else{
              SerialMon.print("WRONG STATE: yellow");
              SerialMon.print("|");

            //lets check for TNB and ELCB
/*******************************************************************************************/
            SerialMon.print("02 FUNCTION: check TNB & ELCB");
            SerialMon.println("");
              if (A1State != lastA1State) {
                if (A1State == HIGH){
                if(ReportMode==String("SMS")){
                }

                if(ReportMode==String("MQTT")){
                //   AllAlarms = AllAlarms+ String("\"TNB\":") + String(1) +String(" ,");
                //   SerialMon.print("TNB check, AllAlarms: ");
                //   SerialMon.println(AllAlarms);
                  count=0;
                  checkPinStatus();

                }
                }
                else{
                if(ReportMode==String("SMS")){
                }
                  
                if(ReportMode==String("MQTT")){
                //   AllAlarms = AllAlarms+ String("\"TNB\":") + String(0) +String(" ,");
                //   SerialMon.print("TNB check, AllAlarms: ");
                //   SerialMon.println(AllAlarms);
                }
                }
              }
              else if(ReportMode==String("MQTT"))
              {
                if (A1State == HIGH){
                //   AllAlarms = AllAlarms+ String("\"TNB\":") + String(1) +String(" ,");
                //   SerialMon.print("TNB check, AllAlarms: ");
                //   SerialMon.println(AllAlarms);
                count=0;
                checkPinStatus();
                }
                else{
                //   AllAlarms = AllAlarms+ String("\"TNB\":") + String(0) +String(" ,");
                //   SerialMon.print("TNB check, AllAlarms: ");
                //   SerialMon.println(AllAlarms);
                }
              }
              
              lastA1State = A1State;

              if (A1State == LOW /*&& A2State != lastA2State*/){
                if (A2State == HIGH){
                if(ReportMode==String("SMS")){
                //   if (A2State != lastA2State)
                //   {
                //   SerialMon.println("TRYSENDSMS: ELCB=TRIP");
                //   myString = String(ID) + "\nELCB=TRIP";
                //   sendSms(myString);
                //   }
                }
                if(ReportMode==String("MQTT")){
                //   AllAlarms = AllAlarms+ String("\"ELCB\":") + String(1) +String(" ,");
                //   SerialMon.print("ELCB check, AllAlarms: ");
                //   SerialMon.println(AllAlarms);
                count=0;
                checkPinStatus();
                }
                }
                else{
                if(ReportMode==String("SMS")){
                //  if (A2State != lastA2State)
                //  {
                //   SerialMon.println("TRYSENDSMS: ELCB=OK");
                //   myString = String(ID) + "\nELCB=OK";
                //   sendSms(myString);
                //  }
                }
                if(ReportMode==String("MQTT")){
                //   AllAlarms = AllAlarms+ String("\"ELCB\":") + String(0) +String(" ,");
                //   SerialMon.print("ELCB check, AllAlarms: ");
                //   SerialMon.print(AllAlarms);
                }
                }
              }
              
              
              else if(ReportMode==String("MQTT"))
              {
                if (A2State == HIGH){
                //   AllAlarms = AllAlarms+ String("\"ELCB\":") + String(1) +String(" ,");
                //   SerialMon.print("ELCB check, AllAlarms: ");
                //   SerialMon.println(AllAlarms);
                count=0;
                checkPinStatus();
                }
                else{
                //   AllAlarms = AllAlarms+ String("\"ELCB\":") + String(0) +String(" ,");
                //   SerialMon.print("ELCB check, AllAlarms: ");
                //   SerialMon.print(AllAlarms);
              }
              }
  /*******************************************************************************************/

            }
          
          SerialMon.print("yellow Timer: ");
          timerNow = millis();
          SerialMon.println((timerNow-timerStart)/1000);
          
          if ( (timerNow-timerStart)/1000> Yellow_Time  ){ //only 10 saat utk amber
            loop_byte=AMBER_FAULTY;
            SerialMon.println("yellow_FAULTY: Break");
            break;
        }
        delay(1000);
      }//foor loop
      return loop_byte;
}

//byte checkRedStatus(){
//  SerialMon.println("04 FUNCTION: checkRedStatus");
//
//  long timerStart=0;
//  long timerNow=0;
//  byte loop_byte = RED_FAULTY;
//
//  timerStart = millis();
//
//
//  for (;;) {
//          SerialMon.print("FOR LOOP: checkRedStatus");
//          read_io();
//          //true only if change state
//          if (A3State != lastA3State){
//            if (A4State == HIGH && A5State == HIGH && A3State == LOW ){
//                SerialMon.println("RED LIGHT OK");
//                loop_byte = RED_OK;
//                break;
//            }
//            else{
//              SerialMon.println("WRONG STATE: RED");
//            }
//          }
//      
//          SerialMon.print("Timer: ");
//          timerNow = millis();
//          SerialMon.println((timerNow-timerStart)/1000);
//          
//          if ( (timerNow-timerStart)/1000> MASA_TUNGGU  ){ //300 saat
//            loop_byte=RED_FAULTY;
//            SerialMon.println("RED_FAULTY: Break");
//            break;
//        }
//        lastA3State = A3State;
//        delay(1000);
//     } //for loop ending
//  return loop_byte;
//}

//byte checkGreenStatus(){
//  SerialMon.println("05 FUNCTION: checkGreenStatus");
//
//  long timerStart=0;
//  long timerNow=0;
//  byte loop_byte = GREEN_FAULTY;
//  
//  timerStart = millis();
//
//  
//        for(;;){
//              SerialMon.print("FOR LOOP: checkGreenStatus");
//              read_io();
//              //true only if change state
//              if (A5State != lastA5State){
//                if (A3State == HIGH && A4State == HIGH && A5State == LOW ){
//                    SerialMon.println("GREEN LIGHT OK");
//                    loop_byte = GREEN_OK;
//                    break;
//                }
//                else{
//                  SerialMon.println("WRONG STATE: GREEN");
//                }
//              }
//              
//              SerialMon.print("GREEN Timer: ");
//              timerNow = millis();
//              SerialMon.println((timerNow-timerStart)/1000);
//              
//              if ( (timerNow-timerStart)/1000> MASA_TUNGGU ){ //300 saat
//                loop_byte=GREEN_FAULTY;
//                SerialMon.println("GREEN_FAULTY: Break");
//                break;
//            }
//            lastA5State = A5State;
//            delay(1000);
//        }
//  return loop_byte;
//}
//
//byte checkAmberStatus(){
//  SerialMon.println("06 FUNCTION: checkAmberStatus");
//
//  long timerStart=0;
//  long timerNow=0;
//  byte loop_byte = AMBER_FAULTY;
//
//  timerStart = millis();
//
//        for(;;){
//          SerialMon.print("FOR LOOP: checkAmberStatus");
//          read_io();
//          //true only if change state
//          if (A4State != lastA4State){
//            if (A3State == HIGH && A5State == HIGH && A4State == LOW ){
//                SerialMon.println("AMBER LIGHT OK");
//                count++;
//                loop_byte = AMBER_OK;
//                break;
//            }
//            else{
//              SerialMon.println("WRONG STATE: AMBER");
//            }
//          }
//          SerialMon.print("AMBER Timer: ");
//          timerNow = millis();
//          SerialMon.println((timerNow-timerStart)/1000);
//          
//          if ( (timerNow-timerStart)/1000> 10  ){ //only 10 saat utk amber
//            loop_byte=AMBER_FAULTY;
//            SerialMon.println("AMBER_FAULTY: Break");
//            break;
//        }
//        lastA4State = A4State;
//        delay(1000);
//      }//foor loop
//      
//      return loop_byte;
//}

byte checkCycleStatus(){
  SerialMon.println("03 FUNCTION: checkCycleStatus");
 
  byte sys_status = LIGHT_FAULTY;
  
  for(;;){
    
    byte red_status = checkRedStatus();
    if (red_status == RED_OK){
      byte green_status = checkGreenStatus();
      if (green_status == GREEN_OK){
        byte amber_status = checkAmberStatus();
        if (amber_status == AMBER_OK){
          sys_status = LIGHT_OK;
          sysStateOK = true;
          break;
        }
        else{        
          sys_status =LIGHT_FAULTY;
          sysStateOK = false;
          break;
        }
      }
      else{
        sys_status =LIGHT_FAULTY;
        sysStateOK = false;
        break;
      }
    }
    else{
      sys_status =LIGHT_FAULTY;
      sysStateOK = false;
      break;
    }
  } //for loop

  SerialMon.print("CHECK: sys_status: ");
  SerialMon.println(sys_status);
  return sys_status;
}
  

void checkPinStatus(){
  AllAlarms = String("{\"") + String(ID) + String("\":[{\"values\" : {"); //now only pairs have to be added
  SerialMon.println("01 FUNCTION: checkPinStatus");

  String myString = "";
  SerialMon.print("02 FUNCTION: check TNB & ELCB");
  read_io();
  SerialMon.println("");
  
  if (A1State != lastA1State) {
    if (A1State == HIGH){
     if(ReportMode==String("SMS")){
      SerialMon.println("TRYSENDSMS: TNB=FAIL");
      myString = String(ID) + "\nTNB=FAIL";
      sendSms(myString);
     }

     if(ReportMode==String("MQTT")){
      // AllAlarms = AllAlarms+ String("\"TNB\":") + String(1) +String(" ,");
      AllAlarms=String(", \"values\": {\"TNB\":") + String("1"); 
      SerialMon.print("TNB check, AllAlarms: ");
      SerialMon.println(AllAlarms);
     }
    }
    else{
     if(ReportMode==String("SMS")){
       SerialMon.println("TRYSENDSMS: TNB=OK");
       myString = String(ID) + "\nTNB=OK";
       sendSms(myString);
     }
      
     if(ReportMode==String("MQTT")){
      // AllAlarms = AllAlarms+ String("\"TNB\":") + String(0) +String(" ,");
      AllAlarms=String(", \"values\": {\"TNB\":") + String("0"); 
      SerialMon.print("TNB check, AllAlarms: ");
      SerialMon.println(AllAlarms);
     }
    }
  }
  else if(ReportMode==String("MQTT"))
  {
    if (A1State == HIGH){
      // AllAlarms = AllAlarms+ String("\"TNB\":") + String(1) +String(" ,");
      AllAlarms=String(", \"values\": {\"TNB\":") + String("1"); 
      SerialMon.print("TNB check, AllAlarms: ");
      SerialMon.println(AllAlarms);
    }
    else{
      // AllAlarms = AllAlarms+ String("\"TNB\":") + String(0) +String(" ,");
      AllAlarms=String(", \"values\": {\"TNB\":") + String("0"); 
      SerialMon.print("TNB check, AllAlarms: ");
      SerialMon.println(AllAlarms);
    }
  }
  
  lastA1State = A1State;

  if (A1State == LOW /*&& A2State != lastA2State*/){
    if (A2State == HIGH){
     if(ReportMode==String("SMS")){
      if (A2State != lastA2State)
      {
      SerialMon.println("TRYSENDSMS: ELCB=TRIP");
      myString = String(ID) + "\nELCB=TRIP";
      sendSms(myString);
      }
     }
     if(ReportMode==String("MQTT")){
      // AllAlarms = AllAlarms+ String(",\"ELCB\":") + String(1) +String(" ,");
      AllAlarms = AllAlarms+ String(",\"ELCB\":") + String(1) +String(" ,");
      SerialMon.print("ELCB check, AllAlarms: ");
      SerialMon.println(AllAlarms);
     }
    }
    else{
     if(ReportMode==String("SMS")){
     if (A2State != lastA2State)
     {
      SerialMon.println("TRYSENDSMS: ELCB=OK");
      myString = String(ID) + "\nELCB=OK";
      sendSms(myString);
     }
     }
     if(ReportMode==String("MQTT")){
      // AllAlarms = AllAlarms+ String("\"ELCB\":") + String(0) +String(" ,");
      AllAlarms = AllAlarms+ String(",\"ELCB\":") + String(0) +String(" ,");
      SerialMon.print("ELCB check, AllAlarms: ");
      SerialMon.print(AllAlarms);
     }
    }
  }
  
  
  else if(ReportMode==String("MQTT"))
  {
    if (A2State == HIGH){
      // AllAlarms = AllAlarms+ String("\"ELCB\":") + String(1) +String(" ,");
      AllAlarms = AllAlarms+ String(",\"ELCB\":") + String(1) +String(" ,");
      SerialMon.print("ELCB check, AllAlarms: ");
      SerialMon.println(AllAlarms);
    }
    else{
      // AllAlarms = AllAlarms+ String("\"ELCB\":") + String(0) +String(" ,");
      AllAlarms = AllAlarms+ String(",\"ELCB\":") + String(0) +String(" ,");
      SerialMon.print("ELCB check, AllAlarms: ");
      SerialMon.print(AllAlarms);
  }
  }
  
  // light sequence testing
  lastA2State = A2State;
#ifdef lightseq
  if (A1State == LOW && A2State == LOW){
  
    byte status_byte = checkCycleStatus();
    if (status_byte == LIGHT_OK){
        if(ReportMode==String("SMS")){
        if (sysStateOK != lastSysStateOK ){
          SerialMon.println("TRYSENDSMS: SYS RUN OK");
          myString = String(ID) + "\nSYS RUN OK";
          sendSms(myString);
         }else
         {
          SerialMon.println("INFO: SYS RUN OK");
         }
        }

        if(ReportMode==String("MQTT")){
          // AllAlarms = AllAlarms+ String("\"Light\":") + String(0) + String("}}]}"); //last state so no comma needed
          AllAlarms=AllAlarms+String("\"Colour\":\"-1\",\"Light\":0, \"AmberFlashing\":1, \"smsserial\": \"")+String("-1")+String("\"}}]}");
          SerialMon.print("Light check, AllAlarms: ");
          SerialMon.println(AllAlarms);
        }
       }
    else{
         if(ReportMode==String("SMS")){
          if (sysStateOK != lastSysStateOK ){
          SerialMon.println("TRYSENDSMS: LIGHT FAULT");
          myString = String(ID) + "\nLIGHT FAULT";
          sendSms(myString);
          }
          else{
          SerialMon.println("ALARM: LIGHT FAULT");
          }
         }

         if(ReportMode==String("MQTT")){
          // AllAlarms = AllAlarms+ String("\"Light\":") + String(1) + String("}}]}"); //last state so no comma needed
          AllAlarms=AllAlarms+String("\"Colour\":\"-1\",\"Light\":1, \"AmberFlashing\":1, \"smsserial\": \"")+String("-1")+String("\"}}]}");
          SerialMon.print("Light check, AllAlarms: ");
          SerialMon.println(AllAlarms);
         }
    }
    lastSysStateOK = sysStateOK;
  }

  else if(ReportMode==String("MQTT"))
  {
          // AllAlarms = AllAlarms+ String("\"Light\":") + String(0) + String("}}]}"); //last state so no comma needed
          AllAlarms=AllAlarms+String("\"Colour\":\"-1\",\"Light\":0, \"AmberFlashing\":1, \"smsserial\": \"")+String("-1")+String("\"}}]}");
          SerialMon.print("Light check, AllAlarms: ");
          SerialMon.println(AllAlarms);
  }
#endif // !defined
if(ReportMode==String("MQTT")) {
 if(AllAlarms != last_AllAlarms){
  if (!client.connected()) {
      reconnect();
  }
     //checking
     Serial.print("last_AllAlarms: ");
     Serial.println(last_AllAlarms);
     Serial.println("AllAlarms: ");
     Serial.println(AllAlarms);
     last_AllAlarms=AllAlarms;

     intMQTTdateTime= (intMQTTdateTime + (unsigned long)(millis() - Zeromillis)/1000);
     AllAlarms= String("{\"") + String(ID)+ String("\": [{\"ts\": ") +String(intMQTTdateTime)+ AllAlarms ;
     sendMQTT(AllAlarms, topic);
 }
  else if ((unsigned long)(currentMillis - previousMillis) >= interval) {
    // It's time to do something!
    // Heartbeat=String("Heartbeat") +AllAlarms;
    // if (!client.connected()) {
    //   reconnect();
    //   }
    // // sendMQTT(Heartbeat, topic);
 
      // Use the snapshot to set track time until next event
      // previousMillis = currentMillis;
   }
 
 
    // AllAlarms=String("");
  }
}

void loop() {
  // currentMillis = millis();
  // if (!client.connected()) {
  //     reconnect();
  // }
  checkPinStatus();
  delay(1000);
  //SerialMon.println("SYS Loop");
  client.loop();
}