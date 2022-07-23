# Fuzzy-Logic-Smart-Light
Library requarment:
1. WiFiClient.h
2. BH1750.h
3. Wire.h
4. Time.h
5. ESP8266WiFi.h
6. ESP8266WebServer.h
7. ESP8266HTTPClient.h

Device requarment:
1. Esp8266
2. Lux Sensor BH1750
3. Dimmer


Fuzzy Process:
1. Fuzzyfication
Membership function from error sensor--> Error value(e) = SP (Set point) - value sensor
![mf](https://user-images.githubusercontent.com/67867879/180611795-3b0aad8a-3dde-4789-8ba6-582e080573a6.PNG)

Membership function from delta error sensor--> Delta error value(de) = e - e_before
![mf1](https://user-images.githubusercontent.com/67867879/180611916-fd99d3b9-7012-42b4-b482-3a0e60c07013.PNG)

2. Inference

Rule table:

![s](https://user-images.githubusercontent.com/67867879/180611950-d8a378b2-cc5b-4913-8e5b-c07fc8401262.PNG)

3. Defuzzyfication --> Using sugeno method
