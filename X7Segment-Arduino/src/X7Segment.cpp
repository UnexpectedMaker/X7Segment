// ---------------------------------------------------------------------------
// Created by Seon Rozenblum - seon@unexpectedmaker.com
// Copyright 2019 License: MIT https://github.com/UnexpectedMaker/X7Segment/LICENSE
//
// See "X7Segment.h" for purpose, syntax, version history, links, and more.
// ---------------------------------------------------------------------------

#include "X7Segment.h"

#if defined(ESP32)
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/event_groups.h"
    #include "esp32-hal.h"
#else
    // For non ESP32 boards, this library uses the Adafruit NeoPixel library - please check their licensing terms in their library.
    #include <Adafruit_NeoPixel.h>
#endif

#define DEBUG
#define NUM_PIXELS_PER_BOARD 15
#define NUM_PIXELS_PER_SEGMENT 2

// Array of pixels per segment, 7 segments with 4 pixels each
byte segmentsPixels[8][4] 	{ { 0, 1 }, 
                              { 2, 3 }, 
                              { 4, 5 }, 
                              { 6, 7 }, 
                              { 8, 9 }, 
                              {10, 11}, 
                              {12, 13},
							  { 14 }
						  	};

// Array of pixel positions in X,Y format for mapping colours in X,Y space
byte pixelsXY[15][2]		{ 	{ 1,0 }, { 2,0 },
								{ 3,1 }, { 3,2 },
	                          	{ 3,4 }, { 3,5 },
							   	{ 1,6 }, { 2,6 },
	                          	{ 0,5 }, { 0,4 },
							   	{ 0,2 }, { 0,1 },
	                          	{ 1,3 }, { 2,3 },
							   	{ 4,7 }
						  	};

// Available characters a 7 Segment display can show					  
const byte ARRAY_SIZE = 32;

byte available_codes[ ARRAY_SIZE ][ 2 ] {
							{ '0', 0b00111111  }, 
							{ '1', 0b00000110 },
							{ '2', 0b01011011 },
							{ '3', 0b01001111 },
							{ '4', 0b01100110 },
							{ '5', 0b01101101 },
							{ '6', 0b01111100 },
							{ '7', 0b00000111 },
							{ '8', 0b01111111 },
							{ '9', 0b01100111 },
							{ 'a', 0b01110111 },
							{ 'b', 0b01111100 },
							{ 'c', 0b00111001 },
							{ 'd', 0b01011110 },
							{ 'e', 0b01111001 },
							{ 'f', 0b01110001 },
							{ 'g', 0b01100111 },
							{ 'h', 0b01110110 },
							{ 'i', 0b00110000 },
							{ 'j', 0b00011110 },
							{ 'l', 0b00111000 },
							{ 'n', 0b01010100 },
							{ 'o', 0b01011100 },
							{ 'p', 0b01110011 },
							{ 'q', 0b01100111 },
							{ 'r', 0b01010000 },
							{ 's', 0b01101101 },
							{ 'u', 0b00111110 },
							{ 'x', 0b01110110 },
							{ 'y', 0b01101110 },
							{ '-', 0b01000000 },
							{ ' ', 0b00000000 }
						};

byte available_codes_upper[ ARRAY_SIZE ][ 2 ] {
							{ '0', 0b00111111 }, 
							{ '1', 0b00000110 },
							{ '2', 0b01011011 },
							{ '3', 0b01001111 },
							{ '4', 0b01100110 },
							{ '5', 0b01101101 },
							{ '6', 0b01111100 },
							{ '7', 0b00000111 },
							{ '8', 0b01111111 },
							{ '9', 0b01100111 },
							{ 'a', 0b01110111 },
							{ 'b', 0b01111111 },
							{ 'c', 0b00111001 },
							{ 'd', 0b00111111 },
							{ 'e', 0b01111001 },
							{ 'f', 0b01110001 },
							{ 'g', 0b01100111 },
							{ 'h', 0b01110110 },
							{ 'i', 0b00110000 },
							{ 'j', 0b00011110 },
							{ 'l', 0b00111000 },
							{ 'n', 0b00110111 },
							{ 'o', 0b00111111 },
							{ 'p', 0b01110011 },
							{ 'q', 0b01100111 },
							{ 'r', 0b00110001 },
							{ 's', 0b01101101 },
							{ 'u', 0b00111110 },
							{ 'x', 0b01110110 },
							{ 'y', 0b01101110 },
							{ '-', 0b01000000 },
							{ ' ', 0b00000000 }
						};					


													
