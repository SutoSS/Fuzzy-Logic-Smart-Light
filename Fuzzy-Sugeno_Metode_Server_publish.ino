#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <time.h>

#include <Wire.h>

#include <BH1750.h>

#define SP 56          
#define led D3
int PIR = D0;
 
const char* ssid = "SSID"; //SSID Wifi
const char* password = "PSW"; //Pasword Wifi

int timezone = 7;
int dst = 0;
const char* ntpServer = "pool.ntp.org";
int  gmtOffset_sec = 25200;
int   daylightOffset_sec = 0;
//uint16_t bh_value;
bool event_t;
int status_t;
int reading;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay10 = 10000;  
unsigned long debounceDelay2 = 1500; 

int lastButtonState = LOW; 

const char *host = "180.250.135.123:5050"; 


void event(){
  event_t = false;}

BH1750 lightMeter;

int x;
bool sensor;
float outfuz, utotal;
float ON,OZ,OP,ONS,OPS;
float e,de, e_before;
float keluaran, bh_value;
float uNe,uZe,uPe;
float uNde,uZde,uPde;
float pwm, pwm_max, pwm_min;
bool Ne, Ze,Pe ,Nde,Zde,Pde;
unsigned long previousTime;

void gerak(){
  HTTPClient http;    

  String ADCData, gerakan_id, postDataPIR;
  int adcvalue = 8;  //Read Analog value of LDR
  int gd_id = 10;
  int room_id = 403;
  int lampu_id = 1;
  int pwm_lampu = pwm;
  int value_bh = bh_value; 
  //ADCData = String(adcvalue);   //String to interger conversion
  gerakan_id = "1";
  time_t timepir = time(nullptr);
  long timep = timepir + gmtOffset_sec;

  //Post Data
  postDataPIR = "gerakan_id=" + gerakan_id + "&timestamp=" + timep + "&ID_Building=" + gd_id + "&ID_Room=" + room_id + "&ID_Light=" + lampu_id + "&Light_Dens=" + value_bh + "&Light_Status=" + reading + "&Pwm=" + pwm_lampu;
  
  http.begin("address"); //Specify request destination http://*****/***
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header

  int httpCodePIR = http.POST(postDataPIR);   //Send the request
  String payload = http.getString();    //Get the response payload

  Serial.println(httpCodePIR);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload

  http.end();  //Close connection
  
  delay(1000);  //Post Data at every 1 seconds
}
void gerak_hhmm(){
  HTTPClient http;    

  String ADCData, gerakan_id_hhmm, postDatahhmm;
  int adcvalue = 8;  //Read Analog value of LDR
  int gd_id = 10;
  int room_id = 403;
  int lampu_id = 1;
  int pwm_lampu = pwm;
  int value_bh = bh_value;  
  //ADCData = String(adcvalue);   //String to interger conversion
  gerakan_id_hhmm = "1";
  time_t timepir = time(nullptr);
  long timep = timepir + gmtOffset_sec;

  //Post Data
  postDatahhmm = "gerakan_id=" + gerakan_id_hhmm + "&timestamp=" + timep + "&ID_Building=" + gd_id + "&ID_Room=" + room_id + "&ID_Light=" + lampu_id + "&Light_Dens=" + value_bh + "&Light_Status=" + reading + "&Pwm=" + pwm_lampu;
  
  http.begin("address"); //Specify request destination http://*****/***
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header

  int httpCodehhmm = http.POST(postDatahhmm);   //Send the request
  String payload = http.getString();    //Get the response payload

  Serial.println(httpCodehhmm);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload

  http.end();  //Close connection
  
  delay(500);  //Post Data at every 0.5 seconds
}
 
void setup () {
 
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting..");
  }
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(led, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIR, INPUT);

  Wire.begin();
  lightMeter.begin();

  configTime(timezone * 3600, dst * 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  
  Ne = 0;
  Ze= 0;
  Pe = 0;
  Nde = 0;
  Zde= 0;
  Pde = 0;
  OP = 10;
  OZ = 0;
  ON = -10;
  OPS = 5;
  ONS = -5;
  //pwm = 1;
  pwm_max = 1020;
  pwm_min = 0;
}
 
void loop()
{
   reading = digitalRead(PIR);
  //cahaya();
  if (reading != lastButtonState) {
  Serial.println("Detected");
  Serial.println(bh_value);
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay10) event_t = true;
  else if ((millis() - lastDebounceTime) > debounceDelay2) event_t = false;
  lastButtonState = reading;
  if(event_t){
    utama();
    Serial.println(pwm);
  }
  else{
  Serial.println("Not Detected");
    if (pwm < pwm_min)
    {
      pwm = pwm_min;
    }
    else if(pwm > pwm_max)
    {
      pwm = pwm_max;
    }
    else
    {
      pwm = pwm-3;
    }
  }
  gerak();
}

