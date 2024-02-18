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

// Индикаторы состояния крана
#define INDIKATOR_VALVE_OPEN 6
#define INDIKATOR_VALVE_CLOSE 7

#define MOTOR_REVERS_PAUSE_TIME 1000 		//время паузы при смене направления вращения крана 
#define MAX_TIME_CONTROL_SIGNAL_ON 60000	//максимальное время поступления сигнала управления на мотор. Предполагается, что за это время кран отработает открытие, или закрытие полностью со 100% вероятностью


class MotoValveClass
{
protected:
	unsigned long _tmrOpen = 0; //время поступления команды открытия крана
	unsigned long _tmrClose = 0;//время поступления команды закрытия крана
	unsigned long _openDuration = 5; //длительность нахождения крана в открытом состоянии
	unsigned long _tmrOffSignalControlValve = 0; //таймер максимального времени действия сигнала управления на кране

public:
	void init();
	void open(unsigned long duration);
	void close();
	void ValveWork();
	unsigned long getRemainingWateringTime(); //возвращает оставшееся время полива

};

extern MotoValveClass MotoValve;

#endif

