#include "painlessMesh.h"

#define   MESH_PREFIX     "Home_Automation"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5555

int Intensity;
int Temperature;
const int fan_pin = 4;

Scheduler userScheduler; 
painlessMesh  mesh;

void sendmsg() ;

Task taskSendmsg( TASK_SECOND * 1 , TASK_FOREVER, &sendmsg );

void sendmsg() {
  Intensity = random(0,10);
  String msg = String(Intensity);
  mesh.sendBroadcast( msg );
  Serial.print("Luminous Intensity is : ");
  Serial.println(Intensity);
  bulb(Intensity);
  taskSendmsg.setInterval( TASK_SECOND * 15);
}


void receivedCallback( uint32_t from, String &msg ) {
  if(from == 2489206864){
    Temperature = msg.toInt();
    fan(Temperature);
//    Serial.println(Temperature);
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
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(fan_pin, OUTPUT);
//  randomSeed(analogRead(0));
  mesh.setDebugMsgTypes( ERROR | STARTUP );  

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendmsg );
  taskSendmsg.enable();
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
