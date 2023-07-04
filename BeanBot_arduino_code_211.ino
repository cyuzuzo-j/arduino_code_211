

//----------hyper parameter --------
int DELTA_AFSTANDS_DELAY5 = 1;
// aantal seconden om continu snelheid te halen met servo
int MACHINE_BEWEEG_DELAY = 2;
//----------mechanische eig---------
int LENTHE_RAIL = 10;
int AFSTAND_CONTAINER = 10;
//-------------------
int Dc_speed = 9;
int Dc_Richting = 11;

int RGB_led_R = 5;
int RGB_led_G = 3;
int RGB_led_B = 2;

int Magneet_sensorL = 36; // vb links
int Magneet_sensorR = 3;  // vb links
int Magneet_sensorM = 41; // vb links
boolean Mopen = HIGH;     // JJ
boolean Mclose = LOW;     // JJ

// nog aanvullen

// vracht
int ultra_soon_vracht_trigger = 2;
int ultra_soon_vracht_echo = 33;

// rail
int ultra_soon_rail_trigger = 11;
int ultra_soon_rail_echo = 10;
float afstand_voor = 0;   // JJ
float afstand_achter = 0; // JJ
float afstand_midden = 0; // JJ

int S0 = 4;
int S1 = 5;
int S2 =  6;
int S3 = 7;
int sensorOut = 8;

int gewicht_sensor_analog_clk = A15;
int gewicht_sensor_analog_2 = A13;
// random numer td
int servo_platform = 10;
int servo_rail = 11;
int servo_bakje = 12;

int bestelde_gewicht;

int redMin = 14;
int redMax = 140;
int greenMin = 15;
int greenMax = 167;
int blueMin = 13;
int blueMax = 131;

int COLORCALLIBRATIES[3][2] = {{redMin, redMax}, {greenMin, greenMax}, {blueMin, blueMax}};
int color_wit[3] = {255,255,255};
int color_zwart[3] = {0,0,0};
int color_bruin[3] = {234,221,202};

int resetpin = 69;

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "BeanBotlib.h"

// called this way, it uses the default address 0x40

// Afhankelijk van het type servo dat je gebruikt zal je hier zelf je eigen min en max moeten definiëren. Check ook de functie servocontrol

