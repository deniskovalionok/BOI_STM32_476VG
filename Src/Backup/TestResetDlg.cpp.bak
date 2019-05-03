
// TestResetDlg.cpp : implementation file
//

#include "TestResetDlg.h"


























/////////////////////////////////////////////////////////////////////////
//			olAverageReset
/////////////////////////////////////////////////////////////////////////
float array_threshold_r[50] = {
	0.0000f,
	0.0044f,
	0.0395f,
	0.127f,
	0.272f,
	0.469f,
	0.713f,
	0.996f,
	1.314f,
	1.664f,
	2.040f,
	2.441f,
	2.863f,
	3.305f,
	3.764f,
	4.240f,
	4.731f,
	5.236f,
	5.753f,
	6.282f,
	6.822f,
	7.372f,
	7.932f,
	8.501f,
	9.078f,
	9.663f,
	10.256f,
	10.856f,
	11.463f,
	12.076f,
	12.696f,
	13.321f,
	13.952f,
	14.589f,
	15.230f,
	15.877f,
	16.528f,
	17.184f,
	17.844f,
	18.508f,
	19.177f,
	19.849f,
	20.525f,
	21.205f,
	21.888f,
	22.575f,
	23.265f,
	23.958f,
	24.655f,
	25.354f,
};

float array_threshold_l_cf[50] = {
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f,
	0.1105f
};

olAverageReset::olAverageReset()
{
	reset();
}

olAverageReset::~olAverageReset()
{

}

void olAverageReset::reset()
{
	count_all = 0;
	time_all = 0;
	array_n = 0;
}

void olAverageReset::reset(unsigned long _N, unsigned long _T)
{
	count_all = _N;
	time_all = _T;
	array_n = 0;
}

BOOL olAverageReset::update(unsigned long _count, BOOL* _bUp)
{
	BOOL result = FALSE;//признак сброса

						//суммируем импульсы
	count_all += _count;
	//суммируем время
	time_all += 1;
	//считаем скорость счёта
	float cps = (time_all > 0) ? ((float)count_all / (float)time_all) : 0.0f;


	//обновляем массивы кол-ва импульсов и времён
	if (array_n > 0 && array_count[0] == 0.0)//если в ближайшем интервале нет импульсов, то добавляем в него же
	{
		array_count[0] += _count;
		array_time[0] += 1;
	}
	else//а если импульсы есть, то передвигаем массивы и обновляем ближайший интервал
	{
		long n = array_n;//кол-во активных интервалов
		if (n > (dAR_N - 1))
			n = dAR_N - 1;
		for (long i = n; i > 0; i--)//передвигаем значения
		{
			array_count[i] = array_count[i - 1];
			array_time[i] = array_time[i - 1];
		}
		//обновляем
		array_count[0] = _count;
		array_time[0] = 1;
		if (array_n < dAR_N)//если надо увеличиваем ко-во активных интервалов
			array_n++;
	}

	if (cps > 0.0f)
	{
		unsigned long T_int = 0;//время диапазона
		unsigned long N_int = 0;//кол-во импульсов диапазона
		float N_mid = 0.0;//кол-во импульсов диапазона, исходя из средней скорости счёта
		for (long i = 0; i < array_n; i++)//по всем интервалам
		{
			T_int += array_time[i];
			N_int += array_count[i];
			N_mid = cps * (float)T_int;

			unsigned long N_mid_l = 0;//левая граница
			unsigned long N_mid_r = 0;//правая граница

			if (N_mid >= 23.0f)
			{
				float sigma = 4.2656f;
				float sgm = sigma * (float)sqrt(N_mid);
				N_mid_r = (unsigned long)(N_mid + sgm);
				N_mid_l = 1 + (unsigned long)(N_mid - sgm);
			}
			else
			{
				unsigned long indJ = 49;
				for (unsigned long j = 2; j < 50; j++)
				{
					if (N_mid <= array_threshold_r[j])
					{
						indJ = j;
						break;
					}
				}
				N_mid_r = indJ;
				float cf = 0.089f;// array_threshold_l_cf[indJ];
				N_mid_l = (unsigned long)(N_mid * cf);
			}

			//проверка
			if (N_int < N_mid_l)
			{
				result = TRUE;
				*_bUp = FALSE;
			}
			else if (N_int > N_mid_r)
			{
				result = TRUE;
				*_bUp = TRUE;
			}

			if (result)//сброс
			{
				count_all = 0;
				time_all = 0;
				array_n = 0;
				break;
			}
		}
	}
	return result;//всё
}

/////////////////////////////////////////////////////////////////////////
//		olMathRandom
/////////////////////////////////////////////////////////////////////////
olMathRandom::olMathRandom()
{
	value_next = 1;
}

olMathRandom::~olMathRandom()
{

}

void olMathRandom::init(unsigned long _value)
{
	value_next = (long long)_value;
}

unsigned long olMathRandom::line()
{
	value_next = value_next * 1103515245 + 12345;
	return (unsigned long)(value_next) % OLMATHRANDOM_MAX;
}

unsigned long olMathRandom::poisson(double _mid)
{
	if (_mid <= 0.0)
		return 0;
	unsigned long result = 0;
	double rndMax_1 = 1.0 / (OLMATHRANDOM_MAX + 1.0);
	double S = log(rndMax_1 * (double)line());
	double A = -_mid;
	while (S >= A)
	{
		result++;
		S += log(rndMax_1 * (double)line());
	};
	return result;
}

double olMathRandom::gauss(double _mid, double _sigma)
{
	double val1 = 0.0;
	double val2 = 0.0;
	double s = 0;
	do {
		val1 = (2.0 * (double)line() / (OLMATHRANDOM_MAX + 1.0)) - 1.0;
		val2 = (2.0 * (double)line() / (OLMATHRANDOM_MAX + 1.0)) - 1.0;
		s = val1 * val1 + val2 * val2;
	} while (s <= 0.0 || s > 1.0);
	double result = val1 * sqrt(-2.0*log(s) / s);
	result = (_mid + _sigma * result);
	return result;
}