X7Segment::X7Segment( uint8_t displayCount, uint8_t dPin )
{
	dispCount = displayCount;
	dispPin = dPin;

    ledCount = dispCount * NUM_PIXELS_PER_BOARD;
    leds = (uint32_t *) malloc( ledCount * sizeof(uint32_t));

    brightness = 90;

#if defined(ESP32)



#else

	pixels = Adafruit_NeoPixel ();
	pixels.updateType( NEO_GRB + NEO_KHZ800 );
  	pixels.updateLength( ledCount );
    pixels.setPin(dispPin);

#endif

    ClearBuffer();
	isReady = false;
}

X7Segment::~X7Segment()
{
	isReady = false;
}

bool X7Segment::IsReady()
{
	return isReady;
}

void X7Segment::Begin( uint8_t bright )
{
#if defined(ESP32)

    #ifdef DEBUG
        Serial.println("\nESP32 baby!!!");
    #endif

    led_data = (rmt_data_t *) malloc( ( 24 * ledCount ) * sizeof(rmt_data_t) );

     if ((rmt_send = rmtInit(dispPin, true, RMT_MEM_64)) == NULL)
    {
        #ifdef DEBUG
            Serial.println("init sender failed\n");
        #endif
    }

    float realTick = rmtSetTick(rmt_send, 100);
    #ifdef DEBUG
        Serial.printf("real tick set to: %fns\n", realTick);
    #endif

    brightness = bright;
#else
	pixels.begin(); // This initializes the NeoPixel library.
	pixels.show();
	pixels.setBrightness( bright );
#endif

	cachedString = "";
	cachedBytes = (byte *) malloc(dispCount * sizeof(byte));
	
	for ( int i = 0; i < dispCount; i++ )
		cachedBytes[i] = 0;

	#ifdef DEBUG
		Serial.print("Brightness: ");
		Serial.println(brightness);
		Serial.println("Ready!");
	#endif

	//Digits are initialised and ready
	isReady = true;
	isForcedUpper = false;
}

void X7Segment::SetBrightness( uint8_t bright )
{
	if ( isReady )
	{
        #if defined(ESP32)
            brightness = bright;
        #else
		    pixels.setBrightness( bright );
        #endif
	}
}

void X7Segment::ForceUppercase( bool force )
{
	isForcedUpper = force;
}

byte X7Segment::GetArraySize()
{
	return ARRAY_SIZE;
}

String X7Segment::GetCharacterAtArrayIndex( int index )
{
	return ( (String)(char)available_codes[ index ][0] );
}

void X7Segment::ClearBuffer()
{
	for ( int i = 0; i < ledCount; i++ )
        leds[i] = 0;
}

