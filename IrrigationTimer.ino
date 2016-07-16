/*
* Name:		IrrigationTimer.ino
* Created:	7/10/2016 21:08:05
* Author:	Yusuf KALSEN
* URL: http://github.com/wusiv

* Arduino Irrigation Timer is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Arduino Irrigation Timer is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with Arduino logger. If not, see <http://www.gnu.org/licenses/>.
*
*
*
*
*
*/



#include <EEPROM.h>
#include <RTClib.h>
#include <Wire.h>
RTC_DS1307 RTC;
int devRelayAlter = 8;  //alternate relay port  
int devRelay = 7;

/*
Baslama: 13:48
Bitis: 17:27
fark: 3:39 saat
*/
int addr = 0;
int startHour = 23;	// pump start hour 0-23
int startMin = 10;  // pump start Minute 0-59
int workHour = 3;	// Working Time (Hour) 
int workMin = 30;	// Working Time (Minute) (1-60)	

unsigned long milStart;	// start millis
unsigned long taskStopTime;	// 
unsigned long taskDuration;	// 
char buff[30];
String endDate = "";

void setup() {
	Serial.begin(9600);
	Wire.begin();
	digitalWrite(devRelay, HIGH); //relay close (for No(normally open) connection )
	pinMode(devRelay, OUTPUT);
	RTC.begin();
	taskDuration = (calcSec(workHour, workMin) * 1000); // Working Time (Hour,Minute) to millis
	//RTC.adjust(DateTime(2016, 07, 16, 21, 17, 40));


}


void loop() {
	delay(1000);
	DateTime now = RTC.now();
	Serial.println("#### I N F O #########################");
	Serial.print("# lastMillis: ");
	Serial.println(EEPROMReadlong(1));
	Serial.print("# TaskStopTime: ");
	Serial.println(EEPROMReadlong(5));
	Serial.print("# endTime: ");
	Serial.print(EEPROM.read(10));	Serial.print("-"); Serial.print(EEPROM.read(12)); Serial.print("-"); Serial.print(EEPROM.read(14));
	Serial.print(" / "); Serial.print(EEPROM.read(16));	Serial.print(":"); Serial.println(EEPROM.read(18));
	
	
	

	Serial.println("######################################");

	Serial.println("*******  D E B U G *************");
	Serial.print("* Date: ");
	Serial.print(now.day(), DEC);
	Serial.print(".");
	Serial.print(now.month(), DEC);
	Serial.print(".");
	Serial.print(now.year(), DEC);
	Serial.print(" / ");
	Serial.print(now.hour(), DEC);
	Serial.print(":");
	Serial.print(now.minute(), DEC);
	Serial.print(":");
	Serial.println(now.second(), DEC);

	Serial.print("* taskDuration: ");
	Serial.println(taskDuration); /// debug text

	Serial.print("* Current Millis: ");
	Serial.println(millis());
	Serial.println("******************************");
	if (now.hour() == startHour) {

		milStart = (millis());  //First Run (millisecond)
		taskStopTime = milStart + taskDuration; // task End time calculation
		if (now.minute() == startMin) {

			for (;;) {
				delay(1000);
				Serial.println("========== D E B U G =================");
				Serial.print("= MilStart: ");
				Serial.println(milStart);
				Serial.print("= Current Millis: ");
				Serial.println(millis());

				Serial.print("= taskStopTime: ");
				Serial.println(taskStopTime);

				if ((millis()) < taskStopTime) {
					Serial.println("= Pump Running...");
					digitalWrite(devRelay, LOW);
					EEPROMWritelong(1, millis());
					EEPROMWritelong(5, taskStopTime);
					EEPROM.write(10, now.day());
					EEPROM.write(12, now.month());
					EEPROM.write(14, (now.year()-2000));
					EEPROM.write(16, now.hour());
					EEPROM.write(18, now.minute());
				}
				else {
					break;
					digitalWrite(devRelay, HIGH);
				}
			}

		}
		else {
			digitalWrite(devRelay, HIGH);
			Serial.println("* Pump does NOT Work...");
		}
	}
}


/**

Calc second
@INPUT: Hour,Minute (integer)

@OUT: second
*/
long calcSec(int hh, int mm) {
	int m;
	int h;
	/*if ((hh > 0) && (hh <= 24)) {
		h = hh * 3600;
	}
	else {
		h = 0;
	}

	if ((mm >= 0) && (mm <= 60)) {
		m = mm * 60;
	}
	else {
		m = 0;
	}
	*/
	h = hh * 3600;
	m = mm * 60;
	return (h + m);
}

/*

This function will write a 4 byte (32bit) long to the eeprom at
the specified address to address + 3.
http://playground.arduino.cc/Code/EEPROMReadWriteLong
*/
void EEPROMWritelong(int address, long value)
{
	//Decomposition from a long to 4 bytes by using bitshift.
	//One = Most significant -> Four = Least significant byte
	byte four = (value & 0xFF);
	byte three = ((value >> 8) & 0xFF);
	byte two = ((value >> 16) & 0xFF);
	byte one = ((value >> 24) & 0xFF);

	//Write the 4 bytes into the eeprom memory.
	EEPROM.write(address, four);
	EEPROM.write(address + 1, three);
	EEPROM.write(address + 2, two);
	EEPROM.write(address + 3, one);
}
long EEPROMReadlong(long address)
{
	//Read the 4 bytes from the eeprom memory.
	long four = EEPROM.read(address);
	long three = EEPROM.read(address + 1);
	long two = EEPROM.read(address + 2);
	long one = EEPROM.read(address + 3);

	//Return the recomposed long by using bitshift.
	return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

