
#ifndef _MISCTOOLS_H_
#define _MISCTOOLS_H_


#include <avr/pgmspace.h>	// https://arduino-esp8266.readthedocs.io/en/latest/PROGMEM.html
#include <WString.h>

#include <ArduinoLog.h>             //  ..\libraries\ArduinoLog\ArduinoLog.h	https://github.com/thijse/Arduino-Log/
// #include <DebounceEvent.h>          // https://github.com/xoseperez/debounceevent

#include <LiquidCrystal_PCF8574.h>

#include <Rtttl.h>


#include <Chrono.h> // ..\Chrono\Chrono.h
#include <LightChrono.h> // ..\Chrono\Chrono.h
// #define CHRONO LightChrono
#define CHRONO Chrono


#define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
#define FSTR(name, value) const char PROGMEM name[] = (value)

/* для работы со сторками из ПЗУ */
typedef __FlashStringHelper*		fchar;
typedef const __FlashStringHelper*	fchar_c;

#ifndef FF
//#define FF(a) 	(a)		// для отладки, переключение на строки в RAM
#define FF(a)       F(a)
#endif

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
	bool				_need_destroy;
	
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

// не забудь про ISR (PCINT?_vect) { }
// для 8..13 - PCINT0_vect, для A0..A5 - PCINT1_vect, для 0..7 - PCINT2_vect
void pciSetup(byte pin);


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
		if (restart)
		{
			Log.notice( FF("Alarm::armed a=%d -> %d s=%X -> %X" CR), _alarms, restart, song, _song);
			_alarms=restart;
			if (song) _song=song;
		}
		
		return _alarms;
	}
	
	void alarmOnce(const char* song = NULL)
	{
		if (_alarms)
			alarm(song);
	}
	
	void alarm(const char* song = NULL)
	{
		Log.notice( FF("Alarm::alarm s=%X _s=%X a=%d" CR), song, _song, _alarms);
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
		else{
			Log.notice( FF("Alarm::disarmed a=%d s=%X" CR), _alarms, _song);
		}
	}
};



  /*
    // разрешение прерываний INT0 и INT1
    //  EIMSK  =  (1<<INT0)  | (1<<INT1);

    // настройка срабатывания прерываний на любому изменению
    //EICRA  =  (0<<ISC11) | (1<<ISC10) | (0<<ISC01) | (1<<ISC00);

  	PORTD |= (1 << PORTD5) | (1 << PORTD6) | (1 << PORTD6);
    PORTB |= (1 << PORTB2);

    // разрешение прерываний с портов B (PCINT[7:0]) и D (PCINT[23:16]), и запрет с порта C (PCINT[14:8])
    PCICR  |= (1 << PCIE2) | (0 << PCIE1) | (1 << PCIE0);

    // маскирование всех ног, кроме PB0 и PD7 - по одной на PCINT0 и PCINT2
    PCMSK0 |= (0 << PCINT7)  | (0 << PCINT6)  | (0 << PCINT5)  | (0 << PCINT4)  | (0 << PCINT3)  | (1 << PCINT2)  | (0 << PCINT1)  | (0 << PCINT0);
    //PCMSK1 |=                (0 << PCINT14) | (0 << PCINT13) | (0 << PCINT12) | (0 << PCINT11) | (0 << PCINT10) | (0 << PCINT9)  | (0 << PCINT8);
    PCMSK2 |= (1 << PCINT23) | (1 << PCINT22) | (1 << PCINT21) | (0 << PCINT20) | (0 << PCINT19) | (0 << PCINT18) | (0 << PCINT17) | (0 << PCINT16);
  */

#endif

