
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

uint8_t clock_brightness;
bool colon = false;
byte prevBri;
unsigned long prevMillTime;
unsigned long prevMillScroll;
unsigned long prevMillSwitch;
int x = matrix.width();
byte i = 0;
byte ci = 1;
bool clockUpdated = false;

tmElements_t timeFromString(String t)
{
	tmElements_t ti;
	uint8_t h;
	if (t.indexOf(":") > -1)
	{
		if (t.indexOf(":") > 0)
		{
			h = (uint8_t)(t.substring(0, t.indexOf(":"))).toInt();
			ti.Minute = (uint8_t)(t.substring(t.indexOf(":") + 1, t.length())).toInt();
			t.toUpperCase();
			if (t.indexOf("AM") > -1)
			{
				ti.Hour = h;
			}
			else if (t.indexOf("PM") > -1)
			{
				ti.Hour = 12 + h;
			}
			else
			{
				ti.Hour = h;
			}
		}
	}
	return ti;
}

String currentTimeString()
{
	updateLocalTime();
	String currentTime;
	String colonString;
	if (colon)
	{
		colonString = ":";
		colon = false;
	}
	else
	{
		colonString = " ";
		colon = true;
	}

	currentTime += (useAMPM) ? ((hour(localTime) % 12 == 0) ? 12 : hour(localTime) % 12) : hour(localTime);
	currentTime = currentTime + colonString;
	if (minute(localTime) < 10)
		currentTime = currentTime + "0";
	currentTime = currentTime + minute(localTime);

	if (time_format == 1)
	{
		currentTime = currentTime + colonString;
		if (second(localTime) < 10)
			currentTime = currentTime + "0";
		currentTime = currentTime + second(localTime);
	}
	else if (time_format == 2)
	{
		currentTime += (hour(localTime) > 11) ? " PM" : " AM";
	}
	else if (time_format == 3)
	{
		String s = currentTime;
		currentTime = "The time is ";
		currentTime += s;
		currentTime += (hour(localTime) > 11) ? " PM" : " AM";
	}

	return currentTime;
}

String currentDateString()
{
	updateLocalTime();
	String currentDate;

	if (date_format == 0)
	{
		currentDate = (String)month(localTime) + "/" + (String)day(localTime);
	}
	else if (date_format == 1)
	{
		currentDate = (String)month(localTime) + "/" + (String)day(localTime) + "/" + (String)(year(localTime) - 2000);
	}
	else if (date_format == 2)
	{
		currentDate = (String)month(localTime) + "/" + (String)day(localTime) + "/" + (String)(year(localTime));
	}
	else if (date_format == 3)
	{
		currentDate = (String)dayStr(weekday(localTime)) + ", " + (String)month(localTime) + "/" + (String)day(localTime) + "/" + (String)(year(localTime) - 2000);
	}
	else if (date_format == 4)
	{
		currentDate = (String)dayStr(weekday(localTime)) + ", " + (String)month(localTime) + "/" + (String)day(localTime) + "/" + (String)(year(localTime));
	}
	else if (date_format == 5)
	{
		currentDate = (String)dayStr(weekday(localTime)) + ", " + (String)monthStr(month(localTime)) + " " + (String)day(localTime) + ", " + (String)(year(localTime));
	}

	else if (date_format == 6)
	{
		currentDate = "Today is " + (String)dayStr(weekday(localTime)) + ", " + (String)monthStr(month(localTime)) + " " + (String)day(localTime) + ", " + (String)(year(localTime));
	}

	return currentDate;
}

String Greeting()
{
	if (hour(localTime) >= 2 && hour(localTime) < 12)
	{
		return "Good Morning!";
	}
	if (hour(localTime) >= 12 && hour(localTime) <= 19)
	{
		return "Good Afternoon!";
	}
	if (hour(localTime) >= 20 && hour(localTime) <= 23)
	{
		return "Good Evening!";
	}
	if (hour(localTime) >= 0 && hour(localTime) <= 1)
	{
		return "Good Night!";
	}
}

bool dimLights()
{
	bool dim = false;
	if (bri == 0)
	{
		uint8_t m = month(localTime);
		uint8_t h = hour(localTime);
		uint8_t mi = minute(localTime);
		if (auto_dim)
		{
			if (((m > 10 || m < 3) && (h > 16 || h < 8)) || ((m > 4 && m < 9) && (h > 21 || h < 8)) || (((m > 2 && m < 5) || (m > 8 && m < 11)) && (h > 18 || h < 8)))
			{
				dim = true;
			}
		}

		else
		{
			if ((h >= dim_from_hour || h <= dim_to_hour) && (mi >= dim_from_minute || mi <= dim_to_minute) )
			{
				dim = true;
			}
		}
	}
	return dim;
}

uint16_t getTextWidth(String text)
{
	int16_t x1, y1;
	uint16_t w, h;
	matrix.getTextBounds(text, 1, 5, &x1, &y1, &w, &h);
	return w;
}

void scrollText(String text)
{
	if (millis() - prevMillScroll >= (scroll_speed))
	{
		matrix.fillScreen(0);
		matrix.setCursor(x, 0);
		int16_t w = getTextWidth(text);

		matrix.print(text);
		for (size_t i = 0; i < 8; i++)
		{
			matrix.SetPixelColor(i,matrix.Color(0,0,0));
			matrix.SetPixelColor(matrix.PixelCount()-i,matrix.Color(0,0,0));
		}
		
		
		if (--x < (w * (-1)))
		{
			x = matrix.width();

			ci++;
			if (ci > i)
			{
				ci = 1;
			}
			matrix.fillScreen(0);
			prevMillSwitch = millis();
		}
		prevMillScroll = millis();
	}
}

