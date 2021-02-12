//﻿/* LCD testaus
// Name:		t12.ino
// Created:	11/25/2020 2:49:27 PM
// Author:	Aleksi Renfors
//*/

#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

char ctr = 0;
char apu = 0;
#define KELLO 0		//kellon tilat
#define K_ASETUS 1
#define H_ASETUS 2
#define HALYTYS 3

#define TUNNIT 0
#define MINUUTIT 1
#define SEKUNNIT 2
#define ANYSW (read_LCD_buttons() == 1| read_LCD_buttons() == 0|read_LCD_buttons() == 2|read_LCD_buttons() == 3|read_LCD_buttons() == 4)
#define CLK_RUN 0b00001100			//Kellotahti  16mhz kide -> ck/256 & autonollaus
#define COMPA_ARVO  62500			//1s vertailuarvo = pulssien lukumäärä	


//
//
char K_tila = KELLO;
char asetus = 0;
char k_aika[6] = { 0,0,9,5,3,2 };		//kello 23:59
char k_aika2[6] = { 0,0,0,0,0,0 };		//kellonajan asetuksen alkuarvo
char h_aika[6] = { 0,1,9,5,3,2 };		//herätys alkuarvo
char yesno = 0;							//kyllä vai ei?
char kel = 0;							//näytetäänkö nykyinen kellonaika vaiko asetettava hälytys/kello.(0 = nykyinen aika, 1 = asetettava aika, 2= varmistus)
//nappulat
int lcd_key = 0;
int adc_key_in = 0;

#define btnRIGHT  3			//nappuoloiden painamisen palautusarvot
#define btnUP     0
#define btnDOWN   4
#define btnLEFT   2
#define btnSELECT 1
#define btnNONE   5
//

char kmax[6] = { 9,5,9,5,9,2 };

//halytys
String teksti = "";
String teksti2 = "HALYTYSss!!!!";
String teksti3 = "               ";
unsigned long pm = 0;
//vanhat

#define T	20
#define ON_1	5
#define ON_2	10
#define ON_3	15
#define ON_4	20

#define KV_MAX	100//kvpoisto
//muuttujat

String yes = "Yes";					//varmistuksen yes/no vilkutus
String no = "No";
String halyt[2] = { "off", "on" };	//halytys on tai off
String YesNo[2] = { no, yes };		//varmistus
char kohta = 5;						//ajan asettamiseen käytettävän navigoinnin sen hetkinen kohta :)
char haly = 0;						//onko halytys asetettu
char temp = 0;						//varmistuksessa ottaa talteen onko vastattu kyllä vai ei 

unsigned long m = 0;				//testilaskentaan millsseille
//
char paikat[7] = { 15,14,12,11,9,8,5 };
int read_LCD_buttons()
{
	adc_key_in = analogRead(0);				// read the value from the sensore
	if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
	if (adc_key_in < 50)   return btnRIGHT;
	if (adc_key_in < 195)  return btnUP;
	if (adc_key_in < 380)  return btnDOWN;
	if (adc_key_in < 555)  return btnLEFT;
	if (adc_key_in < 790)  return btnSELECT;

	return btnNONE;  // when all others fail, return this...
}

