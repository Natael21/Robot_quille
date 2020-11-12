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

<<<<<<< HEAD
const int PIN_SIFFLET = 13;
const int SIFFLET_MIN = 620;
const int SIFFLET_MAX = 710;
=======
//ANALOG IN
const int PIN_SIFFLET = 13;
const int SIFFLET_MIN = 620;
const int SIFFLET_MAX = 710;

>>>>>>> 575a6df0ff0ef2434450d18f70e99a164f1d445c
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

<<<<<<< HEAD
void tourne_gauche(float);

void tourne_droite(float);

int detection_sifflet();
=======
int detection_sifflet();

>>>>>>> 575a6df0ff0ef2434450d18f70e99a164f1d445c
//void suiveur_ligne();

float distanceTotalePulse = distance_pulse(500);

void setup()
{
  BoardInit();
<<<<<<< HEAD
  distanceTotalePulse = distance_pulse(405);
  //while(detection_sifflet() != 1){}
  while (!ROBUS_IsBumper(3))
  {
  }
=======
  distanceTotalePulse = distance_pulse(430);
  while(detection_sifflet() != 1){}
  //while(!ROBUS_IsBumper(3)){}
>>>>>>> 575a6df0ff0ef2434450d18f70e99a164f1d445c
  ligne_droite(35, SPEED_QUILLE, SPEED_QUILLE);
}
void loop()
{
  uint32_t pulse_droit = ENCODER_Read(RIGHT);
  uint32_t pulse_gauche = ENCODER_Read(LEFT);
  Serial.print("pulses lus: ");
  Serial.println((pulse_droit + pulse_gauche) / 2);
  Serial.print("distance totale:");
  Serial.println(distanceTotalePulse);
  float distance_sonar = distance_mur();
  if (distance_sonar <= 60 && distance_sonar >= 20 && !quilleAEteDetectee)
  {
    /*if(ENCODER_Read(0) >= distance_pulse(205) && ENCODER_Read(0) <= distance_pulse(235))
    {
      Serial.println("delai commencer");
      delay(10000);
    }*/
    distanceTotalePulse -= (pulse_droit + pulse_gauche) / 2;
    ligne_droite(6, SPEED_QUILLE, SPEED_QUILLE);
    delay(200);
    tourne90(LEFT);
    Serial.println("tourne90 GAUCHE");
    ligne_droite(55, SPEED_QUILLE, SPEED_QUILLE);
    Serial.println("ligne droite");
    delay(200);
    tourne90(RIGHT);
    Serial.println("tourne90 DROITE");
    //ligne_droite(distanceTotalePulse / 3200 * 23.94, SPEED_QUILLE, SPEED_QUILLE);
    Serial.println("ligne droite 2");
    quilleAEteDetectee = true;
  }
  else if ((pulse_droit + pulse_gauche) / 2 >= distanceTotalePulse)
  {
    speed = 0;
    setSameSpeed_MOTORS(speed);
    Serial.println("fin");
    if(!quilleAEteDetectee){
      tourne90(LEFT);
      ligne_droite(50, SPEED_QUILLE, SPEED_QUILLE);
      tourne90(RIGHT);
      speed = 0;
      setSameSpeed_MOTORS(speed);
      quilleAEteDetectee = true;
    }
    //Finir en tournant à gauche
  }
  else
  {
    if (speed <= SPEED_QUILLE)
    {
      speed += 0.005;
    }
    delay(MAGIC_DELAY_LD);
    correction_moteurs(pulse_gauche, pulse_droit);
    Serial.println("speed");
  }
  Serial.println(speed);
}

//-----------------------Fonctions Capteurs:----------------------------

int detection_sifflet(){
  int voltage;
  voltage = analogRead(PIN_SIFFLET);
  Serial.print(voltage);
  Serial.print("\n");
  if (voltage >= SIFFLET_MIN)
  {
    return 1;
  }
  else 
  {
    return 0;
  }
}