void utama() 
{
  float intensity = lightMeter.readLightLevel(); //baca intensitas
  bh_value = intensity;
  //Serial.print("intensity= ");Serial.println(bh_value);
  e  = SP - bh_value;
  de = e - e_before;
  fuzifikasi();
  inference();
  defuzifikasi();
  //Serial.print("error = ");Serial.println(e);
  //Serial.print("delta error = ");Serial.println(de);
  //Serial.print("keluaran=");Serial.println(keluaran);
  analogWrite(led, pwm);
  //serial.println(pwm);
  clr();
  e_before = e;
  
  //Serial.println(" ");
  delay (130);
}

void clr()
{
  Ne = 0;
  Ze = 0;
  Pe = 0;
  Nde = 0;
  Zde = 0;
  Pde = 0;
  uNe = 0;
  uZe = 0;
  uPe = 0;
  uNde = 0;
  uZde = 0;
  uPde = 0;
  utotal =0;
  outfuz =0; 
}

void fuzifikasi()
{
  //cek bagian error
  // cek bagian Ne
  if (e<0)  
  {
    Ne =1;
    if(e<= -60)uNe = 1;
    else uNe = -e/60;
    //Serial.print("uNe=");Serial.println(uNe);
  }
  
  //cek bagian Ze
  if (e<1 && e>-1)
  {
    Ze= 1;
    if (e>0)uZe = -(e-1)/1;
    else uZe = (e+1)/1;
    //Serial.print("uZe=");Serial.println(uZe);
  }

  //cek bagian Pe
  if (e>0)
  {
    Pe =1;
    if(e>= 60)uPe = 1;
    else uPe = e/60;
    //Serial.print("uPe=");Serial.println(uPe);
  }

  //cek bagian derror
  if (de<0)
  {
    Nde =1;
    if(de<= -60)uNde = 1;
    else uNde = -de/60;
    //Serial.print("uNde=");Serial.println(uNde);
  }

  //cek bagian Ze
  if (de<1 && de>-1)
  {
    Zde= 1;
    if (de>0)uZde = -(de-1)/1;
    else uZde = (de+1)/1;
    //Serial.print("uZde=");Serial.println(uZde);
  }

  //cek bagian Pe
  if (de>0)
  {
    Pde =1;
    if(de>= 60)uPde = 1;
    else uPde = de/60;
    //Serial.print("uPde=");Serial.println(uPde);
  } 
}

void inference()
{
  if (Ne ==1 && Nde ==1)
  {
    outfuz = outfuz + (min(uNe, uNde)*ON);
    utotal = utotal + min(uNe,uNde);
  }
  if (Ne ==1 && Zde ==1)
  {
    outfuz = outfuz + (min(uNe, uZde)*ON);
    utotal = utotal + min(uNe,uZde);
  }
  if (Ne ==1 && Pde ==1)
  {
    outfuz = outfuz + (min(uNe, uPde)*ON);
    utotal = utotal + min(uNe,uPde);
  }
  
  if (Ze ==1 && Nde ==1)
  {
    outfuz = outfuz + (min(uZe, uNde)*ONS);
    utotal = utotal + min(uZe,uNde);
  }

  
  if (Ze ==1 && Zde ==1)
  {
    outfuz = outfuz + (min(uZe, uZde)*OZ);
    utotal = utotal + min(uZe,uZde);
  }
  if (Ze ==1 && Pde ==1)
  {
    outfuz = outfuz + (min(uZe, uPde)*OPS);
    utotal = utotal + min(uZe,uPde);
  }
  
  if (Pe ==1 && Nde ==1)
  {
    outfuz = outfuz + (min(uPe, uNde)*OP);
    utotal = utotal + min(uPe,uNde);
  }
  if (Pe ==1 && Zde ==1)
  {
    outfuz = outfuz + (min(uPe, uZde)*OP);
    utotal = utotal + min(uPe,uZde);
  }
  if (Pe ==1 && Pde ==1)
  {
    outfuz = outfuz + (min(uPe, uPde)*OP);
    utotal = utotal + min(uPe,uPde);
  }
}

void defuzifikasi()
{
  keluaran = outfuz/utotal;
  
  if (pwm < pwm_min)
  {
    pwm = pwm_min;
  }
  else if(pwm > pwm_max)
  {
    pwm = pwm_max;
  }
  else
  {
    pwm = pwm + keluaran;
  }
}
