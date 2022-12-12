#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
// #include <string>

// #define TEMP1 "a4:c1:38:f4:95:0e" //Navn ATC_F4950E
// #define TEMP2 "a4:c1:38:55:88:7d" //Navn ATC_55887D
// #define TEMP3 "a4:c1:38:de:a6:9f" //Navn LYWSD03MMC
// #define TEMP4 "a4:c1:38:c5:a2:f9" //Navn
//  "sensordef": [ "a4:c1:38:f4:95:0e", "a4:c1:38:c5:a2:f9", "a4:c1:38:de:a6:9f", "a4:c1:38:55:88:7d" ],

typedef struct
{
    esp_bd_addr_t mac; // char mac[6];
    int16_t Temperatur;
    uint8_t Hum;
    uint8_t Batt;
    char *Name;
} KnownSensor;

KnownSensor Sensors[] = {
    // Audun: fyll inn adressene her
    {{0xa4, 0xc1, 0x38, 0x58, 0xb2, 0xf2}, -999, 0, 0, "t1"},
    {{0xa4, 0xc1, 0x38, 0xf1, 0xc7, 0xe4}, -999, 0, 0, "t2"},
    {{0xa4, 0xc1, 0x38, 0xba, 0x9b, 0x1a}, -999, 0, 0, "t3"},
    {{0xa4, 0xc1, 0x38, 0xc0, 0xc6, 0xab}, -999, 0, 0, "t4"},
    {{0xa4, 0xc1, 0x38, 0x3c, 0x70, 0x5e}, -999, 0, 0, "t5"},
};

// KnownSensor Sensors[] = // Fjellro
//     {
//         {{0xa4, 0xc1, 0x38, 0xf4, 0x95, 0x0e}, -999, 0, 0, "Stue"},
//         {{0xa4, 0xc1, 0x38, 0x47, 0xaf, 0x66}, -999, 0, 0, "Ute"}, // var Vask, nå ute
//         {{0xa4, 0xc1, 0x38, 0x55, 0x88, 0x7d}, -999, 0, 0, "Inngang"},
//         {{0xa4, 0xc1, 0x38, 0xde, 0xa6, 0x9f}, -999, 0, 0, "Kjøl"},
//         {{0xa4, 0xc1, 0x38, 0x23, 0x4c, 0x0e}, -999, 0, 0, "Frys"},
//         {{0xa4, 0xc1, 0x38, 0x55, 0xf4, 0xb5}, -999, 0, 0, "MGulv"}, // var i garasjen Søråsen
//         {{0xa4, 0xc1, 0x38, 0xe5, 0x63, 0x4e}, -999, 0, 0, "Do"},
//         {{0xa4, 0xc1, 0x38, 0x55, 0x92, 0xa7}, -999, 0, 0, "Bod"}};

// defekt {{0xa4, 0xc1, 0x38, 0xc5, 0xa2, 0xf9}, -999, 0, 0, "Ute"}, // Fjellro, defekt nov 2022

// KnownSensor Sensors[] = // Søråsen
//     {
//         {{0xa4, 0xc1, 0x38, 0x63, 0x43, 0x2d}, -999, 0, 0, "Ute nord"},
//         {{0xa4, 0xc1, 0x38, 0x7f, 0xb4, 0xa0}, -999, 0, 0, "Ute sør"},
//         {{0xa4, 0xc1, 0x38, 0xc3, 0xcd, 0x97}, -999, 0, 0, "VVB"},
//         {{0xa4, 0xc1, 0x38, 0x4b, 0xbd, 0xd5}, -999, 0, 0, "Soverom"},
//         {{0xa4, 0xc1, 0x38, 0xbe, 0x49, 0xc8}, -999, 0, 0, "Bad"}};

void ClearSensors()
{
    int antSensorer = sizeof(Sensors) / sizeof(KnownSensor);
    for (int t = 0; t < antSensorer; t++)
    {
        Sensors[t].Temperatur = -999;
        Sensors[t].Hum = 0;
        Sensors[t].Batt = 0;
    }
}
KnownSensor *GetSensor(esp_bd_addr_t mac)
{
    int antSensorer = sizeof(Sensors) / sizeof(KnownSensor);
    for (int t = 0; t < antSensorer; t++)
    {
        int i = 5;
        while (mac[i] == Sensors[t].mac[i])
        {
            if (--i < 0)
                return &Sensors[t];
        }
    }
    return nullptr;
}

int scanTimeDuration = 3; // In seconds
BLEScan *pBLEScan;

void printBuffer(uint8_t *buf, int len)
{
    for (int i = 0; i < len; i++)
    {
        Serial.printf("%02x", buf[i]);
    }
    Serial.printf("  Len: %d\n", len);
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    int16_t be16_to_cpu_signed(uint8_t *data)
    {
        int16_t r;
        uint16_t u = (unsigned)data[1] | ((unsigned)data[0] << 8);
        memcpy(&r, &u, sizeof r);
        return r;
    }
    uint16_t be16_to_cpu_unsigned(uint8_t *data)
    {
        int16_t r;
        uint16_t u = (unsigned)data[1] | ((unsigned)data[0] << 8);
        return u;
    }

    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        // Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
        // std::string deviceAddress = advertisedDevice.getAddress().toString();
        esp_bd_addr_t *adrNative = advertisedDevice.getAddress().getNative();

        KnownSensor *sensor = GetSensor(*adrNative);
        if (sensor == nullptr)
            return;

        uint8_t *payload = advertisedDevice.getPayload();
        size_t payloadLength = advertisedDevice.getPayloadLength();
        // Serial.printf("%s (%s): ", deviceAddress.c_str(), advertisedDevice.haveName() ? advertisedDevice.getName().c_str() : "");
        // Serial.printf(" - payload: ");
        // printBuffer(payload, payloadLength);

        if (payloadLength >= 17)
        {
            // custom firmware with non-encrypted payload. https://github.com/atc1441/ATC_MiThermometer
            float temp = be16_to_cpu_signed(payload + 10) / 10.0;
            uint8_t hum = *(payload + 12);
            uint8_t batt = *(payload + 13);
            float volt = be16_to_cpu_unsigned(payload + 14) / 1000.0;

            // Serial.printf(" - %s Temp: %5.1f Humidity: %d%% Batt: %d%% Spenning: %fV\n",sensor->Name, temp, hum, batt, volt);
            sensor->Temperatur = be16_to_cpu_signed(payload + 10);
            sensor->Hum = hum;
            sensor->Batt = batt;
            Serial.printf(" - Sensor %s Temp: %5.1f Humidity: %d%% Batt: %d%%\n", sensor->Name, sensor->Temperatur / 10.0, sensor->Hum, sensor->Batt);

            // Advertising format of the custom firmware
            //  Byte 5-10 MAC in correct order
            //  Byte 11-12 Temperature in int16
            //  Byte 13 Humidity in percent
            //  Byte 14 Battery in percent
            //  Byte 15-16 Battery in mV uint16_t
            //  Byte 17 frame packet counter
        }
    }
};

void BLEsetup()
{
    // Serial.begin(115200);

    Serial.println("BLE initå.");
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); // create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(false); // active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99); // less or equal setInterval value
                             // BLEDevice::deinit(true);
}

void BLEscan()
{
    Serial.println("BLE scanning...");
    BLEScanResults foundDevices = pBLEScan->start(scanTimeDuration, false);
    Serial.print("Devices found: ");
    Serial.println(foundDevices.getCount());
    pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
}
void BLEstop()
{
    Serial.println("BLE stop.");
    pBLEScan->stop();
    BLEDevice::deinit(true);
}
