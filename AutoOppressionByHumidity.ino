/*
    Name:       AutoOppressionByHumidity.ino
    Created:	01.05.2021 10:38:51
    Author:     AUTO33826TPLINK\Sergey
*/


#include <EEPROM.h>

#define EB_FAST 0         // таймаут быстрого поворота энкодера (переопределение в программе)
#include <EncButton.h>
#include "MotoValve.h"
#include <TM1637TinyDisplay.h>


// Module TM1637 connection pins
#define TM1637_CLK_PIN 3
#define TM1637_DIO_PIN 4

//Input capacitive sensor. GPIO пин с поддержкой АЦП
#define SENSOR_HUMIDITY_PIN A0  /*constexpr auto*/ 

//Input light sensor
#define SENSOR_LIGHT_PIN A1
#define MIN_LIGHT_PIN 2  //вывод дает 1 при установке флага выполненного полива сегоняшним вечером

//Input encoder 
#define ENC_A_PIN 10
#define ENC_B_PIN 8
#define ENC_KEY_PIN 9

//// Индикаторы состояния крана
//#define INDIKATOR_VALVE_OPEN 6
//#define INDIKATOR_VALVE_CLOSE 7

////Output Valve
//#define VALVE_OPEN_PIN 11
//#define VALVE_CLOSE_PIN 12



//параметры системы
struct srcSettings {
	int thresholdLight; //минимальная освещенность разрешающая полив
	int thresholdHumidity; //минимальная влажность для начала полива 
	int displayBrightness; //Текущая яркость дисплея
	unsigned long durationWatering; //длительность полива(открытого крана) в минутах
};

	enum work_mode {
    NORMAL_WORK,   // 0
    SETTING,       // 1
    };

//переменная содержит настройки системы
srcSettings parametrs; 

//Текущие значения освещенности и влажности
int currentLight;
int currentHumidity;
bool flagWateringDone = false; //сигнализирует об выполненном поливе сегодняшним вечером

work_mode CurrentWorkMode = NORMAL_WORK;  //текущий режим работы системы
static int SettingsLevel=0;			//Текущий пункт в меню настройки 
static int NormalWorkDisplayInfo = 0; //выводимая на дисплей информация в режиме NormalWork 

	// Initialize TM1637TinyDisplay - 4 Digit Display
	TM1637TinyDisplay displayTM1637(TM1637_CLK_PIN, TM1637_DIO_PIN);

	// энкодер
	// если аппаратной подтяжки к VCC нет - можно инициализировать ...enc(INPUT_PULLUP)
	EncButton<EB_TICK, ENC_A_PIN, ENC_B_PIN, ENC_KEY_PIN> enc(INPUT_PULLUP);       //энкодер с кнопкой <A, B, KEY>

	


void setup()
{
	Serial.begin(9600);

	//Загрузка начальных установок из EEPROM
	EEPROM.get(10, parametrs);
	/*яркость индикатора*/
	(parametrs.displayBrightness > 7) ? 7 : parametrs.displayBrightness; //допустимый диапазон для яркости 0-7
	
	displayTM1637.setBrightness(parametrs.displayBrightness);
	displayTM1637.showNumber(8888); delay(600); displayTM1637.clear();

	//Ports setting
	pinMode(SENSOR_LIGHT_PIN, INPUT_PULLUP);
	pinMode(SENSOR_HUMIDITY_PIN, INPUT);
	pinMode(MIN_LIGHT_PIN, OUTPUT);
	MotoValve.init();

	enc.setHoldTimeout(3000); //time-out для длительного нажатия кнопки энкодера
	/*enc.setEncType(1);*/
}


