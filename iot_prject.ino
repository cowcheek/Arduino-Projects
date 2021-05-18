#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#define RX 2
#define TX 3
String AP = "Raj";       // AP NAME
String PASS = "insert password here"; // AP PASSWORD
String API = "100R35B0CN8P4DKQ";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field2";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
SoftwareSerial esp8266(RX,TX);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
int pump=4;//where the digital output for relay is connected

 
#define echo 5//echopin at 8
#define trig 6 // trig at 9
 
void setup() 
{
  Serial.begin(9600); //Start the Serial Port at 9600 (default)
  pinMode (trig,OUTPUT);
  pinMode(echo,INPUT);
  pinMode(pump, OUTPUT);
  mlx.begin();
  
      
 
}
void loop() {
  double temp = GetTemp();  //gets value returned from GetTemp and stores in temp.
  Serial.println(temp);
 delay(3000);
  if(temp>38)
  {
    Serial.println("You are denied of service since your temperature is greater than 38 degrees");
    
    Connect_ThingSpeak(temp);
    delay(5000);
    
  }
  else 
  {
    Serial.println("Welcome, proceed to sanitise your hands");
    Connect_ThingSpeak(temp);
     delay(5000);
 while(distanceCheck()<15)
  {
    dispenseSanitiser();
  }
  
   
  }
}

int distanceCheck() // function that measures hand distance from Sensors
{
 digitalWrite(trig, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  double duration = pulseIn(echo, HIGH);
  // Calculating the distance
  int distance = duration * 0.034 / 2; 
  return distance;
}

void dispenseSanitiser() //function that dispenses sanitiser
{
  digitalWrite(pump, HIGH);
  delay(100);
  digitalWrite(pump,LOW);
  
}

double GetTemp() //function to read Object Temp in Celsius, returns to void loop
{
  double temp=mlx.readObjectTempC();
  return temp;
 
}

void Connect_ThingSpeak(int temperature) //connects to the web server via Wifi
{
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
   valSensor = temperature;
 String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(valSensor);
sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
}
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
