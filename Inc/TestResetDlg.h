
// TestResetDlg.h : header file
//

#pragma once


#include <math.h>
#include "types.h"





//Константы
#define OLMATH_PI					3.14159265358979323846	// Pi
#define OLMATH_E					2.71828182845904523536	// e
#define OLMATH_LOG2E				1.44269504088896340736	// log2(e)
#define OLMATH_LOG10E				0.434294481903251827651	// log10(e)
#define OLMATH_LN2					0.693147180559945309417	// log(2)
#define OLMATH_LN10					2.30258509299404568402	// log(10)
#define OLMATH_PI_2					1.57079632679489661923	// Pi / 2
#define OLMATH_PI_4					0.785398163397448309616	// Pi / 4
#define OLMATH_1_PI					0.318309886183790671538	// 1 / Pi
#define OLMATH_2_PI					0.636619772367581343076	// 2 / Pi
#define OLMATH_1_SQRTPI				0.564189583547756286948	// 1 / sqrt(Pi)
#define OLMATH_SQRT2				1.41421356237309504880	// sqrt(2)
#define OLMATH_SQRT1_2				0.707106781186547524401	// sqrt(1/2)
#define OLMATH_1_SQRT2PI			0.398942280401432677940	// 1 / sqrt(2Pi)
#define OLMATH_SQRT2PI				2.506628274631000502415	// sqrt(2Pi)
#define OLMATH_1_255				0.003921568627450980392	// 1 / 255
#define OLMATH_DEGREE_TO_RADIAN		0.017453292519943295769	//Pi / 180
#define OLMATH_RADIAN_TO_DEGREE		57.29577951308232087679	//180 / Pi
#define OLMATH_1SECOND_IN_DAY		1.157407407407407407407E-5	//1 / 86400














//Сигнализатор сброса усреднения
class olAverageReset
{
#define dAR_N			8

public:
	olAverageReset();
public:
	~olAverageReset();

public:
	//Общее кол-во импульсов
	unsigned long count_all;
	//Общее время накопления
	unsigned long  time_all;

private:
	unsigned long array_count[dAR_N];
	unsigned long array_time[dAR_N];
	long array_n;

public:
	//Сброс
	void reset();
	//Сброс
	void reset(unsigned long _N, unsigned long _T);
	//Обновление
	//_count - кол-во импульсов за интервал времени 1 c
	//возвращает необходимость сброса усреднения
	BOOL update(unsigned long _count, BOOL* _bUp);

};







//Максимальное значение для линейного распределения
#define OLMATHRANDOM_MAX				2147483648

//Генератор случайных чисел
class olMathRandom
{
public:
	olMathRandom();
	~olMathRandom();

private:
	long long value_next;

public:
	//Инициализация значением
	void init(unsigned long _value);
	//Равномерное распределение
	unsigned long line();
	//Пуассон
	unsigned long poisson(double _mid);
	//Гаус
	double gauss(double _mid, double _sigma);

};



//Статистика
class olMathStatistic
{
public:
	olMathStatistic();

public:
	virtual ~olMathStatistic();

public:
	//Значение распределения Пуассона
	static double poisson(double _mid, unsigned long _num);
	//Квантиль распределения Пуассона
	static double quantile_poisson(double _probability, double _mid);
	//Квантиль распределения Пуассона
	static double quantile_poisson_value(double _probability, double _mid);
	//Квантиль нормального распределения
	static double quantile_gauss(double _probability);
	//Стандартное отклонение по данным
	static double standard_deviation(double* _data, unsigned long _index_from, unsigned long _index_to, double* _out_mid);
	//Расчёт Хи2
	//_array_model - эталонная зависимость
	//_array_source - экспериментальная зависимость
	//_pos_from - с какаой позиции считать
	//_pos_to - по какую позицию считать
	static double xi2(double* _array_model, double* _array_source, unsigned long _pos_from, unsigned long _pos_to);
	//Расчёт абсолютной ошибки
	//_value - значение
	//_error_basic - базовая ошибка (число, не процент)
	//_error_statistic - статистическая ошибка (число, не процент)
	static double error_absolut(double _value, double _error_basic, double _error_statistic);

};