#include <Arduino.h>
#include <PrintEx.h>

PrintEx serial = Serial;
RTC_DATA_ATTR int bootCount = 0;

//globale Variablen _ stored in RTC_data
RTC_DATA_ATTR int soilmid[8];
RTC_DATA_ATTR int soilshould[8];
//Schleifenzähler der Bewässerung _ stored in RTC_data
RTC_DATA_ATTR int x=0;

//Hysterese kann an Werte für Boden und Eigenschaften der Sensoren angepasst werden!

//INPUTS für Sensoren GPIO
#define in0 34
#define in1 35
#define in2 32
#define in3 33
#define in4 25
#define in5 26
#define in6 27
#define in7 14

//Wie viele Sensoren sind angeschlossen? -> unbedingt vorab definieren!
#define anzin 2

//Output für Pumpe
#define pump0 23

//Output für Ventile
//Sensoren 0 & 1 gehören zu Ventil 0
#define vent0 5
//Sensoren 2 & 3 gehören zu Ventil 1
#define vent1 18
//Sensoren 4 & 5 gehören zu Ventil 2
#define vent2 19
//Sensoren 6 & 7 gehören zu Ventil 3
#define vent3 21

//Anzahl der Messwerte (MW) für Median
#define MW 9

//Hysterese für Überfeuchtung
#define HYSTERESE 100


//Pumpenstatus umschalten
void activatepump()
{
digitalWrite(pump0,HIGH);
}
void deactivatepump()
{
  digitalWrite(pump0,LOW);
}

//alles abschalten
void deactivateall()
{
digitalWrite(vent0,LOW);
digitalWrite(vent1,LOW);
digitalWrite(vent2,LOW);
digitalWrite(vent3,LOW);
digitalWrite(pump0,LOW);
}

//FEHLER bei Bewässerung - AUS
void failure()
{
  deactivateall();
  serial.printf("FEHLER - Bewässerung gestoppt.");
  sleep(86400);
  serial.printf("Kehre nach Fehler in Bewässerung zum Programm zurück.");
}

//Funktion für Median aus MW Messwerten für Bodenfeuchte
double median (int * soil)
{
int index = MW/2;
return soil[index];
}


//Sensorabfrage
int getsensor (int sensor)
{
  int i;
  int soil[MW];
  for (i=0;i<MW;i++)
  {
    int help;
    switch(sensor)
                {
                    case 1: help= in0; break;
                    case 2: help= in1; break;
                    case 3: help= in2; break;
                    case 4: help= in3; break;
                    case 5: help= in4; break;
                    case 6: help= in5; break;
                    case 7: help= in6; break;
                    case 8: help= in7; break;
                    default: serial.printf("Fehler bei Abfrage der Sensoren!"); break;
                }

  soil[i]=analogRead(help);
  //analogRead erwartet den int des GPIO -> der String muss also als int ausgegeben werden -> define Wert dynamisch abrufen unmöglich (da beim compilen entfernt)
  delay(2000);
  }
 return median(soil);
}

//Ausgabe der Feuchtigkeitswerte pro Sensor
void moisturecal()
{
  int i;
  for(i=0;i<8;i++)
    serial.printf("Sensor:%d | Feuchtigkeitswert:%d\n",i,soilshould[i]);

}

//Ausgabe der Feuchtigkeitswerte pro Sensor mit Soll
void moisturesens()
{
  int i;
  for(i=0;i<8;i++)
    serial.printf("Sensor:%d | Sollwert:%d | Feuchtigkeitswert:%d\n" ,i,soilshould[i],soilmid[i]);

}

