//T16
//Aleksi Renfors
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <string.h>

#include <math.h>
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 2 

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int kierros = 0;

//tilat
char l_tila = 0;
#define LAMPO 0
#define MAX0 1
#define MAX1 2
#define MIN0 3
#define MIN1 4

//minimi ja maksimi lämpötilat
float max0 = 0;
float max1 = 0;
float* ptr = &max0;
float* ptr1 = &max1;
float* maxit[2] = {ptr, ptr1};

float min0 = 0;
float min1 = 0;
float* ptrm = &min0;
float* ptrm1 = &min1;
float* minit[2] = { ptrm, ptrm1 };


//nappulat
char k_tila = 0;
char kytkin_on = 0;
int lcd_key = 0;
int adc_key_in = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5


String pyorista(float x)
{
	x = round(x * 10);
	x = x / 10;
	String temp = String(x);
	return temp.substring(0,temp.length()-1);
}
int read_LCD_buttons()
{
	adc_key_in = analogRead(0);      // read the value from the sensor
	// my buttons when read are centered at these valies: 0, 144, 329, 504, 741
	// we add approx 50 to those values and check to see if we are close
	if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result

	// For V1.0 comment the other threshold and use the one below:

	if (adc_key_in < 50)   return btnRIGHT;
	if (adc_key_in < 195)  return btnUP;
	if (adc_key_in < 380)  return btnDOWN;
	if (adc_key_in < 555)  return btnLEFT;
	if (adc_key_in < 790)  return btnSELECT;



	return btnNONE;  // when all others fail, return this...
}
int k = 0;//määrittelee kumpaa lämpömittaria käsitellään(0=mittari0 ja 1= mittari1)
void maxeka()
{

	//Käytin tota esimerkeissä käytettyä tapaa tarkistaa kytkimet

	if (!kytkin_on)					//Jos edellinen kytkinpainallus palautunut, niin...
	{
		lcd_key = read_LCD_buttons();	//Luetaan kytkimi� uudelleen

		switch (lcd_key)				//Kytkimien lukemisen j�lkeen toiminnot
		{
			case btnSELECT:
			{
				k_tila = LAMPO;		//jos painetaan selectiä, mennään takaisin lämpötilan näyttöön
				kytkin_on = 1;			
				break;
			}
			case btnUP:				//buttonUP ja buttonLEFTillä voi vaihtaa kumman anturin maksimia tarkastellaan 
			{
				if (k < 1)
					k++;
				lcd.clear();
				kytkin_on = 1;
				break;
			}
			case btnDOWN:
			{
				if (k > 0)
					k--;
				lcd.clear();
				kytkin_on = 1;
				break;
			}
			case btnRIGHT:			//"nollaa" maksimin arvon
			{
;
				lcd.clear();
				sensors.requestTemperatures();
				
				float temp1 =sensors.getTempCByIndex(0);
				float temp2 =sensors.getTempCByIndex(1);

				if (k == 0)
					max0 = temp1;
				else
					max1 = temp2;

				lcd.clear();
				kytkin_on = 1;
				break;
			}


		}

	}

	lcd.setCursor(0, 0);			//pyöristää ja printtaa lämmön
	lcd.print("Max"+ String(k) +":");
	lcd.print(pyorista(*maxit[k]));
	lcd.print("C");


}

void minii()
{

	if (!kytkin_on)					//Jos edellinen kytkinpainallus palautunut, niin...
	{
		lcd_key = read_LCD_buttons();	//Luetaan kytkimi� uudelleen

		switch (lcd_key)				//Kytkimien lukemisen j�lkeen toiminnot
		{
			case btnSELECT:
			{
				k_tila = LAMPO;		//jos painetaan selectiä, mennään takaisin lämpötilan näyttöön
				kytkin_on = 1;
				break;
			}
			case btnUP:				//buttonUP ja buttonLEFTillä voi vaihtaa kumman anturin maksimia tarkastellaan 
			{
				if (k < 1)
					k++;
				lcd.clear();
				kytkin_on = 1;
				break;
			}
			case btnDOWN:
			{
				if (k > 0)
					k--;
				lcd.clear();
				kytkin_on = 1;
				break;
			}
			case btnRIGHT:			//"nollaa" minimin arvon
			{
				lcd.clear();
				sensors.requestTemperatures();
				
				float temp1 = sensors.getTempCByIndex(0);
				float temp2 = sensors.getTempCByIndex(1);
				if (k == 0)
					min0 = temp1;
				else
					min1 = temp2;

				lcd.clear();
				kytkin_on = 1;
				break;
			}


		}

	}
	 
	
	lcd.setCursor(0, 0);				//printtaa minimiarvon
	lcd.print("Min" + String(k) + ":");
	lcd.print(pyorista(*minit[k]));
	lcd.print("C");

}

