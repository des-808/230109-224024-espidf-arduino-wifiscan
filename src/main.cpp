#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/queue.h"
#include "driver/uart.h"
#include "sdkconfig.h"
#include <WiFi.h>
#include "arduino.h"
#include "ntp.h"
#include "esp_timer.h"
////////////////////
//#include "esp_netif_sntp.h"
#include "lwip/ip_addr.h" 
#include "esp_sntp.h"
////////////////////
#include <stdint.h>
#include "WiFiScan.h"
HardwareSerial Serial_hmi(2); 
#include "hmi_mini.h"  

NTP ntp(3);
String incStr;
String intToString(int tmp,int sistema_shislenyya);
//void wifiScan();
//String intToString(int tmp,int sistema_shislenyya);
void ReadSerial_2();
//void AnaliseString();

bool ssid_ok=false;
bool password_ok=false;
bool boolean_password=false;
bool boolean_ssid=false;
uint8_t sBufSerial_hmi[64];
uint8_t sBufSerial[64];
int bytesArraySerial_hmi=0;
int count = 0;
bool read_buf_serial_hmi_bool =false;
bool boolean_xz = false;
bool xx = false;

u_int8_t pinR = 8;

void ntp_status(){
int stat = ntp.status();
   switch(stat){     
       case 0:  Serial.println("всё ок");                               break;
       case 1:  Serial.println("не запущен UDP");                       break;
       case 2:  Serial.println("не подключен WiFi");                    break;
       case 3:  Serial.println("ошибка подключения к серверу");         break;
       case 4:  Serial.println("ошибка отправки пакета");               break;
       case 5:  Serial.println("таймаут ответа сервера");               break;
       case 6:  Serial.println("получен некорректный ответ сервера");   break;
       default:break;
    }
}

