#include <Wire.h>

#include "BluetoothSerial.h"

#define TRIG_PIN 22
#define ECHO_PIN 13

// Check if Bluetooth configs are enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Bluetooth Serial object
BluetoothSerial SerialBT;
// GPIO where LED is connected to
const int ledPin =  17;

// Variables to hold sensor readings
float distance = 0;

// Stores the elapsed time from device start up
unsigned long elapsedMillis = 0; 
// The frequency of sensor updates to firebase, set to 10seconds
unsigned long update_interval = 1000; 


// Handle received and sent messages
String message = "";
char incomingChar;
String temperatureString = "";

void updateSensorReadings() {
  Serial.println("------------------------------------");
  Serial.println("Reading Sensor data ...");
  digitalWrite(ECHO_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  unsigned long time = pulseIn(ECHO_PIN, HIGH);
  distance = time * 0.000001 * 34300.0 / 2.0;
  Serial.printf("Distance reading: %.2f \n", distance);
  delay(500);
}

String readDistance() {
     return String(distance);
}

void bluetooth_Init(){
  pinMode(ledPin, OUTPUT);
  
  delay(100);
  // Bluetooth device name
  if (!SerialBT.begin("ESP32")){
    Serial.println("No se encontro bluetooth");
  }else {
    Serial.println("bluetooth listo");
  }
  
}

void sendDataToBluetooth(){

    Serial.println("Sending Data to Bluetooth ");
    
    SerialBT.println(String(F("Distance = "))+readDistance()+" CM");
    
    Serial.println("End transmission");
  
}
void readDataFromBluetooth(){
    // Read received messages
  if (SerialBT.available()){
    char incomingChar = SerialBT.read();
    if (incomingChar != '\n'){
      message += String(incomingChar);
    }
    else{
      message = "";
    }
    Serial.write(incomingChar);  
  }
    // Check received message and control output accordingly
    if (message =="led_on"){
      digitalWrite(ledPin, HIGH);
    }
    else if (message =="led_off"){
      digitalWrite(ledPin, LOW);
    }
    else if (message =="hello"){
      SerialBT.println("holis");  
    }
    delay(3);
}

void uploadSensorData() {
  if (millis() - elapsedMillis > update_interval){
    elapsedMillis = millis();
    updateSensorReadings();
    sendDataToBluetooth();
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  bluetooth_Init();
}



void loop() {
    uploadSensorData();
    readDataFromBluetooth();
}
