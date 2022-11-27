#define Y_BATTERY 20 // 20 or 480

#include <Arduino.h>           // In-built
#include <esp_task_wdt.h>      // In-built
#include "freertos/FreeRTOS.h" // In-built
#include "freertos/task.h"     // In-built
#include "epd_driver.h"        // https://github.com/Xinyuan-LilyGO/LilyGo-EPD47
#include "esp_adc_cal.h"       // In-built
#include "hexpwd.h"

// #include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <HTTPClient.h> // In-built

#include <WiFi.h> // In-built
#include <WiFiClientSecure.h>
#include <SPI.h>  // In-built
#include <time.h> // In-built
#include "tid.h"

#define SCREEN_WIDTH EPD_WIDTH
#define SCREEN_HEIGHT EPD_HEIGHT
#define SLEEP_MINUTES 10

#define BUTTON_0 0
#define BUTTON_1 35
#define BUTTON_2 34
#define BUTTON_3 39
#define OUT_VVB 15
enum alignment
{
    LEFT,
    RIGHT,
    CENTER
};
#define White 0xFF
#define LightGrey 0xBB
#define Grey 0x88
#define DarkGrey 0x44
#define Black 0x00

#define autoscale_on true
#define autoscale_off false
#define barchart_on true
#define barchart_off false >

String Time_str = "--:--:--";
String Date_str = "-- --- ----";
int wifi_signal, CurrentHour = 0, CurrentMin = 0, CurrentSec = 0, EventCnt = 0, vref = 1100;
// //################ PROGRAM VARIABLES and OBJECTS ##########################################
const char *wifinet[] = {
    HEX_WIFI_IDM,
    HEX_WIFI_IDG};
const char *wifipwd[] = {
    HEX_WIFI_passwordM,
    HEX_WIFI_passwordG};
RTC_DATA_ATTR int wifiNum = 0;

long SleepDuration = 10; // 60; // Sleep time in minutes, aligned to the nearest minute boundary, so if 30 will always update at 00 or 30 past the hour
int WakeupHour = 8;      // Wakeup after 07:00 to save battery power
int SleepHour = 23;      // Sleep  after 23:00 to save battery power
long StartTime = 0;
long SleepTimer = 0;
long Delta = 30; // ESP32 rtc speed compensation, prevents display at xx:59:yy and then xx:00:yy (one minute later) to save power

// fonts
#include "opensans8b.h"
// #include "opensans10b.h"
// #include "opensans12b.h"
// #include "opensans18b.h"
#include "opensans24b.h"
#include "hzwnn_96.h"
// #include "FreeSerifBold120pt7b.h"

GFXfont currentFont;
uint8_t *framebuffer;

RTC_DATA_ATTR bool vvbOn = false;
RTC_DATA_ATTR int curDisplay = 0; // 0:pris 1:temperature
RTC_DATA_ATTR char currentYYYMMDD[9];
RTC_DATA_ATTR char currentEndYYYMMDD[9];

void BeginSleep()
{
    epd_poweroff_all();
    UpdateLocalTime();
    SleepTimer = (SleepDuration * 60 - ((CurrentMin % SleepDuration) * 60 + CurrentSec)) + Delta; // Some ESP32 have a RTC that is too fast to maintain accurate time, so add an offset
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_1, LOW);
    // esp_sleep_enable_ext1_wakeup((gpio_num_t)BUTTON_3,  ESP_EXT1_WAKEUP_ALL_LOW);
    esp_sleep_enable_timer_wakeup(SleepTimer * 1000000LL); // in Secs, 1000000LL converts to Secs as unit = 1uSec
    Serial.println("Awake for : " + String((millis() - StartTime) / 1000.0, 3) + "-secs");
    Serial.println("Entering " + String(SleepTimer) + " (secs) of sleep time");
    Serial.println("Starting deep-sleep period...");
    esp_deep_sleep_start(); // Sleep for e.g. 30 minutes
}
void QuickSleep() // 1 sec.
{
    epd_poweroff_all();
    UpdateLocalTime();
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_1, LOW);
    esp_sleep_enable_timer_wakeup(1 * 1000000LL); // in Secs, 1000000LL converts to Secs as unit = 1uSec
    Serial.println("Starting quick-sleep.");
    esp_deep_sleep_start();
}
boolean SetupTime()
{
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer, "time.nist.gov"); //(gmtOffset_sec, daylightOffset_sec, ntpServer)
    setenv("TZ", Timezone, 1);                                                 // setenv()adds the "TZ" variable to the environment with a value TimeZone, only used if set to 1, 0 means no change
    tzset();                                                                   // Set the TZ environment variable
    delay(100);
    return UpdateLocalTime();
}

