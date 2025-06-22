
const int NUM_READ = 4;  // количество усреднений для средних арифм. фильтров (лучше выбирать: 2.4.8.16...)

//Получение значения освещенности
int getLight(int light){
	//считываем не чаще раза в 0,5с
	static unsigned long _time=0;  

	if (millis() > (_time + 500UL)) {
		_time = millis();
		light = map(analogRead(SENSOR_LIGHT_PIN),0,1023,0,999); //сожмем диапазон до трех цифр, более не влазит на индикатор, т.к. есть спец символ L
	}
		return light;
}

//Получение значения влажности
int getHumidity(int humidity) {
	//считываем не чаще раза в 0,5с
	static unsigned long _time=0;
	if (millis() > (_time + 500UL)) {
		_time = millis();
		///Humidity = analogRead(SENSOR_HUMIDITY_PIN); //Единичное считывание. !При этом без фильтра показания плавают, при длинной линии до датчика
		//фильтр
		humidity = (int)midArifm2(analogRead(SENSOR_HUMIDITY_PIN));

		//Перевод значений 0-1023 в диапазон 0-100
		//500/300'калибровка' по сухому и погруженному в воду. Выход у датчика 0-3В
		//currentHumidity = map(currentHumidity, 500, 300, 0, 100);
		}
	return humidity;
}


//Фильтры от Алекса Гувера: https://alexgyver.ru/lessons/filters/

/*const int NUM_READ = 10;  // количество усреднений для средних арифм. фильтров
// обычное среднее арифметическое для float
float midArifm() {
	float sum = 0;                      // локальная переменная sum
	for (int i = 0; i < NUM_READ; i++)  // согласно количеству усреднений
		sum += значение;                  // суммируем значения с любого датчика в переменную sum
	return (sum / NUM_READ);
}
// обычное среднее арифметическое для int
int midArifm() {
	long sum = 0;                       // локальная переменная sum
	for (int i = 0; i < NUM_READ; i++)  // согласно количеству усреднений
		sum += значение;                  // суммируем значения с любого датчика в переменную sum
	return ((float)sum / NUM_READ);
}*/


//Растянутая выборка
float midArifm2(float newVal) {
	static byte counter = 0;     // счётчик
	static float prevResult = 0; // хранит предыдущее готовое значение
	static float sum = 0;  // сумма
	sum += newVal;   // суммируем новое значение
	counter++;       // счётчик++
	if (counter == NUM_READ) {      // достигли кол-ва измерений
		prevResult = sum / NUM_READ;  // считаем среднее
		sum = 0;                      // обнуляем сумму
		counter = 0;                  // сброс счётчика
	}
	return prevResult;
}