
#include <ESP8266WiFi.h>


#define sensePin A0 //where the temp sensor is connected
int redLed=2;
int greenLed=3;
int sensorInput; //stores analog value sensed from sensepin
double temp;  //The variable we will use to store temperature in degrees.  
int valSensor = 1;
String apiKey = "SJXTO9XEL2OIYGI5";     //  Enter your Write API key from ThingSpeak
const char *ssid =  "Kaushik's wifi-5G";     // replace with your wifi ssid and wpa2 key
const char *pass =  "*******"; //WiFi password
const char* server = "api.thingspeak.com";  //api server 
 
#define echo 5 //echopin at 5
#define trig 4 // trig at 4
 
void setup() 
{
  Serial.begin(9600); //Start the Serial Port at 9600 baud (default)
  pinMode (trig,OUTPUT);
  pinMode(echo,INPUT);
  pinMode (sensePin, INPUT);
       Serial.begin(115200);
       delay(10);
       Serial.println("Connecting to ");
       Serial.println(ssid);
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println(WiFi.localIP());
 
}
void loop() {
  // put your main code here, to run repeatedly: 
  sensorInput = analogRead(sensePin);    //read the analog sensor and store it
  temp = (double)sensorInput / 1024;       //find percentage of input reading
  temp = temp * 5;                 //multiply by 5V to get voltage
  temp = temp - 0.5;               //Subtract the offset 
  temp = temp * 100; //converts to degrees   
  Connect_ThingSpeak(temp);
 delay(500);
  if(temp>38)
  {
    Serial.println("You are denied of service since your temperature is greater than 38 degrees");
    digitalWrite(redLed,HIGH);
      digitalWrite(greenLed,LOW);
      
  }
  else 
  {
    
    digitalWrite(greenLed,HIGH);
      digitalWrite(redLed,LOW);
    Serial.println("Welcome, proceed to sanitise your hands");
   if( distanceCheck()<15)
   {
     Serial.println("sanitiser has been dispensed");
     dispenseSanitiser();
   }
  }
}

int distanceCheck()
{
digitalWrite(trig, HIGH);
delay(10);
digitalWrite(trig, LOW);

int DURATION = pulseIn(echo, HIGH);

int DISTANCE = DURATION / 29.41;
  return DISTANCE;
}

void dispenseSanitiser()
{
  
}

void Connect_ThingSpeak(int temperature)
{
   WiFiClient client;
    
             
 
                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                         {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(temperature);
                          
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.print("temperature");
                             Serial.print(temperature);
                             Serial.println("%. Send to Thingspeak.");
                        }
          client.stop();
 
          Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates
  delay(1000);
}
