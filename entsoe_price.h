// root certificate from https://transparency.entsoe.eu
const char *root_ca =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB\n"
    "iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n"
    "cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n"
    "BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw\n"
    "MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV\n"
    "BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU\n"
    "aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy\n"
    "dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK\n"
    "AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B\n"
    "3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY\n"
    "tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/\n"
    "Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2\n"
    "VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT\n"
    "79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6\n"
    "c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT\n"
    "Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l\n"
    "c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee\n"
    "UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE\n"
    "Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd\n"
    "BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G\n"
    "A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF\n"
    "Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO\n"
    "VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3\n"
    "ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs\n"
    "8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR\n"
    "iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze\n"
    "Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ\n"
    "XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/\n"
    "qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB\n"
    "VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB\n"
    "L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG\n"
    "jjxDah2nGN59PRbxYvnKkKj9\n"
    "-----END CERTIFICATE-----\n";

RTC_DATA_ATTR float hoursNow;
static String PeriodeFra;
static String PeriodeTil;
RTC_DATA_ATTR char lastYYYYMMDD[9] = {
    'N',
    'i',
    'x',
    '0',
    '0',
    '0',
    '0',
    '0',
    '0',
};

RTC_DATA_ATTR uint8_t priceLevel[48];
RTC_DATA_ATTR float prices[48] = {
    469.67,
    431.24,
    407.16,
    373.51,
    404.26,
    446.59,
    530.91,
    565.09,
    581.97,
    546.76,
    512.39,
    426.53,
    291.43,
    270.00,
    271.85,
    319.90,
    394.81,
    489.53,
    549.96,
    570.10,
    542.50,
    438.58,
    354.24,
    216.78,

    237.55,
    252.80,
    270.85,
    257.99,
    242.83,
    301.00,
    283.21,
    381.13,
    412.39,
    392.42,
    440.91,
    452.40,
    467.60,
    455.16,
    458.54,
    429.83,
    386.09,
    413.53,
    467.18,
    530.00,
    511.69,
    441.89,
    393.92,
    329.95,
};

RTC_DATA_ATTR float Avg1 = 0;
RTC_DATA_ATTR float Avg2 = 0;
RTC_DATA_ATTR uint8_t HoursOn = 4;
RTC_DATA_ATTR uint8_t hoursInGroup = 12; // 24
RTC_DATA_ATTR float MaxPrice = 581.97;
RTC_DATA_ATTR float MinPrice = 237;
void CalcPriceLevels()
{
    float min = 999999;
    float max = 0;

    { // Calc day average
        float sum = 0;
        for (int i = 0; i < 24; i++)
            sum += prices[i];
        Avg1 = sum / 24;
        sum = 0;
        for (int i = 24; i < 48; i++)
            sum += prices[i];
        Avg2 = sum / 24;
    }

    if (hoursInGroup == 24)
    {
        for (int i = 0; i < 24; i++)
        {
            int num = 0;
            for (int j = 0; j < 24; j++)
            {
                if (prices[j] < prices[i])
                    num++;
            }
            priceLevel[i] = num;
        }

        min = 999999;
        max = 0;
        for (int i = 24; i < 48; i++)
        {
            int num = 0;
            for (int j = 24; j < 48; j++)
            {
                if (prices[j] < prices[i])
                    num++;
            }
            priceLevel[i] = num;
        }
    }
    else
    {
        // for (int h = hoursInGroup; h <= 48; h += hoursInGroup)

        for (int b = 0; b < 48 / hoursInGroup; b++)
        {
            min = 999999;
            max = 0;
            for (int i = b * hoursInGroup; i < (b + 1) * hoursInGroup; i++)
            {
                int num = 0;
                for (int j = b * hoursInGroup; j < (b + 1) * hoursInGroup; j++)
                {
                    if (prices[j] < prices[i])
                        num++;
                }
                priceLevel[i] = num;
            }
        }
    }
}

