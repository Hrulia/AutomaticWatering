

#include "MotoValve.h"
static bool flagOpenedValve = 0; // ���� ��������� ��������� �����

void MotoValveClass::init()
{
	pinMode(VALVE_OPEN_PIN, OUTPUT); digitalWrite(VALVE_OPEN_PIN, HIGH); //�������� LOW
	pinMode(VALVE_CLOSE_PIN, OUTPUT); digitalWrite(VALVE_CLOSE_PIN, HIGH); //�������� LOW
	pinMode(INDIKATOR_VALVE_OPEN, OUTPUT);
	pinMode(INDIKATOR_VALVE_CLOSE, OUTPUT);

	Serial.println("Valve reset (init)");
}

void MotoValveClass::open(unsigned long duration)
{
	//������� ������� ���������� � �����
	digitalWrite(VALVE_OPEN_PIN, HIGH);
	digitalWrite(VALVE_CLOSE_PIN, HIGH);
	Serial.println("Valve reset (open1)");

	//��������� ����� ����������� ������� 
	_tmrOpen = millis();
	_openDuration = duration;
}

void MotoValveClass::close()
{
	//������� ������� ���������� � �����
	digitalWrite(VALVE_OPEN_PIN, HIGH);
	digitalWrite(VALVE_CLOSE_PIN, HIGH);
	Serial.println("Valve reset (close1)");
	//��������� ����� ����������� ������� 
	_tmrClose = millis();
}

void MotoValveClass::ValveWork()
{

	//�������� ����������� ������� ��������
	if ((_tmrOpen > 0) && (millis() > (_tmrOpen + MOTOR_REVERS_PAUSE_TIME))) {
		// ��������� ����
		if (!flagOpenedValve) {
			digitalWrite(VALVE_CLOSE_PIN, HIGH);
			digitalWrite(VALVE_OPEN_PIN, LOW); digitalWrite(INDIKATOR_VALVE_OPEN, HIGH); digitalWrite(INDIKATOR_VALVE_CLOSE, LOW);
			Serial.println("Valve OPEN (open)");
			flagOpenedValve = true;
			//��������� ����� ���������� ������� ����������
			_tmrOffSignalControlValve = millis();
		}

		if (millis() > (_tmrOpen + (_openDuration * (unsigned long)60000))) { /*�������� _openDuration � �������*/
																													 //��������� ����, ���������� �����
			digitalWrite(VALVE_OPEN_PIN, HIGH);
			digitalWrite(VALVE_CLOSE_PIN, LOW); digitalWrite(INDIKATOR_VALVE_OPEN, LOW); digitalWrite(INDIKATOR_VALVE_CLOSE, HIGH);
			Serial.println("Valve CLOSE (open)");
			_tmrOpen = 0;
			flagOpenedValve = false;
			//��������� ����� ���������� ������� ����������
			_tmrOffSignalControlValve = millis();
		}
	}

	//�������� ����������� ������� ��������
	if ((_tmrClose > 0) && (millis() > (_tmrClose + MOTOR_REVERS_PAUSE_TIME))) {
		_tmrClose = 0;
		// ��������� ����
		digitalWrite(VALVE_OPEN_PIN, HIGH);
		digitalWrite(VALVE_CLOSE_PIN, LOW); digitalWrite(INDIKATOR_VALVE_OPEN, LOW); digitalWrite(INDIKATOR_VALVE_CLOSE, HIGH);
		Serial.println("Valve CLOSE (close)");
		//��������� ����� ���������� ������� ����������
		_tmrOffSignalControlValve = millis();
		flagOpenedValve = false;
		_tmrOpen = 0;
	}


	//������ ������� ���������� � ����� ����� ���������� ������������(������ �������� �� �������� ����������� ����� ��� �������� ������������.)
	if ((_tmrOffSignalControlValve > 0) && (millis() > (_tmrOffSignalControlValve + MAX_TIME_CONTROL_SIGNAL_ON))) {
		//������� ������� ���������� � �����
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

