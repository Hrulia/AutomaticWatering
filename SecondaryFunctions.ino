
const int NUM_READ = 4;  // ���������� ���������� ��� ������� �����. �������� (����� ��������: 2.4.8.16...)

//��������� �������� ������������
int getLight(int light){
	//��������� �� ���� ���� � 0,5�
	static unsigned long _time=0;  

	if (millis() > (_time + 500UL)) {
		_time = millis();
		light = map(analogRead(SENSOR_LIGHT_PIN),0,1023,0,999); //������ �������� �� ���� ����, ����� �� ������ �� ���������, �.�. ���� ���� ������ L
	}
		return light;
}

//��������� �������� ���������
int getHumidity(int humidity) {
	//��������� �� ���� ���� � 0,5�
	static unsigned long _time=0;
	if (millis() > (_time + 500UL)) {
		_time = millis();
		///Humidity = analogRead(SENSOR_HUMIDITY_PIN); //��������� ����������. !��� ���� ��� ������� ��������� �������, ��� ������� ����� �� �������
		//������
		humidity = (int)midArifm2(analogRead(SENSOR_HUMIDITY_PIN));

		//������� �������� 0-1023 � �������� 0-100
		//500/300'����������' �� ������ � ������������ � ����. ����� � ������� 0-3�
		//currentHumidity = map(currentHumidity, 500, 300, 0, 100);
		}
	return humidity;
}


//������� �� ������ ������: https://alexgyver.ru/lessons/filters/

/*const int NUM_READ = 10;  // ���������� ���������� ��� ������� �����. ��������
// ������� ������� �������������� ��� float
float midArifm() {
	float sum = 0;                      // ��������� ���������� sum
	for (int i = 0; i < NUM_READ; i++)  // �������� ���������� ����������
		sum += ��������;                  // ��������� �������� � ������ ������� � ���������� sum
	return (sum / NUM_READ);
}
// ������� ������� �������������� ��� int
int midArifm() {
	long sum = 0;                       // ��������� ���������� sum
	for (int i = 0; i < NUM_READ; i++)  // �������� ���������� ����������
		sum += ��������;                  // ��������� �������� � ������ ������� � ���������� sum
	return ((float)sum / NUM_READ);
}*/


//���������� �������
float midArifm2(float newVal) {
	static byte counter = 0;     // �������
	static float prevResult = 0; // ������ ���������� ������� ��������
	static float sum = 0;  // �����
	sum += newVal;   // ��������� ����� ��������
	counter++;       // �������++
	if (counter == NUM_READ) {      // �������� ���-�� ���������
		prevResult = sum / NUM_READ;  // ������� �������
		sum = 0;                      // �������� �����
		counter = 0;                  // ����� ��������
	}
	return prevResult;
}