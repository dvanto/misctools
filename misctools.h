
#ifndef _MISCTOOLS_H_
#define _MISCTOOLS_H_

// #include "Print.h"
#include <LiquidCrystal_PCF8574.h>
#include <Chrono.h> // ..\Chrono\Chrono.h

// https://arduino-esp8266.readthedocs.io/en/latest/PROGMEM.html
#include <avr/pgmspace.h>
#include <WString.h>
#define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
/* для работы со сторками из ПЗУ */
typedef __FlashStringHelper* fchar;
typedef const __FlashStringHelper* fchar_c;

//#define FF(a) 	(a)		// для отладки, переключение на строки в RAM
#define FF(a)       F(a)

class LCD_misc: public LiquidCrystal_PCF8574
{

	
public:
	LCD_misc(int i2cAddr, Chrono *timeout, int timout_sec);
	~LCD_misc();
	
	inline Chrono &timeout () { return *_timeout;}
	void on(bool timeout = false);
	
	void printL(char buf[], unsigned int line);
	void printP(const char* buf, unsigned int line);	
	
// using LiquidCrystal_PCF8574::print;	
private:
	Chrono*	_timeout;
	int		_timout_interval;
	bool	_need_destroy;
	
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

#endif

