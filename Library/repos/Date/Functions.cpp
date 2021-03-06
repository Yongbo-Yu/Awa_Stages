#define _USE_MATH_DEFINES 
#include "Functions.hh"
#include "cFloatingLeg.hh"
#include "cFixedLeg.hh"

//using namespace std;


vector<cDate>
Schedule(const cDate& start,
		 const cPeriod& maturity, 
	     const cPeriod& freq,
		 const eConvention_NonBusinessDay& NonBusinessDayConvention)
{
	if (!start.IsValid())
	{
		cerr << "Starting date is not valid" << endl;
		exit(1);
	}
	if (!maturity.IsValid())
	{
		cerr << "Maturity is not valid" << endl;
		exit(1);
	}
	if (!freq.IsValid())
	{
		cerr << "Frequency is not valid" << endl;
		exit(1);
	}
	vector<cDate> schedule;
	cDate tempo = start + maturity;
	schedule.push_back(tempo);
	if (maturity.GetDay() == 0 && maturity.GetMonth() == 0 && maturity.GetYear() == 0)
	{
		return schedule;
	}
	while (start < tempo - freq)
	{
		tempo = tempo - freq;
		schedule.insert(schedule.begin(), tempo);
	}
	SetAsValidSchedule(schedule, NonBusinessDayConvention);
	return schedule; 
}

void
ShowSchedule(const vector<cDate>& schedule)
{
	for (size_t i = 0; i < schedule.size(); i++)
	{
		schedule[i].Show();
	}
}

cDate 
NumberOfDays_To_Date(int ndays)
{//Convert a number of days since 1/1/1900 to a date
	ndays -= 36526;
	if (ndays <= 0)
	{
		cerr << "Please enter a number of days larger than 36526" << endl;
		exit(1);
	}
	int year = 2000; 
	int month = 1; 
	int day = 1;
	while ((ndays > 366 && cDate::IsLeapYear(year)) || (ndays > 365 && !cDate::IsLeapYear(year)))
	{
		if (ndays > 366 && cDate::IsLeapYear(year))
		{
			ndays -= 366;
			year++;
		}
		else
		{
			ndays -= 365;
			year++;
		}
	}
	vector<int> ndaysPerMonthReverse{ 31,28,31,30,31,30,31,31,30,31,30,31 };
	if (cDate::IsLeapYear(year))
		ndaysPerMonthReverse[1] = 29;
	int i = 0;
	while (ndays > 0)
	{
		if (ndays >= ndaysPerMonthReverse[i])
		{
			ndays -= ndaysPerMonthReverse[i];
			month++;
		}
		else
		{
			day += ndays;
			ndays -= ndaysPerMonthReverse[i];
		}
		i++;
	}
	return cDate(day, month, year);
}

int 
Date_To_NumberOfDays(const cDate& date)
{//Convert a date to a number of days since	1/1/1900
	int day = date.GetDay();
	int month = date.GetMonth();
	int year = date.GetYear();
	int res = 0;
	for (int countYear = 1900; countYear < year; countYear++)
	{
		if (cDate::IsLeapYear(countYear)) 
			res += 366;
		else 
			res += 365;
	}
	for (int countMonth = 1; countMonth < month; countMonth++)
	{
		if (countMonth == 4 || countMonth == 6 || countMonth == 9 || countMonth == 11) {
			res += 30;
			continue;
		}
		if (countMonth == 2 && cDate::IsLeapYear(year)) {
			res += 29;
			continue;
		}
		if (countMonth == 2) {
			res += 28;
			continue;
		}
		else
			res += 31;
	}
	res += day;
	return res + 1;
}

void
SetAsValidSchedule(vector<cDate>& schedule, const eConvention_NonBusinessDay& NonBusinessDayConvention)
{//Change the schedule so there are only business days
	cPeriod oneDay(1, 0, 0, conv_ACT_ACT);
	cPeriod twoDay(2, 0, 0, conv_ACT_ACT);
	for (size_t dateIndex = 0; dateIndex < schedule.size(); dateIndex++)
	{
		cDate tempo = schedule[dateIndex];
		if (tempo.WhatDayIsIt() == 6)
		{
			if (NonBusinessDayConvention == GoForward)
			{
				schedule[dateIndex] = tempo + twoDay;
			}
			if (NonBusinessDayConvention == GoBackward)
			{
				schedule[dateIndex] = tempo - oneDay;
			}
			if (NonBusinessDayConvention == GoToTheClosest)
			{
				schedule[dateIndex] = tempo - oneDay;
			}
		}
		if (tempo.WhatDayIsIt() == 7)
		{
			if (NonBusinessDayConvention == GoForward)
			{
				schedule[dateIndex] = tempo + oneDay;
			}
			if (NonBusinessDayConvention == GoBackward)
			{
				schedule[dateIndex] = tempo - twoDay;
			}
			if (NonBusinessDayConvention == GoToTheClosest)
			{
				schedule[dateIndex] = tempo + oneDay;
			}
		}
	}
}

