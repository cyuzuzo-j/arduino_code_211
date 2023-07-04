boolean withWifi = true; // Test your Arduino code with Serial Monitor, set on true to test with the App en Arduinoshield.

// ***************************************************************


// NIET AANPASSEN, start commando's voor de wifimodule en servomotors.
String startsequence = "CMDS/";
String stopsequence = "/CMDEND/";

String startsequenceSetup = "SETUPB/";
String stopsequenceSetup = "/SETUPE/";
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

//********************************BELANGRIJK***************************

///  Dit hangt af van het type servomotor! Deze waardes zijn voor de MG90's die 180° bereik heeft.
#define SERVOMIN 80  // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 409 // This is the 'maximum' pulse length count (out of 4096)
#include <DFRobot_HX711.h>
DFRobot_HX711 gewicht_sensor(gewicht_sensor_analog_clk, gewicht_sensor_analog_2);


float Current_Afstand(int trigPin, int echoPin) { // JJ meet afstand van bakje naar silo
  int metingen[3];
  for (int i = 0; i <= 2; i++) {

    // Clears the trigPin condition
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    int duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    int distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    // Displays the distance on the Serial Monitor

    metingen[i] = distance;
  }
  int waarde = metingen[1];

  return waarde;
}

float current_weight() {
  return gewicht_sensor.readWeight();
}
float calc_eta(int besteld_gewicht, int huidig_gewicht) {
  // komt wiskundig_model in
  return 0.0;

}
float weight_bakske() {
  return 0.0;
}
void displayESP32Setup(String command) {
  command = command.substring(command.indexOf(startsequenceSetup) + startsequenceSetup.length());
  command = command.substring(0, command.indexOf(stopsequenceSetup));
  Serial.println(command);
}
String checkwifi() {
  // Kijkt of er iets werd verzonden over wifi, ontvangt het en decodeert het ook.
  String command = "";

  if (withWifi)
  {
    // Read input from ESP32
    while (Serial2.available())
    {
      command = Serial2.readString();
    }
    // Haal de startsequence van het command
    if (command.indexOf(startsequence) > 0)
    {
      command = command.substring(command.indexOf(startsequence) + startsequence.length());
      command = command.substring(0, command.indexOf(stopsequence));
    }
    // Is het een setup commando?
    else if (command.indexOf(startsequenceSetup) > 0)
    {
      displayESP32Setup(command);
      command = "";
    }
    else
    {
      command = "";
    }
  }
  else
  {
    // Read input from Serial Monitor
    while (Serial.available())
    {
      command = Serial.readString();
    }
  }
  return command;
}

void sendWifi(String message) {
  if (withWifi)
  {
    Serial2.println(message);
  }
  else
  {
    Serial.print("Message for Wifi: ");
    Serial.println(message);
  }
}

String checkkleur() {
  // COLOR configs bevat de nodige combinaties om de sensor te vragen om R G of B values door te sturen
  boolean  COLORCONFIGS[3][2] = {{LOW, LOW}, {HIGH, HIGH}, {LOW, HIGH}};
  int color[3];
  for (int i = 0; i <= 2; i++) {
    // zet de benodigde pins aan om R/G/B aan te vragen
    digitalWrite(S2, COLORCONFIGS[i][0]);
    digitalWrite(S3, COLORCONFIGS[i][1]);
    //wacht om de sensor te stabiliseren
    delay(200);
    //leest de pulse bepaalde de intensiteit van de R/G/B waarde
    int PW = pulseIn(sensorOut, LOW);
    //remaped de kleuren naar 0-255 via de callibarite waarden
    int val =  map(PW, COLORCALLIBRATIES[i][0], COLORCALLIBRATIES[i][1], 255, 0);
    color[i] = val;
  }
  float d_wit = sqrt((color[0] - color_wit[0]) ^ 2 + (color[1] - color_wit[1]) ^ 2 + (color[2] - color_wit[2]) ^ 2);
  float d_bruin = sqrt((color[0] - color_bruin[0]) ^ 2 + (color[1] - color_bruin[1]) ^ 2 + (color[2] - color_bruin[2]) ^ 2);
  float d_zwartsqrt((color[0] - color_zwart[0]) ^ 2 + (color[1] - color_zwart[1]) ^ 2 + (color[2] - color_zwart[2]) ^ 2);
  if (d_wit <= d_bruin && d_wit <= d_zwartsqrt) {
    return "wit";
  }
  else if ( d_bruin <= d_wit && d_bruin <= d_zwartsqrt) {
    return "bruin";
  }
  else {
    return "zwart";
  }

}


void servoControl(String message_str) {
  int message = message_str.toInt(); // zelf bijgezet om problemen te voorkomen
  // message = number XXYYY with XX servonumber and YYY value
  int servoNumber = message / 1000;  // XX servonumber
  int servoPWM = message % 1000;     // YYY cornervalue between 0 and 256
  servoPWM = map(servoPWM, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(15 - servoNumber, 0, servoPWM);
}

void update_app(int besteld_gewicht) {
  int huidig_gewicht = current_weight();
  sendWifi("GEWICHT/" + String(huidig_gewicht));
  sendWifi("ETA/" + String(calc_eta(besteld_gewicht, huidig_gewicht)));
  String command = checkwifi();
  if (command.length() > 0) {
    Serial.println("Ontvangen commando: " + command); // Print het commando (debug)
    // Splits het command op in commando/parameters
    String commando = command.substring(0, command.indexOf('/'));
    String parameters = command.substring(command.indexOf('/') + 1);
    if (commando.equals("PAUZE")) {
      while (true) {
        command = checkwifi();
        if (command.length() > 0) {
          Serial.println("Ontvangen commando: " + command); // Print het commando (debug)
          // Splits het command op in commando/parameters
          String commando = command.substring(0, command.indexOf('/'));
          if (commando.equals("RESET")) {
            digitalWrite(resetpin,HIGH);
          }
          else if (commando.equals("CONTINU")){
            break;
            }
          }
 
        }
      }
    }

  }
