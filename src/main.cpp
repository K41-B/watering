#include <Arduino.h>

//globale Variablen
int soilmid[8];
int soilshould[8];
//Schleifenzähler der Bewässerung
int x=0;

//Hysterese muss an Werte für Boden und Eigenschaften der Sensoren angepasst werden!


//INPUTS für Sensoren Stiftleiste
#define in0 34
#define in1 35
#define in2 32
#define in3 33
#define in4 25
#define in5 26
#define in6 27
#define in7 14

//Output für Pumpe
#define pump0 23

//Output für Ventile
//Sensoren 0 & 1 gehören zu Ventil0
#define vent0 5
//Sensoren 2&3 gehören zu Ventil 1
#define vent1 18
//Sensoren 4&5 gehören zu Ventil 2
#define vent2 19
//Sensor6&7 gehören zu Ventil 3
#define vent3 21

//Anzahl der Messwerte für Mittelwert
#define MW 10

//Hysterese für Überfeuchtung
#define HYSTERESE 200


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
  //digitalWRITE(LED,HIGH)
      while(1)
      {sleep(100000000);}
}
//Funktion für Mittelwert aus MW Messwerten für Bodenfeuchte
double mittelwert(int * soil)
{
int sum=0, i;

for (i=0; i<MW; i++)
  {
    sum=sum+soil[i];

  }
return sum/MW;
}

//Sensorabfrage 0
int getsensor0()
{
  int i;
  int soil[MW];
  for (i=0;i<MW;i++)
  {
  soil[i]=analogRead(in0);
  delay(2000);
  }
 return mittelwert(soil);
}

//Sensorabfrage 1
int getsensor1()
{
  int i;
  int soil[MW];
  for (i=0;i<MW;i++)
  {
  soil[i]=analogRead(in1);
  delay(2000);
  }
 return mittelwert(soil);
}

//Sensorabfrage 2
int getsensor2()
{
  int i;
  int soil[MW];
  for (i=0;i<MW;i++)
  {
  soil[i]=analogRead(in2);
  delay(2000);
  }
 return mittelwert(soil);
}

//Sensorabfrage 3
int getsensor3()
{
  int i;
  int soil[MW];
  for (i=0;i<MW;i++)
  {
  soil[i]=analogRead(in3);
  delay(2000);
  }
 return mittelwert(soil);
}

//Sensorabfrage 4
int getsensor4()
{
  int i;
  int soil[MW];
  for (i=0;i<MW;i++)
  {
  soil[i]=analogRead(in4);
  delay(2000);
  }
 return mittelwert(soil);
}

//Sensorabfrage 5
int getsensor5()
{
  int i;
  int soil[MW];
  for (i=0;i<MW;i++)
  {
  soil[i]=analogRead(in5);
  delay(2000);
  }
 return mittelwert(soil);
}

//Sensorabfrage 6
int getsensor6()
{
  int i;
  int soil[MW];
  for (i=0;i<MW;i++)
  {
  soil[i]=analogRead(in6);
  delay(2000);
  }
 return mittelwert(soil);
}

//Sensorabfrage 7
int getsensor7()
{
  int i;
  int soil[MW];
  for (i=0;i<MW;i++)
  {
  soil[i]=analogRead(in7);
  delay(2000);
  }
 return mittelwert(soil);
}


//Ausgabe der Feuchtigkeitswerte pro Sensor
void moisturecal()
{
  int i;
  for(i=0;i<8;i++)
    printf("Sensor:%d | Feuchtigkeitswert:%d\n",i,soilshould[i]);

}

//Ausgabe der Feuchtigkeitswerte pro Sensor mit Soll
void moisturesens()
{
  int i;
  for(i=0;i<8;i++)
    printf("Sensor:%d | Sollwert:%d | Feuchtigkeitswert:%d\n" ,i,soilshould[i],soilmid[i]);

}