bool DecodePriceXml(String xml)
{
    Serial.printf("Read xml,\n");
    // Serial.println(xml);

    int start = 0;
    int stop = 0;
    // <period.timeInterval>
    //     <start>2022-08-31T22:00Z</start>
    //     <end>2022-09-02T22:00Z</end>
    // </ period.timeInterval>

    start = xml.indexOf("<period.timeInterval>", start);
    if (start < 0)
        return false;
    start = xml.indexOf("<start>", start);
    if (start < 0)
        return false;
    start += 7;
    stop = xml.indexOf("</start>", start);
    if (stop < 0)
        return false;
    if (start >= stop)
        return false;
    PeriodeFra = xml.substring(start, stop);
    start = xml.indexOf("<end>", stop);
    if (start < 0)
        return false;
    start += 5;
    stop = xml.indexOf("</end>", start);
    if (stop < 0)
        return false;
    if (start >= stop)
        return false;
    PeriodeTil = xml.substring(start, stop);
    // Serial.printf("Fra %s til %s\n", PeriodeFra, PeriodeTil);
    Serial.println("Motatt data for perioden " + PeriodeFra + " til " + PeriodeTil);

    for (int i = 0; i < 48; i++)
    {
        prices[i] = 0.0;
    }
    MaxPrice = 0;
    MinPrice = 9000.0;
    for (int i = 0; i < 48; i++)
    {
        start = xml.indexOf("<price.amount>", stop);
        if (start < 0)
            return false;
        stop = xml.indexOf("</price.amount>", start);
        if (stop < 0)
            return false;
        start += 14;
        // Serial.printf("Start %d Stop %d: ", start, stop);
        if (start >= stop)
            return false;
        String vals = xml.substring(start, stop);
        prices[i] = vals.toFloat();
        Serial.printf("%f ", prices[i]);
        if (prices[i] > MaxPrice)
            MaxPrice = prices[i];
        if (prices[i] < MinPrice)
            MinPrice = prices[i];
    }
    Serial.println("\nGot 48 entries.");
    CalcPriceLevels();
    return true;
}

bool getPriceData(WiFiClientSecure &client, const String &yyyyMMdd, const String &yyyyMMddEnd)
{
    if (!yyyyMMdd.startsWith("20"))
    {
        Serial.printf("Skip request. Incorrect date request: %s\n", yyyyMMdd);
        return false;
    }
    Serial.printf("Send request for price data %s.\n", yyyyMMdd);
    client.stop(); // close connection before sending a new request
    HTTPClient http;

    String uri = "https://transparency.entsoe.eu/api?documentType=A44&in_Domain=10YNO-1--------2&out_Domain=10YNO-1--------2&periodStart=" +
                 yyyyMMdd + "0000&periodEnd=" + yyyyMMddEnd + "2300&securityToken=" + ENTSOE_SecurityToken;

    http.begin(uri, root_ca);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
        // if (!DecodePriceXml(http.getStream()))
        if (!DecodePriceXml(http.getString()))
            return false;
        // strcpy(lastYYYYMMDD, yyyyMMdd);
        yyyyMMdd.toCharArray(lastYYYYMMDD, 9);
        client.stop();
    }
    else
    {
        Serial.printf("connection failed, error: %s\n", http.errorToString(httpCode).c_str());
        client.stop();
        http.end();
        return false;
    }
    http.end();
    return true;
}

#define enGRAPHTHICKNESS 4
#define enPLOTWIDTH 930
#define enPLOTHEIGHT 350
#define enPLOTSTART_X 19
#define enPLOTSTART_Y 100

