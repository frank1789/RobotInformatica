#include "automatic.h"

Automatic::Automatic()
{
	check = false;
	error = 0;
}

void Automatic::setInputSensor(float p)
{
	dist_obs = p;
	Automatic::CheckObstacle();
}

void Automatic::setInputLine(uint8_t pDX,
                             uint8_t pSX)
{
	sensorStateDX = pDX;
	sensorStateSX = pSX;
	Automatic::ReadPath();
	Automatic::PathError();
}

int Automatic::getPathError() { return error; }

bool Automatic::getCheckObstacle() { return check; }

void Automatic::CheckObstacle()
{ 
	//Serial.print("distanza: ");
	//Serial.print(dist_obs);
	//Serial.println(" cm");
	if (dist_obs >= 15.00 || dist_obs == 0.0) check = true;
	else check = false;
}		

void Automatic::ReadPath()
{
	if(sensorStateDX == 1 && sensorStateSX == 2)
	{
		input_path = 1;
		//Serial.print("sensor input, ");
		//Serial.println(input_path);
	}  //GO_FORWARD 

	else if(sensorStateDX == 3 && sensorStateSX == 2)
	{
		input_path = 2;
		//Serial.print("sensor input, ");
		//Serial.println(input_path);
	}//TURN_LEFT_VERY_SOFT

	else if(sensorStateDX == 1 && sensorStateSX == 0)
	{
		input_path = 3;
		//Serial.print("sensor input, ");
		//Serial.println(input_path);
	}  //TURN_LEFT_SOFT

	else if(sensorStateDX == 3 && sensorStateSX == 0)
	{
		input_path = 4;
		//Serial.print("sensor input, ");
		//Serial.println(input_path);
	}  //TURN_LEFT_HARD 

	else if(sensorStateDX == 3 &&  sensorStateSX == 1)
	{
		input_path = 5;
		//Serial.print("sensor input, ");
		//Serial.println(input_path);
	}  //TURN_LEFT_VERYHARD

	else if(sensorStateDX == 1 && sensorStateSX == 3)
	{
		input_path = 6;
		//Serial.print("sensor input, ");
		//Serial.println(input_path);
	}  //TURN_RIGHT_VERY_SOFT

	else if(sensorStateDX == 0 && sensorStateSX == 2)
	{
		input_path = 7;
		//Serial.print("sensor input, ");
		//Serial.println(input_path);
	}  //TURN_RIGHT_SOFT

	else if(sensorStateDX == 0 && sensorStateSX == 3)
	{
		input_path = 8;
		//Serial.print("sensor input, ");
		//Serial.println(input_path);
	}  //TURN_RIGHT_HARD 

	else if(sensorStateDX == 2 && sensorStateSX == 3)
	{
		input_path = 9;
		//Serial.print("sensor input, ");
		//Serial.println(input_path);
	}  //TURN_RIGHT_VERYHARD

	else if(sensorStateDX == 3 && sensorStateSX == 3)
	{
		input_path = 10;
		//Serial.print("sensor input, ");
		//Serial.println(input_path);
	}  //NO_LINE

	else if(sensorStateDX == 2 && sensorStateSX == 0)
	{
		input_path = 11;
		//Serial.print("sensor input, ");
		//Serial.println(input_path);
	}  //EXCEPTION1

	else if(sensorStateDX == 0 && sensorStateSX == 1)
	{
		input_path = 12;
		//Serial.print("sensor input, ");
		//Serial.println(input_path);
	}  //EXCEPTION2 

	else if(sensorStateDX == 2 && sensorStateSX == 1)
	{
		input_path = 13;
		//Serial.print("sensor input, ");
		//Serial.println(input_path);
	}  //GO_FORWARD_bis

	else if(sensorStateDX == 0 && sensorStateSX == 0)
	{
		input_path = 14;
		//Serial.print("sensor input, ");
		//Serial.println(input_path);
	}  //CROSS

	else if(sensorStateDX == 2 && sensorStateSX == 2)
	{
		input_path = 15;
		//Serial.print("sensor input, ");
		//Serial.println(input_path);
	}  //EXCEPTION3

	else if(sensorStateDX == 1 && sensorStateSX == 1)
	{
		input_path = 16;
		//Serial.print("sensor input, ");
		//Serial.println(input_path);
	}  //EXCEPTION4 
}

/*FUNZIONE path_error
  in questa funzione in ingresso vengono ricevuti
  due interi ricavati dai metodi dei line follewer
  e in uscita viene restituito un intero che individua la posizione del 
  robot rispetto alla linea da seguire (error = output - setpoint).
  
  Sulla base dei valori ricevuti dai sensori si ottengono i seguenti casi:

                                   (X=nero; O=bianco)
                           DX  SX        SX  DX        Error
  GO_FORWARD           	==  1 , 2       O X  X O         0  (line = nero & background = bianco) 
  
  TURN_LEFT_VERY_SOFT  	==  3 , 2       O X  O O         1
  TURN_LEFT_SOFT       	==  1 , 0       X X  X O         2
  TURN_LEFT_HARD       	==  3 , 0       X X  O O         3
  TURN_LEFT_VERYHARD   	==  3 , 1       X O  O O         4

  TURN_RIGHT_VERY_SOFT 	==  1 , 3       O O  X O        -1
  TURN_RIGHT_SOFT      	==  0 , 2       O X  X X        -2
  TURN_RIGHT_HARD      	==  0 , 3       O O  X X        -3
  TURN_RIGHT_VERYHARD  	==  2 , 3       O O  O X        -4

  NO_LINE              	==  3 , 3       O O  O O         5  

  EXCEPTION1           	==  2 , 0       X X  O X         1
  EXCEPTION2           	==  0 , 1       X O  X X        -1 
  
  GO_FORWARD_bis       	==  2 , 1       X O  O X         0

  CROSS                	==  0 , 0       X X  X X         6     
  EXCEPTION3 		       	==  2 , 2	    	O X  O X         0     
  EXCEPTION4           	==  1 , 1       X O  X O         0     
*/
	