/////////////////////////////////////////////////////////////////////////
//		olMathStatistic
/////////////////////////////////////////////////////////////////////////
olMathStatistic::olMathStatistic()
{

}

olMathStatistic::~olMathStatistic()
{

}

double olMathStatistic::poisson(double _mid, unsigned long _num)
{
	double dNum = (double)_num;
	double v_sum = 0.0;
	for (unsigned long i = 1; i <= _num; i++)
		v_sum += log((double)i);
	double result = exp(dNum * log(_mid) - _mid - v_sum);
	return result;
}

double olMathStatistic::quantile_poisson(double _probability, double _mid)
{
	if (_mid <= 0.0 || _mid > 4.0E9)
		return 0.0;

	double sigma_n = 10.0;
	double mid_sigma_val = sigma_n * ((_mid > 2.25) ? sqrt(_mid) : 1.5);
	double left = _mid - mid_sigma_val;
	double right = _mid + mid_sigma_val;
	if (left < 0.0)		left = 0.0;
	unsigned long nleft = (unsigned long)left;
	unsigned long nright = 1 + (unsigned long)right;

	double c2 = log(_mid) + 1.0f;
	double threshold = (1.0 - _probability) * OLMATH_SQRT2PI;
	double sum = 0.0;
	for (unsigned long i = nright; i > nleft; i--)
	{
		double di = (double)i;
		double v_i = log(di);
		v_i = di * c2 - _mid - (di + 0.5f) * v_i;
		v_i = exp(v_i);
		sum += v_i;
		if (sum > threshold)
			return ((di - _mid) / sqrt(_mid));
	}
	return (((double)nleft - _mid) / sqrt(_mid));
}

double olMathStatistic::quantile_poisson_value(double _probability, double _mid)
{
	if (_mid <= 0.0 || _mid > 4.0E9)
		return 0.0;

	double sigma_n = 10.0;
	double mid_sigma_val = sigma_n * ((_mid > 2.25) ? sqrt(_mid) : 1.5);
	double left = _mid - mid_sigma_val;
	double right = _mid + mid_sigma_val;
	if (left < 0.0)		left = 0.0;
	unsigned long nleft = (unsigned long)left;
	unsigned long nright = 1 + (unsigned long)right;


	double c2 = log(_mid) + 1.0f;
	double threshold = (1.0 - _probability) * OLMATH_SQRT2PI;
	double sum = 0.0;
	for (unsigned long i = nright; i > nleft; i--)
	{
		double di = (double)i;
		double v_i = log(di);
		v_i = di * c2 - _mid - (di + 0.5f) * v_i;
		v_i = exp(v_i);
		sum += v_i;
		if (sum > threshold)
		{
			return i;
		}
	}
	return left;
}

double olMathStatistic::quantile_gauss(double _probability)
{
	if (_probability < 0.0)
		_probability = 0.0;
	else if (_probability > 1.0)
		_probability = 1.0;
	double a = 6.3830764864229228470E-4;//1.0/(sigma*sqrt(2.0*pi))   sigma = 625.0
	double b = 0.00000128;// 1.0/(2.0*sigma*sigma)	
	double lim = (_probability <= 0.5) ? (1.0 - 2.0 * _probability) : (1.0 - 2.0 * (1.0 - _probability));
	double result = a;
	for (long i = 1; i < 5000; i++)
	{
		double di = (double)i;
		double gauss_value_i = a * exp(-di * di * b);
		result += 2.0 * gauss_value_i;
		if (result >= lim)
		{
			result = di * 0.0016;
			if (_probability <= 0.5)
				result *= -1.0;
			return result;
		}
	}
	return (_probability <= 0.5) ? -8.0 : 8.0;
}

double olMathStatistic::standard_deviation(double* _data, unsigned long _index_from, unsigned long _index_to, double* _out_mid)
{
	double result = 0.0;
	double mid = 0.0;
	if (_index_to > _index_from)
	{
		double n_1 = 1.0 / (double)(_index_to - _index_from);
		for (unsigned long i = _index_from; i < _index_to; i++)
			mid += _data[i];
		mid *= n_1;
		for (unsigned long i = _index_from; i < _index_to; i++)
		{
			double v = _data[i] - mid;
			result += v * v;
		}
		result *= n_1;
	}
	if (_out_mid)
		*_out_mid = mid;
	return result;
}

double olMathStatistic::xi2(double* _array_model, double* _array_source, unsigned long _pos_from, unsigned long _pos_to)
{
	double Xi2 = 0.0f;
	for (unsigned long i = _pos_from; i < _pos_to; i++)
	{
		double v = _array_source[i] - _array_model[i];
		Xi2 += v*v;
	}
	return Xi2;
}

double olMathStatistic::error_absolut(double _value, double _error_basic, double _error_statistic)
{
	double result = 0.0;
	if (_error_basic < 0.0)
		_error_basic = 0.0;
	double F = (_error_statistic > 0.0) ? (2.0 * _error_basic / _error_statistic) : 1.0E10;
	if (F < 0.8)
		result = _value * _error_statistic;
	else if (F > 8.0)
		result = _value * _error_basic;
	else
	{
		double F2 = F * F;
		double F3 = F2 * F;
		double K = -0.0000884145 * (F3 * F2) + 0.0024877597 * (F3 * F) - 0.0265966748 * F3 + 0.1316000984 * F2 - 0.2743894078 * F + 0.9075633842;
		result = _value * K * (_error_basic + _error_statistic);
	}
	return result;
}