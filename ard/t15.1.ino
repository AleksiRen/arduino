/*
 Name:		_15.ino
 Created:	11/30/2020 9:51:34 AM
 Author:	Aleksi
*/
#include <NewPing.h>>
#include <LiquidCrystal.h>
// the setup function runs once when you press reset or power the boardi
int admuunnos10 = 0; // AD muunnos haetaan t�nne
float ilmankosteus_float = 0;
unsigned char ilmankosteus = 0;
#define MAX_DISTANCE 200
//nappulat
int lcd_key = 0;

#define CLK_RUN 0b00001100			//Kellotahti  16mhz kide -> ck/256 & autonollaus
#define COMPA_ARVO  20000			//1s vertailuarvo = pulssien lukumäärä	
//näyttö
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int kierros = 0;
char maksimi = 0;
char minimi = 0;


void laske_kosteus()
{
	ilmankosteus_float = (admuunnos10 - 0.16 * 1023) / (0.0062 * 1023);
	ilmankosteus = ilmankosteus_float;
	if (ilmankosteus >= 100)
		ilmankosteus = 100;
}
//AD muuntimen keskeytysohjelma

ISR(TIMER1_COMPA_vect)
{
	ADCSRA |= 0b01000000;
}

ISR(ADC_vect)
{
	admuunnos10 = ADC;
}

void setup() {
	Serial.begin(115200); //USB aktivointi
	lcd.begin(16, 2);
	ADMUX = 0;
	ADMUX |= 0b01000010;		//5v Aref ja AD2 kanava k�ytt��n
	ADCSRA = 0;
	ADCSRA |= 0b10001111;	//AD muunnin ON ja 16mhz/128 = 125khz tahti ja ADint on
	TCCR1A = 0;					//Alustetaan rekisterit
	OCR1A = COMPA_ARVO;			//2s vertailuarvo 
	TCCR1B = CLK_RUN;			//klokki runni ---
	TIMSK1 |= 0x02;				//Timer1 COMPA on
	sei(); //master keskeytys sallitty

}

//the loop function runs over and over again until power down or reset
void loop() {

	//ADCSRA |= 0b01000000;		//Start ad muunnos
	laske_kosteus();
	delay(100);

	lcd.clear();

	lcd.setCursor(0, 0);
	lcd.print("ilmankosteus ");
	lcd.setCursor(13, 0);
	lcd.print(ilmankosteus);
	lcd.print("%");



	if (kierros == 0)		//ekalla kierroksella asetetaan minimiarvo ilmankosteudelle
		minimi = ilmankosteus;

	if (ilmankosteus > maksimi)		//jos kosteus on suurempi kuin aikaisempi maksimi
		maksimi = ilmankosteus;

	if (ilmankosteus < minimi)		//jos kosteus on pienimpi kuin aikaisempi maksimi
		minimi = ilmankosteus;


	lcd.setCursor(0, 1);		//maksimin printtaus
	lcd.print("Max:");
	lcd.print(int(maksimi));
	lcd.print("%");


	lcd.setCursor(9, 1);		//minimin printtaus
	lcd.print("Min:");
	lcd.print(int(minimi));
	lcd.print("%");

	kierros++;
}