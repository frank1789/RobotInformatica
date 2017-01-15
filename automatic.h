#ifndef __AUTOMATIC_H
#define __AUTOMATIC_H
#include <Arduino.h>

class Automatic
{
public:
	Automatic();//costruttore
	//set input 
	void setInputSensor(float p);
	void setInputLine(uint8_t pDX, uint8_t pSX);
	//input function
	void CheckObstacle();					//controlla ostacoli sul percoso
	void ReadPath();					//restituisce un tipo percoso letto dai linefollower
	//exit function
	void SpeedControl();				        //verifica che il valore di al motore non superi 255
	void PathError ();				        //valuta l'errore sul percoso 
	//get 
	int getPathError(); 
	bool getCheckObstacle();
private:
	//input variables
	uint8_t sensorStateDX;
	uint8_t sensorStateSX;
	float dist_obs;
	//transform variable
	uint8_t input_path;
	bool check;
	//exit variable
	int error;
};

#endif
