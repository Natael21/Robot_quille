#include <Arduino.h>
#include <LibRobus.h>
//#include <PololuQTRSensors.h>

//Constantes Défi du parcours:
//const float MAGIC_NUMBER = 0.00035;
const float MAGIC_NUMBER = 0.0004;
const int MAGIC_DELAY_LD = 25;
const uint32_t PPT = 3200;
const float RAYON = 18.9;
const float SPEED_ANGLE = 0.25;
const float SPEED_QUILLE = 0.3;
//Constantes Épreuve du combattant:

float speed = 0;
bool quilleAEteDetectee = false;

//map de l'arduino
//PWM
const int PIN_B = 8;
const int PIN_R = 3;
const int PIN_J = 2;

//float SONAR_GetRange(uint8_t 0); un seul sonnar, dans port 1

void reset_ENCODERS();

void setSameSpeed_MOTORS(float);

uint32_t distance_pulse(float);

float distance_angle(float);

void correction_moteurs(uint32_t, uint32_t);

void ligne_droite(int, float, float);

void tourne180();

void tourne90(uint8_t);

void tourne(uint8_t idMoteur, float angle);

void detection_couleur();

float distance_mur();

//void suiveur_ligne();

float distanceTotalePulse = distance_pulse(500);

void setup() 
{
  BoardInit();
  distanceTotalePulse = distance_pulse(435);
  while(!ROBUS_IsBumper(3)){}
  ligne_droite(35, SPEED_QUILLE, SPEED_QUILLE);
}
void loop(){
  uint32_t pulse_droit = ENCODER_Read(RIGHT);
  uint32_t pulse_gauche = ENCODER_Read(LEFT);
  
  if(distance_mur() <= 60 && distance_mur() >= 20 && !quilleAEteDetectee){
    
    distanceTotalePulse -= (pulse_droit + pulse_gauche)/2;
    delay(200);
    tourne90(LEFT);
    ligne_droite(70, SPEED_QUILLE, SPEED_QUILLE);
    tourne90(RIGHT);
    quilleAEteDetectee = true;
  }
  else if((pulse_droit + pulse_gauche)/2 >= distanceTotalePulse){
    setSameSpeed_MOTORS(0);
    Serial.println("fin");
  }
  else
  {
    if(speed <= SPEED_QUILLE){
      speed += 0.005;
    }
    delay (MAGIC_DELAY_LD);
    correction_moteurs(pulse_gauche, pulse_droit);
    
  }
  Serial.println(speed);
}

//-----------------------Fonctions Capteurs:----------------------------
float distance_mur()
{
  const int NB_VERIFICATIONS = 1;
  float tab_distance[NB_VERIFICATIONS] = {};
  for(int i = 0; i < NB_VERIFICATIONS; i++){
    tab_distance[i] = SONAR_GetRange(0);
  }
  /*
  bool memeDistances = true;
  for(int i = 1; i < NB_VERIFICATIONS; i++){
    if(tab_distance[0] != tab_distance[i]){
      memeDistances = false;
    }
  }
  Serial.println(tab_distance[NB_VERIFICATIONS-1]);
  if(memeDistances){
    */return tab_distance[NB_VERIFICATIONS-1];/*
  }
  else {
    return 500.0;
  }*/
}

/*void suiveur_ligne()
{
  pinMode(8, OUTPUT);
  digitalWrite(8, 1);
  while(ENCODER_Read(0) < 68000)
  {
    int pin1 = analogRead(A4);
    int pin2 = analogRead(A5);
    int pin6 = analogRead(A9);
    int pin7 = analogRead(A10);
    Serial.print(pin1);
    Serial.print("\t");
    Serial.print(pin2);
    Serial.print("\t");
    Serial.print(pin6);
    Serial.print("\t");
    Serial.print(pin7);
    Serial.print("\n");
    setSameSpeed_MOTORS(0.2);
    if(pin1 < 40)
    {
      MOTOR_SetSpeed(0, 0.22);
      MOTOR_SetSpeed(1, 0.2);

      //tourne un peu a droite
    }
    if(pin7 < 40)
    {
      MOTOR_SetSpeed(1, 0.21);
      MOTOR_SetSpeed(0, 0.20);
      //tourne un peu à gauche
    }
    if(pin2 >= 40)
    {
      MOTOR_SetSpeed(1, 0.21);
      MOTOR_SetSpeed(0, 0.20);
      //tourne un peu à gauche
    }
    if(pin6 >= 40)
    {
      MOTOR_SetSpeed(1, 0.2);
      MOTOR_SetSpeed(0, 0.21);
      //tourne un peu à droite
    }
  }
  
  delay(100);
}*/

//--------------------Fonctions Défi du parcours:-------------------------

void reset_ENCODERS()
{
  ENCODER_Reset(RIGHT);
  ENCODER_Reset(LEFT);
}

void setSameSpeed_MOTORS(float sous_speed)
{
  MOTOR_SetSpeed(RIGHT, sous_speed);
  MOTOR_SetSpeed(LEFT, sous_speed);
}

uint32_t distance_pulse(float distance)
{
  uint32_t pulse_attendu = distance / 23.94 * PPT;
  return pulse_attendu;
}

float distance_angle(float angle)
{
  float distance = (2 * PI * angle * RAYON) / 360;
  return distance;
}

