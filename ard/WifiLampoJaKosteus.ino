#include <SPI.h>
#include <WiFiNINA.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define sensorPin A0
#define sensorPin2 A1
#define sensorPin3 A5
#define led 9

//lampotilan mittaaminen
#define ONE_WIRE_BUS 2 
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

char ssid[] = "DNA-WLAN-2G-19ED";		//yhdistett‰v‰n verkon nimi ja salasana
char pass[] = "50858758080";
int keyIndex = 0;
//lampotilan ja ilmankosteuden mittaaminen
int reading = 0;
int reading2 = 0;
float ilmankosteus_float = 0;
unsigned char ilmankosteus = 0;

//wifi
String readString;
int status = WL_IDLE_STATUS;
WiFiServer server(80);


//laskee ilmankosteuden
void laske_kosteus()
{
	ilmankosteus_float = (reading - 0.16 * 1023) / (0.0062 * 1023);
	ilmankosteus = ilmankosteus_float;
	if (ilmankosteus >= 100)
		ilmankosteus = 100;
}
String pyorista(float x)
{
	x = round(x * 10);
	x = x / 10;
	String temp = String(x);
	return temp.substring(0, temp.length() - 1);
}

void setup() {
	pinMode(led, OUTPUT);		//ledi
	Serial.begin(9600);

	while (status != WL_CONNECTED) {				//kun ei olla yhteydess‰ verkkoon, koitetaan yhdist‰‰ aina 10 sec v‰lein
		Serial.print("Attempting to connect to Network named: ");
		Serial.println(ssid);
		status = WiFi.begin(ssid, pass);
		delay(10000);
	}
	server.begin();

	Serial.print("SSID: ");		//kun connectataan verkkoon, printtaa laitteen ipn ja verkon nimen
	Serial.println(WiFi.SSID());
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);
}

void loop() {
	sensors.requestTemperatures();		//l‰mpˆtilan mittaus
	float temp1 = sensors.getTempCByIndex(0);

	reading = analogRead(sensorPin);	//kosteuden mittaus	
	laske_kosteus();

	digitalWrite(led, LOW);			//asettaa ledin off

	WiFiClient client = server.available();
	if (client)
	{
		Serial.println("new client");

		while (client.connected())		//kun otetaan yhteys
		{
			digitalWrite(led, HIGH);		//laittaa ledin p‰‰lle sen merkiksi, ett‰ joku otti yhteytt‰
			if (client.available())
			{
				char c = client.read();		//lukee characteri kerrallaan tulevaa tietoa
				if (readString.length() < 100)		//kun readstringi on alle 100
				{
					readString += c;		//lis‰‰ characterin readstringiin
					Serial.write(c);		//serial writee saadun tiedon

					if (c == '\n') {		//jos on linebreak

						client.println("<a href=\"/?kosteus\"\">Kosteus</a>");
						client.println("<br />");
						client.println("<br />");
						client.println("<a href=\"/?lampotila\"\">Lampotila</a><br />");
						client.println("<br />");

						delay(200);
							
						if (readString.indexOf("?kosteus") > 0)		//jos readstringi sis‰lt‰‰ "kosteus", printtaa kosteuden
						{

							client.print("Kosteus: ");
							client.print(pyorista(ilmankosteus_float));
							client.print("%");
							delay(500);

						}
						else {
							if (readString.indexOf("?lampotila") > 0)	//jos readstringi sis‰lt‰‰ "lampotila", printtaa lammon
							{

								client.print("Lampotila: ");
								client.print(pyorista(temp1));
								client.print("C");
								delay(500);

							}
						}
						readString = "";			//nollaa readstringin

						delay(1);
						client.stop();		//lopettaa yhteyden
						Serial.println("client disonnected");
					}
				}
			}
		}
	}
}