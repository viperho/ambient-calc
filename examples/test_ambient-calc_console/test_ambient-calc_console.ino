#include <ambient-calc.h>
Ambient umgebung(true);

float Temp = 12.0;
float Humidity = 50;
float Pressure = 101310; // ambient pressure in Pa
float Wind = 100; // wind-speed in km/h
String Season = "Winter";

void setup() {
  Serial.begin(115200);
  Serial.println();
}

void loop() {
  Serial.print("temperature: ");
  while (Serial.available() == 0 )
  {}
  Temp = Serial.parseFloat();
  Serial.println(Temp);

  String dump = Serial.readString();
  Serial.print("humidity: ");
  // Serial.flush();
  while (Serial.available() == 0)
  {}
  Humidity = Serial.parseFloat();
  Serial.println(Humidity); 


  float hi = umgebung.calcHeatindex(Temp, Humidity);
  Serial.print("  HeatIndex:  ");
  Serial.println(hi);

  float tp = umgebung.calcTaupunkt(Temp, Humidity);
  Serial.print("  dew point:   ");
  Serial.println(tp);

  float chill = umgebung.calcWindchill(Temp, Wind);
  Serial.print("  Windchill:   ");
  Serial.println(chill);

  float h = umgebung.calcEnthalpie(Temp, Humidity, Pressure);
  Serial.print("  Enthalpy:   ");
  Serial.println(h);


  comfortStruct neuesErgebnis = umgebung.calcComfortStatus(Temp, Humidity, Season);
  int e = neuesErgebnis.empfNummer;
  float f = neuesErgebnis.comfortProzent;
  String g = neuesErgebnis.comfortString;
  Serial.print("  Komfort:   ");
  Serial.print(g);
  Serial.print("  Komfort % :   ");
  Serial.println(f);
  Serial.println();
  delay(5000);
}