// ---------------------------------------------------- setup() en loop () ---------------------------------------------------------
void initialize() {
  pinMode(Magneet_sensorL, INPUT);
  pinMode(Magneet_sensorR, INPUT);
  pinMode(Magneet_sensorM, INPUT);

  pinMode(Dc_speed, OUTPUT);
  pinMode(Dc_Richting, OUTPUT);

  pinMode(ultra_soon_vracht_trigger, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(ultra_soon_vracht_echo, INPUT);

  pinMode(ultra_soon_rail_trigger, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(ultra_soon_rail_echo, INPUT);

  //kleuren_sensor
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  // SensorOut dient voor het meten van de kleur
  pinMode(sensorOut, INPUT);

  // zet de frequentie schaling op 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  // maakt het mogelijk om te resete
  pinMode(resetpin,OUTPUT);


}


// ---------------------------------- PAS HIERONDER AAN WELKE COMMANDO's JE ZELF WILT ONTVANGEN --------------------------------------------------------

void callibrate() // JJ zet op silo 2
{

  //***************************************************************************************************************************
  // calibrate platform---------------------------------
  // NIEUWE CODE START
  // JJ logica van magneten in boolean gestopt zodat dit eenvoudig om te draaien is
  // boolean Mopen = HIGH; // gedefinieerd als globale variabele
  // boolean Mclose = LOW; // gedefinieerd als globale variabele
  // JJ waarom niet onmiddellijk alle sensor te lezen
  int aligned_L = digitalRead(Magneet_sensorL);
  int aligned_R = digitalRead(Magneet_sensorR);
  int aligned_M = digitalRead(Magneet_sensorM);

  // JJ zolang alle magneetcontacten open zijn
  // JJ uitgangspunt is dat het midden ergens tussen 1 en 3 staat want anders gaat dit niet werken (lees servo gaat mogelijks te ver draaien )
  // JJ zolang alle magneetsensoren open zijn, draai dan naar links. Indien we reeds in het midden staan gaat de loop toch overgeslagen worden
  while (aligned_L == Mopen && aligned_M == Mopen && aligned_R == Mopen)
  {
    // JJ draai naar links
    servoControl(String(servo_platform) + "045"); // JJ draai naar links (geen idee hoe snel 45 naar links is maar dus iets tussen 0 en 90)
    // JJ lees de sensors opnieuw uit
    aligned_L = digitalRead(Magneet_sensorL);
    aligned_R = digitalRead(Magneet_sensorR);
    aligned_M = digitalRead(Magneet_sensorM);
  }
  // JJ na de while moet minstens één van de magneetsensoren gesloten zijn.

  // JJ alle drie gesloten => staat in het midden => niets te doen
  if (aligned_L == Mclose && aligned_M == Mclose && aligned_R == Mclose)
  {
    // doe niets
  }

  // STAAT LINKS
  // JJ L open en M en R gesloten => staat links
  if (aligned_L == Mopen && aligned_M == Mclose && aligned_R == Mclose)
  {
    // JJ draai naar rechts zolang alle drie niet gesloten zijn
    while (!(aligned_L == Mclose && aligned_M == Mclose && aligned_R == Mclose))
    {
      servoControl(String(servo_platform) + "135"); // JJ draai naar rechts (geen idee hoe snel 135 naar rachts is maar dus iets tussen 90 en 180)
      // JJ lees de sensors opnieuw uit
      aligned_L = digitalRead(Magneet_sensorL);
      aligned_R = digitalRead(Magneet_sensorR);
      aligned_M = digitalRead(Magneet_sensorM);
    }
  }

  // STAAT RECHTS
  // JJ L en M gesloten en R open => staat rechts (dit kan op zich niet gezien we verwachten dat startpositie tussen 1 en 3 is en we reeds naar links hebben gedraaid)
  if (aligned_L == Mclose && aligned_M == Mclose && aligned_R == Mopen)
  {
    // JJ draai naar links zolang alle drie niet gesloten zijn
    while (!(aligned_L == Mclose && aligned_M == Mclose && aligned_R == Mclose))
    {
      servoControl(String(servo_platform) + "045"); // JJ draai naar links (geen idee hoe snel 45 naar links is maar dus iets tussen 0 en 90)
      // JJ lees de sensors opnieuw uit
      aligned_L = digitalRead(Magneet_sensorL);
      aligned_R = digitalRead(Magneet_sensorR);
      aligned_M = digitalRead(Magneet_sensorM);
    }
  }

  // STAAT VEEL TE VER LINKS
  // JJ uitzondering R alleen gesloten => staat veel te ver naar links (dit kan op zich niet gezien we verwachten dat startpositie tussen 1 en 3)
  if (aligned_L == Mopen && aligned_M == Mopen && aligned_R == Mclose)
  {
    // JJ draai naar rechts zolang alle drie niet gesloten zijn
    while (!(aligned_L == Mclose && aligned_M == Mclose && aligned_R == Mclose))
    {
      servoControl(String(servo_platform) + "135"); // JJ draai naar rechts (geen idee hoe snel 135 naar rachts is maar dus iets tussen 90 en 180)
      // JJ lees de sensors opnieuw uit
      aligned_L = digitalRead(Magneet_sensorL);
      aligned_R = digitalRead(Magneet_sensorR);
      aligned_M = digitalRead(Magneet_sensorM);
    }
  }

  // STAAT VEEL TE VER RECHTS
  // MJ uitzondering L alleen gesloten => staat veel te ver naar rechts
  if (aligned_L == Mclose && aligned_M == Mopen && aligned_R == Mopen)
  {
    // MJ draai naar links zolang alle drie niet gesloten zijn
    while (!(aligned_L == Mclose && aligned_M == Mclose && aligned_R == Mclose))
    {
      servoControl(String(servo_platform) + "045"); // MJ draai naar links (geen idee hoe snel 45 naar links is maar dus iets tussen 0 en 90)
      // MJ lees de sensors opnieuw uit
      aligned_L = digitalRead(Magneet_sensorL);
      aligned_R = digitalRead(Magneet_sensorR);
      aligned_M = digitalRead(Magneet_sensorM);
    }
  }

  // JJ we moeten nu altijd in het midden staan dus stop de servo
  servoControl(String(servo_platform) + "000");


  //***************************************************************************************************************************
  // calibrate  rail---------------------
  // JJ gevaarlijk om eerst de rail te calibreren gezien je net op een muurtje zou kunnen botsen  !!!!!!!!!!!!!!!!!!!
  // JJ advies is om eerst midden silo 2 te zoek en dan dit uit te voeren
  // JJ kan je niet gewoon één keer afstand bij silo, afstand midden en afstand afleverpunt bepalen en die als globale variabelen vastleggen ????
  // dan moet je enkel nog sturen naar voor midden en achter afhankelijk van je vooraf bepaalde waardes en je gemeten afstand
  // NIEUWE CODE START
  Serial.println(Current_Afstand(ultra_soon_rail_trigger, ultra_soon_rail_echo));
  int prev_afstand = Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo);
  int max_tijd = 20;
  int teller = 0;
  Serial.println("START");
  
  // rij naar voor en meet afstand
  servoControl(String(servo_rail) + "045"); // JJ we rijden vooruit dus afstand moet dalen
  delay(10);                                // zorg dat servo minstens al iets gereden heeft voor je opnieuw meet
  while ((Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo) < prev_afstand) && teller < max_tijd)
  {
    teller = teller + 1;
    prev_afstand = Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo);
    delay(1000);
  }
  // loop stopt van zodra afstand niet meer kleiner wordt of de maximum tijd is verstreken
  servoControl(String(servo_rail) + "000");
  Serial.println("VANVOOR"); // stop de servo
  // OLD int afstand_voor = prev_afstand;  //XXX dit is een globale variabele !!!!!
  afstand_voor = prev_afstand;

  // rij naar achter en meet afstand
  teller = 0;                               // zet teller terug op nul
  servoControl(String(servo_rail) + "135"); // JJ we rijden achteruit dus afstand moet stijgen
  delay(100);                               // zorg dat servo minstens al iets gereden heeft voor je opnieuw meet
  while ((Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo) > prev_afstand) && teller < max_tijd)
  {
    teller = teller + 1;
    prev_afstand = Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo);
    delay(1000);
  }
  Serial.println("VANACHTER"); // stop de servo
  // loop stopt van zodra afstand niet meer groter wordt of de maximum tijd is verstreken
  servoControl(String(servo_rail) + "000"); // stop de servo
  // OLD int afstand_achter = prev_afstand;  //XXX dit is een globale variabele !!!!!
  afstand_achter = prev_afstand;

  // halveer verschil, bepaal afstand tot silo
  // OLD int afstand_midden = ((afstand_achter - afstand_voor) / 2) + afstand_voor; //XXX dit is een globale variabele !!!!!
  afstand_midden = ((afstand_achter - afstand_voor) / 2) + afstand_voor;
  servoControl(String(servo_rail) + "000");  //XXX vier lijnen hoger reeds stil gezet dus lijkt overbodig !!!!!
  Serial.println("DONE");
  delay(100);

}

