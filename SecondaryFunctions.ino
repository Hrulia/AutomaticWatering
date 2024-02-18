
//Получение значения освещенности
int getLight(){
	//считываем не чаще раза в 0,5с
	static unsigned long _time;  
	if (millis() > (_time + 500UL)) {
		_time = millis();
		currentLight = map(analogRead(SENSOR_LIGHT_PIN),0,1023,0,999); //сожмем диапазон до трех цифр, более не влазит на индикатор, т.к. есть спец символ L

	}
		return currentLight;
}

//Получение значения освещенности
int getHumidity() {
	//считываем не чаще раза в 0,5с
	static unsigned long _time;
	if (millis() > (_time + 500UL)) {
		_time = millis();
		currentHumidity = analogRead(SENSOR_HUMIDITY_PIN);
		//Перевод значений 0-1023 в диапазон 0-100
		//500/300'калибровка' по сухому и погруженному в воду. Выход у датчика 0-3В
		//currentHumidity = map(currentHumidity, 500, 300, 0, 100);
		}
	return currentHumidity;
}