void wifiScan() {
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    //sendString_2("t15","scan done");
    if (n == 0) {
        sendString(Serial2,"t15","no networks found");
    } else {
        //sendComand_2("page page2");
        //sendString("t0.txt",+analogRead(pinR)+"\"");
        sendString(Serial2,"t15.txt","netw found "+intToString(n,10));
        for (int i = 0; i < 14; ++i) {//i<n
            String ff = intToString(i,10);//10 = десятиричная система счисления;
            String xz = "t"+ String(ff)+".txt";
                if(i<n){
                    // Print SSID and RSSI for each network found
                    String tmp=(WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*";
                    sendString(Serial2,xz, WiFi.SSID(i)+" ("+WiFi.RSSI(i)+")"+tmp);
                    delay(10);
                }
                else{
                    sendString(Serial2,xz, "                              ");delay(10);
                }
        }
    }
}

void ReadSerial_2(){
    if(Serial2.available()){
        char inc = Serial2.read();
        incStr+=inc;
        if(inc==0x0A){//конец команды
            //AnaliseString();
            incStr = "";
        }
    }
}
void AnaliseString(void){

    
}
//#include "hmi_mini.cpp"
void UART_RX_IRQ(){Serial.print("irq");}

void serialEventRun () {
  if (Serial2.available ()) {
    // получаем новый байт:
    byte inByte = (byte) Serial2.read ();
    sBufSerial_hmi[bytesArraySerial_hmi] = inByte;
     bytesArraySerial_hmi++;     
    if(inByte==0xff){count++;
        if(count==3){count=0;
        read_buf_serial_hmi_bool = true;
        }
    }
  }
}  
String ssid;     
String password;


char buff[64];
int intssid; 
void read_buf_serial_hmi(){
    //for(int i = 0;i<bytesArraySerial2;i++){Serial.write(sBufSerial2[i]);}
    if(boolean_password){for(int i =1,j=0;i<bytesArraySerial_hmi-3;i++,j++){buff[j]= sBufSerial_hmi[i];}password = String(buff);boolean_password = false;password_ok=true;}
    if(boolean_ssid){ssid = WiFi.SSID(sBufSerial_hmi[1]);boolean_ssid = false;ssid_ok=true;intssid=sBufSerial_hmi[1];}
    if((sBufSerial_hmi[1]==0x00)&&(sBufSerial_hmi[2]==0x01)&&(sBufSerial_hmi[3]==0x01)){sendString(Serial2,"t0.txt","Idite naxuy.. ya vas ne znayu!!");}
    else if((sBufSerial_hmi[1]==0x02)&&(sBufSerial_hmi[2]==0x12)&&(sBufSerial_hmi[3]==0x01)){wifiScan();sendInt(Serial2,"va1.val",0);sendInt(Serial2,"z0.val",0);}//skan
    else if((sBufSerial_hmi[1]==0x02)&&(sBufSerial_hmi[2]==0x13)&&(sBufSerial_hmi[3]==0x01)){boolean_xz = true;}//autoscan z0.val
    else if((sBufSerial_hmi[1]==0x02)&&(sBufSerial_hmi[2]==0x01)&&(sBufSerial_hmi[3]==0x00)){boolean_xz = false;}//exit
    else if((sBufSerial_hmi[1]==0x04)&&(sBufSerial_hmi[2]==0x06)&&(sBufSerial_hmi[3]==0x01)){boolean_password = true;}//exit
    else if((sBufSerial_hmi[1]==0x04)&&(sBufSerial_hmi[2]==0x06)&&(sBufSerial_hmi[3]==0x00)){boolean_ssid = true;}//
    else if((sBufSerial_hmi[1]==0x05)&&(sBufSerial_hmi[2]==0x0C)&&(sBufSerial_hmi[3]==0x00)){WiFi.disconnect();sendComand(Serial2,"page page0");sendString(Serial2,"wifiConnect.txt", "wifi not connected");sendInt(Serial2,"va10.val",0);sendInt(Serial2,"tm2.en",1);}//exit
    else if((sBufSerial_hmi[1]==0x05)&&(sBufSerial_hmi[2]==0x00)&&(sBufSerial_hmi[3]==0x01)){
       //char localip = WiFi.localIP();
       sendString(Serial2,"t1.txt", WiFi.SSID());
       sendString(Serial2,"t3.txt", String(WiFi.localIP().toString().c_str()));
       //sendInt(Serial2,"n0",localip);
       sendString(Serial2,"t5.txt", WiFi.macAddress());
       sendString(Serial2,"t7.txt", "zjalupa");
       sendString(Serial2,"t9.txt", String(WiFi.channel()));
       sendString(Serial2,"t11.txt", String(WiFi.RSSI(intssid)));
       
       //Serial.println(localip);
        //Serial.println(WiFi.localIP().toString().c_str());
        //Serial.println(WiFi.channel());
        //Serial.println(WiFi.SSID());
        //Serial.println(WiFi.dnsIP());
        //Serial.println(WiFi.encryptionType(intssid));
        //Serial.println(WiFi.macAddress());

    }//wifi info
    //else if((sBufSerial2[1]==0x02)&&(sBufSerial2[2]==0x02)&&(sBufSerial2[3]==0x01)){sendComand_2("page page0");sendString_2("t3.txt", WiFi.SSID(0));}//  02 02 01  /0
    bytesArraySerial_hmi = 0;
    for(int i = 0;i<64;i++)sBufSerial_hmi[i]=0xFF;
    for(int i = 0;i<64;i++)buff[i]=0x00;
    }

void setup() {
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.begin(115200);
    Serial2.begin(115200);
    //ntp.begin();
    //ntp.setPeriod(5);
    sntp_sync_status_t sync_status;
    sntp_set_sync_status( sync_status);
    sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);
    if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000)) != ESP_OK) {
    printf("Failed to update system time within 10s timeout");
}
    delay(20);
    
    sendInt(Serial2,"va10.val",0);
    sendString(Serial2,"wifiConnect.txt", "wifi not connected");sendInt(Serial2,"va10.val",0);sendInt(Serial2,"tm2.en",1);
    
    
}

void loop() {
    if(boolean_xz==true){boolean_xz=false; wifiScan();}
    if(read_buf_serial_hmi_bool){read_buf_serial_hmi();read_buf_serial_hmi_bool = false;}
    if(ssid_ok&&password_ok){
        ssid_ok=false;
        password_ok=false;
        WiFi.begin((const char*) ssid.c_str(), (const char*) password.c_str());
        int counter =0;
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            counter++;
            if(counter==20){WiFi.disconnect();break;}
            sendString(Serial2,"wifiConnect.txt","Connecting to WiFi..");
            //Serial.println("Connecting to WiFi..");
        }
        //Serial.println(WiFi.broadcastIP());
        //Serial.println(WiFi.channel());
        //Serial.println(WiFi.SSID());
        //Serial.println(WiFi.dnsIP());
        //Serial.println(WiFi.encryptionType(intssid));
        //Serial.println(WiFi.macAddress());
        //Serial.write((const char*) password.c_str());
        sendString(Serial2,"wifiConnect.txt","connect to "+ssid);sendInt(Serial2,"tm2.en",1);
        sendInt(Serial2,"va10.val",1);
        //Serial.write((const char*) ssid.c_str());
    }
    
if (ntp.ms() == 500) {
    ntp_status();
    //if(WiFi.status() == WL_CONNECTED) {
    delay(2);
    //digitalWrite(LED_BUILTIN, 0);
    Serial.print("синхронизация времени ");Serial.print(ntp.ms());
    Serial.println(ntp.timeString());
    Serial.println(ntp.dateString());
    Serial.println();
  }

    delay(20);
}

