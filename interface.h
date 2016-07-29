#ifndef __INTERFACE_H
#define __INTERFACE_H

void message()
{
	Serial.println("Ciao! Sono in attesa di un comando");
	Serial.println("puoi usare:");
	Serial.println(" w -> per avanzare");
	Serial.println(" d -> per girare a destra");    
	Serial.println(" a -> per girare a sinistra");
	Serial.println(" s -> per fermarti o per andare indietro");
	Serial.println(" e -> uscita stato auttuale");
}

void message_state()
{
	Serial.println("Ciao! Scegli la modalità di funzionamento");
	Serial.println("puoi usare:");
	Serial.println(" l -> line follower");
	Serial.println(" m -> controllo manuale");
}

void message_manual()
{
	Serial.println("Modalità Manuale,");
	Serial.println("comandi disponibili:");
	Serial.println(" w -> per avanzare");
	Serial.println(" d -> per girare a destra");    
	Serial.println(" a -> per girare a sinistra");
	Serial.println(" s -> per fermarti o per andare indietro");
	Serial.println(" e -> uscita stato auttuale");
}

#endif