void MatrixBegin()
{
	matrix.Begin();

	// pass the remap function
	matrix.setRemapFunction(&remap);
	matrix.setTextWrap(false);
	setClockBrightness(norm_brightness);
	matrix.setTextColor(clock_col);
	matrix.setFont(&Org_04);
	//matrix.setFont();
}

void Matrix_Print_Time(String time, bool show, bool dim)
{
	if (time_format == 3)
	{
		matrix.setFont();
		uint16_t w = getTextWidth(time);
		scrollText(time);
	}
	else
	{

		matrix.fillScreen(0);
		matrix.setFont(&Org_04);
		uint16_t w = getTextWidth(time);

		if (w > 28)
		{
			matrix.setCursor(uint16_t((32 - (w)) / 2) - 3, 5);
		}
		else if (w < 22)
		{
			matrix.setFont();
			w = getTextWidth(time);
			matrix.setCursor(uint16_t((32 - w) / 2), 0);
		}
		else
		{
			matrix.setCursor(uint16_t((32 - w) / 2), 5);
		}
		matrix.print(time);
	}

	if (show)
	{
		if (dim)
		{
			setClockBrightness(dim_brightness);
		}
		else
		{
			setClockBrightness(norm_brightness);
		}

		matrix.Show();
	}
	else
	{
		setClockBrightness(0);
	}
}

void Matrix_Print_Date(String date, bool show, bool dim)
{
	matrix.setFont();
	uint16_t w = getTextWidth(date);

	if (w > 30)
	{
		scrollText(date);
	}
	else
	{
		matrix.fillScreen(0);
		matrix.setCursor(uint16_t((32 - w) / 2), 0);
		matrix.print(date);
		clockUpdated = true;
	}

	if (show)
	{
		if (dim)
		{
			setClockBrightness(dim_brightness);
		}
		else
		{
			setClockBrightness(norm_brightness);
		}

		matrix.Show();
	}
	else
	{
		setClockBrightness(0);
	}
}

void Matrix_Print_Greeting(String greeting, bool show, bool dim)
{
	matrix.setFont();
	uint16_t w = getTextWidth(greeting);

	if (w > 30)
	{
		scrollText(greeting);
	}
	else
	{
		matrix.fillScreen(0);
		matrix.setCursor(uint16_t((32 - w) / 2), 0);
		matrix.print(greeting);
		clockUpdated = true;
	}

	if (show)
	{
		if (dim)
		{
			setClockBrightness(dim_brightness);
		}
		else
		{
			setClockBrightness(norm_brightness);
		}

		matrix.Show();
	}
	else
	{
		setClockBrightness(0);
	}
}

void setClockColor(uint16_t color)
{
	matrix.setTextColor(color);
}

void setClockBrightness(uint8_t brightness)
{
	matrix.SetBrightness(brightness);
	clock_brightness = brightness;
}

void turnOffClock()
{
	matrix.clear();
	matrix.Show();
}

void userSetup()
{
	MatrixBegin();
}

//gets called every time WiFi is (re-)connected. Initialize own network interfaces here
void userConnected()
{
}

//loop. You can use "if (WLED_CONNECTED)" to check for successful connection
void userLoop()
{

	if (bri > 0)
	{

		prevBri = bri;
	}

	if (clock_on)
	{

		i = 0;
		if (show_time)
		{
			i++;
		}
		if (show_date)
		{
			i++;
		}
		if (show_greeting)
		{
			i++;
		}

		if (i > 1)
		{
			if (millis() - prevMillSwitch >= (opt_alt_speed * 1000))
			{
				if (ci == 1 && time_format < 3)
				{
					ci++;
					prevMillSwitch = millis();
					clockUpdated = false;
				}

				if (ci > i)
				{
					ci = 1;
				}
			}
			if (ci == 1)
			{
				if (time_format==3)
				{
					colon = true;
					Matrix_Print_Time(currentTimeString(), true, dimLights());
				}
				else if (millis() - prevMillTime >= 500)
				{
					Matrix_Print_Time(currentTimeString(), true, dimLights());
					prevMillTime = millis();
				}
			}
			if (ci == 2)
			{
				if (!clockUpdated)
				{
					Matrix_Print_Date(currentDateString(), true, dimLights());
				}
				else
				{
					if (millis() - prevMillSwitch >= (opt_alt_speed * 1000))
					{
						ci++;
						if (ci > i)
						{
							ci = 1;
						}
						clockUpdated = false;
						prevMillSwitch = millis();
					}
				}
			}
			if (ci == 3)
			{
				Matrix_Print_Date(Greeting(), true, dimLights());
			}
		}
		else if (i == 1)
		{
			if (show_time)
			{
				if (millis() - prevMillTime >= 500)
				{
					Matrix_Print_Time(currentTimeString(), true, dimLights());
					prevMillTime = millis();
				}
			}
			if (show_date)
			{
				if (!clockUpdated)
				{
					Matrix_Print_Date(currentDateString(), true, dimLights());
				}
				else
				{
					if (millis() - prevMillSwitch >= (opt_alt_speed * 1000))
					{
						ci++;
						if (ci > i)
						{
							ci = 1;
						}
						clockUpdated = false;
						prevMillSwitch = millis();
					}
				}
			}
			if (show_greeting)
			{

				// Matrix_Print_Time(currentTimeString(), true, dimLights());
				// prevMillTime = millis();
			}
		}
	}
	else
	{
		if (clock_brightness > 0)
		{
			setClockBrightness(0);
		}
	}
}