//kellonaika  lcd:lle
void LCD_ohjaus(void)
{

	lcd.setCursor(0, 0);
	lcd.print("Kello ");

	lcd.setCursor(10, 0);
	lcd.print(":");

	lcd.setCursor(13, 0);
	lcd.print(":");
	//1 sec
	char apu;
	lcd.setCursor(15, 0);
	apu = k_aika[0] | 0x30;
	lcd.print(apu);

	//10s
	lcd.setCursor(14, 0);
	apu = k_aika[1] | 0x30;
	lcd.print(apu);

	//1min
	lcd.setCursor(12, 0);
	apu = k_aika[2] | 0x30;
	lcd.print(apu);
	//10min
	lcd.setCursor(11, 0);
	apu = k_aika[3] | 0x30;
	lcd.print(apu);
	//1h
	lcd.setCursor(9, 0);
	apu = k_aika[4] | 0x30;
	lcd.print(apu);
	//10h
	lcd.setCursor(8, 0);
	apu = k_aika[5] | 0x30;
	lcd.print(apu);


	lcd.setCursor(10, 1);
	lcd.print(":");

	lcd.setCursor(13, 1);
	lcd.print(":");

	//halytysajan printtaus
	lcd.setCursor(15, 1);
	apu = h_aika[0] | 0x30;
	lcd.print(apu);

	//10s
	lcd.setCursor(14, 1);
	apu = h_aika[1] | 0x30;
	lcd.print(apu);

	//1min
	lcd.setCursor(12, 1);
	apu = h_aika[2] | 0x30;
	lcd.print(apu);
	//10min
	lcd.setCursor(11, 1);
	apu = h_aika[3] | 0x30;
	lcd.print(apu);
	//1h
	lcd.setCursor(9, 1);
	apu = h_aika[4] | 0x30;
	lcd.print(apu);
	//10h
	lcd.setCursor(8, 1);
	apu = h_aika[5] | 0x30;
	lcd.print(apu);

	lcd.setCursor(0, 1);
	lcd.print("H=");
	lcd.setCursor(2, 1);
	lcd.print(halyt[haly]);

}
void LCD_ohjaus2(void)
{

	lcd.setCursor(10, 0);
	lcd.print(":");

	lcd.setCursor(13, 0);
	lcd.print(":");
	//1 sec
	char apu;
	lcd.setCursor(15, 0);
	apu = k_aika2[0] | 0x30;
	lcd.print(apu);

	//10s
	lcd.setCursor(14, 0);
	apu = k_aika2[1] | 0x30;
	lcd.print(apu);

	//1min
	lcd.setCursor(12, 0);
	apu = k_aika2[2] | 0x30;
	lcd.print(apu);
	//10min
	lcd.setCursor(11, 0);
	apu = k_aika2[3] | 0x30;
	lcd.print(apu);
	//1h
	lcd.setCursor(9, 0);
	apu = k_aika2[4] | 0x30;
	lcd.print(apu);
	//10h
	lcd.setCursor(8, 0);
	apu = k_aika2[5] | 0x30;
	lcd.print(apu);

	lcd.setCursor(0, 1);
	lcd.print("H=");

	lcd.setCursor(2, 1);
	lcd.print("   ");

	lcd.setCursor(2, 1);
	lcd.print(halyt[haly]);
}
void LCD_ohjaus3(void)
{

	lcd.setCursor(0, 0);		//printtaa yes
	lcd.print("Oletko varma??");

	lcd.setCursor(0, 1);		//printtaa yes
	lcd.print(YesNo[1]);

	lcd.setCursor(14, 1);		//printtaa no
	lcd.print(YesNo[0]);



}

void kello(void)					// näyttää kellonajan ja kyttää tuleeko hälytysajan tai kellonajan asetuksen napin painalluksia
{
	kel = 0;						//asettaa näytettäväksi kellonajan
	if (read_LCD_buttons() == 1)
	{
		kel = 1;
		//TCCR1B = 0;				//sammuttaa kellon heti
		K_tila = K_ASETUS;
		
	}
	if (read_LCD_buttons() == 2)
	{
		kel = 1;
		K_tila = H_ASETUS;
		
	}
	if (ANYSW)
		kv_pois();


}
const long interval = 400;
unsigned long previousMillis = 0;

String valit = "          ";
String nuoli = "^";
String nuoli2 = "<";
String kirjotettava = valit;
String kirjotettava2 = valit;
String kirjotettava3 = valit;


