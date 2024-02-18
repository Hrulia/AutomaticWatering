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
//2 ������

//Input capacitive sensor. GPIO ��� � ���������� ���
#define SENSOR_HUMIDITY_PIN A0  /*constexpr auto*/ 

//Input light sensor
#define SENSOR_LIGHT_PIN A1

//Input button 
#define BUTTON_CONFIG_PIN 2

//Output Valve
#define VALVE_OPEN_PIN 12
//#define ValveClose 11


unsigned long durationWatering = 1200000; //������������ ������. �� ��������� 5 �����. 300'000
unsigned long frequencyWatering = 54400000; //������������� ������. �� ��������� ��� � �����. 86'400'000

int thresholdLight = 60; //����������� ������������ ����������� �����
int thresholdHumidity = 45; //����������� ��������� ��� ������ ������
																						
bool permissionWatering = true; //���� ���������� ������

	// Initialize TM1637TinyDisplay - 4 Digit Display
	TM1637TinyDisplay displayTM1637(TM1637_CLK_PIN, TM1637_DIO_PIN);

	//���� ���������� ���������� ������.
	myCycle frequencyCycle(frequencyWatering, true);

	//������ ����� ������������ ������
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
	pinMode(VALVE_OPEN_PIN, OUTPUT); digitalWrite(VALVE_OPEN_PIN, HIGH); //�������� LOW
	//pinMode(ValveClose, OUTPUT); digitalWrite(ValveClose, HIGH);

}


void loop(){
	Serial.println();

	//������� ������ ������
	if (frequencyCycle.check()) { permissionWatering = true; }
	if (durationCycle.check()) { Serial.println("durationCycle 1 "); digitalWrite(VALVE_OPEN_PIN, HIGH); }; //���������� �����, �������� ����


	/* ��������� ������ � ������� ������������
	����� ������ �������.*/
	int valueLightSensor = analogRead(SENSOR_LIGHT_PIN);
	Serial.print("Light "); Serial.println(valueLightSensor); displayTM1637.showNumber(8000+valueLightSensor); delay(1000);

	// ��������� ������ � ������� ��������� �����
	int valueHumiditySensor = analogRead(SENSOR_HUMIDITY_PIN);
	Serial.print("Humidity "); Serial.println(valueHumiditySensor);
	valueHumiditySensor=map(valueHumiditySensor, 650, 400, 0, 100);//500/300'����������' �� ������ � ������������ � ����. ����� � ������� 0-3�
	Serial.print("Humidity ");Serial.println(valueHumiditySensor);
	
	displayTM1637.showNumber(valueHumiditySensor);
	

	//������� �������� ������ � ���������� ����� ����� �� 10 ������
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
				/*�������� �����*/
				permissionWatering = false;
				frequencyCycle.reStart(); frequencyCycle.clear(); //����� ����� ���������� ���������� ������.
				//open valve
				digitalWrite(VALVE_OPEN_PIN, LOW);
				durationCycle.reStart(); durationCycle.clear(); //����� ����� ������������ ������.
			}
		}


	}
	
	



	// ��� 100 ��
	delay(3000);


	////// ������������ �������� �� 8 �����������
	////countled = map(avalue, maxvalue, minvalue, 0, 7);
	////// ��������� ������ ���������
	////for (int i = 0; i<8; i++)
	////{
	////	if (i <= countled)
	////		digitalWrite(ledPins[i], HIGH); //�������� ���������
	////	else
	////		digitalWrite(ledPins[i], LOW);  // ����� ���������
	////}
}