void loop(){
	//Serial.println();

// тикер энкодера, вызывать как можно чаще или в прерывании
// вернёт отличное от нуля значение, если произошло какое то событие с кнопкой/энкодером
	enc.tick();
	
	//обслуживание внутренних процедур конроля за режимом работы крана
	MotoValve.ValveWork();
		
		//Читаем значение освещенности
		currentLight = getLight();
		if (currentLight < 200) { //сбросим флаг уже проведенного в эти сутки полива (стало светло-наступило утро)
			flagWateringDone = false; 
			digitalWrite(MIN_LIGHT_PIN, LOW);
		} 

		//получение значения влажности почвы
		currentHumidity = getHumidity();
		
	//Проверка условий для полива
	if ((currentHumidity >= parametrs.thresholdHumidity) && (currentLight >= parametrs.thresholdLight) && !flagWateringDone) {
		//Открываем кран
		MotoValve.open(parametrs.durationWatering);
		//устанавливаем флаг уже выполненного полива сегодня
		flagWateringDone = true; 
		//выведем информацию об установке флага выполнения полива на вывод MIN_LIGHT_PIN 
		digitalWrite(MIN_LIGHT_PIN, HIGH);
}

	switch (CurrentWorkMode)
	{
	case NORMAL_WORK:
		//Установка яркости индикатора
		displayTM1637.setBrightness(parametrs.displayBrightness);

		//отображаем различную информацию на дисплее в нормальнои режиме работы
		switch (NormalWorkDisplayInfo) {
		case 0:
			//*Параметр - 0. Отображаем влажность почвы 
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
			//*Параметр - 1. Отображаем	уровень освещенности 
			displayTM1637.showNumber(currentLight);
			//displayTM1637.showString((String("L") + String(parametrs.thresholdLight)).c_str());

			//добавим символ L перед вывоом значения уровня освещения.
			uint8_t digitToSegment[1];
			digitToSegment[0] = displayTM1637.encodeASCII('L');
			displayTM1637.setSegments(digitToSegment, 1, 0);

			if (enc.left()) { NormalWorkDisplayInfo = 4;  break; } //отображаем информацию об уменьшении длительности полива, если он производится
			if (enc.right()) { NormalWorkDisplayInfo = 0;  break;}

			break;

		case 2:
			//*Параметр - 2. Отображаем	надпись OPEN. Дальнейший клик приводит к открытию крана
			displayTM1637.showString("OPEN");

			if (enc.left()) { NormalWorkDisplayInfo = 0; break;}
			if (enc.right()) { NormalWorkDisplayInfo = 3; break;}
			if (enc.click()) {
				//открываем кран
				Serial.println("Manual valve opening");
				MotoValve.open((unsigned long)10); //раньше стояло 999, было зависание процессора, возможно в этом проблема???
			}
			break;

		case 3:
			//*Параметр - 3. Отображаем	надпись CLOUS. Дальнейший клик приводит к открытию крана
			displayTM1637.showString("CLOS");

			if (enc.left()) { NormalWorkDisplayInfo = 2; break;}
			if (enc.click()) {
				//закрываем кран
				Serial.println("Manual valve clousing");
				MotoValve.close();
			}
			break;
		case 4:
			//*Параметр - 4. Отображаем	длительность полива в секундах
			
			displayTM1637.showNumber((long)(MotoValve.getRemainingWateringTime() / 1000UL));

			if (enc.right()) { NormalWorkDisplayInfo = 1; break; }

			break;
		}

		// длительное нажатие - переход в режим настройки
		if (enc.held()) {CurrentWorkMode = SETTING; Serial.println(String("enc.held ") + String(CurrentWorkMode)); }
		
		break;
	case SETTING:
		///Serial.println(String("case 1 (SETTING) "));

		//мигаем индикатором с частотой 2Гц (Обязательно нужно вывести данные на индикатор, иначе не обновится яркость)
		static unsigned long timeFlash; //время начала цикла мигания индикатора в режиме SETTING
		static bool flagFlash; //флаг мигания яркости индикатора
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
			//*уровень меню - 0. Отображаем значение пороговой влажности 
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
				//*уровень меню - 1. Отображаем значение пороговой освещенности
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
			//*уровень меню - 2. Отображаем значение длительности полива
				displayTM1637.setBrightness(parametrs.displayBrightness);
				displayTM1637.showString("DP");
				displayTM1637.showNumber((long)parametrs.durationWatering);

				if (enc.right()) { ++parametrs.durationWatering>99 ? parametrs.durationWatering = 99 : parametrs.durationWatering; }
				if (enc.left()) { --parametrs.durationWatering < 0 ? parametrs.durationWatering = 0 : parametrs.durationWatering; }

			break;

		case 3:
			//*уровень меню - 3. Яркость дисплея
			//////if (flagFlash) {
			//////	displayTM1637.setBrightness(BRIGHT_LOW);
			//////	}
			//////else {
			//////	displayTM1637.setBrightness(BRIGHT_HIGH);
			//////}
			//////displayTM1637.showString((String("DL ")+ String(parametrs.displayBrightness)).c_str());

			if (enc.right()) { ++parametrs.displayBrightness>7? parametrs.displayBrightness=7: parametrs.displayBrightness; }
			if (enc.left()) { --parametrs.displayBrightness < 0 ? parametrs.displayBrightness = 0 : parametrs.displayBrightness; }
			
			//сразу применим новый уровень яркости
			displayTM1637.setBrightness(parametrs.displayBrightness);
			displayTM1637.showString((String("DL ") + String(parametrs.displayBrightness)).c_str());

			break;

	

		}

		//переход к следующему пункту меню
		if (enc.click()) { 
			//По клику переход к следующему параметру меню SETTINGS 
			SettingsLevel > 2 ? SettingsLevel = 0 : SettingsLevel += 1;
		}

		// длительное нажатие - переход в основной режим работы
		if (enc.held()) {
			//Сохраняем данные и выходим из режима нстроек
			CurrentWorkMode = NORMAL_WORK; Serial.println(String("enc.held_inMenuSittings ") + String(CurrentWorkMode));
			EEPROM.put(10, parametrs); //сохраняем все настройки
			displayTM1637.clear();
		}
		break;
	}



	return;
}


