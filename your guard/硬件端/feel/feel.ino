/*
原版作者: 张林羽觐

由于新版arduino IDE for mCookie中对SHT2x.h和ESP8266.h进行了修改
因此对原有代码进行修改 不需要任何外部头文件
*/

/**
 * 需要做替换的部分!!!!!!!!!
 * 
*/
#define SSID        "ray" //改为你的热点名称, 不要有中文
#define PASSWORD    "19831209128"//改为你的WiFi密码Wi-Fi密码
#define DEVICEID    "562238002" //OneNet上的设备ID
String apiKey = "8TW=WvhrWQrBvE6VXJF2uf5=RII=";//与你的设备绑定的APIKey

/***/
#define HOST_NAME   "api.heclouds.com"
#define HOST_PORT   (80)
#define INTERVAL_SENSOR   1000             //定义传感器采样时间间隔  597000
#define INTERVAL_NET      1000             //定义发送时间
//传感器部分================================   
#include <Wire.h>                                  //调用库  
#include <ESP8266.h>
#include <I2Cdev.h>                                //调用库  
/*******温湿度*******/
#include <Microduino_SHT2x.h>
/*******光照*******/
#define  sensorPin_1  A0
#define sensorPin_2   A2
#define IDLE_TIMEOUT_MS  3000      // Amount of time to wait (in milliseconds) with no data 
                                   // received before closing the connection.  If you know the server
                                   // you're accessing is quick to respond, you can reduce this value.

//WEBSITE     
char buf[10];

#define INTERVAL_sensor 2000
unsigned long sensorlastTime = millis();
const int AirValue = 2000;   

const int WaterValue = 308;  

int intervals = (AirValue - WaterValue)/3;   
float tempOLED,  lightnessOLED;
int soilMoistureValue = 0;

#define INTERVAL_OLED 1000

String mCottenData;
String jsonToSend;

//3,传感器值的设置 
float sensor_tem,  sensor_lux;                    //传感器温度、湿度、光照   
char  sensor_tem_c[7], soilMoistureValue_c[7], sensor_lux_c[7] ;    //换成char数组传输
#include <SoftwareSerial.h>
#define EspSerial mySerial
#define UARTSPEED  9600
SoftwareSerial mySerial(2, 3); /* RX:D3, TX:D2 */
ESP8266 wifi(&EspSerial);
//ESP8266 wifi(Serial1);                                      //定义一个ESP8266（wifi）的对象
unsigned long net_time1 = millis();                          //数据上传服务器时间
unsigned long sensor_time = millis();                        //传感器采样时间计时器

//int SensorData;                                   //用于存储传感器数据
String postString;                                //用于存储发送数据的字符串
//String jsonToSend;                                //用于存储发送的json格式参数

Tem_Hum_S2 TempMonitor;

void setup(void)     //初始化函数  
{       
  //初始化串口波特率  
    Wire.begin();
    Serial.begin(115200);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately
    Serial.print(F("setup begin\r\n"));
    delay(100);
    pinMode(sensorPin_1, INPUT);

  WifiInit(EspSerial, UARTSPEED);

  Serial.print(F("FW Version:"));
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStationSoftAP()) {
    Serial.print(F("to station + softap ok\r\n"));
  } else {
    Serial.print(F("to station + softap err\r\n"));
  }

  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print(F("Join AP success\r\n"));

    Serial.print(F("IP:"));
    Serial.println( wifi.getLocalIP().c_str());
  } else {
    Serial.print(F("Join AP failure\r\n"));
  }

  if (wifi.disableMUX()) {
    Serial.print(F("single ok\r\n"));
  } else {
    Serial.print(F("single err\r\n"));
  }

  Serial.print(F("setup end\r\n"));
    
  
}
void loop(void)     //循环函数  
{   
  if (sensor_time > millis())  sensor_time = millis();  
    
  if(millis() - sensor_time > INTERVAL_SENSOR)              //传感器采样时间间隔  
  {  
    getSensorData();                                        //读串口中的传感器数据
    sensor_time = millis();
  }  

    
  if (net_time1 > millis())  net_time1 = millis();
  
  if (millis() - net_time1 > INTERVAL_NET)                  //发送数据时间间隔
  {                
    updateSensorData();                                     //将数据上传到服务器的函数
    net_time1 = millis();
  }






  
}

void getSensorData(){  
    sensor_tem = TempMonitor.getTemperature();  
 soilMoistureValue= analogRead(A2); 
    //获取光照
    sensor_lux = analogRead(A0);    
    delay(1000);
    dtostrf(sensor_tem, 2, 1, sensor_tem_c);
    dtostrf(soilMoistureValue, 2, 1, soilMoistureValue_c);
    dtostrf(sensor_lux, 3, 1, sensor_lux_c);
}
void updateSensorData() {
  if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
    Serial.print("create tcp ok\r\n");
if(soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))

{

  Serial.println("非常潮湿");

}

else if(soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))

{

  Serial.println("湿润");

}

else if(soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))

{

  Serial.println("干燥");

}
jsonToSend="{\"Temperature\":";
    dtostrf(sensor_tem,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+=",\"Humidity\":";
    dtostrf(soilMoistureValue,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+=",\"Light\":";
    dtostrf(sensor_lux,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+="}";

 postString="";
    postString+=DEVICEID;
    postString+="";
   
    postString+="\r\n";
   
    postString+=jsonToSend.length();
   
    postString+="\r\n";
    postString+=jsonToSend;
    postString+="\r\n";
    postString+="\r\n";
    postString+="\r\n";



   
  const char *postArray = postString.c_str();                 //将str转化为char数组
  Serial.println(postArray);
  wifi.send((const uint8_t*)postArray, strlen(postArray));    //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
  Serial.println("send success");   
     if (wifi.releaseTCP()) {                                 //释放TCP连接
        Serial.print("release tcp ok\r\n");
        } 
     else {
        Serial.print("release tcp err\r\n");
        }
      postArray = NULL;                                       //清空数组，等待下次传输数据
  
  } else {
    Serial.print("create tcp err\r\n");
  }
}