uint8_t StartWiFi()
{

    IPAddress dns(8, 8, 8, 8); // Use Google DNS
    WiFi.disconnect();
    WiFi.mode(WIFI_STA); // switch off AP
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);

    Serial.printf("Connecting to %s ", wifinet[wifiNum]);
    WiFi.begin(wifinet[wifiNum], wifipwd[wifiNum]);
    int i = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        i++;
        if (i == 20)
        {
            if (++wifiNum > 1)
                wifiNum = 0;
            Serial.printf("\nConnecting to %s ", wifinet[wifiNum]);
            WiFi.begin(wifinet[wifiNum], wifipwd[wifiNum]);
        }
        else if (i > 60)
        {
            Serial.println("WiFi connection *** FAILED ***");
        }
    }
    Serial.println("");

    // WiFi.begin(HEX_WIFI_IDG, HEX_WIFI_passwordG);
    // if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //   Serial.printf("STA: Failed!\n");
    //   WiFi.disconnect(false);
    //   delay(500);
    //   WiFi.begin(HEX_WIFI_IDG, HEX_WIFI_passwordG);
    // }
    if (WiFi.status() == WL_CONNECTED)
    {
        wifi_signal = WiFi.RSSI(); // Get Wifi Signal strength now, because the WiFi will be turned off to save power!
        Serial.println("WiFi connected at: " + WiFi.localIP().toString());
    }
    else
        Serial.println("WiFi connection *** FAILED ***");
    return WiFi.status();
}

void StopWiFi()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    Serial.println("WiFi switched Off");
}

void InitialiseSystem()
{
    StartTime = millis();
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println(String(__FILE__) + "\nStarting...");
    epd_init();
    framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
    if (!framebuffer)
        Serial.println("Memory alloc failed!");
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
}

void VVB_On()
{
    Serial.println("VVB PÅ.");
    digitalWrite(OUT_VVB, LOW);
    delay(500);
    digitalWrite(OUT_VVB, HIGH);
    vvbOn = true;
}
void VVB_Off()
{
    Serial.println("VVB AV.");
    digitalWrite(OUT_VVB, LOW);
    delay(300);
    digitalWrite(OUT_VVB, HIGH);
    delay(300);
    digitalWrite(OUT_VVB, LOW);
    delay(300);
    digitalWrite(OUT_VVB, HIGH);
    vvbOn = false;
}
#include "BLERead.h"
#include "entsoe_price.h"
// #include "espNowPow.h"

#define ONE_MINUTE 60000ul   // 1 Minutes
#define FIVE_MINUTE 300000ul // 5 Minutes
#define TEN_MINUTE 600000ul  // 10 Minutes
ulong lastTime = 0;

void setup()
{
    InitialiseSystem();

    pinMode(BUTTON_1, INPUT);
    pinMode(BUTTON_2, INPUT);
    pinMode(BUTTON_3, INPUT);
    pinMode(OUT_VVB, OUTPUT);
    digitalWrite(OUT_VVB, HIGH);

    setenv("TZ", Timezone, 1); // setenv()adds the "TZ" variable to the environment with a value TimeZone, only used if set to 1, 0 means no change
    tzset();                   // Set the TZ environment variable

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED)
    {
        Serial.println("\n***** LilyGo-EPD-47-ElPrice **** ");
        clearTempBuff();
        if (StartWiFi() == WL_CONNECTED && SetupTime() == true)
        {
            Serial.println("Time setup done.");
        }
    }

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0)
    {

        // menuPage();
        if (++curDisplay > 1)
            curDisplay = 0;
        Serial.println("Wakeup by ext0.");
        showPage();
    }
    else
    {
        ReadTemperature();
        // ReadPower();

        if (curDisplay == 0)
        {
            if (UpdateLocalTime() == false)
            {
                if (StartWiFi() == WL_CONNECTED && SetupTime() == true)
                {
                    Serial.println("Time setup done.");
                }
            }

            Serial.printf("last: %s need %s\n", lastYYYYMMDD, currentYYYMMDD);
            if (strcmp(lastYYYYMMDD, currentYYYMMDD) != 0)
            {
                if (WiFi.status() == WL_CONNECTED || (StartWiFi() == WL_CONNECTED && SetupTime() == true)) // SetupTime once every day
                {
                    if (strcmp(lastYYYYMMDD, currentYYYMMDD) != 0) // just in case time was wrong
                    {
                        byte Attempts = 1;
                        bool RxPrice = false;
                        WiFiClientSecure client; // wifi client object
                        while ((RxPrice == false) && Attempts <= 2)
                        { // Try up-to 2 time
                            if (RxPrice == false)
                                RxPrice = getPriceData(client, String(currentYYYMMDD), String(currentEndYYYMMDD));
                            Attempts++;
                        }
                        // Serial.println("Received price data...");
                        // if (RxPrice)
                        // {
                        //     StopWiFi(); // Reduces power consumption
                        // }
                        if ((RxPrice == false) && hoursNow < 24.0)
                            hoursNow += 24.0; // fail to read new data. Keep the old. Hjelper dette?
                    }
                }
            }
            // else
            {
                // Serial.println("No need to get prices.");
                if (WiFi.status() == WL_CONNECTED)
                    StopWiFi(); // Reduces power consumption
            }
        }

        showPage();

        BeginSleep();
    }
    lastTime = millis();
}