void Automatic::PathError()
{
	switch(input_path)
	{
		case 1:
			//attribusico l'errore da calcolare poi nella funzione PID
			error = 0;
			//stampa sulla //Seriale per debugging
			//Serial.print("GO_FORWARD");
			//Serial.print(", ");
			//Serial.print(input_path);
			//Serial.print(", error: ");
			//Serial.println(error);
		break;
		

		case 2:
			//attribusico l'errore da calcolare poi nella funzione PID
			error = 1;
			//stampa sulla //Seriale per debugging
			//Serial.print("TURN_LEFT_VERY_SOFT");
			//Serial.print(", ");
			//Serial.print(input_path);
			//Serial.print(", error: ");
			//Serial.println(error);
		break;
		

		case 3 :
			//attribusico l'errore da calcolare poi nella funzione PID
			error = 2;
			//stampa sulla //Seriale per debugging
			//Serial.print("TURN_LEFT_SOFT ");
			//Serial.print(", ");
			//Serial.print(input_path);
			//Serial.print(", error: ");
			//Serial.println(error);
		break;
		

		case 4:
			//attribusico l'errore da calcolare poi nella funzione PID
			error = 3;
			//stampa sulla //Seriale per debugging
			//Serial.print("TURN_LEFT_HARD");
			//Serial.print(", ");
			//Serial.print(input_path);
			//Serial.print(", error: ");
			//Serial.println(error);
		break;
		

		case 5:
		
			//attribusico l'errore da calcolare poi nella funzione PID
			error = 4;
			//stampa sulla //Seriale per debugging
			//Serial.print("TURN_LEFT_VERYHARD");
			//Serial.print(", ");
			//Serial.print(input_path);
			//Serial.print(", error: ");
			//Serial.println(error);
		break;
		

		case 6:
			//attribusico l'errore da calcolare poi nella funzione PID
			error = -1;
			//stampa sulla //Seriale per debugging
			//Serial.print("TURN_RIGHT_VERY_SOFT");
			//Serial.print(", ");
			//Serial.print(input_path);
			//Serial.print(", error: ");
			//Serial.println(error);
		break;
		

		case 7:
	
			//attribusico l'errore da calcolare poi nella funzione PID
			error = -2;
			//stampa sulla //Seriale per debugging
			//Serial.print("TURN_RIGHT_SOFT");
			//Serial.print(", ");
			//Serial.print(input_path);
			//Serial.print(", error: ");
			//Serial.println(error);
		break;
		

		case 8:
			//attribusico l'errore da calcolare poi nella funzione PID
			error = -3;
			//stampa sulla //Seriale per debugging
			//Serial.print("TURN_RIGHT_HARD");
			//Serial.print(", ");
			//Serial.print(input_path);
			//Serial.print(", error: ");
			//Serial.println(error);
		break;
		

		case 9:
			//attribusico l'errore da calcolare poi nella funzione PID
			error = -4;
			//stampa sulla //Seriale per debugging
			//Serial.print("TURN_RIGHT_VERYHARD");
			//Serial.print(", ");
			//Serial.print(input_path);
			//Serial.print(", error: ");
			//Serial.println(error);
		break;
		

		case 10:
			//attribusico l'errore da calcolare poi nella funzione PID
			error = 5;
			//stampa sulla //Seriale per debugging
			//Serial.print("case 10");
			//Serial.print(", ");
			//Serial.print(input_path);
			//Serial.print(", error: ");
			//Serial.println(error);
		break;
		

		case 11:	
			//attribusico l'errore da calcolare poi nella funzione PID
			error = 1;
			//stampa sulla //Seriale per debugging
			//Serial.print("case 11");
			//Serial.print(", ");
			//Serial.print(input_path);
			//Serial.print(", error: ");
			//Serial.println(error);
		break;
		

		case 12:
			//attribusico l'errore da calcolare poi nella funzione PID
			error = -1;
			//stampa sulla //Seriale per debugging
			//Serial.print("case 12");
			//Serial.print(", ");
			//Serial.print(input_path);
			//Serial.print(", error: ");
			//Serial.println(error);
			break;
		

		case 13:
		
			//attribusico l'errore da calcolare poi nella funzione PID
			error = 0;
			//stampa sulla //Seriale per debugging
			//Serial.print("case 13");
			//Serial.print(", ");
			//Serial.print(input_path);
			//Serial.print(", error: ");
			//Serial.println(error);
			break;
		

		case 14:
			//attribusico l'errore da calcolare poi nella funzione PID
			error = 6;
			//stampa sulla //Seriale per debugging
			//Serial.print("case 14");
			//Serial.print(", ");
			//Serial.print(input_path);
			//Serial.print(", error: ");
			//Serial.println(error);
		break;
		

		case 15:		
			//attribusico l'errore da calcolare poi nella funzione PID
			error =0 ;
			//stampa sulla //Seriale per debugging
			//Serial.print("case 15");
			//Serial.print(", ");
			//Serial.print(input_path);
			//Serial.print(", error: ");
			//Serial.println(error);
		break;
		

		case 16:		
			//attribusico l'errore da calcolare poi nella funzione PID
			error = 0;
			//stampa sulla //Seriale per debugging
			//Serial.print("case 16");
			//Serial.print(", ");
			//Serial.print(input_path);
			//Serial.print(", error: ");
			//Serial.println(error);
		break;
		
		default: 
			exit; 
		break;
	}
}
