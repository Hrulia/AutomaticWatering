/*
    Name:       AutoOppressionByHumidity.ino
    Created:	01.05.2021 10:38:51
    Author:     AUTO33826TPLINK\Sergey
*/

#include <FastIO.h>
#include <EncButton.h>
#include "myCycle.h"
#include <TM1637TinyDisplay.h>

// Module TM1637 connection pins
#define TM1637_CLK_PIN 3
#define TM1637_DIO_PIN 4
//6,7
//2 кнопка

//Input capacitive sensor. GPIO пин с поддержкой АЦП
#define SENSOR_HUMIDITY_PIN A0  /*constexpr auto*/ 

//Input light sensor
#define SENSOR_LIGHT_PIN A1

//Input button 
#define BUTTON_CONFIG_PIN 2

//Output Valve
#define VALVE_OPEN_PIN 12
//#define ValveClose 11


unsigned long durationWatering = 1200000; //Длительность полива. По умолчанию 5 минут. 300'000
unsigned long frequencyWatering = 54400000; //периодичность полива. По умолчанию раз в сутки. 86'400'000

int thresholdLight = 60; //минимальная освещенность разрешающая полив
int thresholdHumidity = 45; //минимальная влажность для начала полива
																						
bool permissionWatering = true; //флаг разрешения полива

	// Initialize TM1637TinyDisplay - 4 Digit Display
	TM1637TinyDisplay displayTM1637(TM1637_CLK_PIN, TM1637_DIO_PIN);

	//цикл разрешения повторного полива.
	myCycle frequencyCycle(frequencyWatering, true);

	//запуск цикла длительности полива
	myCycle durationCycle(durationWatering, true);



void setup()
{
	Serial.begin(9600);
		//Display output
	displayTM1637.setBrightness(BRIGHT_HIGH);

	//Ports setting
	pinMode(SENSOR_LIGHT_PIN, INPUT);
	//analogReference(INTERNAL);
	pinMode(BUTTON_CONFIG_PIN, INPUT_PULLUP);
	pinMode(VALVE_OPEN_PIN, OUTPUT); digitalWrite(VALVE_OPEN_PIN, HIGH); //Активный LOW
	//pinMode(ValveClose, OUTPUT); digitalWrite(ValveClose, HIGH);

}


void loop(){
	Serial.println();

	//обновим данные циклов
	if (frequencyCycle.check()) { permissionWatering = true; }
	if (durationCycle.check()) { Serial.println("durationCycle 1 "); digitalWrite(VALVE_OPEN_PIN, HIGH); }; //прекращаем полив, закрывем кран


	/* считываем данные с датчика освещенности
	Полив только вечером.*/
	int valueLightSensor = analogRead(SENSOR_LIGHT_PIN);
	Serial.print("Light "); Serial.println(valueLightSensor); displayTM1637.showNumber(8000+valueLightSensor); delay(1000);

	// считываем данные с датчика влажности почвы
	int valueHumiditySensor = analogRead(SENSOR_HUMIDITY_PIN);
	Serial.print("Humidity "); Serial.println(valueHumiditySensor);
	valueHumiditySensor=map(valueHumiditySensor, 650, 400, 0, 100);//500/300'калибровка' по сухому и погруженному в воду. Выход у датчика 0-3В
	Serial.print("Humidity ");Serial.println(valueHumiditySensor);
	
	displayTM1637.showNumber(valueHumiditySensor);
	

	//считаем значение кнопки и переключим режим крана на 10 секунд
	if (digitalRead(BUTTON_CONFIG_PIN) == LOW) {
		//digitalWrite(VALVE_OPEN_PIN, !(digitalRead(VALVE_OPEN_PIN)));
		digitalWrite(VALVE_OPEN_PIN, LOW);
		Serial.print("Manual opening of the valve "); Serial.println(digitalRead(VALVE_OPEN_PIN));
		delay(durationWatering);
	}
	

	Serial.print("permissionWatering ");Serial.println(permissionWatering);
		if (permissionWatering) {
		if (valueLightSensor < thresholdLight) {
			if (valueHumiditySensor < thresholdHumidity) {
				/*начинаем полив*/
				permissionWatering = false;
				frequencyCycle.reStart(); frequencyCycle.clear(); //сброс цикла разрешения повторного полива.
				//open valve
				digitalWrite(VALVE_OPEN_PIN, LOW);
				durationCycle.reStart(); durationCycle.clear(); //сброс цикла длительности полива.
			}
		}


	}
	
	



	// ждём 100 мс
	delay(3000);


	////// масштабируем значение на 8 светодиодов
	////countled = map(avalue, maxvalue, minvalue, 0, 7);
	////// индикация уровня влажности
	////for (int i = 0; i<8; i++)
	////{
	////	if (i <= countled)
	////		digitalWrite(ledPins[i], HIGH); //зажигаем светодиод
	////	else
	////		digitalWrite(ledPins[i], LOW);  // гасим светодиод
	////}
}


