#include <Arduino.h>
#include <LibRobus.h>

void setup() 
{
  Serial.begin(9600);
  BoardInit();
}

void loop() {
  delay(2000);
  Serial.println(ROBUS_ReadIR(0));
}


  