void plotTpr(int16_t *buff, int minY, int maxY, int minA, int maxA, int maxX, int buffIndex)
{
    int num = hoursNow * 60.0 / SLEEP_MINUTES;

    Serial.printf("minY =%d maxY =%d min =%d max =%d \n", minY, maxY, minA, maxA);
    minA -= 10;
    maxA += 10;
    const int minX = enPLOTSTART_X;
    // const int maxX = PLOTWIDTH - 1;
    if (num < 3)
        return;

    double span = SLEEP_MINUTES * num;
    double dx = (double)(maxX - minX) / (double)span;          // 128/7505=0.017
    double dy = (double)(maxY - minY) / (double)(maxA - minA); // 48/0.2=240

#pragma region Temerature grid

    // drawLine(minX - 1, minY, minX - 1, maxY, Black); // y axis
    // for (int tens = -300; tens < maxA; tens += 100)
    // {
    //     if (tens > minA && tens < maxA)
    //     {
    //         int y = (int)(maxY - (tens - minA) * dy);
    //         epd_draw_hline(minX, y - 1, maxX - minX, Black, framebuffer);
    //         epd_draw_hline(minX, y, maxX - minX, Black, framebuffer);
    //         epd_draw_hline(minX, y + 1, maxX - minX, Black, framebuffer);
    //         drawString(minX - 10, y + 2, String((int)(tens / 10)), LEFT);
    //     }
    // }
    // for (int tens = -250; tens < maxA; tens += 100)
    // {
    //     if (tens > minA && tens < maxA)
    //     {
    //         int y = (int)(maxY - (tens - minA) * dy);
    //         epd_draw_hline(minX, y, maxX - minX, Black, framebuffer);
    //         epd_draw_hline(minX, y - 1, maxX - minX, Black, framebuffer);
    //         drawString(minX - 10, y + 2, String((int)(tens / 10)), LEFT);
    //     }
    // }
    // if (maxA - minA < 140)
    //     for (int tens = -300; tens < maxA; tens += 10)
    //     {
    //         if (tens > minA && tens < maxA)
    //         {
    //             int y = (int)(maxY - (tens - minA) * dy);
    //             epd_draw_hline(minX, y, maxX - minX, Grey, framebuffer);
    //         }
    //     }
#pragma endregion

    int i = 0;
    for (int i1 = buffIndex - 1; i++ < num; i1--)
    {
        if (i1 < 0)
            i1 = BUFFSIZE3 - 1;
        int i2 = i1 - 1;
        if (i2 < 0)
            i2 = BUFFSIZE3 - 1;
        if (buff[i1] == EMPTYBUFF)
            break;
        if (buff[i2] == EMPTYBUFF)
            break;

        for (int j = 0; j < 3; j++)
            drawLine(
                (int)(maxX - (double)(SLEEP_MINUTES * (i - 1)) * dx),
                (int)(maxY - (buff[i1] - minA) * dy) + j,
                (int)(maxX - (double)(SLEEP_MINUTES * i) * dx),
                (int)(maxY - (buff[i2] - minA) * dy) + j,
                Black);
    }
}

