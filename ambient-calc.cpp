/* ambient-calc.h ist eine Bibliothek zur Berechnung diverser Werte der menschlichen Empfindung von Umgebung
   keine Gewähr auf die Rictigkeit der Formeln, daher ist diese Bibliothek nur für den privaten Gebrauch gedacht
   letzte Aktualisierung: August 2019
*/

#include <Arduino.h>
#include "ambient-calc.h"

Ambient::Ambient(bool displayMsg)
{
	
}

float Ambient::calcTaupunkt(float Temperatur, float Luftfeuchte)
{
	float K1 = 4222.03716;
	float K2 = 17.5043;
	float K3 = 241.2;
	_tp=(K3*log(Luftfeuchte/100)+K1*Temperatur/(K3+Temperatur))/(K2-log(Luftfeuchte/100)-K2*Temperatur/(K3+Temperatur));
	return (_tp);
	
	// Der berechnete Taupunkt basiert auf der Berechnung des Sättigungsdampdruckes über Wasser. 
	// Grundlage dieser Berechnung ist die Magnus Formel, beschrieben in:
	// https://www.chemie.de/lexikon/Taupunkt.html
}

float Ambient::calcEnthalpie(float Temperatur, float Luftfeuchte, float Luftdruck)
{
  float _esatw = 611.2 * exp(17.62 * Temperatur / (243.12 + Temperatur)); // Sättigungsdruck Wasserdampf über Wasser, Magnus Formel
  float _ew = Luftfeuchte / 100 * _esatw; //Wasserdampfpartialdruck in Pa
  float _dichteDampf = _ew / 461.51 / (273.15 + Temperatur); // in kg/m³
  float _dichteLuft = (Luftdruck - _ew) / 287.058 / (273.15 + Temperatur); // in kg/m³. Dichte trockener Luft. Umgebungsdruck in Pa
  float _spezFeuchte = _dichteDampf / (_dichteDampf + _dichteLuft); // kg Wasser/kg feuchte Luft
  Enthalpie = 1.006 * Temperatur + _spezFeuchte * (1.86 * Temperatur + 2500); // Berechnung für ungesättigte Luft. temp in °C
  /*if (displayMsg == true)
  {
  Serial.print("Input zur Berechnung der Enthalpie   ");
  Serial.print(Temperatur);
  Serial.print("    ");
  Serial.print(Luftfeuchte);
  Serial.print("    ");
  Serial.print(Luftdruck);
  Serial.print("  --> berechnete Enthalpie:  ");
  Serial.println(Enthalpie);
  }*/
  /*Serial.print("Sättigungsdruck Wasser: ");
    Serial.println(esatw);
    Serial.print("Partialdruck Wasser: ");
    Serial.println(ew);
    Serial.print("Dichte Dampf: ");
    Serial.println(dichteDampf);
    Serial.print("Dichte trockene Luft: ");
    Serial.println(dichteLuft);
    Serial.print("spezFeuchte: ");
    Serial.println(spezFeuchte); 
    Serial.print("Enthalpie: ");
    Serial.println(Enthalpie);*/
	return (Enthalpie);
}

float Ambient::calcHeatindex(float Temperatur, float Luftfeuchte)
{   _hi=1.1*Temperatur -3.9444 + 0.02666*Luftfeuchte;
if (_hi > 26.6)
{	double c1 = -8.784695;
	double c2 = 1.61139411;
	double c3 = 2.338549;
	double c4 = -1.4611605E-1;
	double c5 = -1.2308094E-2;
	double c6 = -1.6424828E-2;
	double c7 = 2.211732E-3;
	double c8 = 7.2546E-4;
	double c9 = -3.582E-6;
	_hi = c1 + c2*Temperatur + c3*Luftfeuchte +c4*Temperatur*Luftfeuchte + c5*pow(Temperatur,2)+c6*pow(Luftfeuchte,2)+c7*pow(Temperatur,2)*Luftfeuchte+c8*Temperatur*pow(Luftfeuchte,2)+c9*pow(Temperatur,2)*pow(Luftfeuchte,2);
	
	//1. Korrektur wenn Luftfeuchte <13% und die Temperatur zwischen 80 und 112F liegt
	if(Luftfeuchte <13 && Temperatur >26.67 && Temperatur<44.44)
	{
	_hi=_hi-(13-Luftfeuchte)/4*sqrt((17-abs(1.8*Temperatur+32-95))/17);
	}
	
	// 2. Korrektur, anzuwenden wenn die Luftfeuchte größer als 85% ist und die Temperatur zwischen 80 un 87F liegt
	if(Luftfeuchte >85 && Temperatur >26.67 && Temperatur<30.56)
	{
	_hi=_hi+(Luftfeuchte-85)/10*(11-0.36*Temperatur);
	}
}

return(_hi);
	// Grundlage ist die Definition der NOAA: https://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml
	// Gleichung basiert auf Artikel in Wikipedia: https://de.wikipedia.org/wiki/Hitzeindex
}

float Ambient::calcWindchill(float Temperatur, float Windgeschwindigkeit)
{
	float Twind = 13.12 + 0.6215*Temperatur + (0.3965*Temperatur-11.37)*pow(Windgeschwindigkeit,0.16);
return(Twind);	
}