//Testabschnitt
void test()
{
//Funktion der Pumpe gegeben?
  serial.printf("Starte Funktionstest der Wasserversorgung\n");

  serial.printf("Starte Pumpe\n");
  activatepump();
  sleep(5);
  
  serial.printf("Öffne Ventil 1\n");
  digitalWrite(vent0,HIGH);
  sleep(60);
  serial.printf("Schließe Ventil 1\n");
  digitalWrite(vent0,LOW);
  sleep(5);

  /*
  serial.printf("Öffne Ventil 2\n");
  digitalWrite(vent1,HIGH);
  sleep(60);
  serial.printf("Schließe Ventil 2\n");
  digitalWrite(vent1,LOW);
  sleep(10);

  serial.printf("Öffne Ventil 3\n");
  digitalWrite(vent2,HIGH);
  sleep(60);
  serial.printf("Schließe Ventil 3\n");
  digitalWrite(vent2,LOW);
  sleep(10);

  serial.printf("Öffne Ventil 4\n");
  digitalWrite(vent3,HIGH);
  sleep(60);
  serial.printf("Schließe Ventil 4\n");
  digitalWrite(vent3,LOW);
  sleep(10);
  */

  serial.printf("Deaktiviere Pumpe und schliesse alle Ventile\n");
  deactivateall();
  }

//SETUP
void setup() 
{
  //Start Serial
  Serial.begin(115200);
  //Pause für Aufbau Serial-Conntection
  delay(1000);

  //increment bootCount
  bootCount++;
  Serial.println("Boot number:" + String(bootCount));

  //ADC auf 12 Bit umschalten
  analogReadResolution(12);

  //Sensoren sind INPUTS
  {
  pinMode(in0,INPUT);
  pinMode(in1,INPUT);
  pinMode(in2,INPUT);
  pinMode(in3,INPUT);
  pinMode(in4,INPUT);
  pinMode(in5,INPUT);
  pinMode(in6,INPUT);
  pinMode(in7,INPUT);
  }

 // Pumpe und Ventile sind OUTPUTS
 {
  pinMode(pump0,OUTPUT);
  pinMode(vent0,OUTPUT);
  pinMode(vent1,OUTPUT);
  pinMode(vent2,OUTPUT);
  pinMode(vent3,OUTPUT);
 }

  //Rückstellung Ventile und Pumpe
  deactivateall();

  //Kalibrierung der Feuchtigkeit auf Einschaltzustand
  {
for (int input=0; input<anzin; input++)
    {
    soilshould[input]=getsensor(input);
       
    }   
  }

  //Ausgabe der Kalibrierten Werte
  moisturecal();

  //Teste auf Funktion der Wasserversorgung
  test();

  //Warte 10sec
  sleep(10);
  }



