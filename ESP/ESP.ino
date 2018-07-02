//Map_pins:
/*
 * #Relay D1
 * Flow D2
 */

//Libraries:
//Import the libraries of...
//Wifi-->
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

//MQTT-->
#include <PubSubClient.h>
//defines of id mqtt and topics for publish and subscribe
#define FLOW_PUBLISH   "/sensor/FlowSensor", "/sensor/FlowDifference"
#define Relay_SUBSCRIBE   "/sensor/Relay"   //topic MQTT to receive information from the Broker(Raspberry Pi)
#define ID_MQTT  "Anderson"

//Variables and Global objects:
//Wifi-->
const char* SSID = "flow";
const char* PASSWORD = "flowmeter";

ESP8266WiFiMulti WiFiMulti;

//Flow-->
float count = 0.0; 
float lastvalue = 0.0;
const int  Flow = 4; // variable for D2 pin
const int FCorreccion = (0.5 / 1765);
int time_without_value = 0;
float difference;

//Relay-->
const int Relay = D1;
char* Relay_status = "OFF";

//MQTT-->
WiFiClient espClient; // Create the object espClient
PubSubClient MQTT(espClient); // Instance the Client MQTT passing the object espClient

const char* BROKER_MQTT = "192.168.43.110"; //URL do broker MQTT that you want to user
int BROKER_PORT = 1883; // Port of Broker MQTT


//Prototypes(Methods):
//Serial-->
void initSerial();
void InitOutputs(void);

//Wifi-->
void initWiFi();
void reconectWiFi();
void VerificaConexionesWiFIyMQTT(void);

//Flow-->
void getAmountOfBeer();
void initFlow();
void SendFlowmeter(void);

//MQTT-->
void initMQTT();
void reconnectMQTT();
void mqtt_callback(char* topic, byte* payload, unsigned int length);

void setup() {
  initSerial();
  InitOutputs();
  initWiFi();
  initMQTT();
}


//Functions:
//Function: Inicialize the Serial
void initSerial()
{
    Serial.begin(115200);
}

//Function: Inicialize the outputs and inputs
void InitOutputs(void)
{
  //Relay-->
  pinMode(Relay, OUTPUT);
  digitalWrite(Relay,0);
  //Flow-->
  pinMode(Flow, INPUT);
}

//Function: Inicialize and connect on the WI-FI
void initWiFi()
{
    delay(10);
    Serial.println("------Conection WI-FI with NodeMCU -----");
    Serial.print("Connecting on WI-FI: ");
    Serial.println(SSID);
    Serial.println("Wait");
    // We start by connecting to a WiFi network
    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP(SSID, PASSWORD);

    if (WiFiMulti.run() != WL_CONNECTED)
      reconectWiFi();
}


//Function: Inicialize parameters of connetion MQTT
//Parameters: nothing
//Retorn: nothing
void initMQTT()
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);
    MQTT.setCallback(mqtt_callback);
}

//Function: reconnect to WiFi
//Parameters: nothing
//Retorn: nothing
void reconectWiFi()
{
    Serial.print("Wait for WiFi... ");

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
}

//Function: reconnect to broker MQTT
void reconnectMQTT()
{
    while (!MQTT.connected())
    {
        Serial.print("* Trying connect to Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT))
        {
            Serial.println("Connected to broker MQTT!");
            MQTT.subscribe(Relay_SUBSCRIBE);
        }
        else
        {
            Serial.println("Fail to reconnect with broker.");
            Serial.println("There will be another connection in 2s");
            delay(2000);
        }
    }
}

//Function: reconnect to broker MQTT and WI-FI
void VerificaConexionesWiFIyMQTT(void)
{
    if (!MQTT.connected())
      reconnectMQTT(); //reconnect with the broker

    if (WiFiMulti.run() != WL_CONNECTED)
      reconectWiFi(); //reconnect on WI-Fi
}


//Function: function callback
//        this funtion will be called everytime that one information come from the broker
void mqtt_callback(char* topic, byte* payload, unsigned int length)
{
    String msg;

    //Get the string of payload received
    for(int i = 0; i < length; i++)
    {
       char c = (char)payload[i];
       msg += c;
    }

    if (msg.equals("ON"))
    {
        digitalWrite(Relay, 1);

        Relay_status = "ON";
        delay(6000);
        Serial.println(Relay_status);
    }

    if (msg.equals("OFF"))
    {
        digitalWrite(Relay, 0);

        Serial.println(Relay_status);
        Relay_status = "OFF";
    }
}


//Function: Get the amount of beer
void getAmountOfBeer()
{
  time_without_value = 0;
  count = count + FCorreccion;
  if (lastvalue != 0.0){
    difference = count - lastvalue;
  }
  lastvalue = count;
}


//Function: in order to not send in each reading
void duration()
{
  if ((count > 0.0) && (time_without_value > 3)){
    SendFlowmeter();
  }

  time_without_value++;
}

//Function: Send to Raspberry Pi the Flow value
void SendFlowmeter(void)
{
    static char Flow[7];
    //double to string format, 6 cifras, 6 decimales
    dtostrf(count,6,6,Flow);
    static char Dif[7];
    dtostrf(difference,6,6,Dif);

    MQTT.publish("/sensor/FlowSensor", Flow);
    MQTT.publish("/sensor/FlowDifference", Dif);

    Serial.print("Flow =");
    Serial.println(count,DEC);
    Serial.print("Diference = ");
    Serial.print(difference,DEC);
    Serial.println("-Values sent to Raspberry(Server)");

    //Restart de variables for the next iteration
    count = 0.0;
    lastvalue = 0.0;
    
  }



void loop() {
  //If need reconnect with the WI-FI and MQTT Broker
  VerificaConexionesWiFIyMQTT();

  //For know if we have to sent
  duration();
  
  //Get amount of beer
  if (digitalRead(Flow) == HIGH){
  getAmountOfBeer();
  }
  
  //keep-alive the comunication with the Broker MQTT
  MQTT.loop();

  // We wait 1 second so due to the duration() function if in 3 seconds there are no reads for flow sensor
  // we will send the message.
  delay(1000);
}
