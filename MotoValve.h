// MotoValve.h

#ifndef _MOTOVALVE_h
#define _MOTOVALVE_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

//Output Valve
#define VALVE_OPEN_PIN 11
#define VALVE_CLOSE_PIN 12

// ���������� ��������� �����
#define INDIKATOR_VALVE_OPEN 6
#define INDIKATOR_VALVE_CLOSE 7

#define MOTOR_REVERS_PAUSE_TIME 1000 		//����� ����� ��� ����� ����������� �������� ����� 
#define MAX_TIME_CONTROL_SIGNAL_ON 60000	//������������ ����� ����������� ������� ���������� �� �����. ��������������, ��� �� ��� ����� ���� ���������� ��������, ��� �������� ��������� �� 100% ������������


class MotoValveClass
{
protected:
	unsigned long _tmrOpen = 0; //����� ����������� ������� �������� �����
	unsigned long _tmrClose = 0;//����� ����������� ������� �������� �����
	unsigned long _openDuration = 5; //������������ ���������� ����� � �������� ���������
	unsigned long _tmrOffSignalControlValve = 0; //������ ������������� ������� �������� ������� ���������� �� �����

public:
	void init();
	void open(unsigned long duration);
	void close();
	void ValveWork();
	unsigned long getRemainingWateringTime(); //���������� ���������� ����� ������

};

extern MotoValveClass MotoValve;

#endif