void h_asetus()
{

	
	unsigned long currentMillis = millis();
	lcd.setCursor(0, 0);
	lcd.print("H_set ");								//(hälytys set)
	lcd.setCursor(5, 1);								//tyhjentää aiemman nuolen pois
	lcd.print("            ");

	lcd.setCursor(paikat[kohta], 1);					//kirjoittaa nuolen vaihdettavan numeron alle
	if (currentMillis - previousMillis >= interval) {	//vilkuttaa nuolta näytöllä
		previousMillis = currentMillis;

		if (kirjotettava == valit) {
			kirjotettava = nuoli;
		}
		else {
			kirjotettava = valit;
		}
		if (kirjotettava2 == valit)
			kirjotettava2 = nuoli2;
		else
		{
			kirjotettava2 = valit;
		}
	}


	if (kohta != 6)						//jos ollaan kutoskohdassa, eli (H=), näytetään (<) perusnuolen sijaan
	{
		lcd.print(kirjotettava);
	}
	else
	{
		lcd.print(kirjotettava2);
	}


	if (read_LCD_buttons() == 3)		//jos painetaan right- nappulaa, numeron valinta menee oikealle
	{
		if (kohta > 0)
			kohta--;


	}

	if (read_LCD_buttons() == 2)		//jos painetaan left- nappulaa, numeron valinta menee vasemmalle
	{
		if (kohta < 6)
			kohta++;

	}


	if (read_LCD_buttons() == 0)						//numeroiden plussaaminen
	{
		if (kohta != 4 && kohta != 6)					//jos ei olla neloskohdassa, eli tuntien ykkösissä
		{
			if (k_aika2[kohta] >= kmax[kohta])
				k_aika2[kohta] = 0;
			else
				k_aika2[kohta]++;
		}
		else											//jos ollaan neloskohdassa eli tuntien ykkösissä
		{
			if (k_aika2[5] == 2)						//jos alkaa 2 (eli 20,21,22.. yms) tuntien ykkösille asetetaan maksimiksi 4
			{
				if (k_aika2[kohta] >= 4)
					k_aika2[kohta] = 0;
				else
					k_aika2[kohta]++;

			}
			else
			{											//jos ei ole 2 tuntien kympeissä(09, 15... 19 yms)
				if (k_aika2[kohta] >= kmax[kohta])
					k_aika2[kohta] = 0;
				else
					k_aika2[kohta]++;
			}

			if (kohta == 6)								//jos ollaan on/off kohdassa hälytykselle
			{
				if (haly >= 1)
					haly = 0;
				else
					haly++;
			}

		}

		if (k_aika2[5] == 2 && k_aika2[4] == 4)				//jos on asetettu 24 tunneille, loput voi olla vaan nollia
		{
			k_aika2[3] = 0;
			k_aika2[2] = 0;
			k_aika2[1] = 0;
			k_aika2[0] = 0;
		}

	}
	if (read_LCD_buttons() == 4)					//numeroiden miinustaminen
	{
		if (kohta != 4 && kohta != 6)				//jos ei olla neloskohdassa, tai kutosessa(ON/OFF haly) asetetaan minimit ja maksimit normaalisti
		{
			if (k_aika2[kohta] <= 0)
				k_aika2[kohta] = kmax[kohta];
			else
				k_aika2[kohta]--;
		}
		else										//jos ollaan neloskohdassa, tuntien kymppien ollessa 2, asetetaan tuntien ykkösten maksimiksi 4 
		{
			if (k_aika2[5] == 2)
			{
				if (k_aika2[kohta] <= 0)			//jos miinustetaan nollasta, laitetaan max arvoon
					k_aika2[kohta] = 4;
				else
					k_aika2[kohta]--;				//muutoin vähennetään normaalisti
			}
			else									//jos ei ole kakkosta tuntien kympeissä, normaalit maksimit
			{
				if (k_aika2[kohta] == 0)
					k_aika2[kohta] = kmax[kohta];
				else
					k_aika2[kohta]--;
			}

			if (kohta == 6)							//jos ollaan on/off kohdassa hälytykselle
			{
				if (haly <= 0)
					haly = 1;
				else
					haly--;
			}
		}
		if (k_aika2[5] == 2 && k_aika2[4] == 4)		//jos ollaan 24 tunneissa, asetetaan muut nolliksi
		{
			k_aika2[3] = 0;
			k_aika2[2] = 0;
			k_aika2[1] = 0;
			k_aika2[0] = 0;
		}

	}

	if (read_LCD_buttons() == 1)
	{
											//jos painetaan selectiä, takaisin kelloon

		for (size_t i = 0; i < 6; i++)
		{
			h_aika[i] = k_aika2[i];
		}

		K_tila = KELLO;
		lcd.setCursor(5, 1);				//tyhjentää aiemman nuolen pois
		lcd.print("            ");

	}
	if (ANYSW)		
		kv_pois();

}
void  k_asetus()
{

	
	unsigned long currentMillis = millis();
	lcd.setCursor(0, 0);
	lcd.print("K_set");

	lcd.setCursor(5, 1);					//tyhjentää aiemman nuolen pois
	lcd.print("            ");

	lcd.setCursor(paikat[kohta], 1);		//kirjoittaa nuolen vaihdettavan numeron alle
	if (currentMillis - previousMillis >= interval) {
		// save the last time you blinked the LED
		previousMillis = currentMillis;

		// if the LED is off turn it on and vice-versa:
		if (kirjotettava == valit) {
			kirjotettava = nuoli;
		}
		else {
			kirjotettava = valit;
		}
		if (kirjotettava2 == valit)
			kirjotettava2 = nuoli2;
		else
		{
			kirjotettava2 = valit;
		}
	}

	if (kohta != 6)
	{
		lcd.print(kirjotettava);
	}
	else
	{
		lcd.print(kirjotettava2);
	}


	if (read_LCD_buttons() == 3)		//jos painetaan right- nappulaa, numeron valinta menee oikealle
	{
		if (kohta > 0)
			kohta--;


	}

	if (read_LCD_buttons() == 2)		//jos painetaan left- nappulaa, numeron valinta menee vasemmalle
	{
		if (kohta < 6)
			kohta++;

	}




	if (read_LCD_buttons() == 0)				//numeroiden plussaaminen
	{
		if (kohta != 4 && kohta != 6)			//jos ei olla nelos kohdassa, eli tuntien ykkösissä
		{
			if (k_aika2[kohta] >= kmax[kohta])
				k_aika2[kohta] = 0;
			else
				k_aika2[kohta]++;
		}
		else									//jos ollaan nelos kohdassa eli tuntien ykkösissä
		{
			if (k_aika2[5] == 2)				//jos alkaa 2 (eli 20,21,22.. yms) tuntien ykkösille asetetaan maksimiksi 4
			{
				if (k_aika2[kohta] >= 4)
					k_aika2[kohta] = 0;
				else
					k_aika2[kohta]++;

			}
			else
			{											//jos ei ole 2 tuntien kympeissä(09, 15... 19 yms)
				if (k_aika2[kohta] >= kmax[kohta])
					k_aika2[kohta] = 0;
				else
					k_aika2[kohta]++;
			}
			if (kohta == 6)						//jos ollaan on/off kohdassa hälytykselle
			{
				if (haly >= 1)
					haly = 0;
				else
					haly++;
			}

		}

		if (k_aika2[5] == 2 && k_aika2[4] == 4)			//jos on asetettu 24 tunneille, loput voi olla vaan nollia
		{
			k_aika2[3] = 0;
			k_aika2[2] = 0;
			k_aika2[1] = 0;
			k_aika2[0] = 0;
		}

	}
	if (read_LCD_buttons() == 4)				//numeroiden miinustaminen
	{
		if (kohta != 4 && kohta != 6)			//jos ei olla neloskohdassa, asetetaan minimit ja maksimit normaalisti
		{
			if (k_aika2[kohta] <= 0)
				k_aika2[kohta] = kmax[kohta];
			else
				k_aika2[kohta]--;
		}
		else						//jos ollaan neloskohdassa, tuntien kymppien ollessa 2, asetetaan tuntien ykkösten maksimiksi 4 
		{
			if (k_aika2[5] == 2)
			{
				if (k_aika2[kohta] <= 0)		//jos miinustetaan nollasta, laitetaan max arvoon
					k_aika2[kohta] = 4;
				else
					k_aika2[kohta]--;		//muutoin vähennetään normaalisti
			}
			else						//jos ei ole kakkosta tuntien kympeissä, normaalit maksimit
			{
				if (k_aika2[kohta] == 0)
					k_aika2[kohta] = kmax[kohta];
				else
					k_aika2[kohta]--;
			}
			if (kohta == 6)		//jos ollaan on/off kohdassa hälytykselle
			{
				if (haly <= 0)
					haly = 1;
				else
					haly--;
			}
		}
		if (k_aika2[5] == 2 && k_aika2[4] == 4)		//jos ollaan 24 tunneissa, asetetaan muut nolliksi
		{
			k_aika2[3] = 0;
			k_aika2[2] = 0;
			k_aika2[1] = 0;
			k_aika2[0] = 0;
		}
	}
	varmistus();
	if (ANYSW)
		kv_pois();
}