void lampo()
{



	Serial.print(" Requesting temperature 1");
	sensors.requestTemperatures(); // Send the command to get temperature readings 
	Serial.println("DONE");

	Serial.print("Temperature 1 is: ");
	Serial.println(sensors.getTempCByIndex(1));   

	Serial.print("Temperature 0 is: ");
	Serial.println(sensors.getTempCByIndex(0)); // Why "byIndex"? 
	  // You can have more than one DS18B20 on the same bus.  
	  // 0 refers to the first IC on the wire 

	lcd.setCursor(0, 0);
	lcd.print("                ");
	lcd.setCursor(0, 1);
	lcd.print("                ");

	lcd.setCursor(0, 0);
	float l = sensors.getTempCByIndex(1);
	String ll = String(l);
	ll = ll.substring(0, ll.length() - 1);
	lcd.print(ll);

	lcd.setCursor(0, 0);
	lcd.print("lampo0:");
	lcd.print(pyorista(sensors.getTempCByIndex(0)));
	lcd.print("C");

	lcd.setCursor(0, 1);
	lcd.print("lampo1:");
	lcd.print(pyorista(sensors.getTempCByIndex(1)));
	lcd.print("C");

	if (kierros == 0)		//ekalla kierroksella asetetaan minimi- ja maksimiarvot
		min0 = sensors.getTempCByIndex(0);

	if (kierros == 0)		
		min1 = sensors.getTempCByIndex(1);
	if (kierros == 0)		
		max0 = sensors.getTempCByIndex(0);

	if (kierros == 0)		
		max1 = sensors.getTempCByIndex(1);




	if (sensors.getTempCByIndex(0) > max0)		//jos lämpö on suurempi kuin aikaisempi maksimi
		max0 = sensors.getTempCByIndex(0);

	if (sensors.getTempCByIndex(1) > max1)		//jos lämpö on suurempi kuin aikaisempi maksimi
		max1 = sensors.getTempCByIndex(1);

	if ((sensors.getTempCByIndex(0) < min0))		//jos lämpö on pienimpi kuin aikaisempi maksimi
		min0 = sensors.getTempCByIndex(0);


	if ((sensors.getTempCByIndex(1) < min1))		//jos lämpö on pienimpi kuin aikaisempi maksimi
		min1 = sensors.getTempCByIndex(1);

	kierros++;


	if (!kytkin_on)					//Jos edellinen kytkinpainallus palautunut, niin...
	{
		lcd_key = read_LCD_buttons();	//Luetaan kytkimi� uudelleen

		switch (lcd_key)				//Kytkimien lukemisen j�lkeen toiminnot
		{
			case btnLEFT:
			{
				k_tila = MAX0;		//Muutetaan kellon tila kellonajan asetukseen -> K_ASETUS
				kytkin_on = 1;			//Kytkin painettu -> liputetaan kunnes kytkin nousee yl�s
				lcd.clear();
				break;
			}
			case btnRIGHT:
			{
				k_tila = MIN0;
				kytkin_on = 1;			//Kytkin painettu -> liputetaan kunnes kytkin nousee yl�s
				lcd.clear();
				break;
			}

		}

	}
	
}


void setup(void)
{
	//sensors.setWaitForConversion(false);		//saa vähän paremmin vastaavaksi mut voi tulla ongelmia
	//setupissa vois olla alustus lämmöille
	lcd.begin(16, 2);
	// start serial port 
	Serial.begin(9600);
	// Start up the library 
	sensors.begin();
}
void loop(void)
{
	if (kytkin_on)						//Jos kytkin painettu, testataan onko noussut takaisin yl�s
	{
		lcd_key = read_LCD_buttons();	//Luetaan kytkimi�

		switch (lcd_key)				//Kytkimien lukemisen j�lkeen toiminnot
		{
		case btnNONE:
		{
			kytkin_on = 0;			//Painettu kytkin on noussut takaisin yl�s
			break;
		}
		}
	}


	switch (k_tila)
	{
		case LAMPO:		//näyttää lämpöä
		{
			lampo();
			break;
		}

		case MIN0:		//näyttää minimilämpöä
		{
			minii();
			break;
		}

		case MAX0:		//näyttää maksimilämpöä
		{
			maxeka();
			break;
		}


	}


}