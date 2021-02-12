/*T11 2 sek testeri
 Name:		t1ar.ino
 Created:	10/30/2020 9:15:58 AM
 Author:	Aleksi Renfors
*/




// the setup function runs once when you press reset or power the board


//Kytkimien tunnukset
#define SW1 (~PIND & 0b00000100)   //Tosi jos SW1  ON

#define COMPA_ARVO  20000			
#define T	20
#define ON_1	5
#define ON_2	10
#define ON_3	15
#define ON_4	20


#define KV_MAX	100
#define CLK_RUN 0b00001010			//Kellotahti = CK/8 ja autonollaus

//muuttujat
char s_sek = 0;
char k_sek = 0;
char sek = 0;
char kv_laskuri = KV_MAX;

void kv_pois(void)
{


	if (kv_laskuri)
	{
		if (SW1)				//jos napin painallus rekisteröityy, pitää jumissa while-loopissa
								//kunnes painallusta ei ole rekisteröity sataan kierrokseen
			kv_laskuri = KV_MAX;
		else
			kv_laskuri--;
	}
	if (kv_laskuri == 0)
	{
		EIMSK |= 0b00000001;		//sallitaan INT0
		kv_laskuri = KV_MAX;		//voisi jättää pois?
		//sallitaan keskeytykset(INT1)

	}

}
//Multipleksattu näytönohjaus
//PB0 -PB3 kutkettu 4511-ohjaimen D0-D3 BCD-ohjaukseen
//PD3 = 0,01s, PD4 = 0,1s, PD5=1s trankun ohjaukseen
void mux_naytto(void)
{
	PORTB &= 0b11110000;		//näytön nollaus
	PORTB |= s_sek;		//liitetaan 0,01s PORTB:lle
	PORTD |= 0b00111000;		//Näytöt  pimeenä
	PORTD &= 0b11110111; //0,01s näyttö ON
	delay(1);

	PORTB &= 0b11110000;		//näytön nollaus
	PORTB |= k_sek;		//liitetaan 0,1s PORTB:lle
	PORTD |= 0b00111000;		//Näytöt  pimeenä
	PORTD &= 0b11101111; //0,1s näyttö ON
	delay(1);

	PORTB &= 0b11110000;		//näytön nollaus
	PORTB |= sek;		//liitetaan 1s PORTB:lle
	PORTD |= 0b00111000;		//Näytöt  pimeenä
	PORTD &= 0b11011111; //1s näyttö ON
	delay(1);
}
//int0 keskeytyspalvelu
//kellon käynnistys ja pysäytys
ISR(INT0_vect)
{
	if (TCCR1B)	//jos kello ON -> pysäytys
	{
		TCCR1B = 0;
		
	}
	else {				// resetoidaan aika
		resetkello();
		TCCR1B = CLK_RUN;
		
		TCNT1 = 0;
	}
	kv_laskuri = KV_MAX;
	EIMSK &= 0b11111110;		//INT0 OFF
}
//Timer1 compa keskeytys
//- päivitetään 0,01s , 0,1s ja 1s laskureita
ISR(TIMER1_COMPA_vect)
{
	s_sek++;	//sadasosasekunnit  käy koko ajan

	if (s_sek == 10)		//joka  kymmenennellä sadasosalla yksi kymmenesosa
		k_sek++;

	if (k_sek == 10)		//joka kymmenennellä kymmenesosalla sekuntti
		sek++;
	//
	//
	if (s_sek == 10)
		s_sek = 0;

	if (k_sek == 10)
		k_sek = 0;

	if (sek == 10)
		sek = 0;

}

void resetkello()
{

	s_sek = 0;
	k_sek = 0;
	sek = 0;
}

void setup() {


	//porttim��rittelyt c:ll�
	PORTB |= 0b00111111;		//LEDIt off 
	DDRB |= 0b00111111; //LEDI 1-6 output

	PORTD |= 0b01111100;		//piezo on pois ja sw1 ylösveto päälle, trankut off
	DDRD |= 0b01111000;			//piezo ja trankut output
	DDRD &= 0b11111011;  //SW2 = input



	//INT0 aktivointi
	EICRA = 0;		//tasoherk�t INT0  (eicra ja eimsk ==0)=tasoherkkä keskeytys
	EIMSK = 0;
	//EICRA |= 0b00000010; //INT0 laskeva reuna herkkä
	EIMSK |= 0b00000001;	//IINT0  ON


	//TIMER1 COMPA aktivointi
//
	TCCR1A = 0;			//Alustetaan rekisterit
	TCCR1B = 0;
	//TCCR1B |= 0b00001101;	//CK /1024 ja Autonollaus COMPAn j�lkeen
	OCR1A = COMPA_ARVO;		//2s vertailuarvo 
	TIMSK1 |= (1 << OCIE1A);//Timer1 COMPA on
	sei();

}


void loop()
{


	if (kv_laskuri)
		kv_pois();
	

	if (TCCR1B != 0)
	{
		
		PORTD |= 0b00111000;		//jos timeri käynnissä, näyttö pimeenä
		

	}
	else
	{
		mux_naytto();		//kun timeri ei ole käynnissä, näytetään numerot
		
		
		
	}

}