comfortStruct Ambient::calcComfortStatus(float Temperatur, float Luftfeuchte, String Jahreszeit)
{
	/* Die Bestimmung des "Thermal Comfort" folgt dem in folgendem Artikel beschriebenen Konzept:
	https://www.azosensors.com/article.aspx?ArticleID=487
	Es werden die in ASHRAE 55-1992 festgelegte comfort-zone für den Sommer bzw. Winter verwendet. Die jeweils zu berücksichtigende Zone kann über den im der Funktion übermittelten Parameter definiert werden. Default ist Winter.
	
	Zur Bestimmung des aktuellen "Komfort" basierend auf der an die Methode übermittelte Temperatur und relative Feuchtigkeit wird geprüft, ob sich der aktuelle Zustand innerhalb der "comfort-zone" befindet. 
	Dazu werden in einem ersten Schritt die Geraden zwischen den 4 Eckpunkten des die Comfort-zone umschließenden Tetraeders bestimmt. 
	In einem zweiten Schritt werden die Schnittpunkte zwischen vom Umgebungspunkt rechtwinklig ausgehenden Linien und den 4 Grenzgeraden berechnet. 
	In einem dritten Schrittt erfolgt die Abfrage, wo sich die berechneten Schnittpunkte befinden. Entsprechend der Position können sich 9 verschiedene ComfortStatus (Variable empfVar) ergeben.
	
	Diese Methode bestimmt in einem Gang sowohl den Comfort-Status (Angenehm, Zu Feucht...) als auch einen Komfortwert in %. 100% werden nur dann erreicht, wenn die aktuelle Umgebung (Temperatur + Feuchte) in der "comfort-zone" sind. 
	Bei Umgebungsbedingungen auserhalb der Comfort-zone wird entsprechend des Abstandes der Komfortwert reduziert. Liegt z.B. die aktuelle Temperaur um denselben Betrag von der Grenzlinie entfernt, wie die Spanne der comfort-zone, ergibt ein solcher Zustand 0% Komfort. 
	*/
	if(Jahreszeit == "Winter")
	{
		_t1=20.5, _rh1=29.3;
		_t2=24.5, _rh2=23.0;
		_t3=33.5, _rh3=57.3;
		_t4=19.5, _rh4=86.5;
	}
	else{
		_t1=23.5, _rh1=24.4;
		_t2=27.0, _rh2=19.8;
		_t3=26.0, _rh3=57.3;
		_t4=22.5, _rh4=79.5;
	}
	//Berechnung der Schnittpunkte zwischen aktueller Temperatur und den Grenzen der comfort-zone.
	_aLL=(_rh1-_rh2)/(_t1-_t2);
	_bLL=_rh1-_aLL*_t1;
	_rhLL=_aLL*Temperatur+_bLL;
	
	_aUL=(_rh4-_rh3)/(_t4-_t3);
	_bUL=_rh4-_aUL*_t4;
	_rhUL=_aUL*Temperatur+_bUL;
			
	_aLI=(_rh4-_rh1)/(_t4-_t1);
	_bLI=_rh4-_aLI*_t4;
	_tLI=(Luftfeuchte-_bLI)/_aLI;
		
	_aRE=(_rh3-_rh2)/(_t3-_t2);
	_bRE=_rh3-_aRE*_t3;
	_tRE=(Luftfeuchte-_bRE)/_aRE;

	//	Berechnung der Temperatur- bzw. Feuchtespanne zwischen oberem und unterem Grenzwert als Basis für die spätere Berechnung des Komfort%
	_bandrh=_rhUL - _rhLL;
	_bandt=_tRE - _tLI;
	
	//Vergleich aktueller Temperatur&Feuchte mit den Schnittpunkten.
	if(Temperatur<_tLI)
	{
		empfVar = 8;
		_deltat=(_tLI-Temperatur)/_bandt*100;
		if(Luftfeuchte<_rhLL)
		{empfVar=9;
		_deltarh=(_rhLL-Luftfeuchte)/_bandrh*100;}
		if(Luftfeuchte>_rhUL)
		{empfVar=7;
		_deltarh=(Luftfeuchte-_rhUL)/_bandrh*100;}
	}

	if(Temperatur>_tRE)
	{
		empfVar = 4;
		_deltat=(Temperatur-_tRE)/_bandt*100;
		if(Luftfeuchte<_rhLL)
		{empfVar=3;
		_deltarh=(_rhLL-Luftfeuchte)/_bandrh*100;}
		if(Luftfeuchte>_rhUL)
		{empfVar=5;
		_deltarh=(Luftfeuchte-_rhUL)/_bandrh*100;}
	}
	if(Temperatur>_tLI && Temperatur<_tRE)
	{
		empfVar=1;
		if(Luftfeuchte<_rhLL)
		{empfVar=2;
		_deltarh=(_rhLL-Luftfeuchte)/_bandrh*100;}
		if(Luftfeuchte>_rhUL)
		{empfVar=6;
		_deltarh=(Luftfeuchte-_rhUL)/_bandrh*100;}
	}
		comfortWert=100-_deltat - _deltarh;
		if(comfortWert < 0) 
			{comfortWert=0;}
				
	switch(empfVar)
	{
		case 1:	comfortStatus = "Angenehm";
			break;
		case 2:	comfortStatus = "Zu trocken";
			break;
		case 3:	comfortStatus = "Warm und trocken";
			break;
		case 4:	comfortStatus = "Zu warm";
			break;
		case 5:	comfortStatus = "Warm und feucht";
			break;
		case 6:	comfortStatus = "Zu feucht";
			break;
		case 7:	comfortStatus = "Kalt und feucht";
			break;
		case 8:	comfortStatus = "Sehr kalt";
			break;
		case 9:	comfortStatus = "Kalt und trocken";
			break;	
		default:comfortStatus = "unbekannt";
			break;
	}
	comfortStruct result;
	result.empfNummer = empfVar;
	result.comfortProzent = comfortWert;
	result.comfortString = comfortStatus;
	return result;
}