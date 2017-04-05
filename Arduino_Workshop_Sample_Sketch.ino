/* 
by Burak Aykan - IBM Cloud Ecosystem Developer
   e-mail: burak.aykan@tr.ibm.com
*/

#include <SPI.h>
#include <YunClient.h>
#include <PubSubClient.h>
#include <DHT.h>

void callback(char* topic, byte* payload, unsigned int length);
float getTemp();
float getHum();

//Creating Sensor Pins
const int pinTemp = A0, pinRed = 5, pinGreen = 4, trigPin = 3, echoPin = 2;

//Global Variables
char message_buff[100];

//Prepare functions
void callback(char* topic, byte* payload, unsigned int length);
String buildClientName();
String buildJson();
float getDistance();

//IBM Bluemix information
// *******************************************************************************************************
char orgName[] = "<ORGANIZATION_ID>"; // CHANGE THIS
char macstr[] = "<DEVICE_NAME>"; // CHANGE THIS
char server[] = "<ORGANIZATION_ID>.messaging.internetofthings.ibmcloud.com"; // CHANGE THIS
char type[] = "<DEVICE_TYPE>"; // CHANGE THIS
char token[] = "<DEVICE_TOKEN>"; // CHANGE THIS
int port = 1883;
String clientName = buildClientName();
String topicName = String("iot-2/cmd/+/fmt/json");
// **********************************************************************************************************

//Objects for Arduino works
YunClient yunClient;
PubSubClient client(server, port, callback, yunClient);
DHT dht(pinTemp, DHT11);

//Initialize Arduino
void setup(){
  Serial.begin(9600);
  Bridge.begin();
  dht.begin();
  pinMode(pinTemp, INPUT);
  pinMode(pinRed, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  delay(60000);
}

//Running code
void loop(){ 
  char clientStr[34];
  clientName.toCharArray(clientStr,34);
  char topicStr[26];
  topicName.toCharArray(topicStr,26);
  if (!client.connected()) {
    digitalWrite(pinRed, HIGH);
    digitalWrite(pinGreen, LOW);
    Serial.print("Trying to connect to: ");
    Serial.println(clientStr);
    client.connect(clientStr, "use-token-auth", token);
    if (client.connect(clientStr, "use-token-auth", token)) {
      client.setCallback(callback);
      Serial.println("Able to connect to Bluemix during setup ...");
      if (client.subscribe("iot-2/cmd/cid/fmt/json", 0)) {
        Serial.println("Subscribed to iot-2/cmd/cid/fmt/json");      
      } else {
        Serial.println("NOT Subscribed to iot-2/cmd/cid/fmt/json");      
      }
    }
  }
  
  if(client.connected()) {
      digitalWrite(pinRed, LOW);
      digitalWrite(pinGreen, HIGH);
      //Serial.println("Success getting online...Begin transmit...");
    
      // Build the JSON data to publish.
      String json = buildJson();
      char jsonStr[200];
      json.toCharArray(jsonStr,200);

      Serial.println(jsonStr);
    
    // Publish the data.
      client.publish("iot-2/evt/status/fmt/json", jsonStr);
  }
}

// Builds the clientName
String buildClientName (){
  String data = "";
  data+="d:";
  data+=orgName;
  data+=":";
  data+=type;
  data+=":";
  data+=macstr;
  return data;
}

// Sends the JSON
String buildJson() {
  char buffer[60];
  String data = "{\"d\":{\"Temperature\":";
  dtostrf(getTemp(), 1, 2, buffer);
  data += buffer;
  data += ",\"Humidity\":";
  dtostrf(getHum(), 1, 2, buffer);
  data += buffer;
  data += ",\"Distance\":";
  dtostrf(getDistance(), 1, 2, buffer);
  data += buffer;
  data += "}}";
  return data;
}

// Getting Temperature and Humidity Values
void callback(char* topic, byte* payload, unsigned int length){}
float getTemp(){
  float t = dht.readTemperature();
  return t;
}
float getHum(){
  float h = dht.readHumidity();
  return h;
}

float getDistance(){
  long duration;
  float distance;
  
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculating the distance
  distance= duration*0.034/2;  
  return distance;
}