//LOOP-Bewässerung
void loop() 
{
x=x+1;
serial.printf("Bewässerungsschleife Durchlauf:%d\n", x);

//Prüfung der Werte
//Abfragedauer ca. 20 sec pro Sensor.
{
for (int input=0; input<anzin; input++)
    {
        soilmid[input]=getsensor(input);
    }
}

//Ausgabe der Feuchtigkeit
moisturesens();

//Bewässern, wenn nötig 
int k=0;
//VENTIL 0
//feuchter = niedrigere Spannung -> Wert geringer
if((soilmid[k] > soilshould[k]) || (soilmid[k+1] > soilshould[k+1]))
{
  serial.printf("Bewässerung für %d nötig",k);
  activatepump();
  delay(5000);
  digitalWrite(vent0,HIGH);
  sleep(120);

  //Prüfen ob Bewässerung die Feuchtigkeit erhöht. Wenn nicht failure() auslösen!
  int help1, help2, go;
  {
      help1 = getsensor(k);
      help2 = getsensor(k+1);
  }
  
  if (soilmid[k]<= help1 || soilmid[k+1]<=help2)
    {
      failure();
    }
  
  //weiterhin bewässern
  for(go=0; go<1;)
    {
    soilmid[k]=getsensor(k);
    soilmid[k+1]=getsensor(k+1);
      if((soilmid[k] > (soilshould[k]-HYSTERESE)) || (soilmid[k+1] > (soilshould[k+1]-HYSTERESE) ))
        {
          sleep(120);
          int help1, help2;
          help1 = getsensor(k);
          help2 = getsensor(k+1);
          if (soilmid[k]<= help1 || soilmid[k+1]<=help2)
            {       
              failure();
            }
        }
      else
        {
        go=5;
        deactivateall();
        }
    }
}

/*
{

//VENTIL 1
k=2;
if((soilmid[k] > soilshould[k]) || (soilmid[k+1] > soilshould[k+1]))
{
  serial.printf("Bewässerung für %d nötig",k);
  activatepump();
  delay(2000);
  digitalWrite(vent1,HIGH);
  sleep(120);

  //Prüfen ob Bewässerung die Feuchtigkeit erhöht. Wenn nicht failure() auslösen!
  int help1, help2, go;
  help1 = getsensor2();
  help2 = getsensor3();
  if (soilmid[k]-10<= help1 || soilmid[k+1]-10<=help2)
    {
      failure();
    }
  
  //weiterhin bewässern
  for(go=0; go<1;)
    {
    soilmid[k]=getsensor2();
    soilmid[k+1]=getsensor3();
      if((soilmid[k] > (soilshould[k]-HYSTERESE)) || (soilmid[k+1] > (soilshould[k+1]-HYSTERESE) ))
      {
        sleep(120);
        int help1, help2;
        help1 = getsensor2();
        help2 = getsensor3();
        if (soilmid[k]-10<= help1 || soilmid[k+1]-10<=help2)
          {
             failure();
          }
      }  
      else
        {
        go=5;
        deactivateall();
        }
    }
}

//VENTIL 2
k=4;
if((soilmid[k] > soilshould[k]) || (soilmid[k+1] > soilshould[k+1]))
{
  serial.printf("Bewässerung für %d nötig",k);
  activatepump();
  delay(2000);
  digitalWrite(vent2,HIGH);
  sleep(120);

  //Prüfen ob Bewässerung die Feuchtigkeit erhöht. Wenn nicht failure() auslösen!
  int help1, help2, go;
  help1 = getsensor4();
  help2 = getsensor5();
  if (soilmid[k]-10<= help1 || soilmid[k+1]-10<=help2)
    {
      failure();
    }
  
  //weiterhin bewässern
  for(go=0; go<1;)
    {
    soilmid[k]=getsensor4();
    soilmid[k+1]=getsensor5();
      if((soilmid[k] > (soilshould[k]-HYSTERESE)) || (soilmid[k+1] > (soilshould[k+1]-HYSTERESE) ))
        {
            sleep(120);
          //Prüfen ob Bewässerung die Feuchtigkeit erhöht. Wenn nicht failure() auslösen!
             int help1, help2;
             help1 = getsensor4();
             help2 = getsensor5();
              if (soilmid[k]-10<= help1 || soilmid[k+1]-10<=help2)
              { 
                failure();
              }
        }
      else
        {
        go=5;
        deactivateall();
        }
    }
}
  
//VENTIL 3
k=6;
if((soilmid[k] > soilshould[k]) || (soilmid[k+1] > soilshould[k+1]))
{
  serial.printf("Bewässerung für %d nötig",k);
  activatepump();
  delay(2000);
  digitalWrite(vent3,HIGH);
  sleep(120);

  //Prüfen ob Bewässerung die Feuchtigkeit erhöht. Wenn nicht failure() auslösen!
  int help1, help2, go;
  help1 = getsensor6();
  help2 = getsensor7();
  if (soilmid[k]-10<= help1 || soilmid[k+1]-10<=help2)
    {
      failure();
    }
  
  //weiterhin bewässern
  for(go=0; go<1;)
    {
    soilmid[k]=getsensor6();
    soilmid[k+1]=getsensor7();
      if((soilmid[k] > (soilshould[k]-HYSTERESE)) || (soilmid[k+1] > (soilshould[k+1]-HYSTERESE) ))
       {
          sleep(120);
        //Prüfen ob Bewässerung die Feuchtigkeit erhöht. Wenn nicht failure() auslösen!
          int help1, help2;
          help1 = getsensor6();
          help2 = getsensor7();
            if (soilmid[k]-10<= help1 || soilmid[k+1]-10<=help2)
            {
              failure();
            } 
        }
      else
        {
        go=5;
        deactivateall();
        }
    }
}
}
  */


  //Rückstellung der Werte
  deactivateall();
  serial.printf("Bewässerungszyklus %d abgeschlossen", x);

  //Schlafe 1h
  sleep(3600);
}