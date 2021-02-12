#include <NewPing.h>

#define TRIGGER_PIN  3
#define ECHO_PIN     2
#define MAX_DISTANCE 400
#include <LiquidCrystal.h>
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
void setup() {
	Serial.begin(115200);
	lcd.begin(16, 2);
}
int maksimi = 0;
int minimi = 0;
int atm = 0;
int kierros = 0;
void loop() {

	lcd.setCursor(0, 0);
	lcd.print("                ");		//tyhjentää näytön
	//delay(100);

	

	Serial.print("Ping: ");
	Serial.print(sonar.ping_cm());


	lcd.setCursor(0, 0);
	lcd.print("Ping: ");
	lcd.print(sonar.ping_cm());
	lcd.print("cm");
	
	atm = sonar.ping_cm();
	if (kierros == 0)		//ekalla kierroksella asetetaan minimiarvo ilmankosteudelle
		minimi = atm;

		if (maksimi < atm)		//jos max arvo on pienempi kuin sen hetkinen arvo
			maksimi = atm;

		if (atm < minimi)		//jos minimi on suurempi kuin sen hetkinen arvo
			minimi = atm;
	

	lcd.setCursor(4, 1);
	lcd.print("   ");		//tyhjentää näytön
	lcd.setCursor(0, 1);		//printtaa max
	lcd.print("Max:");
	lcd.print(maksimi);

	lcd.setCursor(13, 1);
	lcd.print("   ");		//tyhjentää näytön
	lcd.setCursor(9, 1);		//printtaa min
	lcd.print("Min:");
	lcd.print(minimi);

	kierros++;
	delay(100);
}