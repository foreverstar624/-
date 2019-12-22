#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
/*****************AP设置项 *****************/
const char *APssid="无线控制继电器的吸合" ;//AP名字
const char *APpassword="12345678";  //AP密码
/****************UDP设置项 *****************/
unsigned int read_Port = 41001;     //监听端口
unsigned int send_Port = 41002;     //发送端口
char readdata[512];                 //数据缓存
WiFiUDP Udp;                        //定义udp
/****************io设置项 *****************/
int deng = 14;                      //IO14（D5），用来做控制继电器的吸合的
/*  io数值对应关系：
 *  D0=16，D1=5，D2=4，D4=2，D5=14，D6=12，D7=13
 *  对于新手建议使用这些io，使用比较稳定，不会影响系统正常运行
 */
void setup() {
  Serial.begin(115200);
  Serial.println("");
  pinMode(deng, OUTPUT);             //设置指定io为输出模式
  digitalWrite(deng, 0);             //初始化为低电平，关闭灯状态
  WiFi.mode(WIFI_AP);               //设置为AP模式
  WiFi.softAP(APssid, APpassword);  //配置AP参数，启动AP
  Udp.begin(read_Port);             //UDP设置监听指定端口
}

void loop() {
  /*********AP如果有设备连接，开启接收处理数据机制 **********/  
  if(WiFi.softAPgetStationNum()){
      Udp_H
