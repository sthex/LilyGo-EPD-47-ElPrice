#include <esp_now.h>
// #include <WiFi.h>

#define ONE_HOUR 3600000
#define TEN_MINUTES 600000

RTC_DATA_ATTR ulong lastReceived = 0;
RTC_DATA_ATTR ulong gotPulsTime = 0;
RTC_DATA_ATTR ulong pulsTime = 0; // delta time

RTC_DATA_ATTR ulong timeStartOfThisHour = 0;
RTC_DATA_ATTR ulong pulsesStartOfThisHour = 0;
RTC_DATA_ATTR ulong pulsesStartOfThisHour = 0;
RTC_DATA_ATTR ulong lastpulscount = 0;
RTC_DATA_ATTR ulong pulscount = 0;

typedef struct struct_message
{
    char a[32];
    int b;
    float c;
    ulong count;
} struct_message;
struct_message myData;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&myData, incomingData, sizeof(myData));
    // Serial.print("Bytes received: ");
    // Serial.println(len);
    // Serial.print("Char: ");
    // Serial.println(myData.a);
    // Serial.print("Int: ");
    // Serial.println(myData.b);
    // Serial.print("Float: ");
    // Serial.println(myData.c);
    // Serial.print("Counter: ");
    // Serial.println(myData.count);
    // Serial.println();

    if (pulsesStartOfThisHour == 0)
    {
        pulsesStartOfThisHour = myData.count;
        pulscount = lastpulscount = myData.count;
        lastReceived = gotPulsTime = millis();
    }
    else if (myData.b > 0)
    {
        lastpulscount = pulscount;
        pulscount = myData.count;
        lastReceived = millis();
        pulsTime = lastReceived - gotPulsTime;
        gotPulsTime += pulsTime;
    }
    else // timer wakup. No puls in 30 sec
    {
        lastReceived = millis();
        pulsTime = lastReceived - gotPulsTime;
    }
}

void ReadPower()
{
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(OnDataRecv);
    ulong start = millis();
    ulong last = lastReceived;
    while (millis() - start < 5000 && lastReceived == last)
    { // wait
        delay(5);
    }
}
double kWh_Now()
{
    if (pulsTime == 0 || lastpulscount == 0)
        return 0.0;
    // Serial.print(pulscount);
    // Serial.print(" ");
    // Serial.print(lastpulscount);
    // Serial.print(" ");
    // Serial.print(pulsTime);
    // Serial.print(" ");
    // Serial.println(1800.0 * (pulscount - lastpulscount) / pulsTime);
    return 1800.0 * (pulscount - lastpulscount) / pulsTime;
}
double kWh_ThisHour()
{
    ulong dt = (millis() - timeStartOfThisHour);
    if (dt < 5000 || pulsesStartOfThisHour >= pulscount)
        return 0.0;
    return (pulscount - pulsesStartOfThisHour) / 2000.0 * ONE_HOUR / dt;
}
// double kWh_LastHour()
// {
//     return pulsesLastHour / 2000.0;
// }
