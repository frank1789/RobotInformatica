#include "MeOrion.h"
#include <SoftwareSerial.h>
#include "finite_state_machine.h"
#include "automatic.h"
#include "interface.h"

//velocità motore 
#define motorSpeedMAX   140
#define motorSpeedMIN  -140

MeLineFollower lineFinderDX(PORT_3); /* Line Finder module can only be connected to PORT_3, PORT_4, PORT_5, PORT_6 of base shield. */
MeLineFollower lineFinderSX(PORT_4);

MeUltrasonicSensor ultraSensor(PORT_6); //Ultrasonic module can ONLY be connected to port 3, 4, 6, 7, 8 of base shield.

MeDCMotor motorDX(PORT_1);  // value: between -255 and 255.
MeDCMotor motorSX(PORT_2);  // value: between -255 and 255.

MeBluetooth bluetooth(PORT_5);

//dichiarazione state functions
void idle();
void start();
void line_follower();
void stop();
void remote();
void command_state(char ch);
void command_parsing(char ch, unsigned long val);

//dichirazione degli stati
State Init(0, stop, start, NULL);
State Idle(1, message_state,idle, NULL);
State Line_Follower(2, message, line_follower, NULL);
State ManualControl(3, message_manual,remote, NULL);

//implementatizone macchina a stati finiti
FiniteStateMachine stateMachine = FiniteStateMachine(&Init);

void setup()
{
	Serial.begin(115200);
	bluetooth.begin(115200);
	bluetooth.println("Bluetooth start!");
}

void loop()
{
	stateMachine.update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void idle () 
{
	stop();
	if(bluetooth.available()) 
	{	
		char ch;
		ch = Serial.read();
		command_state(ch);
	}
}

void start() 
{
	stateMachine.transitionTo(&Idle);
}

Automatic asimov = Automatic();

void line_follower()
{
	//velocità base
	uint8_t motorSpeed = motorSpeedMAX;
	//PID varibili 
	static int last_error = 0;
	static int integral = 0;

 	int kp = 10;
  int kd = 12;
 	int ki = 0;

  static bool print = 0;
  static bool print_ost = 0;
  static int lettura = 0;
  
	//lettura dai sensori
	if(bluetooth.available()) 
	{	
		char ch;
		ch = Serial.read();
      	if(ch == 'e')
      	{
			    stateMachine.transitionTo(&Idle);
      	}		
        if(ch == 's')
        {
          Serial.println("STOP!!");
          stop();
          delay(5000);
        }
    }
	asimov.setInputSensor(ultraSensor.distanceCm());
	/*	verifica costantemente se ci sono oggetti sul percorso e 
	 *	segue la linea se ci sono ostacoli torna inverte il senso di 
	 *	marcia
	*/
	if (asimov.getCheckObstacle() == true)
	{
		asimov.setInputLine(lineFinderDX.readSensors(),
                            lineFinderSX.readSensors());
		//analisi percorso
		if (asimov.getPathError() >= -4 && asimov.getPathError() <= 4)
		{
			int correction = kp * asimov.getPathError() + kd * (asimov.getPathError() - last_error) + ki * (integral + asimov.getPathError());
			last_error = asimov.getPathError();
			motorDX.run(-motorSpeed - correction);
			motorSX.run( motorSpeed - correction);
			print = 0;
      print_ost = 0;
      lettura = 0;
		}
		else if(asimov.getPathError() == 5 && last_error < 0 )        // caso DX
		{
			motorSX.run(motorSpeedMAX/1.5);
			motorDX.run(motorSpeedMAX/1.5);
		}
		else if(asimov.getPathError() == 5 && last_error >= 0 )         // caso SX
		{
			motorDX.run(motorSpeedMIN/1.5);
			motorSX.run(motorSpeedMIN/1.5);
		}
		else if (asimov.getPathError() == 6)
		{
      		stop();
          if (print == 0 && lettura > 65) // controlla la stampa della domanda una sola volta
      		{
          		bluetooth.println( "INCROCIO!! -> Destra (d) o Sinistra (a)?" );
				      print = 1;
              lettura = 0;
                     
        	}// attende il comando via bluetooth
          lettura +=1;
			if(bluetooth.available()) 
			{	
				char ch;
				ch = Serial.read();
      			command_parsing(ch, 0);
      		}
      	}
	}   
	else // presenza ostacoli
	{
		stop();
		last_error = 0;
    if (print_ost == 0) // controlla la stampa della domanda una sola volta
        {
              bluetooth.println( "ATTENZIONE OSTACOLO!! -> inversione in corso..." );
        print_ost = 1;
        }
		motorDX.run(-motorSpeed);
		motorSX.run(-motorSpeed);	
	}
}

void stop()
{
	motorDX.stop();
	motorSX.stop();
}

void command_state(char ch)
{
	switch (ch) 
	{
		case 'm': //va in manual
			stateMachine.transitionTo(&ManualControl);
		break;

		case 'l': // va in linefolower
			stateMachine.transitionTo(&Line_Follower);
		break;
	}
}

void command_parsing(char ch, unsigned long val) 
{
	switch (ch) 
	{
		case 's': //stop
			Serial.println("STOP!!");
			stop();
		break;

		case 'w': //forza l'avanzamento
			motorDX.run(-motorSpeedMAX);
			motorSX.run( motorSpeedMAX);
		break;

		case 'd': //destra
			motorSX.run(motorSpeedMAX/1.5);
			motorDX.run(motorSpeedMAX/1.5);
			delay(250);
			stateMachine.transitionTo(&Line_Follower);
		break;

		case 'a': //sinistra
			motorDX.run(motorSpeedMIN/1.5);
			motorSX.run(motorSpeedMIN/1.5);
			delay(250);
			stateMachine.transitionTo(&Line_Follower);
		break;

		case 'e'://uscita -> Idle
			stop();
			stateMachine.transitionTo(&Idle);
		break;
	}
}

void remote()
{
	char cmd = 'p';
  	cmd = Serial.read();
   	if (cmd == 'w' || cmd == 's' || cmd == 'a' || cmd == 'd' || cmd == 'e') 
   	{  
   		if (cmd == 's') //marcia indietro
        { 
        	motorDX.run(180);
            motorSX.run(-180);
            delay(30);
        }
        else if (cmd == 'w') //avanti
        { 
        	asimov.setInputSensor(ultraSensor.distanceCm());
            /*  verifica costantemente se ci sono oggetti sul percorso e 
            *  segue la linea se ci sono ostacoli torna inverte il senso di 
            *  marcia
            */
            if (asimov.getCheckObstacle() == true)
            {
            	motorDX.run(-180);
                motorSX.run(+184);  // +2 per correzione traiettoria rettilinea
                delay(30);
            }             
            else // presenza ostacoli
            {
            	bluetooth.println( "ATTENZIONE OSTACOLO!! -> inversione in corso..." );
            	stop();
                motorDX.run(-120);
                motorSX.run(-120);
                delay(220); 
       		}
        }
		else if (cmd == 'd') //destra
        {
            motorDX.run(+120);
            motorSX.run(+120);
            delay(25);
        }
        else if ( cmd == 'a')  //sinistra
        {
          	motorDX.run(-120);
        	motorSX.run(-120);
            delay(25);
        }
        else if ( cmd == 'e' ) //uscita -> idle
     	{ 
         	stop();
          	stateMachine.transitionTo(&Idle);
		}
	}  
	else 
	{
    	motorDX.run(0);
    	motorSX.run(0); 
    }   
}