void plotPrice()
{
    // CalcPriceLevels();

    // if (prices[buffIndex1] != EMPTYBUFF)
    {
        const int minX = enPLOTSTART_X;
        const int maxX = enPLOTSTART_X + enPLOTWIDTH;
        const int minY = enPLOTSTART_Y;
        const int maxY = enPLOTSTART_Y + enPLOTHEIGHT; // max 128
        int min1 = 0, max1 = MaxPrice;
        int num = 48;

        setFont(OpenSans8B);
        int span = 48; // 48 timer
        double dx = (double)(maxX - minX) / (double)span;
#pragma region hour grid
        int pxTime = dx; // one hour
        for (int i = 1; i < span; i++)
        {
            int time = i % 24;
            int x = minX + i * dx;
            if (i % 6 == 0)
            {
                drawString(x, minY - 9, String(time), CENTER);
                drawLine(x - 1, minY, x - 1, maxY + 4, Black);
                drawLine(x, minY, x, maxY + 4, Black);
                drawLine(x + 1, minY, x + 1, maxY + 4, Black);
            }
            else if (i % 2 == 0)
            {
                drawString(x, minY - 9, String(time), CENTER);
                drawLine(x, minY - 4, x, maxY + 4, Black);
            }
            else
            {
                drawLine(x, minY, x, maxY, Black);
                drawString(x, maxY + 6, String(time), CENTER);
            }
        }

#pragma endregion

        {
            int minT = 0; // 0
            int maxT = 10 + max1;
            double dy = (double)(maxY - minY) / (double)(maxT - minT);
            setFont(OpenSans8B);
            Serial.printf("minT =%d maxT =%d dy =%f\n", minT, maxT, dy);

#pragma region Price grid

            drawLine(minX - 1, minY, minX - 1, maxY, Black); // y axis past
            drawLine(maxX - 1, minY, maxX - 1, maxY, Black); // y axis now
            // display.drawFastHLine(minX - 2, (int)(maxY + minA * dy), 4, Black); // x axis
            int priceStep = 100;
            if (maxT < 120)
                priceStep = 10;
            for (int tens = 0; tens < maxT; tens += priceStep)
            {
                if (tens >= minT && tens <= maxT)
                {
                    int y = (int)(maxY - (tens - minT) * dy);
                    // epd_draw_hline(minX, y - 1, maxX - minX, Black, framebuffer);
                    epd_draw_hline(minX, y, maxX - minX, Black, framebuffer);
                    // epd_draw_hline(minX, y + 1, maxX - minX, Black, framebuffer);
                    drawString(minX - 10, y + 2, String(tens), LEFT);

                    // for (int x = minX - 1; x < maxX; x += 2)
                    // 	drawPixel(x, y, Black); // x axis
                    // // setCursor(minX - 10, y + 2);
                    // drawString(minX - 10, y + 2, String((float)(tens / 10), 0), LEFT);
                }
            }

            if (maxT - minT < 80)
                for (int tens = 5; tens < maxT; tens += 10)
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
            { // Plot average
                int avg = (int)(maxY - (Avg1 - minT) * dy);
                for (int x = minX; x < maxX / 2; x += 16)
                {
                    drawLine(x, avg, x + 8, avg, Black);
                    drawLine(x, avg + 1, x + 8, avg + 1, Black);
                }
                avg = (int)(maxY - (Avg2 - minT) * dy);
                for (int x = maxX / 2 + 10; x < maxX; x += 16)
                {
                    drawLine(x, avg, x + 8, avg, Black);
                    drawLine(x, avg + 1, x + 8, avg + 1, Black);
                }
            }

            { // Plot pris graf
                for (int i = 0; i < 48; i++)
                {
                    for (int j = 0; j < enGRAPHTHICKNESS; j++)
                        drawLine(
                            (int)(minX + (double)i * dx),
                            (int)(maxY - (prices[i] - minT) * dy) + j,
                            (int)(minX + (double)(i + 1) * dx),
                            (int)(maxY - (prices[i] - minT) * dy + j),
                            Black);
                }
                for (int i = 1; i < 48; i++)
                {
                    for (int j = 0; j < enGRAPHTHICKNESS; j++)
                        drawLine(
                            (int)(minX + (double)i * dx + j),
                            (int)(maxY - (prices[i - 1] - minT) * dy),
                            (int)(minX + (double)i * dx + j),
                            (int)(maxY - (prices[i] - minT) * dy),
                            Black);
                }

                for (int i = 0; i < 48; i++)
                {
                    if (priceLevel[i] < HoursOn && prices[i] < ((i < 24) ? Avg1 : Avg2))
                        fillRect(
                            (int)(minX + (double)i * dx) + 4,
                            (int)(maxY - (prices[i] - minT) * dy) + 5,
                            (int)dx - 7,
                            (int)((prices[i] + minT) * dy) - 6,
                            LightGrey);
                }
            }
            { // Now indicator
                int x = minX + hoursNow * dx;
                drawLine(x - 2, minY + 10, x - 2, maxY - 10, Black);
                for (int i = minY + 25; i < maxY - 25; i += 20)
                {
                    drawLine(x - 1, i, x - 1, i + 10, Black);
                    drawLine(x, i, x, i + 10, Black);
                    drawLine(x + 1, i, x + 1, i + 10, Black);
                }
                drawLine(x + 2, minY + 10, x + 2, maxY - 10, Black);
                epd_fill_triangle(x, minY - 10, x - 10, minY - 20, x + 10, minY - 20, Black, framebuffer);
                epd_fill_triangle(x, maxY - 10, x - 10, maxY - 0, x + 10, maxY - 0, Black, framebuffer);
            }

            { // Do start/stop VVB

                int hourI = (int)hoursNow;
                if (hourI < 48)
                {
                    if (vvbOn == false && priceLevel[hourI] < HoursOn && prices[hourI] < ((hourI < 24) ? Avg1 : Avg2))
                        VVB_On();
                    else if (vvbOn == true && priceLevel[hourI] >= HoursOn) // 1 hour too late?
                        VVB_Off();
                }
            }
        }
        plotTpr(buff3, minY, maxY, 100, 600, minX + hoursNow * dx, buffIndex3);
    }
}

void showPrice()
{
    // 540 x 960
    plotPrice();

    setFont(OpenSans24B);
    drawString(10, 2, "Strømpriser", LEFT);
    drawString(0, 480, "Ute " + String(temperature1, 1) + "   VVB " + String(temperature3, 1) + " °C", LEFT);
    if (vvbOn == true)
        drawString(900, 480, String("På"), RIGHT);
    else
        drawString(900, 480, String("Av"), RIGHT);
}