void MAIN(int silo, int gram)
{
  bestelde_gewicht = gram;
  callibrate();

  // XXX Denk dat hier nog het volgende ontbreekt maar mis de paramter kleur in main
  // Indien je de kleur en het gewicht hebt gebruikt functie
  // * schep_en_lever(String kleur, int max_weight);
  // * uncomment lijn hieronder
  // schep_en_lever(kleur, gram);

  // Indien je enkel silo nummer en het gewicht hebt gebruikt functie
  // * schep_en_lever_silo_x (int silo, int max_weight)
  // * uncomment lijn hieronder
  // schep_en_lever_silo_x (int silo, int max_weight)

}

// functietjes
void rotatie_machine_UL()
{
  // logica van magneten in boolean gestopt zodat dit eenvoudig om te draaien is
  // boolean Mopen = HIGH; // gedefinieerd als globale variabele
  // boolean Mclose = LOW; // gedefinieerd als globale variabele
  // BELANGRIJK : we gaan er vanuit dat we starten vanaf een gekende silo positie 1, 2 of 3 !!!!!!!!!!!!!!

  // lees positie magneten
  int aligned_L = digitalRead(Magneet_sensorL);
  int aligned_R = digitalRead(Magneet_sensorR);
  int aligned_M = digitalRead(Magneet_sensorM);

  // We draaien altijd naar links (calibratie zou ons op 2 moeten gebracht hebben en zoeken bonen tussen 1 en 3)
  // JJ We wensen de situatie : aligned_L == Mopen && aligned_M == Mclose && aligned_R == Mclose
  while (!(aligned_L == Mopen && aligned_M == Mopen && aligned_R == Mclose))
  {
    // draai naar links
    servoControl(String(servo_platform) + "045"); // JJ draai naar links (geen idee hoe snel 45 naar links is maar dus iets tussen 0 en 90)
    // lees de sensors opnieuw uit
    aligned_L = digitalRead(Magneet_sensorL);
    aligned_R = digitalRead(Magneet_sensorR);
    aligned_M = digitalRead(Magneet_sensorM);
  }
  // stop de servo
  servoControl(String(servo_platform) + "000");
  Serial.println("Op positie uiterst links.");
}