void varmistus()
{

	if (read_LCD_buttons() == 1)//jos painetaan selectiä, mennään varmistukseen kellon asetusten tallentamisesta
	{
		
		kel = 2;	//laittaa varmistusnäytön näkyviin
		lcd.setCursor(0, 0);		//tyhjentää näytön
		lcd.print("                ");
		lcd.setCursor(0, 1);
		lcd.print("                ");

		while (true)
		{
			unsigned long currentMillis = millis();
			if (ANYSW)
				kv_pois();
			if (temp == 1)
			{
				lcd.setCursor(13, 1);
				lcd.print(" ");

				lcd.setCursor(3, 1);			//tyhjentää aiemman nuolen pois
				lcd.print("<");
			}
			else
			{
				lcd.setCursor(3, 1);
				lcd.print(" ");

				lcd.setCursor(13, 1);			//tyhjentää aiemman nuolen pois
				lcd.print(">");
			}

			LCD_ohjaus3();
			//right ja left nappulat
			if (read_LCD_buttons() == 3)		//jos painetaan right- nappulaa, numeron valinta menee oikealle
			{

				YesNo[0] = no;
				YesNo[1] = yes;
				if (temp > 0)
					temp--;

			}

			if (read_LCD_buttons() == 2)		//jos painetaan left- nappulaa, numeron valinta menee vasemmalle
			{


				YesNo[0] = no;
				YesNo[1] = yes;
				if (temp < 1)
					temp++;

			}


			if (currentMillis - previousMillis >= interval) {//laittaa valittuna olevan valinnan vilkkumaan näytöllä

				previousMillis = currentMillis;

				if (YesNo[temp] == yes || YesNo[temp] == no)// jos yes tai no, laitetaan tyjhäksi ja jos tyhjä, laitetaan yes tai no tempin arvon mukaan(0=no, 1=yes) (vilkuttaa niitä näytöllä)
				{
					YesNo[temp] = "   ";
				}
				else
				{
					if (temp == 1)
						YesNo[temp] = yes;
					else

						YesNo[temp] = no;
				}

			}
			if (read_LCD_buttons() == 1)		//kun painetaan toisen kerran selectiä, valinnan mukaan joko asettaa uuden ajan, tai ei tee mitään
			{

				if (temp == 1)
				{
					for (size_t i = 0; i < 6; i++)
					{
						k_aika[i] = k_aika2[i];
					}

					K_tila = KELLO;
					lcd.setCursor(5, 1);			//tyhjentää aiemman nuolen pois
					lcd.print("            ");
			
					kel = 0;		//taitaa olla turha? kellonäyttö päälle
					lcd.clear();
					delay(20);
					break;
				}
				else
				{
					K_tila = KELLO;
					lcd.setCursor(5, 1);			//tyhjentää aiemman nuolen pois
					lcd.print("            ");
		;
					kel = 0;		//taitaa olla turha? kellonäyttö päälle
					lcd.clear();
					delay(20);
					break;
				}
				//laittaa kellon näkymään
				temp = 0;		//temppi takas 0
			}
			if (ANYSW)
				kv_pois();
		}
		if (ANYSW)
			kv_pois();

	}
	lcd.setCursor(0, 0);			//tyhjentää ruudun

}



