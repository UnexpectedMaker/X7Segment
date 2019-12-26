#include <X7Segment.h>

// Initalise the display with 5 X7Segment boards connected to GPIO 10
X7Segment disp( 5, 10 );

int loopIndex = 0;
byte rainbowIndex = 0;
unsigned long nextRainbow = 0;
int displayFeature = 0;
int nextSwitch = 10000;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  // Start the display with a brightness value of 20
  disp.Begin(20);

  // Set the initial display feature to show as 0
  displayFeature = 0;
}

void loop()
{
  // Wait until the display is initialised before we try to show anything
  if ( !disp.IsReady() )
  {
    nextSwitch = millis();
    return;
  }

  // Switch what we are showing every 5 seconds
  if ( millis() > nextSwitch )
  {
    nextSwitch = millis() + 5000;
    displayFeature = ( displayFeature + 1 ) % 7;
    Serial.println(displayFeature);
  }

  // Display stuff on the Neo7Segment displays
  if ( nextRainbow < millis() )
  {
    switch( displayFeature )
    {
      case 0:
        for ( int i = 0; i < disp.GetArraySize(); i++ )
        {
          disp.DisplayTextColor( disp.GetCharacterAtArrayIndex(i), disp.Wheel( rainbowIndex & 255 ) );
          rainbowIndex+=10;
          delay(500);
        }
        break;
        
      case 1:
        disp.DisplayTextColorCycle( "12345", rainbowIndex );
        nextRainbow = millis() + 10;
        rainbowIndex++;
        break;

      case 2:
        disp.DisplayTextVerticalRainbow("     ", disp.Color(255,0,0), disp.Color(0,0,255) );
        delay(250);
        disp.DisplayTextVerticalRainbow("C    ", disp.Color(255,0,0), disp.Color(0,0,255) );
        delay(250);
        disp.DisplayTextVerticalRainbow("CL   ", disp.Color(255,0,0), disp.Color(0,0,255) );
        delay(250);
        disp.DisplayTextVerticalRainbow("CLE  ", disp.Color(255,0,0), disp.Color(0,0,255) );
        delay(250);
        disp.DisplayTextVerticalRainbow("CLEA ", disp.Color(255,0,0), disp.Color(0,0,255) );
        delay(250);
        disp.DisplayTextVerticalRainbow("CLEAR", disp.Color(255,0,0), disp.Color(0,0,255) );
        delay(1250);
        disp.DisplayTextVerticalRainbow(" LEAR", disp.Color(255,0,0), disp.Color(0,0,255) );
        delay(250);
        disp.DisplayTextVerticalRainbow("  EAR", disp.Color(255,0,0), disp.Color(0,0,255) );
        delay(250);
        disp.DisplayTextVerticalRainbow("   AR", disp.Color(255,0,0), disp.Color(0,0,255) );
        delay(250);
        disp.DisplayTextVerticalRainbow("    R", disp.Color(255,0,0), disp.Color(0,0,255) );
        delay(250);
        disp.DisplayTextVerticalRainbow("     ", disp.Color(255,0,0), disp.Color(0,0,255) );
        delay(500);
        nextSwitch = 0;
        break;

      case 3:
        disp.ForceUppercase( false );
        disp.DisplayTextVerticalRainbow("ABCDE", disp.Wheel( rainbowIndex & 255 ) , disp.Wheel( ( rainbowIndex + 50 ) & 255 )  );
        nextRainbow = millis() + 10;
        rainbowIndex--;
        break;
        
       case 4:
        disp.ForceUppercase( false );
        disp.DisplayTextHorizontalRainbow("67890", disp.Wheel( rainbowIndex & 255 ) , disp.Wheel( ( rainbowIndex + 150 ) & 255 )  );
        nextRainbow = millis() + 10;
        rainbowIndex--;
        break;
      
      case 5:
        disp.DisplayTime( 22, 16, (nextRainbow % 60), disp.Color(255, 200, 0), disp.Color(0, 0, 255) );
        nextRainbow = millis() + 500;
        break;

      case 6:
        disp.DisplayTextColorCycle( "0---0", rainbowIndex );
        nextRainbow = millis() + 10;
        rainbowIndex++;
        break;
       
    }
  }
}