void loop()
{
    if (millis() - lastTime > ONE_MINUTE)
        QuickSleep();

    if (digitalRead(BUTTON_1) == LOW)
    {
        while (digitalRead(BUTTON_1) == LOW)
            delay(2);
        if (++curDisplay > 1)
            curDisplay = 0;
        showPage();
        lastTime = millis();
    }
    if (digitalRead(BUTTON_2) == LOW)
    {
        while (digitalRead(BUTTON_2) == LOW)
        {
            if (digitalRead(BUTTON_3) == LOW)
            {
                QuickSleep();
            }
            delay(2);
        }
        if (HoursOn < 24)
            HoursOn++;
        Serial.println("Button 2 pressed. Increase hours");
        showPage();
        lastTime = millis();
    }
    if (digitalRead(BUTTON_3) == LOW)
    {
        while (digitalRead(BUTTON_3) == LOW)
        {
            if (digitalRead(BUTTON_2) == LOW)
            {
                QuickSleep();
            }
            delay(2);
        }
        Serial.println("Button 3 pressed. Decrease hours");
        if (HoursOn > 0)
            HoursOn--;
        showPage();
        lastTime = millis();
    }
}

boolean UpdateLocalTime()
{
    struct tm timeinfo;
    char time_output[30], day_output[30], update_time[30];
    while (!getLocalTime(&timeinfo, 7000))
    { // Wait for 5-sec for time to synchronise
        Serial.println("Failed to obtain time");
        return false;
    }
    CurrentHour = timeinfo.tm_hour;
    CurrentMin = timeinfo.tm_min;
    CurrentSec = timeinfo.tm_sec;
    // See http://www.cplusplus.com/reference/ctime/strftime/
    Serial.println(&timeinfo, "%a %b %d %Y   %H:%M:%S"); // Displays: Saturday, June 24 2017 14:05:49
    sprintf(day_output, "%s, %02u %s %04u", weekday_D[timeinfo.tm_wday], timeinfo.tm_mday, month_M[timeinfo.tm_mon], (timeinfo.tm_year) + 1900);
    strftime(update_time, sizeof(update_time), "%H:%M:%S", &timeinfo); // Creates: '@ 14:05:49'   and change from 30 to 8 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    sprintf(time_output, "%s", update_time);
    Date_str = day_output;
    Time_str = time_output;
    Serial.println(day_output);

    // strftime(currentYYYMMDD, sizeof(currentYYYMMDD), "%Y%m%d", &timeinfo);
    // Serial.printf("Time 1 %s\n", currentYYYMMDD);
    // sprintf(currentYYYMMDD, "%04u%02u%02u", (timeinfo.tm_year) + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
    // Serial.printf("Time 2 %s   time=%d\n", currentYYYMMDD, timeinfo.tm_hour);
    // setYesterday(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
    // Serial.printf("Time 3 %s\n", currentYYYMMDD);
    Serial.printf("Sommertid %d\n", timeinfo.tm_isdst);

    if (timeinfo.tm_hour > 13 || (timeinfo.tm_hour == 13 && timeinfo.tm_min >= 15))
    {
        strftime(currentYYYMMDD, sizeof(currentYYYMMDD), "%Y%m%d", &timeinfo);
        setTomorrow(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
        hoursNow = timeinfo.tm_hour + timeinfo.tm_min / 60.0;
    }
    else
    {
        setYesterday(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
        strftime(currentEndYYYMMDD, sizeof(currentEndYYYMMDD), "%Y%m%d", &timeinfo);

        hoursNow = 24.0 + timeinfo.tm_hour + timeinfo.tm_min / 60.0;
    }
    Serial.printf("currentYYYMMDD = %s\n", currentYYYMMDD);

    return true;
}

void setYesterday(int year, int mon, int day)
{
    if (--day == 0)
    {
        if (--mon == 0)
        {
            mon = 12;
            year--;
        }
        switch (mon)
        {
        case 1:
            day = 31;
            break;
        case 2:
            if (year == 2024 || year == 2028 || year == 2032 || year == 2036 || year == 2040 || year == 2044)
                day = 29;
            else
                day = 28;
        case 3:
            day = 31;
            break;
        case 4:
            day = 30;
            break;
        case 5:
            day = 31;
            break;
        case 6:
            day = 30;
            break;
        case 7:
            day = 31;
            break;
        case 8:
            day = 31;
            break;
        case 9:
            day = 30;
            break;
        case 10:
            day = 31;
            break;
        case 11:
            day = 30;
            break;
        case 12:
            day = 31;
            break;
        }
    }
    sprintf(currentYYYMMDD, "%04u%02u%02u", year, mon, day);
}
int daysInMonth(int year, int mon)
{
    switch (mon)
    {
    case 1:
        return 31;
    case 2:
        if (year == 2024 || year == 2028 || year == 2032 || year == 2036 || year == 2040 || year == 2044)
            return 29;
        else
            return 28;
    case 3:
        return 31;
    case 4:
        return 30;
    case 5:
        return 31;
    case 6:
        return 30;
    case 7:
        return 31;
    case 8:
        return 31;
    case 9:
        return 30;
    case 10:
        return 31;
    case 11:
        return 30;
    case 12:
        return 31;
    }
}
void setTomorrow(int year, int mon, int day)
{
    if (++day > daysInMonth(year, mon))
    {
        day = 1;
        if (++mon > 12)
        {
            mon = 1;
            year++;
        }
    }
    sprintf(currentEndYYYMMDD, "%04u%02u%02u", year, mon, day);
}
void showPage()
{
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

    if (curDisplay == 0)
        showPricePage();
    else
        showTempPage();
}

void showPricePage()
{
    epd_poweron();               // Switch on EPD display
    epd_clear();                 // Clear the screen
    DisplayGeneralInfoSection(); // Top line of the display

    showPrice();
    // plot();

    setFont(OpenSans8B);
    DrawBattery(750, Y_BATTERY);
    // drawString(750, 480, "hN=" + String(hoursNow, 1), LEFT);

    edp_update();       // Update the display to show the information
    epd_poweroff_all(); // Switch off all power to EPD
}
void showTempPage()
{
    epd_poweron();               // Switch on EPD display
    epd_clear();                 // Clear the screen
    DisplayGeneralInfoSection(); // Top line of the display

    showTemp();
    setFont(OpenSans8B);
    DrawBattery(750, Y_BATTERY);

    edp_update();       // Update the display to show the information
    epd_poweroff_all(); // Switch off all power to EPD
}

void menuPage()
{
    epd_poweron(); // Switch on EPD display
    lastTime = millis();
    int menu = 2;
    makeMenuPage();
    setMenuPage(menu);
    while (true)
    {
        if (millis() - lastTime > ONE_MINUTE)
            return;

        if (digitalRead(BUTTON_1) == LOW)
        {
            while (digitalRead(BUTTON_1) == LOW)
                delay(2);
            if (++menu > 2)
                menu = 0;
            setMenuPage(menu);
            lastTime = millis();
        }
        if (digitalRead(BUTTON_2) == LOW)
        {
            while (digitalRead(BUTTON_2) == LOW)
                delay(2);
            Serial.println("Button 2 pressed.");
            if (menu == 0)
            {
                if (--curDisplay < 0)
                    curDisplay = 1;
            }
            else if (menu == 1)
            {
                if (HoursOn > 0)
                    HoursOn--;
            }
            else if (menu == 2)
            {
                return;
            }
            lastTime = millis();
            makeMenuPage();
            setMenuPage(menu);
        }
        if (digitalRead(BUTTON_3) == LOW)
        {
            while (digitalRead(BUTTON_3) == LOW)
                delay(2);
            Serial.println("Button 3 pressed.");
            if (menu == 0)
            {
                if (++curDisplay > 1)
                    curDisplay = 0;
            }
            else if (menu == 1)
            {
                if (HoursOn < 24)
                    HoursOn++;
            }
            else if (menu == 2)
            {
                return;
            }
            lastTime = millis();
            makeMenuPage();
            setMenuPage(menu);
        }
    }
}
void makeMenuPage()
{
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

    // epd_poweron(); // Switch on EPD display
    epd_clear(); // Clear the screen
    setFont(OpenSans24B);
    drawString(350, 10, "^    ^    ^", LEFT);
    drawString(350, 40, "v    -    +", LEFT);
    drawString(90, 100, (curDisplay == 0 ? "Skjerm: Temperatur" : "Skjerm: Strømpris"), LEFT);
    drawString(90, 200, "Timer " + String(HoursOn), LEFT);
    drawString(90, 300, "Fortsett ..", LEFT);

    // edp_update();       // Update the display to show the information
    // epd_poweroff_all(); // Switch off all power to EPD
}
void setMenuPage(int menu)
{
    // Rect_t rec;
    // rec.x = 10;
    // rec.y = 20;
    // rec.width = 40;
    // rec.height = 100;
    // epd_clear_area(rec);
    // epd_fill_rect(some_rect, 0x0, fb);
    fillRect(0, 90, 90, 400, White);

    setFont(OpenSans24B);
    switch (menu)
    {
    case 0:
        drawString(10, 100, "=>", LEFT);
        break;
    case 1:
        drawString(10, 200, "=>", LEFT);
        break;
    case 2:
        drawString(10, 300, "=>", LEFT);
        break;
    }

    edp_update(); // Update the display to show the information
    // epd_poweroff_all(); // Switch off all power to EPD
}

#pragma region Misc
void DisplayGeneralInfoSection()
{
    // setFont(OpenSans10B);
    // drawString(5, 2, City, LEFT);
    setFont(OpenSans8B);
    drawString(500, 2, Date_str + "  @   " + Time_str, LEFT);
}

void DrawBattery(int x, int y)
{
    uint8_t percentage = 100;
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
    {
        Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
        vref = adc_chars.vref;
    }
    float voltage = analogRead(36) / 4096.0 * 6.566 * (vref / 1000.0);
    if (voltage > 1)
    { // Only display if there is a valid reading
        Serial.println("\nVoltage = " + String(voltage));
        percentage = 2836.9625 * pow(voltage, 4) - 43987.4889 * pow(voltage, 3) + 255233.8134 * pow(voltage, 2) - 656689.7123 * voltage + 632041.7303;
        if (voltage >= 4.20)
            percentage = 100;
        if (voltage <= 3.20)
            percentage = 0; // orig 3.5
        drawRect(x + 25, y - 14, 40, 15, Black);
        fillRect(x + 65, y - 10, 4, 7, Black);
        fillRect(x + 27, y - 12, 36 * percentage / 100.0, 11, Black);
        drawString(x + 85, y - 14, String(percentage) + "%  " + String(voltage, 1) + "v", LEFT);
    }
}

void drawString(int x, int y, String text, alignment align)
{
    char *data = const_cast<char *>(text.c_str());
    int x1, y1; // the bounds of x,y and w and h of the variable 'text' in pixels.
    int w, h;
    int xx = x, yy = y;
    get_text_bounds(&currentFont, data, &xx, &yy, &x1, &y1, &w, &h, NULL);
    if (align == RIGHT)
        x = x - w;
    if (align == CENTER)
        x = x - w / 2;
    int cursor_y = y + h;
    write_string(&currentFont, data, &x, &cursor_y, framebuffer);
}

void fillCircle(int x, int y, int r, uint8_t color)
{
    epd_fill_circle(x, y, r, color, framebuffer);
}

void drawFastHLine(int16_t x0, int16_t y0, int length, uint16_t color)
{
    epd_draw_hline(x0, y0, length, color, framebuffer);
}

void drawFastVLine(int16_t x0, int16_t y0, int length, uint16_t color)
{
    epd_draw_vline(x0, y0, length, color, framebuffer);
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    epd_write_line(x0, y0, x1, y1, color, framebuffer);
}

void drawCircle(int x0, int y0, int r, uint8_t color)
{
    epd_draw_circle(x0, y0, r, color, framebuffer);
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    epd_draw_rect(x, y, w, h, color, framebuffer);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    epd_fill_rect(x, y, w, h, color, framebuffer);
}

void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    epd_fill_triangle(x0, y0, x1, y1, x2, y2, color, framebuffer);
}

void drawPixel(int x, int y, uint8_t color)
{
    epd_draw_pixel(x, y, color, framebuffer);
}

void setFont(GFXfont const &font)
{
    currentFont = font;
}

void edp_update()
{
    epd_draw_grayscale_image(epd_full_screen(), framebuffer); // Update the screen
}
#pragma endregion