void correction_moteurs(uint32_t pulse_gauche, uint32_t pulse_droit)
{
  float erreur;
  
  if(pulse_droit > pulse_gauche) {
    erreur = (pulse_droit - pulse_gauche) * MAGIC_NUMBER;
    MOTOR_SetSpeed(LEFT, speed + erreur);
    MOTOR_SetSpeed(RIGHT, speed - erreur);
  }
  else {
    erreur = (pulse_gauche - pulse_droit) * MAGIC_NUMBER;
    MOTOR_SetSpeed(LEFT, speed - erreur);
    MOTOR_SetSpeed(RIGHT, speed + erreur);
  }
}

void ligne_droite(int distance, float vitesseMax, float vitesseMin){
  reset_ENCODERS();
  
  uint32_t pulse_droit = 0;
  uint32_t pulse_gauche = 0;
  uint32_t pulse_attendu = distance_pulse(distance);

  while (pulse_droit <= pulse_attendu)
  {
    pulse_droit = ENCODER_Read(RIGHT);
    pulse_gauche = ENCODER_Read(LEFT);
    
    if(pulse_droit <= pulse_attendu * 0.6 )// || pulse_gauche <= pulse_attendu * 0.8
    {
      if(speed <= vitesseMax)
      {
        speed += 0.005;//2 secondes: 0 => 0.4 ou 2 secondes: 0 => 0.5 pour 0.00625
      }
    }

    else if(pulse_droit >= pulse_attendu * 0.6 && pulse_droit <= pulse_attendu * 0.9)
    {
      if(speed >= vitesseMin)
      {
        speed -= 0.005;//1 seconde: 0.5 => 0.3 (-0.2)
      }
    }
    setSameSpeed_MOTORS(speed);

    correction_moteurs(pulse_gauche, pulse_droit);
    delay (MAGIC_DELAY_LD);//40 fois / seconde
  }
  reset_ENCODERS();
}

void tourne180()
{
  delay(200);
  reset_ENCODERS();
  setSameSpeed_MOTORS(0);
  
  float angle = 93;

  uint32_t pulse_distance = distance_pulse(distance_angle(angle));
  uint32_t pulse_droit = 0;
  int32_t pulse_gauche = 0;
  
  MOTOR_SetSpeed(RIGHT, SPEED_ANGLE);
  MOTOR_SetSpeed(LEFT, -SPEED_ANGLE);
  
  while (pulse_droit <= pulse_distance)
  {
    pulse_droit = ENCODER_Read(RIGHT);
    pulse_gauche = ENCODER_Read(LEFT);
    
    if (pulse_droit > pulse_distance)
      MOTOR_SetSpeed(RIGHT, 0);
    if (pulse_gauche < - (int) pulse_distance)
      MOTOR_SetSpeed(LEFT, 0);
  }
}

void tourne90(uint8_t direction){
  setSameSpeed_MOTORS(0);
  delay(200);
  reset_ENCODERS();

  float angle = 41;//43.5;

  uint32_t pulse_distance_rotation = distance_pulse(distance_angle(angle));
  uint32_t sous_pulse_droit = 0;
  int32_t sous_pulse_gauche = 0;

  while (sous_pulse_droit <= pulse_distance_rotation) {
    sous_pulse_droit = ENCODER_Read(RIGHT);
    sous_pulse_gauche = ENCODER_Read(LEFT);
    if(direction == LEFT){
      MOTOR_SetSpeed(RIGHT, speed);
      MOTOR_SetSpeed(LEFT, -speed);
      delay(20);
      
      if (sous_pulse_droit > pulse_distance_rotation)
        MOTOR_SetSpeed(RIGHT, 0);
      if (sous_pulse_gauche < - (int) pulse_distance_rotation)
        MOTOR_SetSpeed(LEFT, 0);
        
    }
    else {
      MOTOR_SetSpeed(RIGHT, -speed);
      MOTOR_SetSpeed(LEFT, speed); 
      delay(20);

      if (sous_pulse_droit < - (int) pulse_distance_rotation)
        MOTOR_SetSpeed(RIGHT, 0);
      if (sous_pulse_gauche > pulse_distance_rotation)
        MOTOR_SetSpeed(LEFT, 0);
      
    }
  }
  //setSameSpeed_MOTORS(0);//MODIFIÉs
  delay(200);
  reset_ENCODERS();
}

void tourne(uint8_t idMoteur, float angle)
{
  reset_ENCODERS();
  
  uint32_t pulse_distance = distance_pulse(distance_angle(angle));
  uint32_t pulse = 0;
  
  MOTOR_SetSpeed(idMoteur, 0);

  uint8_t autreMoteur;

  if(idMoteur == 0){
    autreMoteur = 1;
    Serial.print("tourne à gauche de ");
    Serial.println(angle);
    Serial.print("Moteur: ");
    Serial.println(autreMoteur);
  }
  else {
    autreMoteur = 0;
    Serial.print("tourne à droite de ");
    Serial.println(angle);
    Serial.print("Moteur: ");
    Serial.println(autreMoteur);
  }

  while (pulse <= pulse_distance)
  {
    pulse = ENCODER_Read(autreMoteur);
  }
  delay(200);
  MOTOR_SetSpeed(idMoteur, speed);
}


  
