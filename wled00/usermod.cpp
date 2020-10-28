#include "wled.h"
/*
 * This v1 usermod file allows you to add own functionality to WLED more easily
 * See: https://github.com/Aircoookie/WLED/wiki/Add-own-functionality
 * EEPROM bytes 2750+ are reserved for your custom use case. (if you extend #define EEPSIZE in const.h)
 * If you just need 8 bytes, use 2551-2559 (you do not need to increase EEPSIZE)
 * 
 * Consider the v2 usermod API if you need a more advanced feature set!
 */

//Use userVar0 and userVar1 (API calls &U0=,&U1=, uint16_t)

//gets called once at boot. Do all initialization that doesn't depend on network here
void userSetup()
{
  
}

//gets called every time WiFi is (re-)connected. Initialize own network interfaces here
void userConnected()
{

}

bool colon = false;
byte prevBri;
//loop. You can use "if (WLED_CONNECTED)" to check for successful connection
void userLoop()
{
  if (bri > 0)
	{

		prevBri = bri;
	}

	EVERY_N_MILLIS(500)
	{
		updateLocalTime();

		String currentTime;

		if (colon == true)
		{
			currentTime += (useAMPM) ? ((hour(localTime) % 12 == 0) ? 12 : hour(localTime) % 12) : hour(localTime);
			currentTime = currentTime + ":";
			if (minute(localTime) < 10)
				currentTime = currentTime + "0";
			currentTime = currentTime + minute(localTime);
			currentTime = currentTime + ":";
			if (second(localTime) < 10)
				currentTime = currentTime + "0";
			currentTime = currentTime + second(localTime);
			/*if (useAMPM)
				{
					ret += (hour(localTIme) > 11) ? " PM" : " AM";
				}*/
			colon = false;
		}
		else
		{
			currentTime += (useAMPM) ? ((hour(localTime) % 12 == 0) ? 12 : hour(localTime) % 12) : hour(localTime);
			currentTime = currentTime + " ";
			if (minute(localTime) < 10)
				currentTime = currentTime + "0";
			currentTime = currentTime + minute(localTime);
			currentTime = currentTime + " ";
			if (second(localTime) < 10)
				currentTime = currentTime + "0";
			currentTime = currentTime + second(localTime);
			/*if (useAMPM)
				{
					ret += (hour(localTime) > 11) ? " PM" : " AM";
				}*/

			colon = true;
		}
		if (bri == 0)
		{
			bool dim = false;
			 uint8_t m = month(localTime);
			  uint8_t h = hour(localTime);

            if (((m > 10 || m < 3) && (h > 16 || h < 8)) || ((m > 4 && m < 9) && (h > 21 || h < 8)) || (((m > 2 && m < 5) || (m > 8 && m < 11)) && (h > 18 || h < 8)))
            {
	             dim = true;

	        }

            strip.Print_Time(currentTime, true, dim);
		}
		else
		{
			strip.Print_Time(currentTime, false, false);
		}
}