void rotatie_machine(int silo)
{
  // JJ
  // NIEUWE CODE START

  // JJ logica van magneten in boolean gestopt zodat dit eenvoudig om te draaien is
  // boolean Mopen = HIGH; // gedefinieerd als globale variabele
  // boolean Mclose = LOW; // gedefinieerd als globale variabele
  // BELANGRIJK : we gaan er vanuit dat we starten vanaf een gekende silo positie 1, 2 of 3 !!!!!!!!!!!!!!

  // JJ lees positie magneten
  int aligned_L = digitalRead(Magneet_sensorL);
  int aligned_R = digitalRead(Magneet_sensorR);
  int aligned_M = digitalRead(Magneet_sensorM);

  // JJ Voor silo 1 moeten we altijd naar links draaien of we staan er reeds
  if (silo == 1) {
    // JJ We wensen de situatie : aligned_L == Mopen && aligned_M == Mclose && aligned_R == Mclose
    while (!(aligned_L == Mopen && aligned_M == Mclose && aligned_R == Mclose))
    {
      // JJ draai naar links
      servoControl(String(servo_platform) + "045"); // JJ draai naar links (geen idee hoe snel 45 naar links is maar dus iets tussen 0 en 90)
      // JJ lees de sensors opnieuw uit
      aligned_L = digitalRead(Magneet_sensorL);
      aligned_R = digitalRead(Magneet_sensorR);
      aligned_M = digitalRead(Magneet_sensorM);
    }
    // JJ stop de servo
    servoControl(String(servo_platform) + "000");
    Serial.println("Silo 1 is bereikt");
  }
  // JJ Voor silo 3 moeten we altijd naar rechts draaien of we staan er reeds
  else if (silo == 3) // JJ silo rechts
  {
    // JJ We wensen de situatie : aligned_L == Mclose && aligned_M == Mclose && aligned_R == Mopen
    while (!(aligned_L == Mclose && aligned_M == Mclose && aligned_R == Mopen))
    {
      // JJ draai naar rechts
      servoControl(String(servo_platform) + "135"); // JJ draai naar rechts (geen idee hoe snel 135 naar links is maar dus iets tussen 0 en 90)
      // JJ lees de sensors opnieuw uit
      aligned_L = digitalRead(Magneet_sensorL);
      aligned_R = digitalRead(Magneet_sensorR);
      aligned_M = digitalRead(Magneet_sensorM);
    }
    // JJ stop de servo
    servoControl(String(servo_platform) + "000");
    Serial.println("Silo 3 is bereikt");
  }
  // JJ Voor silo 2 is de richting van bewegen afhankelijk van de huidige positie links, midden of rechts
  else if (silo == 2) // JJ silo midden
  {
    // JJ We wensen de situatie : aligned_L == Mclose && aligned_M == Mclose && aligned_R == Mclose
    // JJ Dit is een speciale situatie.
    //      Indien we vanaf silo 1 vertrekken moeten we naar rechts draaien
    //      Indien we vanaf silo 3 vertrekken moeten we naar links draaien
    // check of we op silo 1 of 3 staan
    // indien silo 1
    
    //  XXX deze logic lijkt fout  !!!!!!!!!!!!!
    // OLD if (aligned_L != Mopen && aligned_M != Mclose && aligned_R != Mclose)  // hier werd negatie logica omgeraaid maar is ok
    // OLD if (!(aligned_L == Mopen && aligned_M == Mclose && aligned_R == Mclose))
    // lijkt dit te moeten zijn
    if (aligned_L == Mopen && aligned_M == Mclose && aligned_R == Mclose)  // check silo 1
    {
      // JJ draai naar rechts
      while (!(aligned_L == Mclose && aligned_M == Mclose && aligned_R == Mclose))
      {
        // JJ draai naar rechts
        servoControl(String(servo_platform) + "135"); // JJ draai naar rechts (geen idee hoe snel 135 naar links is maar dus iets tussen 0 en 90)
        // JJ lees de sensors opnieuw uit
        aligned_L = digitalRead(Magneet_sensorL);
        aligned_R = digitalRead(Magneet_sensorR);
        aligned_M = digitalRead(Magneet_sensorM);
      }
    }
    // JJ indien silo 3
    // XXX deze logic lijkt fout
    // OLD else (aligned_L != Mclose && aligned_M != Mclose && aligned_R != Mopen)  // hier werd negatie logica omgeraaid maar is ok
    // OLD // else if  (!(aligned_L == Mclose && aligned_M == Mclose && aligned_R == Mopen)) 
    // lijkt dit te moeten zijn 
    else if (aligned_L == Mclose && aligned_M == Mclose && aligned_R == Mopen)  // check silo 3
    {
      // JJ draai naar links
      while (!(aligned_L == Mclose && aligned_M == Mclose && aligned_R == Mclose)) {
        // JJ draai naar links
        servoControl(String(servo_platform) + "45"); // JJ draai naar links (geen idee hoe snel 45 naar links is maar dus iets tussen 0 en 90)
        // JJ lees de sensors opnieuw uit
        aligned_L = digitalRead(Magneet_sensorL);
        aligned_R = digitalRead(Magneet_sensorR);
        aligned_M = digitalRead(Magneet_sensorM);
      }
    }
  
    // JJ indien reeds 2
    //OLD else if (!(aligned_L == Mclose && aligned_M == Mclose && aligned_R == Mclose)) 
    else if (aligned_L == Mclose && aligned_M == Mclose && aligned_R == Mclose)
    {
      // JJ doe niets, we staan correct
  
    }
    // JJ stop de servo
    servoControl(String(servo_platform) + "000");
    Serial.println("Silo 2 is bereikt");
  }
}



