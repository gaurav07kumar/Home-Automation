#include "painlessMesh.h"
//#include <DHT.h>

//#define DHTPIN D2        // Digital pin connected to the DHT sensor
//#define DHTTYPE DHT11    // Change to DHT22 if you're using that sensor

#define   MESH_PREFIX     "Home_Automation"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5555

int Intensity;
int Temperature;
const int fan_pin = 4;

Scheduler userScheduler; 
painlessMesh  mesh;
//DHT dht(DHTPIN, DHTTYPE);

void sendmsg() ;

Task SendTemperature( TASK_SECOND * 1 , TASK_FOREVER, &sendmsg );
//
void sendmsg() {
//  Temperature = dht.readTemperature();
  if(Serial.available() > 0){
    int s = Serial.parseInt(); 
    String msg = String(s); 
    mesh.sendBroadcast( msg ); 
    Serial.print("You give input : ");
    Serial.println(msg);
    while(Serial.available()){
      Serial.read();
    }
  }
  SendTemperature.setInterval( TASK_SECOND * 1 );
}


void receivedCallback( uint32_t from, String &msg ) {
  if(from == 2134097685){
    Intensity = msg.toInt();
    bulb(Intensity);
  }
  if(from == 2489206864){
    Temperature = msg.toInt();
    fan(Temperature);
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
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(fan_pin, OUTPUT);
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
  if(Temperature > 30){
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
