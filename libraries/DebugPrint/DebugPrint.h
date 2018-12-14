
#ifndef DebugPrint_h
#define DebugPrint_h
#define LIBRARY_VERSION	1.0.0

//Debug output macros

//If its not even defined, that makes it a false
#ifndef SERIAL_DEBUG_ENABLED
  #define SERIAL_DEBUG_ENABLED 0
#endif

#if SERIAL_DEBUG_ENABLED
  #define DebugPrint(str)  \
        Serial.print(str);       
  #define DebugPrintln(str)  \
        Serial.println(str);        
  #define DebugPrint2(str,modifier)  \
        Serial.print(str,modifier);
  #define DebugPrintln2(str,modifier)  \
        Serial.println(str,modifier);    
  #define DebugPrintHeader(str)  \
        Serial.print(millis());     \
        Serial.print(": ");    \
        Serial.print(__PRETTY_FUNCTION__); \
        Serial.print(' ');      \
        Serial.print(__LINE__);     \
        Serial.print(' ');      \
        Serial.println(str);         
#else
  #define DebugPrint(str)
  #define DebugPrintln(str)
  #define DebugPrint2(str,modifier)
  #define DebugPrintln2(str,modifier)
#endif

//Everyone needs to know pi
#ifndef M_PI
#define M_PI    3.14159265358979323846	/* pi */
#endif
#ifndef M_PI_2
#define M_PI_2  1.57079632679489661923	/* pi/2 */
#endif

//PWM goes from 0 to 255
#ifndef PWM_MAX
#define PWM_MAX      255
#endif

//ADC goes from 0 to 1023
#ifndef ADC_MAX
#define ADC_MAX      1023
#endif

#endif
