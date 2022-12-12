#include "XiomiTemperatur.h"

#define EMPTYBUFF -999 //
#define PLOTWIDTH 900
#define PLOTHEIGHT 300
#define PLOTSTART_X 19
#define PLOTSTART_Y 250

#ifdef VVB
#define BUFFSIZE 636  // 750 @ 10 min. 1556 almost one week each 5 min. Max 4096 byte in RTC
#define BUFFSIZE3 240 // 288 is 48 hours of 10 min,   overflowed by 228 bytes: 750->636
RTC_DATA_ATTR int buffIndex3 = 0;
RTC_DATA_ATTR int16_t buff3[BUFFSIZE3]; // 2016 = one temp each 5 min for one week. Max 4096 byte in RTC
#else
#define BUFFSIZE 750 // 750 @ 10 min. 1556 almost one week each 5 min. Max 4096 byte in RTC
#endif

RTC_DATA_ATTR int buffIndex1 = 0;
RTC_DATA_ATTR int16_t buff1[BUFFSIZE]; // 2016 = one temp each 5 min for one week. Max 4096 byte in RTC
RTC_DATA_ATTR int buffIndex2 = 0;
RTC_DATA_ATTR int16_t buff2[BUFFSIZE]; // 2016 = one temp each 5 min for one week. Max 4096 byte in RTC

RTC_DATA_ATTR int sensCount;  // number of one wire sensors
RTC_DATA_ATTR int plotNo = 0; // 1: plot only first sensor.  2: plot only second sensor 0:plot1&2

RTC_DATA_ATTR float temperature1 = -99.0;
RTC_DATA_ATTR float temperature2 = -99.0;
float temperature3 = -99.0;
// float temperature4 = -99.0;
// float temperature5 = -99.0;
// float temperature6 = -99.0;

float tmin1 = 0.0;
float tmax1 = 0.0;
float tmin2 = 0.0;
float tmax2 = 0.0;

void ReadTemperature()
{
	int cnt = 0;
	BLEsetup();
	do
	{
		BLEscan();
	} while (
		(
			Sensors[0].Temperatur < -990 ||
			Sensors[1].Temperatur < -990) &&
		++cnt < 5);

	BLEstop();

	temperature1 = Sensors[0].Temperatur / 10.0;
	temperature2 = Sensors[1].Temperatur / 10.0;
	temperature3 = Sensors[2].Temperatur / 10.0;
	// temperature4 = Sensors[3].Temperatur / 10.0;
	// temperature5 = Sensors[4].Temperatur / 10.0;
	// temperature6 = Sensors[5].Temperatur / 10.0;

	if (temperature1 > -80.0)
	{
		buff1[buffIndex1] = (int16_t)(temperature1 * 10.0);
		if (++buffIndex1 >= BUFFSIZE)
			buffIndex1 = 0;
		buff1[buffIndex1] = EMPTYBUFF;
	}
	else if (buffIndex1 > 0)
	{
		buff1[buffIndex1] = buff1[buffIndex1 - 1]; // set previous temp to keep correct time
		if (++buffIndex1 >= BUFFSIZE)
			buffIndex1 = 0;
		buff1[buffIndex1] = EMPTYBUFF;
	}

	if (temperature2 > -80.0)
	{
		buff2[buffIndex2] = (int16_t)(temperature2 * 10.0);
		if (++buffIndex2 >= BUFFSIZE)
			buffIndex2 = 0;
		buff2[buffIndex2] = EMPTYBUFF;
	}
	else if (buffIndex2 > 0)
	{
		buff2[buffIndex2] = buff2[buffIndex2 - 1]; // set previous temp to keep correct time
		if (++buffIndex2 >= BUFFSIZE)
			buffIndex2 = 0;
		buff2[buffIndex2] = EMPTYBUFF;
	}

#ifdef VVB
	if (temperature3 > -80.0)
	{
		buff3[buffIndex3] = (int16_t)(temperature3 * 10.0);
		if (++buffIndex3 >= BUFFSIZE3)
			buffIndex3 = 0;
		buff3[buffIndex3] = EMPTYBUFF;
	}
	else if (buffIndex3 > 0)
	{
		buff3[buffIndex3] = buff3[buffIndex3 - 1]; // set previous temp to keep correct time
		if (++buffIndex3 >= BUFFSIZE3)
			buffIndex3 = 0;
		buff3[buffIndex3] = EMPTYBUFF;
	}
#endif
	// Serial.printf("   Temperatur %7.2f %7.2f   %7.2f %7.2f   %7.2f %7.2f\n", temperature1, temperature2, temperature3,
	// 			  Sensors[3].Temperatur / 10.0, Sensors[4].Temperatur / 10.0, Sensors[5].Temperatur / 10.0);
}