void halytys()
{
	lcd.clear();
	while (true)
	{
		if (ANYSW)
		{
			kv_pois();
			break;
			
		}

		unsigned long currentMillis = millis();

		if (currentMillis - pm >= interval) {
			// save the last time you blinked the LED
			pm = currentMillis;

			// if the LED is off turn it on and vice-versa:
			if (teksti == teksti3) {
				teksti = teksti2;
			}
			else {
				teksti = teksti3;
			}
			lcd.setCursor(0, 0);
			lcd.print(teksti);

		}

	}
		lcd.setCursor(0, 0);			//tyhjentää 
		lcd.print("                ");
		lcd.setCursor(5, 0);
		lcd.print("                ");
		K_tila = 0;
	
}
void resetclock()
{
	k_aika[0] = 0;
	k_aika[1] = 0;
	k_aika[2] = 0;
	k_aika[3] = 0;
	k_aika[4] = 0;
	k_aika[5] = 0;
}


//Timer1compa keskeytys
//askelletaan aikaa 1s välein

ISR(TIMER1_COMPA_vect)
{
	//sek ykköset
	k_aika[0]++;
	if (k_aika[0] == 10)
	{
		k_aika[0] = 0;
		k_aika[1]++;


	}
	//kn sek kympit 60
	if (k_aika[1] == 6)
	{
		k_aika[1] = 0;
		k_aika[2] ++;
	}
	//kun minuuttien ykköset ==10
	if (k_aika[2] == 10)
	{
		k_aika[2] = 0;
		k_aika[3]++;
	}
	//kun minuuttien kympit == 60
	if (k_aika[3] == 6)
	{
		k_aika[3] = 0;
		k_aika[4]++;
	}
	//kun tuntien ykköset ==10
	if (k_aika[4] == 10)
	{
		k_aika[4] = 0;
		k_aika[5]++;
	}
	//kun osuu 24h, resetoidaan kello
	if (k_aika[5] == 2 && k_aika[4] == 4)
	{
		k_aika[0] = 0;
		k_aika[1] = 0;
		k_aika[2] = 0;
		k_aika[3] = 0;
		k_aika[4] = 0;
		k_aika[5] = 0;
	}

	if (haly == 1)		//jos hälytys on asetettu päälle, tarkistaa hälytysajan
	{
		if (k_aika[5] == h_aika[5] && k_aika[4] == h_aika[4] && k_aika[3] == h_aika[3] && k_aika[2] == h_aika[2] && k_aika[1] == h_aika[1] && k_aika[0] == h_aika[0])
		{
			K_tila = HALYTYS;
			
		}
	}

}
//pe
void kv_pois(void)
{
	char kv_laskuri = KV_MAX;

	while (kv_laskuri)
	{
		if (ANYSW)				//jos napin painallus rekisteröityy, pitää jumissa while-loopissa
								//kunnes painallusta ei ole rekisteröity sataan kierrokseen
			kv_laskuri = KV_MAX;
		else
			kv_laskuri--;
	}


}


// the setup function runs once when you press reset or power the board
void setup() {
	lcd.begin(16, 2);			// start the library
	TCCR1A = 0;					//Alustetaan rekisterit
	OCR1A = COMPA_ARVO;			//2s vertailuarvo 
	TCCR1B = CLK_RUN;			//klokki runni ---
	TIMSK1 |= 0x02;				//Timer1 COMPA on
	sei();

	//TImer1compa aktivointi
}

// the loop function runs over and over again until power down or reset
void loop() {
	

	if (kel == 0)		//eka näyttää kellonajan, toka asetettavan kellonajan ja kolmas varmistuksen
	{
		LCD_ohjaus();
	}
	else if (kel == 1)
	{
		
		LCD_ohjaus2();
	}
	else if (kel == 2)
	{
		
		LCD_ohjaus3();
	}

	switch (K_tila)
	{

		case KELLO:
		{
			kello();
			break;
		}
		case K_ASETUS:
		{
			k_asetus();
			break;
		}
		case H_ASETUS:
		{
			h_asetus();
			break;
		}
		case HALYTYS:
		{
			halytys();
			break;
		}
	}



}