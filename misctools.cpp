#include <misctools.h>

#include <stdio.h>
#include <ArduinoLog.h>             //  ..\libraries\ArduinoLog\ArduinoLog.h	https://github.com/thijse/Arduino-Log/

int freeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

char* sprintTime4(char* s, unsigned long v)
{
  if (v < 900)
    sprintf(s, ("%03us"), (unsigned)  v);            	// 000s
  else if (v < 6000)
    sprintf(s, ("%03um"), (unsigned) (v / 60) );				// 000m
  /*   else if (v < 600)
      sprintf(s, ("%c:%03um"), (unsigned) (v/60)+'0', (unsigned) (v%60) );         // 0:00	*/
  else if (v < 36000 )
  {
    v /= 360;
    sprintf(s, ("%u.%ch"), (unsigned) v / 10, (char) (v % 10) + '0' );		// 0.0h
  }
  else
    sprintf(s, ("%03uh"), (unsigned) (v / 3600) );			// 000h

  return s;
}


char* sprintTime5(char* s, unsigned long v)
{
  /*
    // —кетч использует 13770 байт (96%) пам€ти устройства. ¬сего доступно 14336 байт.
    // √лобальные переменные используют 797 байт (77%) динамической пам€ти, оставл€€ 227 байт дл€ локальных переменных. ћаксимум: 1024 байт.
    if (v < 600)
    sprintf(s, ("%04us"),  (unsigned) v);       // 0000s
    else if ( (v/=6) < 900 )  // < 6000
    sprintf(s, ("%02u.%cm"),  (unsigned) v/10,  (char) (v%10) + '0'); //00.1m
    else if ( (v/=6) < 1000 )
  	sprintf(s, ("%u.%02dh"),  (unsigned) v/100, (unsigned) v%100 );// 0.00h
    else if ( (v/=10) < 1000 )
    sprintf(s, ("%02u.%ch"), 	(unsigned) v/10,  (char) (v%10) + '0' );// 00.0h
    else
    sprintf(s, ("%04uh"),  		(unsigned) v/10 );    // 0000h
  */

  // —кетч использует 13756 байт (95%) пам€ти устройства. ¬сего доступно 14336 байт.
  // √лобальные переменные используют 797 байт (77%) динамической пам€ти, оставл€€ 227 байт дл€ локальных переменных. ћаксимум: 1024 байт.
  if (v < 900)
    sprintf(s, ("%04us"),  (unsigned) v);       // 0000s
  /* else if ( v < 5400 )  //   900 сек
    {
  	v /= 6;
    sprintf(s, ("%02u.%cm"),  (unsigned) v/10,  (char) (v%10) + '0'); //00.1m
    }
    else if ( v < 36000 )		// 10 часов
    {
  	v /= 36;
  	sprintf(s, ("%u.%02dh"),  (unsigned) v/100, (unsigned) v%100 );// 0.00h
    } */
  else if ( v < 216000 ) // < 60*60*60  < 60 h
  {
    v /= 60;
    sprintf(s, ("%02u:%02u"), 	(unsigned) v / 60,  (unsigned) (v % 60) ); // 00:00
  }
  else if ( v < 360000 )   // < 100 часов
  {
    v /= 360;
    sprintf(s, ("%02u.%ch"), 	(unsigned) v / 10,  (char) (v % 10) + '0' ); // 00.0h
  }
  else
  {
    sprintf(s, ("%04uh"),  		(unsigned) v / 3600 );     // 0000h
  }


  return s;
}


const  char PROGMEM	STR_sd_CR[]		= ("%s (line:%d)" CR);
const  char PROGMEM	STR_SXd_CR[]	= ("%S %X (line:%d)" CR);


LCD_misc::LCD_misc(int i2cAddr, int timout_sec, CHRONO *timeout)
	: LiquidCrystal_PCF8574(i2cAddr)
	, _timeout(timeout)
	, _timout_interval(timout_sec*1000)
	, _need_destroy(false)
{
	if (_timeout == NULL)
	{
		// _timeout = new Chrono(Chrono::SECONDS);
		_timeout = new CHRONO(
		#if CHRONO == Chrono
		Chrono::SECONDS
		#endif
		);
		_need_destroy = true;
	}
}
	
LCD_misc::~LCD_misc()
{ 
	if (_need_destroy) 
	{
		delete _timeout;
	}
}

void LCD_misc::on(bool timeout)
{
  setBacklight(255);
  clear();

#if (LOG_LEVEL == LOG_LEVEL_VERBOSE)
  Log.trace(FF("lcd_on %d restart = %c" CR), __LINE__, timeout ? 'R' : '-' );
#endif

  if (timeout)
    _timeout->restart();
  else
    _timeout->stop();

}

void LCD_misc::print(char buf[], unsigned int line)
{
  print(buf);
  Log.notice( FPSTR(STR_sd_CR), buf, line);
}

void LCD_misc::printP(const char* buf, unsigned int line)
{
  print( FPSTR(buf) );
  Log.notice( FPSTR(STR_SXd_CR), FPSTR(buf), 0, line);
}

char Clockwise::getNext()
{
	if ( ++_cnt >= _charset_len ) _cnt=0;
	return _clockwise_charset[ _cnt ];
}

char* printDecF(char *buf, int d, char decimal)
{
	return sprintf(buf, "%d.%d", d/decimal, d%decimal), buf;
}