// Convert separate R,G,B into packed 32-bit RGB color.
// Packed format is always RGB, regardless of LED strand color order.
uint32_t X7Segment::Color(uint8_t r, uint8_t g, uint8_t b)
{
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

uint32_t X7Segment::gamma32(uint32_t x) {
  uint8_t *y = (uint8_t *)&x;
  // All four bytes of a 32-bit value are filtered even if RGB (not WRGB),
  // to avoid a bunch of shifting and masking that would be necessary for
  // properly handling different endianisms (and each byte is a fairly
  // trivial operation, so it might not even be wasting cycles vs a check
  // and branch for the RGB case). In theory this might cause trouble *if*
  // someone's storing information in the unused most significant byte
  // of an RGB value, but this seems exceedingly rare and if it's
  // encountered in reality they can mask values going in or coming out.
  for(uint8_t i=0; i<4; i++) y[i] = gamma8(y[i]);
  return x; // Packed 32-bit return
}

uint32_t X7Segment::ColorHSV(uint16_t hue, uint8_t sat, uint8_t val) {

  uint8_t r, g, b;

  // Remap 0-65535 to 0-1529. Pure red is CENTERED on the 64K rollover;
  // 0 is not the start of pure red, but the midpoint...a few values above
  // zero and a few below 65536 all yield pure red (similarly, 32768 is the
  // midpoint, not start, of pure cyan). The 8-bit RGB hexcone (256 values
  // each for red, green, blue) really only allows for 1530 distinct hues
  // (not 1536, more on that below), but the full unsigned 16-bit type was
  // chosen for hue so that one's code can easily handle a contiguous color
  // wheel by allowing hue to roll over in either direction.
  hue = (hue * 1530L + 32768) / 65536;
  // Because red is centered on the rollover point (the +32768 above,
  // essentially a fixed-point +0.5), the above actually yields 0 to 1530,
  // where 0 and 1530 would yield the same thing. Rather than apply a
  // costly modulo operator, 1530 is handled as a special case below.

  // So you'd think that the color "hexcone" (the thing that ramps from
  // pure red, to pure yellow, to pure green and so forth back to red,
  // yielding six slices), and with each color component having 256
  // possible values (0-255), might have 1536 possible items (6*256),
  // but in reality there's 1530. This is because the last element in
  // each 256-element slice is equal to the first element of the next
  // slice, and keeping those in there this would create small
  // discontinuities in the color wheel. So the last element of each
  // slice is dropped...we regard only elements 0-254, with item 255
  // being picked up as element 0 of the next slice. Like this:
  // Red to not-quite-pure-yellow is:        255,   0, 0 to 255, 254,   0
  // Pure yellow to not-quite-pure-green is: 255, 255, 0 to   1, 255,   0
  // Pure green to not-quite-pure-cyan is:     0, 255, 0 to   0, 255, 254
  // and so forth. Hence, 1530 distinct hues (0 to 1529), and hence why
  // the constants below are not the multiples of 256 you might expect.

  // Convert hue to R,G,B (nested ifs faster than divide+mod+switch):
  if(hue < 510) {         // Red to Green-1
    b = 0;
    if(hue < 255) {       //   Red to Yellow-1
      r = 255;
      g = hue;            //     g = 0 to 254
    } else {              //   Yellow to Green-1
      r = 510 - hue;      //     r = 255 to 1
      g = 255;
    }
  } else if(hue < 1020) { // Green to Blue-1
    r = 0;
    if(hue <  765) {      //   Green to Cyan-1
      g = 255;
      b = hue - 510;      //     b = 0 to 254
    } else {              //   Cyan to Blue-1
      g = 1020 - hue;     //     g = 255 to 1
      b = 255;
    }
  } else if(hue < 1530) { // Blue to Red-1
    g = 0;
    if(hue < 1275) {      //   Blue to Magenta-1
      r = hue - 1020;     //     r = 0 to 254
      b = 255;
    } else {              //   Magenta to Red-1
      r = 255;
      b = 1530 - hue;     //     b = 255 to 1
    }
  } else {                // Last 0.5 Red (quicker than % operator)
    r = 255;
    g = b = 0;
  }

  // Apply saturation and value to R,G,B, pack into 32-bit result:
  uint32_t v1 =   1 + val; // 1 to 256; allows >>8 instead of /255
  uint16_t s1 =   1 + sat; // 1 to 256; same reason
  uint8_t  s2 = 255 - sat; // 255 to 0
  return ((((((r * s1) >> 8) + s2) * v1) & 0xff00) << 8) |
          (((((g * s1) >> 8) + s2) * v1) & 0xff00)       |
         ( ((((b * s1) >> 8) + s2) * v1)           >> 8);
}


void X7Segment::CheckToCacheBytes( String str )
{
	if ( str != cachedString )
	{
		cachedString = str;
		
		int index = 0;
		for ( int s = 0; s < str.length(); s++ )
		{
			if ( (String)str.charAt(s) != "." )
			{ 
				cachedBytes[index] = FindByteForCharater( (String)str.charAt(s) );
				index++;
			}
			else if ( s > 0 && bitRead( cachedBytes[index-1], 7 ) != 1 )
			{
				cachedBytes[index-1] = cachedBytes[index-1] | 0b10000000;
			}
			else
			{
				cachedBytes[index] = 0b10000000;
				index++;
			}
		}
	}
}



void X7Segment::FillBuffer()
{
    #if defined(ESP32)

    uint16_t led;
    int col, bit, col_inner;
    int i=0;
    uint16_t num_all_bits = 24 * ledCount;
    int color[] = {0,0,0};

    for ( led=0; led < ledCount; led++ )
    {
        color[0] = AdjustForBrightness ( Green( leds[ led ] ) );
        color[1] = AdjustForBrightness ( Red( leds[ led ] ) );
        color[2] = AdjustForBrightness ( Blue( leds[ led ] ) );

        for ( col=0; col<3; col++ )
        {    
            col_inner = color[col];

            for (bit=0; bit<8; bit++)
            {
                if ( col_inner & ( 1<<( 7-bit ) ) ) 
                {
                    led_data[i].duration0 = 8;
                    led_data[i].duration1 = 4;
                }
                else
                {
                    led_data[i].duration0 = 4;
                    led_data[i].duration1 = 8.5;
                }

                led_data[i].level0 = 1;
                led_data[i].level1 = 0;
                i++;
            }
        }
    }

    // Send the data
    rmtWrite(rmt_send, led_data, num_all_bits);


    #else

        for ( uint16_t i = 0; i < ledCount; i++ )
        {
            pixels.setPixelColor( i, leds[ i ] );
        }
        pixels.show();

    #endif
}


void X7Segment::DisplayTextVerticalRainbow( String text, uint32_t colorA, uint32_t colorB )
{
	if ( !isReady )
		return;
		
	CheckToCacheBytes( text );	
    ClearBuffer();
    uint16_t ledIndex = 0;

	// Clamp the length, so text longer than the display count is ignored
	int lengthOfLoop = min( dispCount, (uint8_t)text.length() );

	uint32_t color;
	
	// Grab the byte (bits) for the segments for the character passed in
	for ( int s = 0; s < lengthOfLoop; s++ )
	{
		byte code = cachedBytes[s];
		if(code > -1)
		{
			int pixelIndex = 0; // current pixel index
		
			for( int segment = 0; segment < 7; segment++ )
			{
				bool on = ( bitRead( code, segment) == 1 );
				for ( int p = 0; p < 2; p++ )
				{
					// we want the Y position (row) so we can use that as the colour index 
					int y = pixelsXY[ pixelIndex ][1];

					uint8_t red = ((Red(colorA) * (10 - y)) + (Red(colorB) * y)) * 0.1;
					uint8_t green = ((Green(colorA) * (10 - y)) + (Green(colorB) * y)) * 0.1;
					uint8_t blue = ((Blue(colorA) * (10 - y)) + (Blue(colorB) * y)) * 0.1;

					color = Color(red, green, blue );
				
					leds[ ledIndex++ ] = ( on ? color : Color(0,0,0) );
					pixelIndex++;
				}
			}

			bool on = ( bitRead( code, 7) == 1 );
			leds[ ledIndex++ ] = ( on ? color : Color(0,0,0) );
		}
	}
		
	// we have finished setting all of the colors on each segment for this X7Segment, so lets turn on the pixels
	FillBuffer();
}

void X7Segment::DisplayTextHorizontalRainbow( String text, uint32_t colorA, uint32_t colorB )
{
	if ( !isReady )
		return;	
	
	CheckToCacheBytes( text );	
    ClearBuffer();
    uint16_t ledIndex = 0;

	int numPixelsPerColumn = 6;
	int numColumns = dispCount * numPixelsPerColumn;

	uint32_t color;

	// Clamp the length, so text longer than the display count is ignored
	int lengthOfLoop = min( dispCount, (uint8_t)text.length() );
	
	// Grab the byte (bits) for the segments for the character passed in
	for ( int s = 0; s < lengthOfLoop; s++ )
	{
		byte code = cachedBytes[s];
		if(code > -1)
		{
			int pixelIndex = 0; // current pixel index
		
			for( int segment = 0; segment < 7; segment++ )
			{
				bool on = ( bitRead( code, segment) == 1 );
				for ( int p = 0; p < NUM_PIXELS_PER_SEGMENT; p++ )
				{
					// we want the Y position (row) so we can use that as the colour index 
					int x = pixelsXY[ pixelIndex ][0] + ( numPixelsPerColumn * s );

					uint8_t red = ((Red(colorA) * (numColumns - x)) + (Red(colorB) * x)) / numColumns;
					uint8_t green = ((Green(colorA) * (numColumns - x)) + (Green(colorB) * x)) / numColumns;
					uint8_t blue = ((Blue(colorA) * (numColumns - x)) + (Blue(colorB) * x)) / numColumns;

					color = Color(red, green, blue );
				
					leds[ ledIndex++ ] = ( on ? color : Color(0,0,0) );
					pixelIndex++;
				}
			}

			bool on = ( bitRead( code, 7) == 1 );
			leds[ ledIndex++ ] = ( on ? color : Color(0,0,0) );
		}
	}
		
	// we have finished setting all of the colors on each segment for this X7Segment, so lets turn on the pixels
	FillBuffer();
}


void X7Segment::DisplayTextColorCycle( String text, uint8_t index )
{
	if ( !isReady )
		return;
		
	CheckToCacheBytes( text );	
    ClearBuffer();
    uint16_t ledIndex = 0;

	// Clamp the length, so text longer than the display count is ignored
	int lengthOfLoop = min( dispCount, (uint8_t)text.length() );

	uint32_t color;
	
	// Grab the byte (bits) for the segments for the character passed in
	for ( int s = 0; s < lengthOfLoop; s++ )
	{
		byte code = cachedBytes[s];
		
		if(code > -1)
		{
			int colorStart = index;
				
			for( int segment = 0; segment < 7; segment++ )
			{
				bool on = ( bitRead( code, segment) == 1 );
				for ( int p = 0; p < NUM_PIXELS_PER_SEGMENT; p++ )
				{
					color = Wheel( colorStart & 255 );
					leds[ ledIndex++ ] = ( on ? color : Color(0,0,0) );
					colorStart+=(255/28);	
				}
			}

			bool on = ( bitRead( code, 7) == 1 );
			leds[ ledIndex++ ] = ( on ? color : Color(0,0,0) );
		}
	}

	// we have finished setting all of the colors on each segment for this X7Segment, so lets turn on the pixels
	FillBuffer();
}


void X7Segment::DisplayTextColor( String text, uint32_t color )
{
	if ( !isReady )
		return;
		
	CheckToCacheBytes( text );

    ClearBuffer();
    uint16_t ledIndex = 0;
	
	// Clamp the length, so text longer than the display count is ignored
	int lengthOfLoop = min( dispCount, (uint8_t)text.length() );

	// Grab the byte (bits) for the segments for the character passed in
	for ( int s = 0; s < lengthOfLoop; s++ )
	{
		byte code = cachedBytes[s];
		
		if(code > -1)
		{
			for( int segment = 0; segment < 7; segment++ )
			{
				bool on = ( bitRead( code, segment) == 1 );
				for ( int p = 0; p < NUM_PIXELS_PER_SEGMENT; p++ )
                    leds[ ledIndex++ ] = ( on ? color : Color(0,0,0) );
			}

			bool on = ( bitRead( code, 7) == 1 );
            leds[ ledIndex++ ] = ( on ? color : Color(0,0,0) );
		}
	}

	// we have finished setting all of the colors on each segment for this X7Segment, so lets turn on the pixels
    FillBuffer();
}

void X7Segment::DisplayTime( uint8_t hours, uint8_t mins, uint8_t secs, uint32_t colorH, uint32_t colorM )
{
	if ( !isReady )
		return;
		
	String text = PadTimeData( hours ) + PadTimeData( mins );

	if ( text.length() < dispCount )
	{
		for ( int i = 0; i < (dispCount - text.length()); i++ )
			text = " " + text;
	}

    ClearBuffer();
    uint16_t ledIndex = 0;

	uint8_t r = Red( colorM ) * 0.5;
	uint8_t g = Green( colorM ) * 0.5;
	uint8_t b = Blue( colorM ) * 0.5;
	uint32_t colorM2 = Color( r, g, b );
	
	// Clamp the length, so text longer than the display count is ignored
	int lengthOfLoop = min( dispCount, (uint8_t)text.length() );

	// Grab the byte (bits) for the segments for the character passed in
	for ( int s = 0; s < lengthOfLoop; s++ )
	{
		byte code = cachedBytes[s];
		
		if(code > -1)
		{
			uint32_t cachedColor = colorH;

			// displaying mins, so work out new color
			if ( s >= dispCount - 2 )
			{
				cachedColor = ( secs % 2 == 0 ) ? colorM2 : colorM;
			}

			for( int segment = 0; segment < 7; segment++ )
			{
				bool on = ( bitRead( code, segment) == 1 );
				for ( int p = 0; p < 2; p++ )
					leds[ ledIndex++ ] = ( on ? cachedColor : Color(0,0,0) );
			}

			bool on = ( bitRead( code, 7) == 1 );
			leds[ ledIndex++ ] = ( on ? cachedColor : Color(0,0,0) );
		}
	}
	// we have finished setting all of the colors on each segment for this X7Segment, so lets turn on the pixels
	FillBuffer();
}

int X7Segment::FindIndexOfChar(String character)
{
	String s = character;
	s.toLowerCase();
	for(int i=0; i< ARRAY_SIZE; i++)
	{
		if( s.equals( (String)( (char)available_codes[i][0] ) ) )
			return i;
	}
	return -1;
}

byte X7Segment::FindByteForCharater( String character )
{
	String s = character;
	s.toLowerCase();
	for(int i=0; i< ARRAY_SIZE; i++)
	{
		if ( isForcedUpper )
		{
			if( s.equals( (String)( (char)available_codes_upper[i][0] ) ) )
				return available_codes_upper[i][1];
		}
		else
		{
			if( s.equals( (String)( (char)available_codes[i][0] ) ) )
				return available_codes[i][1];
		}
	}
	return -1;
}

String X7Segment::PadTimeData( int8_t data )
{
  if( data < 10 )
	return String("0") + String(data);

  return String(data);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t X7Segment::Wheel(byte WheelPos )
{
	WheelPos = 255 - WheelPos;
	if(WheelPos < 85)
		return Color(255 - WheelPos * 3, 0, WheelPos * 3);

	if(WheelPos < 170)
	{
		WheelPos -= 85;
		return Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
	
	WheelPos -= 170;
	return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

uint8_t X7Segment::Red( uint32_t col )
{
	return col >> 16;
}

uint8_t X7Segment::Green( uint32_t col )
{
	return col >> 8;
}


uint8_t X7Segment::Blue( uint32_t col )
{
	return col;
}

uint8_t X7Segment::AdjustForBrightness( uint8_t col )
{
    uint8_t col_fixed = round( (float)col * ( (float)brightness/255.0 ) );
	return col_fixed;
}