#define FONT_24 OpenSans24B
#define FONT_12 OpenSans12B

//#define TEST

void clearTempBuff()
{
	Serial.println("Empty buffer.");
	for (int i = 0; i < BUFFSIZE; i++)
		buff2[i] = buff1[i] = EMPTYBUFF;

#ifdef VVB
	for (int i = 0; i < BUFFSIZE3; i++)
		buff3[i] = EMPTYBUFF;
#endif

#ifdef TEST
	int num = BUFFSIZE - 1;
	for (int i = 0; i < num; i++)
	{
		buff1[i] = 50 + 20.0 * sin((double)(0.05 * (i & 0xff)));
		buff2[i] = 0 + (int16_t)(10.0 * cos((double)(0.03 * (i & 0xff))));
	}
	buffIndex2 = buffIndex1 = num;
#endif
}

void displayText(String str, int16_t y, alignment align, int16_t xOffset)
{
	// int16_t x = 0;
	// int16_t x1, y1;
	// uint16_t w, h;
	// display.setCursor(x, y);
	// display.getTextBounds(str, x, y, &x1, &y1, &w, &h);

	switch (align)
	{
	case RIGHT:
		drawString(SCREEN_WIDTH + xOffset, y, str, align);
		break;
	case LEFT:
		drawString(xOffset, y, str, align);
		break;
	case CENTER:
		drawString((SCREEN_WIDTH / 2) + xOffset, y, str, align);
		break;
	default:
		break;
	}
}
void displayTextAt(int16_t x, int16_t y, String str)
{
	drawString(x, y, str, RIGHT);
}

#define GRAPHTHICKNESS 5
void plotT(int16_t *buff, int minY, int maxY, int minA, int maxA, int num, int buffIndex)
{
	Serial.printf("minY =%d maxY =%d min =%d max =%d \n", minY, maxY, minA, maxA);
	minA -= 10;
	maxA += 10;
	const int minX = 10;
	const int maxX = PLOTWIDTH - 1;
	if (num < 3)
		return;

	double span = SLEEP_MINUTES * num;
	double dx = (double)(maxX - minX) / (double)span;		   // 128/7505=0.017
	double dy = (double)(maxY - minY) / (double)(maxA - minA); // 48/0.2=240

#pragma region Temerature grid

	drawLine(minX - 1, minY, minX - 1, maxY, Black); // y axis
	// display.drawFastHLine(minX - 2, (int)(maxY + minA * dy), 4, Black); // x axis
	for (int tens = -300; tens < maxA; tens += 100)
	{
		if (tens > minA && tens < maxA)
		{
			int y = (int)(maxY - (tens - minA) * dy);
			// for (int x = minX - 1; x < maxX; x += 2)
			// 	drawPixel(x, y, Black); // x axis

			epd_draw_hline(minX, y - 1, maxX - minX, Black, framebuffer);
			epd_draw_hline(minX, y, maxX - minX, Black, framebuffer);
			epd_draw_hline(minX, y + 1, maxX - minX, Black, framebuffer);
			drawString(minX - 10, y + 2, String((int)(tens / 10)), LEFT);
		}
	}
	for (int tens = -250; tens < maxA; tens += 100)
	{
		if (tens > minA && tens < maxA)
		{
			int y = (int)(maxY - (tens - minA) * dy);
			// for (int x = minX - 1; x < maxX; x += 2)
			// 	drawPixel(x, y, Black); // x axis
			epd_draw_hline(minX, y, maxX - minX, Black, framebuffer);
			epd_draw_hline(minX, y - 1, maxX - minX, Black, framebuffer);
			drawString(minX - 10, y + 2, String((int)(tens / 10)), LEFT);
		}
	}
	if (maxA - minA < 140)
		for (int tens = -300; tens < maxA; tens += 10)
		{
			if (tens > minA && tens < maxA)
			{
				int y = (int)(maxY - (tens - minA) * dy);
				epd_draw_hline(minX, y, maxX - minX, Grey, framebuffer);
				// for (int x = minX - 1; x < maxX; x += 4)
				// 	drawPixel(x, y, Black); // x axis
			}
		}
#pragma endregion

	int i = 0;
	for (int i1 = buffIndex - 1; i++ < num; i1--)
	{
		if (i1 < 0)
			i1 = BUFFSIZE - 1;
		int i2 = i1 - 1;
		if (i2 < 0)
			i2 = BUFFSIZE - 1;
		if (buff[i1] == EMPTYBUFF)
			break;
		if (buff[i2] == EMPTYBUFF)
			break;

		for (int j = 0; j < GRAPHTHICKNESS; j++)
			drawLine(
				(int)(maxX - (double)(SLEEP_MINUTES * (i - 1)) * dx),
				(int)(maxY - (buff[i1] - minA) * dy) + j,
				(int)(maxX - (double)(SLEEP_MINUTES * i) * dx),
				(int)(maxY - (buff[i2] - minA) * dy) + j,
				Black);
	}
}

