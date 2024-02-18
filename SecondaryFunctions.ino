
//��������� �������� ������������
int getLight(){
	//��������� �� ���� ���� � 0,5�
	static unsigned long _time;  
	if (millis() > (_time + 500UL)) {
		_time = millis();
		currentLight = map(analogRead(SENSOR_LIGHT_PIN),0,1023,0,999); //������ �������� �� ���� ����, ����� �� ������ �� ���������, �.�. ���� ���� ������ L

	}
		return currentLight;
}

//��������� �������� ������������
int getHumidity() {
	//��������� �� ���� ���� � 0,5�
	static unsigned long _time;
	if (millis() > (_time + 500UL)) {
		_time = millis();
		currentHumidity = analogRead(SENSOR_HUMIDITY_PIN);
		//������� �������� 0-1023 � �������� 0-100
		//500/300'����������' �� ������ � ������������ � ����. ����� � ������� 0-3�
		//currentHumidity = map(currentHumidity, 500, 300, 0, 100);
		}
	return currentHumidity;
}
