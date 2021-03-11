
#ifndef _MISCTOOLS_H_
#define _MISCTOOLS_H_

// #include "Print.h"
#include <LiquidCrystal_PCF8574.h>
#include <Chrono.h> // ..\Chrono\Chrono.h
#include <LightChrono.h> // ..\Chrono\Chrono.h

#include <Rtttl.h>

// #define CHRONO LightChrono
#define CHRONO Chrono

// https://arduino-esp8266.readthedocs.io/en/latest/PROGMEM.html
#include <avr/pgmspace.h>
#include <WString.h>

#define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
#define FSTR(name, value) const char PROGMEM name[] = (value)

/* для работы со сторками из ПЗУ */
typedef __FlashStringHelper* fchar;
typedef const __FlashStringHelper* fchar_c;

//#define FF(a) 	(a)		// для отладки, переключение на строки в RAM
#define FF(a)       F(a)

#define SETFLAG(x) ( ++(x)?(x):(++(x)) )  // если увеличение дало 0, то увеличиваем еще раз

typedef unsigned long	t_time;

// экран с выключением подсветки по таймеру и функциями отладочной печати
class LCD_misc: public LiquidCrystal_PCF8574
{
public:
	LCD_misc(int i2cAddr, int timout_sec, CHRONO *timeout = NULL);
	~LCD_misc();
	
	inline CHRONO &timeout () { return *_timeout;}
	void on(bool timeout = false);
	
	using LiquidCrystal_PCF8574::print;	
	void print(char buf[], unsigned int line);
	void printP(const char* buf, unsigned int line);	
	
private:
	CHRONO*	_timeout;
	CHRONO::chrono_t	_timout_interval;
	bool			_need_destroy;
	
};


/* 
usage:
  if (LCD.timeout.hasPassed(LCD_TIMEOUT))
  {
    LCD.timeout.restart();
	...
  }
 */

// печать времени в 4 символа
// буфер должен быть достаточного размера
// 000s -> 000m -> 0.0h -> 000h
char* sprintTime4(char* s, unsigned long v);

// печать времени в 5 символов
// буфер должен быть достаточного размера
// 0000s -> 00.0m -> 0.00h -> 00:00 -> 0000h
char* sprintTime5(char* s, unsigned long v);

inline char sw(bool f, char c = '\xFF') // полный квадратик на дисплее
{
  return f ? c : '_';
}

int freeRam ();

// буфера должно быть достаточно для печати %d.%d
char* printDecF(char *buf, int d, char decimal=10);
#define DEFAULT_CHARSET " \xDF-\x2B\x2A"

class Clockwise
{
public:
	Clockwise(const char * clockwise_charset = DEFAULT_CHARSET)
		: _clockwise_charset(clockwise_charset)
		, _cnt(-1) { _charset_len = strlen(clockwise_charset); };
		
	
	inline char get() { return _clockwise_charset[_cnt]; }
	char getNext();
	
private:
	const char*	_clockwise_charset;
	char		_charset_len;
	char 		_cnt;
};

// используйет FSTR для задания мелодий в PROGMEM
class Alarm: public Rtttl
{
	unsigned char	_alarms = 1;
	const char*		_song = NULL;
public:
	Alarm(int pin): Rtttl(pin) {}
	
	// используйет FSTR для задания мелодий в PROGMEM
	bool armed(unsigned char restart = 0, const char*	song = NULL)
	{
		return restart?(_song=song,_alarms=restart):_alarms;
	}
	
	void alarmOnce(const char* song = NULL)
	{
		if (_alarms)
			alarm(song);
	}
	
	void alarm(const char* song = NULL)
	{
		if (song != NULL || (song=_song) != NULL )
		{
			// _FLASH_STRING(s, song);
			play( _FLASH_STRING( song ) );
			while (updateMelody());
		}
		else
		{
			// если мелодия не задана, пишать как психованный
			TimerFreeTone(_buzzerPin, 1400, 2000);
		}
		
		if (_alarms) _alarms--;
	}
};

#endif

