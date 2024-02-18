/*
    Name:       AutoOppressionByHumidity.ino
    Created:	01.05.2021 10:38:51
    Author:     AUTO33826TPLINK\Sergey
*/


#include <EEPROM.h>

#define EB_FAST 0         // ������� �������� �������� �������� (��������������� � ���������)
#include <EncButton.h>
#include "MotoValve.h"
#include <TM1637TinyDisplay.h>


// Module TM1637 connection pins
#define TM1637_CLK_PIN 3
#define TM1637_DIO_PIN 4

//Input capacitive sensor. GPIO ��� � ���������� ���
#define SENSOR_HUMIDITY_PIN A0  /*constexpr auto*/ 

//Input light sensor
#define SENSOR_LIGHT_PIN A1
#define MIN_LIGHT_PIN 2  //����� ���� 1 ��� ��������� ����� ������������ ������ ���������� �������

//Input encoder 
#define ENC_A_PIN 10
#define ENC_B_PIN 8
#define ENC_KEY_PIN 9

//// ���������� ��������� �����
//#define INDIKATOR_VALVE_OPEN 6
//#define INDIKATOR_VALVE_CLOSE 7

////Output Valve
//#define VALVE_OPEN_PIN 11
//#define VALVE_CLOSE_PIN 12



//��������� �������
struct srcSettings {
	int thresholdLight; //����������� ������������ ����������� �����
	int thresholdHumidity; //����������� ��������� ��� ������ ������ 
	int displayBrightness; //������� ������� �������
	unsigned long durationWatering; //������������ ������(��������� �����) � �������
};

	enum work_mode {
    NORMAL_WORK,   // 0
    SETTING,       // 1
    };

//���������� �������� ��������� �������
srcSettings parametrs; 

//������� �������� ������������ � ���������
int currentLight;
int currentHumidity;
bool flagWateringDone = false; //������������� �� ����������� ������ ����������� �������

work_mode CurrentWorkMode = NORMAL_WORK;  //������� ����� ������ �������
static int SettingsLevel=0;			//������� ����� � ���� ��������� 
static int NormalWorkDisplayInfo = 0; //��������� �� ������� ���������� � ������ NormalWork 

	// Initialize TM1637TinyDisplay - 4 Digit Display
	TM1637TinyDisplay displayTM1637(TM1637_CLK_PIN, TM1637_DIO_PIN);

	// �������
	// ���� ���������� �������� � VCC ��� - ����� ���������������� ...enc(INPUT_PULLUP)
	EncButton<EB_TICK, ENC_A_PIN, ENC_B_PIN, ENC_KEY_PIN> enc(INPUT_PULLUP);       //������� � ������� <A, B, KEY>

	


void setup()
{
	Serial.begin(9600);

	//�������� ��������� ��������� �� EEPROM
	EEPROM.get(10, parametrs);
	/*������� ����������*/
	(parametrs.displayBrightness > 7) ? 7 : parametrs.displayBrightness; //���������� �������� ��� ������� 0-7
	
	displayTM1637.setBrightness(parametrs.displayBrightness);
	displayTM1637.showNumber(8888); delay(600); displayTM1637.clear();

	//Ports setting
	pinMode(SENSOR_LIGHT_PIN, INPUT_PULLUP);
	pinMode(SENSOR_HUMIDITY_PIN, INPUT);
	pinMode(MIN_LIGHT_PIN, OUTPUT);
	MotoValve.init();

	enc.setHoldTimeout(3000); //time-out ��� ����������� ������� ������ ��������
	/*enc.setEncType(1);*/
}