void platform_naar_bonen() // JJ naar silo
{
  // JJ
  // NIEUWE CODE START
  int prev_afstand = Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo);
  int max_tijd = 20;
  int teller = 0;

  // rij naar voor en meet afstand
  servoControl(String(servo_rail) + "045"); // JJ we rijden vooruit dus afstand moet dalen
  delay(10);                                // zorg dat servo minstens al iets gereden heeft voor je opnieuw meet
  // rij vooruit zolang de afstand kleiner wordt, maximum tijd niet verstreken is of afstand = aan afstand_voor
  while (((Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo) < prev_afstand) && teller < max_tijd) || (Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo) == afstand_voor))
  {
    teller = teller + 1;
    prev_afstand = Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo);
    delay(500);
  }
  // loop stopt van zodra afstand niet meer kleiner wordt, maximum tijd niet verstreken is of afstand = aan afstand_voor
  servoControl(String(servo_rail) + "000"); // stop de servo

}

void platform_naar_midden()
{
  // JJ
  // NIEUWE CODE START
  int prev_afstand = Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo);
  int max_tijd = 20;
  int teller = 0;

  // Check of we richting silo staa of richting afleverpunt
  // rij naar voor en meet afstand
  if (prev_afstand < afstand_midden) // rij naar achter
  {
    servoControl(String(servo_rail) + "135"); // JJ we rijden achteruit dus afstand moet stijgen
    delay(10);                                // zorg dat servo minstens al iets gereden heeft voor je opnieuw meet
    // rij achteruit zolang de afstand kleiner is dan de afstand_midden
    // voor veiligheid : stop als max tijd verstreken is
    while ((teller < max_tijd) && (Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo) < afstand_midden))
    {
      teller = teller + 1;
      prev_afstand = Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo);
      delay(500);
    }
    // loop stopt van zodra afstand niet meer kleiner wordt, maximum tijd niet verstreken is of afstand = aan afstand_voor
    servoControl(String(servo_rail) + "000"); // stop de servo
  }
  else if (prev_afstand > afstand_midden) // rij naar vooruit
  {
    servoControl(String(servo_rail) + "045"); // JJ we rijden vooruit dus afstand moet dalen
    delay(10);                                // zorg dat servo minstens al iets gereden heeft voor je opnieuw meet
    // rij vooruit zolang de afstand groter is dan de afstand_midden
    // voor veiligheid : stop als max tijd verstreken is
    while ((teller < max_tijd) && (Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo) > afstand_midden))
    {
      teller = teller + 1;
      prev_afstand = Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo);
      delay(500);
    }
    // loop stopt van zodra afstand niet meer kleiner wordt, maximum tijd niet verstreken is of afstand = aan afstand_voor
    servoControl(String(servo_rail) + "000"); // stop de servo
  }
  else if (prev_afstand == afstand_midden) // doe niets
  {
    // doe niets, we staan in het midden
  }
}

