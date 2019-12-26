// ---------------------------------------------------------------------------
// X7Segment Library - v1.0 - 26/012/2019
//
// AUTHOR/LICENSE:
// Created by Seon Rozenblum - seon@unexpectedmaker.com
// Copyright 2019 License: MIT https://github.com/UnexpectedMaker/X7Segment/LICENSE
//
// LINKS:
// Project home: https://github.com/UnexpectedMaker/X7Segment
// Blog: http://unexpectedmaker.com
//
// DISCLAIMER:
// This software is furnished "as is", without technical support, and with no 
// warranty, express or implied, as to its usefulness for any purpose.
//
// PURPOSE:
// Seven Segment Library for the X7Segment display boards, or for use with strips of NeoPixels arranged as 7 Segment displays
//
// SYNTAX:
//   X7Segment( digits, pin ) - Initialise the array of displays
//     Parameters:
//		* digits		- The number of digits you will be displaying on
//		* pin			- Pin the data is connected to on your microcontroller
//
// HISTORY:
//
// 26/012/2019v1.0 - Initial release.
//
// ---------------------------------------------------------------------------

#ifndef X7Segment_h
	#define X7Segment_h

    #if defined(ESP32)

        #include "freertos/FreeRTOS.h"
        #include "freertos/task.h"
        #include "freertos/event_groups.h"
        #include "esp32-hal.h"

    #else

        #include <Adafruit_NeoPixel.h>

        #ifdef __AVR__
        #include <avr/power.h>
        #endif

    #endif

    #if defined(ARDUINO) && ARDUINO >= 100
        #include <Arduino.h>
    #else
        #include <WProgram.h>
        #include <pins_arduino.h>
    #endif
	

    static const uint8_t PROGMEM _NeoPixelGammaTable[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,
    3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,
    7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12,
   13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20,
   20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29,
   30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42,
   42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
   58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 75,
   76, 77, 78, 80, 81, 82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96,
   97, 99,100,102,103,105,106,108,109,111,112,114,115,117,119,120,
  122,124,125,127,129,130,132,134,136,137,139,141,143,145,146,148,
  150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,
  182,184,186,188,191,193,195,197,199,202,204,206,209,211,213,215,
  218,220,223,225,227,230,232,235,237,240,242,245,247,250,252,255};

class X7Segment
{
	public:
		X7Segment( uint8_t displayCount, uint8_t dPin );
		~X7Segment();
		
		void Begin( uint8_t brightness );
		void SetBrightness( uint8_t brightness );
		  
		void DisplayTextVerticalRainbow( String text, uint32_t colorA, uint32_t colorB );
		void DisplayTextHorizontalRainbow( String text, uint32_t colorA, uint32_t colorB );
		void DisplayTextColor( String text, uint32_t color );
		void DisplayTextColorCycle( String text, uint8_t index );
		void DisplayTime( uint8_t hours, uint8_t mins, uint8_t secs, uint32_t colorH, uint32_t colorM );
		
	    static uint32_t Color(uint8_t r, uint8_t g, uint8_t b);
        static uint32_t ColorHSV(uint16_t hue, uint8_t sat=255, uint8_t val=255);
        static uint32_t gamma32(uint32_t x);

        static uint8_t  gamma8(uint8_t x) {
            return pgm_read_byte(&_NeoPixelGammaTable[x]); // 0-255 in, 0-255 out
        }
		
		uint32_t Wheel( byte WheelPos );

		void ForceUppercase( bool force );
		
		bool IsReady( void );

		byte GetArraySize();
		String GetCharacterAtArrayIndex( int index );
		
	protected:

		
	private:

		uint8_t dispCount;
		uint8_t dispPin;
        uint16_t ledCount;
        uint8_t brightness;

        #if defined(ESP32)
            rmt_data_t* led_data;
            rmt_obj_t* rmt_send = NULL;
        #else
            Adafruit_NeoPixel pixels;
        #endif
		
		void SetupCharacters();
		int FindIndexOfChar(String character);
		byte FindByteForCharater( String character );
		void CheckToCacheBytes( String s );
        void ClearBuffer();
        void FillBuffer();
		String PadTimeData( int8_t data );
		uint8_t Red( uint32_t col );
		uint8_t Green( uint32_t col );
		uint8_t Blue( uint32_t col );
        uint8_t AdjustForBrightness( uint8_t col );
		byte *cachedBytes;
        uint32_t *leds;
		String cachedString;
		bool isReady;
		bool isForcedUpper;


};
#endif