void loop(){
	//Serial.println();

// ����� ��������, �������� ��� ����� ���� ��� � ����������
// ����� �������� �� ���� ��������, ���� ��������� ����� �� ������� � �������/���������
	enc.tick();
	
	//������������ ���������� �������� ������� �� ������� ������ �����
	MotoValve.ValveWork();
		
		//������ �������� ������������
		currentLight = getLight();
		if (currentLight < 200) { //������� ���� ��� ������������ � ��� ����� ������ (����� ������-��������� ����)
			flagWateringDone = false; 
			digitalWrite(MIN_LIGHT_PIN, LOW);
		} 

		//��������� �������� ��������� �����
		currentHumidity = getHumidity();
		
	//�������� ������� ��� ������
	if ((currentHumidity >= parametrs.thresholdHumidity) && (currentLight >= parametrs.thresholdLight) && !flagWateringDone) {
		//��������� ����
		MotoValve.open(parametrs.durationWatering);
		//������������� ���� ��� ������������ ������ �������
		flagWateringDone = true; 
		//������� ���������� �� ��������� ����� ���������� ������ �� ����� MIN_LIGHT_PIN 
		digitalWrite(MIN_LIGHT_PIN, HIGH);
}

	switch (CurrentWorkMode)
	{
	case NORMAL_WORK:
		//��������� ������� ����������
		displayTM1637.setBrightness(parametrs.displayBrightness);

		//���������� ��������� ���������� �� ������� � ���������� ������ ������
		switch (NormalWorkDisplayInfo) {
		case 0:
			//*�������� - 0. ���������� ��������� ����� 
			displayTM1637.showNumber(currentHumidity);

			//static bool flagleft=false;
			//if (enc.left()) {flagleft = true;}
			//if (enc.click() && flagleft) {
			//	NormalWorkDisplayInfo = 1; 
			//	flagleft = false;

			if (enc.left()) { NormalWorkDisplayInfo = 1;  break;}
			if (enc.right()) { NormalWorkDisplayInfo = 2;  break;}

			//}
			break;

		case 1:
			//*�������� - 1. ����������	������� ������������ 
			displayTM1637.showNumber(currentLight);
			//displayTM1637.showString((String("L") + String(parametrs.thresholdLight)).c_str());

			//������� ������ L ����� ������ �������� ������ ���������.
			uint8_t digitToSegment[1];
			digitToSegment[0] = displayTM1637.encodeASCII('L');
			displayTM1637.setSegments(digitToSegment, 1, 0);

			if (enc.left()) { NormalWorkDisplayInfo = 4;  break; } //���������� ���������� �� ���������� ������������ ������, ���� �� ������������
			if (enc.right()) { NormalWorkDisplayInfo = 0;  break;}

			break;

		case 2:
			//*�������� - 2. ����������	������� OPEN. ���������� ���� �������� � �������� �����
			displayTM1637.showString("OPEN");

			if (enc.left()) { NormalWorkDisplayInfo = 0; break;}
			if (enc.right()) { NormalWorkDisplayInfo = 3; break;}
			if (enc.click()) {
				//��������� ����
				Serial.println("Manual valve opening");
				MotoValve.open((unsigned long)10); //������ ������ 999, ���� ��������� ����������, �������� � ���� ��������???
			}
			break;

		case 3:
			//*�������� - 3. ����������	������� CLOUS. ���������� ���� �������� � �������� �����
			displayTM1637.showString("CLOS");

			if (enc.left()) { NormalWorkDisplayInfo = 2; break;}
			if (enc.click()) {
				//��������� ����
				Serial.println("Manual valve clousing");
				MotoValve.close();
			}
			break;
		case 4:
			//*�������� - 4. ����������	������������ ������ � ��������
			
			displayTM1637.showNumber((long)(MotoValve.getRemainingWateringTime() / 1000UL));

			if (enc.right()) { NormalWorkDisplayInfo = 1; break; }

			break;
		}

		// ���������� ������� - ������� � ����� ���������
		if (enc.held()) {CurrentWorkMode = SETTING; Serial.println(String("enc.held ") + String(CurrentWorkMode)); }
		
		break;
	case SETTING:
		///Serial.println(String("case 1 (SETTING) "));

		//������ ����������� � �������� 2�� (����������� ����� ������� ������ �� ���������, ����� �� ��������� �������)
		static unsigned long timeFlash; //����� ������ ����� ������� ���������� � ������ SETTING
		static bool flagFlash; //���� ������� ������� ����������
		if (millis() > (timeFlash + 500)) {
			timeFlash = millis();
			flagFlash= !(flagFlash);
			////if (flagFlash) {
			////	displayTM1637.setBrightness(BRIGHT_LOW);
			////	}
			////else {
			////		displayTM1637.setBrightness(BRIGHT_HIGH);
			////}
		}

		switch (SettingsLevel) {
		case 0:
			//*������� ���� - 0. ���������� �������� ��������� ��������� 
			if (flagFlash) {
				displayTM1637.setBrightness(BRIGHT_LOW);
				displayTM1637.showString("H");	
				//displayTM1637.showNumber(parametrs.thresholdHumidity);
			}
			else {
				displayTM1637.setBrightness(BRIGHT_HIGH);
				displayTM1637.showNumber(parametrs.thresholdHumidity);
			}

			if (enc.right()) { parametrs.thresholdHumidity += 1; }
			if (enc.rightH()) {parametrs.thresholdHumidity += 10;		}
			if (enc.left()) { parametrs.thresholdHumidity -= 1; }
			if (enc.leftH()) { parametrs.thresholdHumidity -= 10; }
			break;

		case 1:
				//*������� ���� - 1. ���������� �������� ��������� ������������
				if (flagFlash) {
					displayTM1637.setBrightness(BRIGHT_LOW);
					displayTM1637.showString("L");	
					//displayTM1637.showNumber(parametrs.thresholdLight);
				}
				else {
					displayTM1637.setBrightness(BRIGHT_HIGH);
					displayTM1637.showNumber(parametrs.thresholdLight);
				}

				if (enc.right()) { parametrs.thresholdLight += 1; }
				if (enc.rightH()) parametrs.thresholdLight += 10;
				if (enc.left()) { parametrs.thresholdLight -= 1; }
				if (enc.leftH()) { parametrs.thresholdLight -= 10; }

				break;

	case 2:
			//*������� ���� - 2. ���������� �������� ������������ ������
				displayTM1637.setBrightness(parametrs.displayBrightness);
				displayTM1637.showString("DP");
				displayTM1637.showNumber((long)parametrs.durationWatering);

				if (enc.right()) { ++parametrs.durationWatering>99 ? parametrs.durationWatering = 99 : parametrs.durationWatering; }
				if (enc.left()) { --parametrs.durationWatering < 0 ? parametrs.durationWatering = 0 : parametrs.durationWatering; }

			break;

		case 3:
			//*������� ���� - 3. ������� �������
			//////if (flagFlash) {
			//////	displayTM1637.setBrightness(BRIGHT_LOW);
			//////	}
			//////else {
			//////	displayTM1637.setBrightness(BRIGHT_HIGH);
			//////}
			//////displayTM1637.showString((String("DL ")+ String(parametrs.displayBrightness)).c_str());

			if (enc.right()) { ++parametrs.displayBrightness>7? parametrs.displayBrightness=7: parametrs.displayBrightness; }
			if (enc.left()) { --parametrs.displayBrightness < 0 ? parametrs.displayBrightness = 0 : parametrs.displayBrightness; }
			
			//����� �������� ����� ������� �������
			displayTM1637.setBrightness(parametrs.displayBrightness);
			displayTM1637.showString((String("DL ") + String(parametrs.displayBrightness)).c_str());

			break;

	

		}

		//������� � ���������� ������ ����
		if (enc.click()) { 
			//�� ����� ������� � ���������� ��������� ���� SETTINGS 
			SettingsLevel > 2 ? SettingsLevel = 0 : SettingsLevel += 1;
		}

		// ���������� ������� - ������� � �������� ����� ������
		if (enc.held()) {
			//��������� ������ � ������� �� ������ �������
			CurrentWorkMode = NORMAL_WORK; Serial.println(String("enc.held_inMenuSittings ") + String(CurrentWorkMode));
			EEPROM.put(10, parametrs); //��������� ��� ���������
			displayTM1637.clear();
		}
		break;
	}



	return;
}