void platform_naar_achter() // JJ naar afleverpunt
{
  // JJ
  // NIEUWE CODE START
  int prev_afstand = Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo);
  int max_tijd = 20;
  int teller = 0;

  // rij naar voor en meet afstand
  servoControl(String(servo_rail) + "135"); // JJ we rijden achteruit dus afstand moet stijgen
  delay(10);                                // zorg dat servo minstens al iets gereden heeft voor je opnieuw meet
  // rij achteruit zolang de afstand groter wordt, maximum tijd niet verstreken is of afstand = aan afstand_achter
  while (((Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo) > prev_afstand) && teller < max_tijd) || (Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo) == afstand_achter))
  {
    teller = teller + 1;
    prev_afstand = Current_Afstand(ultra_soon_vracht_trigger, ultra_soon_vracht_echo);
    delay(500);
  }
  // loop stopt van zodra afstand niet meer kleiner wordt, maximum tijd niet verstreken is of afstand = aan afstand_voor
  servoControl(String(servo_rail) + "000"); // stop de servo
  // NIEUWE CODE STOP
}

void bonen_opnemen(int max_weight)
// !!!!!!!!!!!!!!!!!
// JJ is het niet het doel dat je drie soorten bonen neemt van een bepaald gewicht
// indien dat het geval is moet je telkens het startgewicht bepalen en een nieuw max gewicht dat vorige gewicht gelijk is aan vorige plus gevraagde gewicht.
// indien niet verwacht ik dat max_weight hier al deze berekening in zich heeft.

{
  analogWrite(Dc_speed, 255);
  // transportband laten draaien tot gewicht (bijna) bereikt is
  while (max_weight * 0.9 > current_weight())
  {
    update_app(bestelde_gewicht);
  }
  //------------------test OPTIMALISATIE
  int loopband_snelheid = 255;
  int og_weight = current_weight();
  while (max_weight >= current_weight())
  {
    loopband_snelheid = map(loopband_snelheid, og_weight, max_weight, 125, 255); // JJ Zou gewoorn voor het laatste stuk de snelheid laag zetten en laten draaien tot het gewenste gewicht !!!!!!!!!!!!!!
    analogWrite(Dc_speed, loopband_snelheid);
  }
  digitalWrite(Dc_Richting,HIGH);
  analogWrite(Dc_speed, 255);
  delay(1000);
  digitalWrite(Dc_Richting,LOW);
  analogWrite(Dc_speed, 0);


  // JJ had begrepen dat eens gewicht bereikt de bonen op de transportband nog moesten terugdraaien  !!!!!!!!!
  // XXX dit is nog steeds niet af
}

void bonen_kieperen(int max_weight) // JJ geen idee wat dit stuk doet !!!!!!!!!!!!!!!  Ge er vanuit dat je alles kan kiepen want anders zat begin gewicht toch niet goed.
{
  servoControl(String(servo_bakje) + "120");
  while (weight_bakske() <= max_weight)
  {
    update_app(bestelde_gewicht);
  }
  servoControl(String(servo_bakje) + "000");
}