//Testabschnitt
void test()
{
//Funktion der Pumpe gegeben?
  printf("Starte Funktionstest der Wasserversorgung\n");

  printf("Starte Pumpe\n");
  activatepump();
  sleep(60);
  
  printf("Öffne Ventil 1\n");
  digitalWrite(vent0,HIGH);
  sleep(60);
  printf("Schließe Ventil 1\n");
  digitalWrite(vent0,LOW);
  sleep(10);

  printf("Öffne Ventil 2\n");
  digitalWrite(vent1,HIGH);
  sleep(60);
  printf("Schließe Ventil 2\n");
  digitalWrite(vent1,LOW);
  sleep(10);

  printf("Öffne Ventil 3\n");
  digitalWrite(vent2,HIGH);
  sleep(60);
  printf("Schließe Ventil 3\n");
  digitalWrite(vent2,LOW);
  sleep(10);

  printf("Öffne Ventil 4\n");
  digitalWrite(vent3,HIGH);
  sleep(60);
  printf("Schließe Ventil 4\n");
  digitalWrite(vent3,LOW);
  sleep(10);

  printf("Deaktiviere Pumpe und alle Ventile\n");
  deactivateall();
  }

//SETUP
void setup() 
{
  //Start Serial
  Serial.begin(115200);

  //ADC auf 12 Bit umschalten
  analogReadResolution(12);

  //Sensoren sind INPUTS
  pinMode(in0,INPUT);
  pinMode(in1,INPUT);
  pinMode(in2,INPUT);
  pinMode(in3,INPUT);
  pinMode(in4,INPUT);
  pinMode(in5,INPUT);
  pinMode(in6,INPUT);
  pinMode(in7,INPUT);
 
 // Pumpe und Ventile sind OUTPUTS
  pinMode(pump0,OUTPUT);
  pinMode(vent0,OUTPUT);
  pinMode(vent1,OUTPUT);
  pinMode(vent2,OUTPUT);
  pinMode(vent3,OUTPUT);

  //Rückstellung Ventile und Pumpe
  deactivateall();

  //Kalibrierung der Feuchtigkeit auf Einschaltzustand
  soilshould[0]=getsensor0();
  soilshould[1]=getsensor1();
  soilshould[2]=getsensor2();
  soilshould[3]=getsensor3();
  soilshould[4]=getsensor4();
  soilshould[5]=getsensor5();
  soilshould[6]=getsensor6();
  soilshould[7]=getsensor7();

  //Ausgabe der Kalibrierten Werte
  moisturecal();

  //Teste auf Funktion der Wasserversorgung
  test();

  //Warte 1min
  sleep(60);
  }


//LOOP-Bewässerung
void loop() 
{
x=x+1;
printf("Bewässerungsschleife Durchlauf:%d\n", x);

//Prüfung der Werte
//Abfragedauer ca. 20 sec pro Sensor. -> ca. 2min 40 sec. bei 8 Sensoren.
{
soilmid[0]=getsensor0();
soilmid[1]=getsensor1();
soilmid[2]=getsensor2();
soilmid[3]=getsensor3();
soilmid[4]=getsensor4();
soilmid[5]=getsensor5();
soilmid[6]=getsensor6();
soilmid[7]=getsensor7();
}

//Ausgabe der Feuchtigkeit
moisturesens();

//Bewässern, wenn nötig 
int k=0;
//VENTIL 0
if((soilmid[k] > soilshould[k]) || (soilmid[k+1] > soilshould[k+1]))
{
  printf("Bewässerung für %d nötig",k);
  activatepump();
  delay(2000);
  digitalWrite(vent0,HIGH);
  sleep(120);

  //Prüfen ob Bewässerung die Feuchtigkeit erhöht. Wenn nicht failure() auslösen!
  int help1, help2, go;
  help1 = getsensor0();
  help2 = getsensor1();
  if (soilmid[k]-10<= help1 || soilmid[k+1]-10<=help2)
    {
      failure();
    }
  
  //weiterhin bewässern
  for(go=0; go<1;)
    {
    soilmid[k]=getsensor0();
    soilmid[k+1]=getsensor1();
      if((soilmid[k] > (soilshould[k]-HYSTERESE)) || (soilmid[k+1] > (soilshould[k+1]-HYSTERESE) ))
        {
          sleep(120);
          int help1, help2;
          help1 = getsensor0();
          help2 = getsensor1();
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

/*

//VENTIL 1
k=2;
if((soilmid[k] > soilshould[k]) || (soilmid[k+1] > soilshould[k+1]))
{
  printf("Bewässerung für %d nötig",k);
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
  printf("Bewässerung für %d nötig",k);
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
  printf("Bewässerung für %d nötig",k);
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

  */


  //Rückstellung der Werte
  deactivateall();
  printf("Bewässerungszyklus %d abgeschlossen", x);

  //Schlafe 1h
  sleep(3600);
}