

#include "MotoValve.h"
static bool flagOpenedValve = 0; // флаг состояния открытого крана

void MotoValveClass::init()
{
	pinMode(VALVE_OPEN_PIN, OUTPUT); digitalWrite(VALVE_OPEN_PIN, HIGH); //Активный LOW
	pinMode(VALVE_CLOSE_PIN, OUTPUT); digitalWrite(VALVE_CLOSE_PIN, HIGH); //Активный LOW
	pinMode(INDIKATOR_VALVE_OPEN, OUTPUT);
	pinMode(INDIKATOR_VALVE_CLOSE, OUTPUT);

	Serial.println("Valve reset (init)");
}

void MotoValveClass::open(unsigned long duration)
{
	//снимаем сигналы управления с крана
	digitalWrite(VALVE_OPEN_PIN, HIGH);
	digitalWrite(VALVE_CLOSE_PIN, HIGH);
	Serial.println("Valve reset (open1)");

	//фиксируем время поступления команды 
	_tmrOpen = millis();
	_openDuration = duration;
}

void MotoValveClass::close()
{
	//снимаем сигналы управления с крана
	digitalWrite(VALVE_OPEN_PIN, HIGH);
	digitalWrite(VALVE_CLOSE_PIN, HIGH);
	Serial.println("Valve reset (close1)");
	//фиксируем время поступления команды 
	_tmrClose = millis();
}

void MotoValveClass::ValveWork()
{

	//проверка поступления сигнала открытия
	if ((_tmrOpen > 0) && (millis() > (_tmrOpen + MOTOR_REVERS_PAUSE_TIME))) {
		// открываем кран
		if (!flagOpenedValve) {
			digitalWrite(VALVE_CLOSE_PIN, HIGH);
			digitalWrite(VALVE_OPEN_PIN, LOW); digitalWrite(INDIKATOR_VALVE_OPEN, HIGH); digitalWrite(INDIKATOR_VALVE_CLOSE, LOW);
			Serial.println("Valve OPEN (open)");
			flagOpenedValve = true;
			//фиксируем время активности сигнала управления
			_tmrOffSignalControlValve = millis();
		}

		if (millis() > (_tmrOpen + (_openDuration * (unsigned long)60000))) { /*значение _openDuration в минутах*/
																													 //закрываем кран, прекращаем полив
			digitalWrite(VALVE_OPEN_PIN, HIGH);
			digitalWrite(VALVE_CLOSE_PIN, LOW); digitalWrite(INDIKATOR_VALVE_OPEN, LOW); digitalWrite(INDIKATOR_VALVE_CLOSE, HIGH);
			Serial.println("Valve CLOSE (open)");
			_tmrOpen = 0;
			flagOpenedValve = false;
			//фиксируем время активности сигнала управления
			_tmrOffSignalControlValve = millis();
		}
	}

	//проверка поступления сигнала закрытия
	if ((_tmrClose > 0) && (millis() > (_tmrClose + MOTOR_REVERS_PAUSE_TIME))) {
		_tmrClose = 0;
		// закрываем кран
		digitalWrite(VALVE_OPEN_PIN, HIGH);
		digitalWrite(VALVE_CLOSE_PIN, LOW); digitalWrite(INDIKATOR_VALVE_OPEN, LOW); digitalWrite(INDIKATOR_VALVE_CLOSE, HIGH);
		Serial.println("Valve CLOSE (close)");
		//фиксируем фремя активности сигнала управления
		_tmrOffSignalControlValve = millis();
		flagOpenedValve = false;
		_tmrOpen = 0;
	}


	//Снятие сигнала управление с крана после выполнения переключения(просто задержка на заведомо достаточное время для операции переключения.)
	if ((_tmrOffSignalControlValve > 0) && (millis() > (_tmrOffSignalControlValve + MAX_TIME_CONTROL_SIGNAL_ON))) {
		//снимаем сигналы управления с крана
		digitalWrite(VALVE_OPEN_PIN, HIGH);
		digitalWrite(VALVE_CLOSE_PIN, HIGH);
		Serial.println("Valve reset (_tmrOffSignalControlValve)");
		_tmrOffSignalControlValve = 0;
	}
}

unsigned long MotoValveClass::getRemainingWateringTime()
{
	if (_tmrOpen > 0) {
		return	((_tmrOpen + (_openDuration * (unsigned long)60000)) - millis());
	}
	else {
		return _openDuration;

	}
}

MotoValveClass MotoValve;

