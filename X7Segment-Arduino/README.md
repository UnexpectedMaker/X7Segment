# X7Segment Display Arduino Library
An API that lets you display numbers and letters on X7Segment RGB LED displays that are available from [tindie](https://www.tindie.com/products/16900/)

This software has been released as open source under the MIT license. Please review the license before using these files in your own projects to understand your obligations.


## Installing and Use
The downloaded code should be added to your arduino IDE libraries folder, and the library requires the [Adafruit NeoPixel library](https://github.com/adafruit/Adafruit_NeoPixel) for it to function.

To include the X7Segment library in your project:

    #include <X7Segment.h>

You can then initialise the display with the following line that includes the number of X7Segment digits and the GPIO to control them:

    X7Segment disp( 5, 4 );

You then start the display with the begin method, passing the brightness:

    disp.Begin(20);

The you simply pass the display the String you would like displayed and a color:

    disp.DisplayText( "1234", disp.Color( 255,0,0) );