double
Interpolation(
	const double& x,
	const map<double, double> curve,
	const string convention)
{//Interpolation for the yield curve from the data
	double t1;
	double t2;
	double r1;
	double r2;
	map<double, double>::const_iterator it = curve.begin();
	if (x < it->first)
	{
		return it->second;
	}
	for (it; it != curve.end(); it++)
	{
		t2 = it->first; r2 = it->second;
		if (x < it->first)
		{
			it--;
			t1 = it->first; r1 = it->second;
			return (x - t2) / (t1 - t2) * r1 + (x - t1) / (t2 - t1) * r2;
		}
	}
	return r2;
}

double
ZC(
	const double& rate,
	const double& maturity,
	string convention,
	const double& t)
{//ZC Price with 2 convention T the maturity and t the date of valuation are exprimed in years
	/*if (convention == "composed")
	{
		return pow(1.0 / (1.0 + rate), maturity - t);
		//return 1;
	}
	if (convention == "expo")
	{
		return exp(-rate * (maturity - t));
	}
	else {
		return 0;
	}*/
	return rate;
}

void
ReplaceComa(string& s)
{//Conversion of 1,43 to 1.43
	int i = 0;
	while (s[i] != '\0')
	{
		if (s[i] == ',')
		{
			s[i] = '.';
		}
		i++;
	}
}

double
ConversionReuters(const string& maturity, const eConvention& convention)
{//Conversion of "1Y6M" to 1.5
	int i = 0;
	string s = "";
	int day = 0;
	int month = 0;
	int year = 0;
	while (maturity[i] != '\0') {
		while (int(maturity[i]) >= 48 && int(maturity[i]) <= 57)
		{
			s.push_back(maturity[i]);
			i++;
		}
		if (maturity[i] == 'Y')
		{
			year += stoi(s);
			s = "";
			i++;
		}
		if (maturity[i] == 'M')
		{
			month += stoi(s);
			s = "";
			i++;
		}
		if (maturity[i] == 'W')
		{
			day += 7 * stoi(s);
			s = "";
			i++;
		}
		if (maturity[i] == 'D')
		{
			day += stoi(s);
			s = "";
			i++;
		}
	}
	return cPeriod(day,month,year,convention).ConvertToDayFraction();
}

map<double, double>
YieldCurve(
	string name,
	const eConvention& convention,
	string user)
{//Get the market data from a csv - mettre en argument le fichier
	map<double, double> curve;
	std::ifstream data("C:/users/" + user + "/Documents/GitHub/Awa_Stages/Data/" + name + ".csv", std::ios::in);
	if (data)
	{
		string content;
		string maturity = "";
		string rate = "";
		for (int i = 0; i < 7; i++)
		{
			getline(data, content);
		}
		while (getline(data, content))
		{
			int i = 0;
			while (content[i] != ';')
			{
				maturity.push_back(content[i]);
				i++;
			}
			int nb = 0;
			while (nb < 4)
			{
				if (content[i] != ';')
				{
					i++;
				}
				else
				{
					nb++; i++;
				}
			}
			while (content[i] != '\0')
			{
				rate.push_back(content[i]);
				i++;
			}
			ReplaceComa(rate);
			curve[ConversionReuters(maturity, convention)] = stod(rate);
			maturity = "";
			rate = "";
		}
		data.close();
	}
	else
	{
		std::cout << "Can't open the file !" << std::endl;
	}
	return curve;
}

void
Affiche(const map<double, double> mymap)
{//show the content of a map
	for (map<double, double>::const_iterator it = mymap.begin(); it != mymap.end(); it++)
	{
		std::cout << it->first << " : " << it->second << endl;
	}
}