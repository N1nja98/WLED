
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
#include "wled.h"
#include <NeoPixelBrightnessBusGfx.h>
#include <NeoPixelBrightnessBus.h>
#include <Fonts/ORG_04.h>

#define M_PIN 4

#define WIDTH 32
#define HEIGHT 8



// See NeoPixelBus documentation for choosing the correct Feature and Method
// (https://github.com/Makuna/NeoPixelBus/wiki/NeoPixelBus-object)
NeoPixelBrightnessBusGfx<NeoGrbFeature, NeoEsp32Rmt7Ws2812xMethod> matrix(WIDTH, HEIGHT, M_PIN);

// See NeoPixelBus documentation for choosing the correct NeoTopology
// you may also use NeoTile or NeoMosaik
// (https://github.com/Makuna/NeoPixelBus/wiki/Matrix-Panels-Support)
NeoTopology<ColumnMajorAlternatingLayout> topo(WIDTH, HEIGHT);

const uint16_t colors[] = {
	matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255)};

// use a remap function to remap based on the topology, tile or mosaik
// this function is passed as remap function to the matrix
uint16_t remap(uint16_t x, uint16_t y)
{
	return topo.Map(x, y);
}

void MatrixBegin()
{
	matrix.Begin();

	// pass the remap function
	matrix.setRemapFunction(&remap);
	matrix.setTextWrap(false);
	matrix.SetBrightness(255);
	matrix.setTextColor(clock_col);
	matrix.setFont(&Org_04);
}

void Matrix_Print_Time(String time, bool show, bool dim)
{
	int16_t x1, y1;
	uint16_t w, h;
	matrix.getTextBounds(time, 1, 5, &x1, &y1, &w, &h);

	if (w > 28)
	{
		matrix.setCursor(uint16_t((32 - (w)) / 2) - 3, 5);
	}
	else
	{
		matrix.setCursor(uint16_t((32 - w) / 2), 5);
	}

	matrix.fillScreen(0);
	matrix.print(time);

	if (show)
	{
		if (dim)
		{
			matrix.SetBrightness(dim_brightness);
		}
		else
		{
			matrix.SetBrightness(255);
		}

		matrix.Show();
	}
	else
	{
		matrix.SetBrightness(255);
	}
}

void userSetup()
{
	MatrixBegin();
}

//gets called every time WiFi is (re-)connected. Initialize own network interfaces here
void userConnected()
{
}

bool colon = false;
byte prevBri;
unsigned long prevMill;

//loop. You can use "if (WLED_CONNECTED)" to check for successful connection
void userLoop()
{

	if (bri > 0)
	{

		prevBri = bri;
	}

	if (millis() - prevMill >= 500)
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

			Matrix_Print_Time(currentTime, true, dim);
		}
		else
		{
			Matrix_Print_Time(currentTime, true, false);
		}
		prevMill = millis();
	}
}
