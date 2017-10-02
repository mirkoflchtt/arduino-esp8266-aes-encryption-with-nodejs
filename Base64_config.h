#ifndef _BASE64_CONFIG_H
#define _BASE64_CONFIG_H

#if defined(__ARDUINO_X86__) || (defined (__linux) || defined (linux))
  #undef PROGMEM
  #define PROGMEM __attribute__(( section(".progmem.data") ))
  #define pgm_read_byte(p) (*(p))
  typedef unsigned char byte;
  #define printf_P printf
  #define PSTR(x) (x)
#elif defined ( ESP8266 )
  #include <pgmspace.h>
  #define PSTR(x) (x)
#else
#if (defined(__AVR__))
  #include <avr\pgmspace.h>
#else
  #include <pgmspace.h>
#endif
#endif

#endif      // _BASE64_CONFIG_H