void plot()
{
	// if (buff1[buffIndex1] != EMPTYBUFF)
	{
		const int minX = PLOTSTART_X;
		const int maxX = PLOTSTART_X + PLOTWIDTH;
		const int minY = PLOTSTART_Y;
		const int maxY = PLOTSTART_Y + PLOTHEIGHT; // max 128
		int min1 = 999, max1 = -999, min2 = 999, max2 = -999;
		int num = 0;
		for (int i = buffIndex1 - 1; num < BUFFSIZE; i--)
		{
			if (i < 0)
				i = BUFFSIZE - 1;
			if (buff1[i] == EMPTYBUFF)
				break;
			if (buff1[i] < min1)
				min1 = buff1[i];
			if (buff1[i] > max1)
				max1 = buff1[i];
			if (plotNo != 1) // Plot graf for t1 only
			{
				if (buff2[i] == EMPTYBUFF)
					break;
				if (buff2[i] < min2)
					min2 = buff2[i];
				if (buff2[i] > max2)
					max2 = buff2[i];
			}
			++num;
		}
		if (plotNo == 1) // Plot graf for t1 only
		{
			max2 = max1;
			min2 = min1;
		}
		else if (plotNo == 2) // Plot graf for t2 only
		{
			max1 = max2;
			min1 = min2;
		}
		tmin1 = min1 / 10.0;
		tmax1 = max1 / 10.0;
		tmin2 = min2 / 10.0;
		tmax2 = max2 / 10.0;
		Serial.printf("1: index=%d, num=%d\n", buffIndex1, num);
		if (num < 3)
			return;

		// display.setFont(FONT_9);
		// displayTextAt(0, 30, "Min:" + String(min1 / 10.0, 1) + "C");
		// displayTextAt(0, 15, "Max:" + String(max1 / 10.0, 1) + "C");
		setFont(OpenSans8B);
		double span = SLEEP_MINUTES * num;
		double dx = (double)(maxX - minX) / (double)span;
#pragma region hour grid
		int time = 1;
		int pxTime = 60 * dx; // one hour
		for (int i = maxX - pxTime; i > minX; i -= pxTime)
		{
			if (time % 24 == 0)
			{
				drawString(i + 2, minY + 6, String(time / 24), LEFT);
				drawLine(i, minY, i, maxY, Black);
				// display.setCursor(i + 2, minY + 6);
				// display.printf("%d", time / 24);
				// display.drawFastVLine(i, minY, maxY - minY, Black);
			}
			else if (time % 12 == 0)
			{
				for (int y = minY; y < maxY; y += 2)
					drawPixel(i, y, Black);
				// display.drawPixel(i, y, Black);
			}
			else if (time % 6 == 0)
				drawLine(i, maxY - 8, i, maxY + 8, Black);
			else if (pxTime > 1)
				drawLine(i, maxY - 4, i, maxY + 4, Black);
			time++;
		}

#pragma endregion

		if (max1 + 40 < min2)
		{
			int mid = minY + (float)(maxY - minY) * (float)(max1 - min1) / (float)(max1 - min1 + max2 - min2);
			plotT(buff2, minY, mid - 2, min2, max2, num, buffIndex2);
			plotT(buff1, mid + 2, maxY, min1, max1, num, buffIndex1);
			drawLine(minX - 2, mid - 1, minX + 2, mid + 1, Black);
		}
		else if (max2 + 40 < min1)
		{
			int mid = minY + (float)(maxY - minY) * (float)(max1 - min1) / (float)(max1 - min1 + max2 - min2);
			plotT(buff1, minY, mid - 2, min1, max1, num, buffIndex1);
			plotT(buff2, mid + 2, maxY, min2, max2, num, buffIndex2);
			drawLine(minX - 2, mid - 1, minX + 2, mid + 1, Black);
		}
		else
		{
			int minT = -10 + ((min1 < min2) ? min1 : min2);
			int maxT = 10 + ((max1 > max2) ? max1 : max2);

			double dy1 = (double)(maxY - minY) / (double)(max1 - min1);
			double dy2 = (double)(maxY - minY) / (double)(max2 - min2);
			double dy = (double)(maxY - minY) / (double)(maxT - minT);
			setFont(OpenSans8B);
			Serial.printf("min1 =%d max1 =%d min2 =%d max2 =%d \n", min1, max1, min2, max2);
			Serial.printf("minT =%d maxT =%d dy =%f\n", minT, maxT, dy);

#pragma region Temerature grid

			drawLine(minX - 1, minY, minX - 1, maxY, Black); // y axis past
			drawLine(maxX - 1, minY, maxX - 1, maxY, Black); // y axis now
			// display.drawFastHLine(minX - 2, (int)(maxY + minA * dy), 4, Black); // x axis
			for (int tens = -300; tens < maxT; tens += 100)
			{
				if (tens > minT && tens < maxT)
				{
					int y = (int)(maxY - (tens - minT) * dy);
					epd_draw_hline(minX, y - 1, maxX - minX, Black, framebuffer);
					epd_draw_hline(minX, y, maxX - minX, Black, framebuffer);
					epd_draw_hline(minX, y + 1, maxX - minX, Black, framebuffer);
					drawString(minX - 10, y + 2, String((int)(tens / 10)), LEFT);

					// for (int x = minX - 1; x < maxX; x += 2)
					// 	drawPixel(x, y, Black); // x axis
					// // setCursor(minX - 10, y + 2);
					// drawString(minX - 10, y + 2, String((float)(tens / 10), 0), LEFT);
				}
			}
			for (int tens = -250; tens < maxT; tens += 100)
			{
				if (tens > minT && tens < maxT)
				{
					int y = (int)(maxY - (tens - minT) * dy);
					epd_draw_hline(minX, y, maxX - minX, Black, framebuffer);
					epd_draw_hline(minX, y - 1, maxX - minX, Black, framebuffer);
					drawString(minX - 10, y + 2, String((int)(tens / 10)), LEFT);

					// for (int x = minX - 1; x < maxX; x += 4)
					// 	drawPixel(x, y, Black); // x axis
					// // display.setCursor(minX - 10, y + 2);
					// // display.printf("%02d", (int)(tens / 10));
					// drawString(minX - 10, y + 2, String((float)(tens / 10), 0), LEFT);
				}
			}
			if (maxT - minT < 80)
				for (int tens = -300; tens < maxT; tens += 10)
				{
					if (tens > minT && tens < maxT)
					{
						int y = (int)(maxY - (tens - minT) * dy);
						epd_draw_hline(minX, y, maxX - minX, Grey, framebuffer);
						// for (int x = minX - 1; x < maxX; x += 8)
						// 	drawPixel(x, y, Black); // x axis
					}
				}
#pragma endregion

			if (plotNo != 2) // Plot graf for t1
			{
				int i = 0;
				for (int i1 = buffIndex1 - 1; i++ < num; i1--)
				{
					if (i1 < 0)
						i1 = BUFFSIZE - 1;
					int i2 = i1 - 1;
					if (i2 < 0)
						i2 = BUFFSIZE - 1;
					if (buff1[i1] == EMPTYBUFF)
						break;
					if (buff1[i2] == EMPTYBUFF)
						break;

					for (int j = 0; j < GRAPHTHICKNESS; j++)
						drawLine(
							(int)(maxX - (double)(SLEEP_MINUTES * (i - 1)) * dx),
							(int)(maxY - (buff1[i1] - minT) * dy) + j,
							(int)(maxX - (double)(SLEEP_MINUTES * i) * dx),
							(int)(maxY - (buff1[i2] - minT) * dy + j),
							Black);
				}
			}
			if (plotNo != 1) // Plot graf for t2
			{
				int i = 0;
				for (int i1 = buffIndex2 - 1; i++ < num; i1--)
				{
					if (i1 < 0)
						i1 = BUFFSIZE - 1;
					int i2 = i1 - 1;
					if (i2 < 0)
						i2 = BUFFSIZE - 1;
					if (buff2[i1] == EMPTYBUFF)
						break;
					if (buff2[i2] == EMPTYBUFF)
						break;

					for (int j = 0; j < GRAPHTHICKNESS; j++)
						drawLine(
							(int)(maxX - (double)(SLEEP_MINUTES * (i - 1)) * dx),
							(int)(maxY - (buff2[i1] - minT) * dy) + j,
							(int)(maxX - (double)(SLEEP_MINUTES * i) * dx),
							(int)(maxY - (buff2[i2] - minT) * dy) + j,
							Black);
				}
			}
		}
	}
}