void Zoek_vrachtwagen_max() // Zoekt in het bereik voor silo 1 tot na silo 3
{
  // OPMERKING :  je hebt geen beginpositie
  // zet eerst op silo VOOR silo één, draai richting silo 3/rechts tot we vrachtwagen vinden
  //rotatie_machine(1);
  rotatie_machine_UL();
  // draai naar rechts
  int aligned_L = digitalRead(Magneet_sensorL);
  int aligned_R = digitalRead(Magneet_sensorR);
  int aligned_M = digitalRead(Magneet_sensorM);
  // We gaan er vanuit dat we een vrachtwagen vinden, anders gaan we op positie NA silo 3 staan zonder vrachtwagen te vinden
  while ((Current_Afstand(ultra_soon_rail_trigger, ultra_soon_rail_echo) > AFSTAND_CONTAINER) && !(aligned_L == Mclose && aligned_M == Mopen && aligned_R == Mopen))
  {
    // JJ draai naar rechts
    servoControl(String(servo_platform) + "135"); // JJ draai naar rechts (geen idee hoe snel 135 naar links is maar dus iets tussen 0 en 90)
    // JJ lees de sensors opnieuw uit
    aligned_L = digitalRead(Magneet_sensorL);
    aligned_R = digitalRead(Magneet_sensorR);
    aligned_M = digitalRead(Magneet_sensorM);
    update_app(bestelde_gewicht); // overgenomen uit oude code
  }
  // Begin vrachtwagen gevonden. Bepaal tijdstip van detectie.
  if (Current_Afstand(ultra_soon_rail_trigger, ultra_soon_rail_echo) <= AFSTAND_CONTAINER)
  {
    // Begin vrachtwagen gevonden. Bepaal tijdstip van detectie. We draaien nog steeds richting silo 3
    int Time_machine_begin = millis(); // begin vinden vrachtwagen
    while (Current_Afstand(ultra_soon_rail_trigger, ultra_soon_rail_echo) <= AFSTAND_CONTAINER)
    {
      update_app(bestelde_gewicht); // gewoon overgenomen
    }
    // einde vrachtwagen gevonden

    int Time_machine_end = millis();                             // einde vinden vrachtwagen
    int Time_delta = (Time_machine_end - Time_machine_begin) / 2; // halveren van verschil in tijd
    // draai time_delta terug richting silo 1 om op midden vrachtwagen te staan.
    servoControl(String(servo_platform) + "000");
    delay(100); // servo gewoon even stoppen voor we omdraaien
    servoControl(String(servo_platform) + "045");
    delay(Time_delta);
    servoControl(String(servo_platform) + "000");
    Serial.println("We staan op midden vrachtwagen");
  }
  else
  {
    // we staan op silo drie zonder vrachtwagen te vinden
    servoControl(String(servo_platform) + "000");
    Serial.println("We staan op silo drie zonder vrachtwagen te vinden");
  }
}


void Zoek_vrachtwagen() // Zoekt inhet bereik silo 1  tot 3
{
  // OPMERKING :  je hebt geen beginpositie
  // zet eerst op silo één, draai richting silo 3/rechts tot we vrachtwagen vinden
  rotatie_machine(1);
  // draai naar rechts
  int aligned_L = digitalRead(Magneet_sensorL);
  int aligned_R = digitalRead(Magneet_sensorR);
  int aligned_M = digitalRead(Magneet_sensorM);
  // We gaan er vanuit dat we een vrachtwagen vinden, anders gaan we op silo 3 staan zonder vrachtwagen te vinden
  while ((Current_Afstand(ultra_soon_rail_trigger, ultra_soon_rail_echo) > AFSTAND_CONTAINER) && !(aligned_L == Mclose && aligned_M == Mclose && aligned_R == Mopen))
  {
    // JJ draai naar rechts
    servoControl(String(servo_platform) + "135"); // JJ draai naar rechts (geen idee hoe snel 135 naar links is maar dus iets tussen 0 en 90)
    // JJ lees de sensors opnieuw uit
    aligned_L = digitalRead(Magneet_sensorL);
    aligned_R = digitalRead(Magneet_sensorR);
    aligned_M = digitalRead(Magneet_sensorM);
    update_app(bestelde_gewicht); // overgenomen uit oude code
  }
  // Begin vrachtwagen gevonden. Bepaal tijdstip van detectie.
  if (Current_Afstand(ultra_soon_rail_trigger, ultra_soon_rail_echo) <= AFSTAND_CONTAINER)
  {
    // Begin vrachtwagen gevonden. Bepaal tijdstip van detectie. We draaien nog steeds richting silo 3
    int Time_machine_begin = millis(); // begin vinden vrachtwagen
    while (Current_Afstand(ultra_soon_rail_trigger, ultra_soon_rail_echo) <= AFSTAND_CONTAINER)
    {
      update_app(bestelde_gewicht); // gewoon overgenomen
    }
    // einde vrachtwagen gevonden

    int Time_machine_end = millis();                             // einde vinden vrachtwagen
    int Time_delta = (Time_machine_end - Time_machine_begin) / 2; // halveren van verschil in tijd
    // draai time_delta terug richting silo 1 om op midden vrachtwagen te staan.
    servoControl(String(servo_platform) + "000");
    delay(100); // servo gewoon even stoppen voor we omdraaien
    servoControl(String(servo_platform) + "045");
    delay(Time_delta);
    servoControl(String(servo_platform) + "000");
    Serial.println("We staan op midden vrachtwagen");
  }
  else
  {
    // we staan op silo drie zonder vrachtwagen te vinden
    servoControl(String(servo_platform) + "000");
    Serial.println("We staan op silo drie zonder vrachtwagen te vinden");
  }

}

