// myCycle.h

#ifndef _MYCYCLE_h
#define _MYCYCLE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class myCycle
{
 protected:

	private:
		bool          _go; //���� �����
		bool          _active;
		unsigned long _start;
		unsigned long _period;


 public:
	 myCycle(unsigned long per, bool act);
	 void reInit(unsigned long per, bool act);

	 //���������� �����
	 void reStart();
	 //�������� �� ��������� ������� ����� 
	 bool check();

	 //������ ��������
	 bool go();

	 //�����
	 void clear();

	 // active
	 bool active();
	 void setActive(bool act);
	 // period
	 unsigned long period();
	 void setPeriod(unsigned long per);
};

//extern MyCycleClass MyCycle;

#endif

