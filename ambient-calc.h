/* ambient-calc.h calculates several climatic values and indicators for how humans perceive the surrounding climate
   only for private use
   last update: November 2019
*/

#ifndef klima
#define klima

#include <Arduino.h>
struct comfortStruct{
		int empfNummer; 
		float comfortProzent;
		String comfortString;
		};

class Ambient
{
  public:
    // Constructor	
	Ambient(bool displayMsg=false);
	
	//Methods
	float calcEnthalpie(float Temperatur, float Luftfeuchte, float Luftdruck=101574);
	float calcHeatindex(float Temperatur, float Luftfeuchte);
	float calcTaupunkt(float Temperatur, float Luftfeuchte);
	float calcWindchill(float Temperatur, float Windgeschwindigkeit);
	comfortStruct calcComfortStatus(float Temperatur, float Luftfeuchte, String Jahreszeit= "Sommer");
	
	//Variables
	float Enthalpie;
	int pin;
	int empfVar;
	float comfortWert;
	String comfortStatus;
	
	
  private:
    float _esatw;
    float _ew;
    float _dichtedampf;
    float _dichteLuft;
    float _spezFeuchte;
	float cf;
	double _hi;
	float _tp;
	float _t1, _t2, _t3, _t4, _rh1, _rh2, _rh3, _rh4;
	float _aLL, _bLL, _rhLL, _aUL, _bUL, _rhUL, _aLI, _bLI, _tLI, _aRE, _bRE, _tRE; 
	float _bandrh, _bandt, _deltarh, _deltat;
	};

#endif