#define YEXTRA 5 // 400 //5
void showTemp()
{
	plot();

	// String alt = String(Sensors[2].Name) + ":" + String(Sensors[2].Temperatur / 10.0, 1) + "  " +
	// 			 String(Sensors[3].Name) + ":" + String(Sensors[3].Temperatur / 10.0, 1) + "  " +
	// 			 String(Sensors[4].Name) + ":" + String(Sensors[4].Temperatur / 10.0, 1);

	String alt = String(Sensors[2].Name) + ":" + String(Sensors[2].Temperatur / 10.0, 1) + "  " +
				 String(Sensors[3].Name) + ":" + String(Sensors[3].Temperatur / 10.0, 1) + "  " +
				 String(Sensors[4].Name) + ":" + String(Sensors[4].Temperatur / 10.0, 1) + "  " +
				 String(Sensors[5].Name) + ":" + String(Sensors[5].Temperatur / 10.0, 1) + "  " +
				 String(Sensors[6].Name) + ":" + String(Sensors[6].Temperatur / 10.0, 1) + "  " +
				 String(Sensors[7].Name) + ":" + String(Sensors[7].Temperatur / 10.0, 1);
	setFont(OpenSans8B);
	drawString(20, YEXTRA, alt, LEFT);

	if (azureMode == MODE_ACTIVE)
		drawString(500, 30, String("Azure"), LEFT);
	// display.drawBitmap(143, 0, gImage_radio16, 16, 16, GxEPD_BLACK); // 143 2,9',  115 2.13'
	else if (azureMode == MODE_TWINONLY)
		drawString(500, 30, String("Azure twin"), LEFT);
	// display.drawBitmap(143, 0, gImage_radioLow16, 16, 16, GxEPD_BLACK);
	else if (azureMode == MODE_NOT_TWIN)
	{
		drawString(500, 30, String("Azure event"), LEFT);
		// display.drawBitmap(143, 0, gImage_radioLow16, 16, 16, GxEPD_BLACK);
	}

	// if (temperature3 > -99.0)
	// 	drawString(0, YEXTRA, String(Sensors[2].Name) + ":" + String(temperature3, 1), LEFT);
	// if (temperature4 > -99.0)
	// 	drawString(0, YEXTRA, String(Sensors[3].Name) + ":" + String(temperature3, 1), LEFT);
	// drawString(100, YEXTRA, String(temperature4, 1), LEFT);
	// if (temperature5 > -99.0)
	// 	drawString(0, YEXTRA, String(Sensors[4].Name) + ":" + String(temperature3, 1), LEFT);
	// drawString(200, YEXTRA, String(temperature5, 1), LEFT);
	// if (temperature6 > -99.0)
	// 	drawString(0, YEXTRA, String(Sensors[5].Name) + ":" + String(temperature3, 1), LEFT);
	// drawString(300, YEXTRA, String(temperature6, 1), LEFT);

	setFont(OpenSans24B);
	displayText(String(Sensors[1].Name) + String(" °C"), 35, RIGHT, -100); // vvb
	// displayText(String("Nord °C"), 35, RIGHT, -100);
	drawString(100, 35, String(Sensors[0].Name) + String(" °C"), LEFT);
	// drawString(100, 35, String("Sør °C"), LEFT);

	setFont(msyh96);
	displayText(String(temperature2, 1), 80, RIGHT, -18);
	drawString(0, 80, String(temperature1, 1), LEFT);
}