// deze functie zoekt de silo met de gevraagde kleur, schept het gevraagde gewicht, zoekt de vrachtwagen en levert af
void schep_en_lever(String kleur, int max_weight)
{
  // Eerst silo met gevraagde kleur zoeken
  rotatie_machine(1);
  if (kleur == checkkleur())
  {
    Serial.print("Kleur ");
    Serial.print(kleur);
    Serial.println(" gevonden in silo 1");
  }
  else
  {
    rotatie_machine(2);
    if (kleur == checkkleur())
    {
      Serial.print("Kleur ");
      Serial.print(kleur);
      Serial.println(" gevonden in silo 2");
    }
    else
    {
      rotatie_machine(3);
      if (kleur == checkkleur())
      {
        Serial.print("Kleur ");
        Serial.print(kleur);
        Serial.println(" gevonden in silo 3");
      }
      else
      {
        Serial.println("Error : kleur in geen enkele silo gevonden");
      }
    }
  }
  // Neem bonen
  platform_naar_bonen();
  bonen_opnemen(max_weight);

  // Set platform in het midden
  platform_naar_midden();

  // zoek de vrachtwagen
  //Zoek_vrachtwagen();   //zoekt enkel in het bereik silo 1 tot 3
  Zoek_vrachtwagen_max(); //zoekt in het bereik silo voor 1 tot silo na 3

  // lever bonen af
  platform_naar_achter();
  bonen_kieperen(bestelde_gewicht); // JJ !!!!!!!!!!!  zie mijn comment voor deze functie
}

// deze functie zoekt de opgegeven silo (doet geen kleur controle), schept het gevraagde gewicht, zoekt de vrachtwagen en levert af
void schep_en_lever_silo_x (int silo, int max_weight)
{
  // Eerst silo met gevraagde kleur zoeken
  rotatie_machine(silo);
  
  // Neem bonen
  platform_naar_bonen();
  bonen_opnemen(max_weight);

  // Set platform in het midden
  platform_naar_midden();

  // zoek de vrachtwagen
  //Zoek_vrachtwagen();   //zoekt enkel in het bereik silo 1 tot 3
  Zoek_vrachtwagen_max(); //zoekt in het bereik silo voor 1 tot silo na 3

  // lever bonen af
  platform_naar_achter();
  bonen_kieperen(bestelde_gewicht); // JJ !!!!!!!!!!!  zie mijn comment voor deze functie
}


// ---------------------------------------------------- Functies voor de connectie met ESP32; NIET AANPASSEN AUB ---------------------------------------------------------

void setup()
{

  // NIET AANPASSEN ------------------------
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);

  pinMode(25, OUTPUT);
  digitalWrite(25, HIGH);

  Serial.begin(9600);
  delay(100);
  Serial2.begin(115200); // Communication with Wifi module

  // ------------------------- Vanaf hier mag je aanpassen voor je eigen programma
  initialize();
  // Zet al de ledjes uit van D30 t.e.m
}

void loop()
{
  // Kijk of er iets werd verzonden over wifi?
  String command = checkwifi();
  // Voer het commando uit als er iets werd ontvangen
  if (command.length() > 0)
  {
    Serial.println("Ontvangen commando: " + command); // Print het commando (debug)
    // Splits het command op in commando/parameters
    String commando = command.substring(0, command.indexOf('/'));
    String parameters = command.substring(command.indexOf('/') + 1);
    if (commando.equals("Startproces")) {
      MAIN(parameters.substring(0, parameters.indexOf('/')).toInt(), parameters.substring(parameters.indexOf('/') + 1).toInt());
    }
    else {
      sendWifi("Commando niet herkend");
    }
  }

}