float distance_mur()
{
  const int NB_VERIFICATIONS = 1;
  float tab_distance[NB_VERIFICATIONS] = {};
  for (int i = 0; i < NB_VERIFICATIONS; i++)
  {
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
    */
  return tab_distance[NB_VERIFICATIONS - 1]; /*
  }
  else {
    return 500.0;
  }*/
}

//--------------------Fonctions Défi du parcours:-------------------------
int detection_sifflet()
{
  int voltage;
  voltage = analogRead(PIN_SIFFLET);
  Serial.print(voltage);
  Serial.print("\n");
  if (voltage >= SIFFLET_MIN)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

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

  if (pulse_droit > pulse_gauche)
  {
    erreur = (pulse_droit - pulse_gauche) * MAGIC_NUMBER;
    MOTOR_SetSpeed(LEFT, speed + erreur);
    MOTOR_SetSpeed(RIGHT, speed - erreur);
  }
  else
  {
    erreur = (pulse_gauche - pulse_droit) * MAGIC_NUMBER;
    MOTOR_SetSpeed(LEFT, speed - erreur);
    MOTOR_SetSpeed(RIGHT, speed + erreur);
  }
}

void ligne_droite(int distance, float vitesseMax, float vitesseMin)
{
  reset_ENCODERS();

  uint32_t pulse_droit = 0;
  uint32_t pulse_gauche = 0;
  uint32_t pulse_attendu = distance_pulse(distance);

  while (pulse_droit <= pulse_attendu)
  {
    pulse_droit = ENCODER_Read(RIGHT);
    pulse_gauche = ENCODER_Read(LEFT);

    if (pulse_droit <= pulse_attendu * 0.6) // || pulse_gauche <= pulse_attendu * 0.8
    {
      if (speed <= vitesseMax)
      {
        speed += 0.005; //2 secondes: 0 => 0.4 ou 2 secondes: 0 => 0.5 pour 0.00625
      }
    }

    else if (pulse_droit >= pulse_attendu * 0.6 && pulse_droit <= pulse_attendu * 0.9)
    {
      if (speed >= vitesseMin)
      {
        speed -= 0.005; //1 seconde: 0.5 => 0.3 (-0.2)
      }
    }
    setSameSpeed_MOTORS(speed);

    correction_moteurs(pulse_gauche, pulse_droit);
    delay(MAGIC_DELAY_LD); //40 fois / seconde
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
    if (pulse_gauche < -(int)pulse_distance)
      MOTOR_SetSpeed(LEFT, 0);
  }
}

void tourne90(uint8_t direction)
{
  setSameSpeed_MOTORS(0);
  delay(200);
  reset_ENCODERS();

  float angle = 42.5;

  int32_t pulse_distance_rotation = distance_pulse(distance_angle(angle));
  int32_t sous_pulse_droit = 0;
  int32_t sous_pulse_gauche = 0;
  if (direction == LEFT)
  {
    while (sous_pulse_droit <= pulse_distance_rotation)
    {
      sous_pulse_droit = ENCODER_Read(RIGHT);
      sous_pulse_gauche = ENCODER_Read(LEFT);

      MOTOR_SetSpeed(RIGHT, speed);
      MOTOR_SetSpeed(LEFT, -speed);
      delay(20);

      if (sous_pulse_droit > pulse_distance_rotation)
        MOTOR_SetSpeed(RIGHT, 0);
      if (sous_pulse_gauche < -pulse_distance_rotation)
        MOTOR_SetSpeed(LEFT, 0);
    }
  }
  else
  {
    while (sous_pulse_gauche <= pulse_distance_rotation)
    {
      sous_pulse_droit = ENCODER_Read(RIGHT);
      sous_pulse_gauche = ENCODER_Read(LEFT);
      MOTOR_SetSpeed(RIGHT, -speed);
      MOTOR_SetSpeed(LEFT, speed);
      delay(20);

      if (sous_pulse_droit < -pulse_distance_rotation)
        MOTOR_SetSpeed(RIGHT, 0);
      if (sous_pulse_gauche > pulse_distance_rotation)
        MOTOR_SetSpeed(LEFT, 0);
    }
  }
  speed = 0;
  setSameSpeed_MOTORS(speed); //MODIFIÉs
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

  if (idMoteur == 0)
  {
    autreMoteur = 1;
    Serial.print("tourne à gauche de ");
    Serial.println(angle);
    Serial.print("Moteur: ");
    Serial.println(autreMoteur);
  }
  else
  {
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
void tourne_gauche(float angle)
{
  //Serial.println("troune_gauche");
  while (ENCODER_Read(1) <= (-1 * angle * 16000))
  {
    MOTOR_SetSpeed(0, 0);
    MOTOR_SetSpeed(1, 0.35);
  }

  ENCODER_Reset(0);
  ENCODER_Reset(1);
}

void tourne_droite(float angle)
{
  while (ENCODER_Read(0) <= angle * 16000)
  {
    MOTOR_SetSpeed(0, 0.35);
    MOTOR_SetSpeed(1, 0);
  }

  ENCODER_Reset(0);
  ENCODER_Reset(1);
}
