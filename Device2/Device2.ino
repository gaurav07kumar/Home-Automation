#include "painlessMesh.h"
#include "Adafruit_Si7021.h"

//#define DHTPIN D2        // Digital pin connected to the DHT sensor
//#define DHTTYPE DHT11    // Change to DHT22 if you're using that sensor

#define   MESH_PREFIX     "Home_Automation"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5555

int Intensity;
int Temperature;
int s;
const int fan_pin = 15;
const int wm_pin = 0;

Scheduler userScheduler; 
painlessMesh  mesh;
Adafruit_Si7021 sensor = Adafruit_Si7021();
//DHT dht(DHTPIN, DHTTYPE);

void sendmsg() ;

Task SendTemperature( TASK_SECOND * 1 , TASK_FOREVER, &sendmsg );
//
void sendmsg() {
  Temperature = sensor.readTemperature();
  String msg = String(Temperature);
  if (isnan(Temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    String null_message = "Failed to read temperature";
    mesh.sendBroadcast( null_message );
  }
  else{
    mesh.sendBroadcast( msg );
    Serial.print("Temperature is : ");
    Serial.println(msg);
    fan(Temperature);
  }
  SendTemperature.setInterval( TASK_SECOND * 15);
}


void receivedCallback( uint32_t from, String &msg ) {
  if(from == 2134097685){
    Intensity = msg.toInt();
    bulb(Intensity);
  }
  if(from == 3265139431){
    s = msg.toInt();
    washingMachine(s);
  }
  
//  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
//    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);
  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    while (true)
      ;
  }
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(fan_pin, OUTPUT);
  pinMode(wm_pin, OUTPUT);
//  dht.begin();
//  randomSeed(analogRead(0));
  mesh.setDebugMsgTypes( ERROR | STARTUP );  

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( SendTemperature );
  SendTemperature.enable();
}
//ADAFRUIT
void washingMachine(int s){
  if(s == 1){
    digitalWrite(wm_pin, HIGH);
    Serial.println("Washing Machine STATUS : ON");
  }
  else if(s == 0){
    digitalWrite(wm_pin, LOW);
    Serial.println("Washing Machine STATUS : OFF");
  }
}
void bulb(int Intensity){
  if(Intensity > 5){
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("BULB STATUS : OFF");
  }
  else{
     digitalWrite(LED_BUILTIN, LOW);
     Serial.println("BULB STATUS : ON");
  }  
}
void fan(int Temperature){
  if(Temperature > 28){
    digitalWrite(fan_pin, HIGH);
    Serial.println("FAN STATUS : ON");
  }
  else{
     digitalWrite(fan_pin, LOW);
     Serial.println("FAN STATUS : OFF");
  }  
}
void loop() {
  mesh.update();
//  delay(5000);
}
