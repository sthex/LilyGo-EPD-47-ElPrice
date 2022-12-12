#include "Azure.h"

AzureClass::AzureClass()
{
	messageCount = 1;
}
void AzureClass::Disconnect()
{
	Esp32MQTTClient_Close();
	if (hasWifi)
		Serial.println("has WiFi");
	if (WiFi.disconnect(true))
		Serial.println("WiFi disconnected");
	else
		Serial.println("Fail to disconnect WiFi");
}
void AzureClass::Reconnect()
{
	if (WiFi.reconnect())
		Serial.println("WiFi Reconnected");
	else
		Serial.println("Fail to reconnect WiFi");
}

void AzureClass::Setup()
{
	Serial.println("Initializing Azure...");
	// Initialize the WiFi module
	Serial.println(" > WiFi");
	hasWifi = false;
	InitWifi();
	if (!hasWifi)
	{
		return;
	}

	Serial.println(" > IoT Hub");
	Esp32MQTTClient_SetOption(OPTION_MINI_SOLUTION_NAME, "ElPriceBleAz");
	// Esp32MQTTClient_Init((const uint8_t*)connectionString, true);

	Esp32MQTTClient_SetSendConfirmationCallback(SendConfirmationCallback);
	Esp32MQTTClient_SetMessageCallback(MessageCallback);
	Esp32MQTTClient_SetDeviceTwinCallback(DeviceTwinCallback);
	Esp32MQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);

	Esp32MQTTClient_Init((const uint8_t *)connectionString, true);

	send_interval_ms = millis() - INTERVAL;
}

void AzureClass::Send(const char *msg)
{
	if (hasWifi)
	{
		if (messageSending &&
			(int)(millis() - send_interval_ms) >= INTERVAL)
		{
			messageCount++;
			Serial.println("-- Sending to Azure --");
			char messagePayload[MESSAGE_MAX_LEN];
			snprintf(messagePayload, MESSAGE_MAX_LEN, messageData, DEVICE_ID, 333, msg);
			//  temperature1, temperature2, temperature3, temperature4,
			//  temperature5, temperature6, temperature7, temperature8, wifiRestarts);
			Serial.println(messagePayload);
			EVENT_INSTANCE *message = Esp32MQTTClient_Event_Generate(messagePayload, MESSAGE);
			// Esp32MQTTClient_Event_AddProp(message, "temperatureAlert", "true");
			Esp32MQTTClient_SendEventInstance(message);

			send_interval_ms = millis();
		}
		else
		{
			Esp32MQTTClient_Check();
		}
	}
	delay(200);
}
void AzureClass::Check()
{
	if (hasWifi)
	{
		Esp32MQTTClient_Check();
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utilities
void AzureClass::InitWifi()
{
	if (WiFi.status() == WL_CONNECTED)
	{
		Serial.println("WiFi already connectied.");
		hasWifi = true;
		return;
	}

	for (int i = 0; i < 3; i++)
	{
		Serial.println("Connecting Wifi...");
		WiFi.begin(ssid, password);
		for (int j = 0; j < 120; j++)
		{
			if (WiFi.status() == WL_CONNECTED)
			{
				hasWifi = true;
				Serial.println("");
				Serial.println("WiFi connected");
				Serial.println("IP address: ");
				Serial.println(WiFi.localIP());
				return;
			}
			delay(500);
			Serial.print(".");
		}
	}
	hasWifi = false;
	Serial.println("");
	Serial.println("No WiFi");
}

void AzureClass::SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result)
{
	if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
	{
		Serial.println("Send Confirmation Callback finished.");
	}
	else
	{
		errorCount++;
		Serial.printf("Send failed. ERROR %d\n", result);
	}
}

void AzureClass::MessageCallback(const char *payLoad, int size)
{
	Serial.println("Message callback:");
	Serial.println(payLoad);

	if (strcmp(payLoad, "start") == 0)
	{
		LogInfo("Start sending data");
		messageSending = true;
	}
	else if (strcmp(payLoad, "stop") == 0)
	{
		LogInfo("Stop sending data");
		messageSending = false;
	}
	else if (strcmp(payLoad, "reboot") == 0)
	{
		LogInfo("Requested reboot");
		rebootRequest = true;
	}
	else if (strcmp(payLoad, "mode0") == 0)
	{
		LogInfo("Set mode OFFLINE");
		azureMode = 0;
	}
	else if (strcmp(payLoad, "mode1") == 0)
	{
		LogInfo("Set mode ACTIVE");
		azureMode = 1;
	}
	else if (strcmp(payLoad, "mode2") == 0)
	{
		LogInfo("Set mode Twin only");
		azureMode = 2;
	}
	else if (strcmp(payLoad, "interval10") == 0)
	{
		LogInfo("Set 10 min interval");
		azureSendInterval = 1;
	}
	else if (strcmp(payLoad, "interval30") == 0)
	{
		LogInfo("Set 1/2 hour interval");
		azureSendInterval = 3;
	}
	else if (strcmp(payLoad, "interval1h") == 0)
	{
		LogInfo("Set 1 hour interval");
		azureSendInterval = 6;
	}
	else if (strcmp(payLoad, "interval6h") == 0)
	{
		LogInfo("Set 6 hour interval");
		azureSendInterval = 36;
	}
	else if (strcmp(payLoad, "interval24h") == 0)
	{
		LogInfo("Set 24 hour interval");
		azureSendInterval = 6 * 24;
	}
	else
	{
		DoCommand(payLoad);
	}
}

void AzureClass::DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size)
{
	char *temp = (char *)malloc(size + 1);
	if (temp == NULL)
	{
		return;
	}
	memcpy(temp, payLoad, size);
	temp[size] = '\0';
	// Display Twin message.
	Serial.print("Twin message: ");
	Serial.println(temp);
	// DoCommand(temp);

	free(temp);
}

int AzureClass::DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size)
{
	LogInfo("Try to invoke method %s", methodName);
	/*const*/ char *responseMessage = "\"Successfully invoke device method\"";
	int result = 200;

	if (strcmp(methodName, "start") == 0)
	{
		LogInfo("Start sending data");
		messageSending = true;
	}
	else if (strcmp(methodName, "stop") == 0)
	{
		LogInfo("Stop sending data");
		messageSending = false;
	}
	// else if (strcmp(methodName, "send") == 0)
	//{
	//	LogInfo("Requested send");
	//	lastSendTimeAzure = 1;
	// }
	else if (strcmp(methodName, "reboot") == 0)
	{
		LogInfo("Requested reboot");
		rebootRequest = true;
	}
	else if (strcmp(methodName, "status") == 0)
	{
		responseMessage = new char[280];
		memset(responseMessage, 0, 280);
		// sprintf(responseMessage, "\"Status: Err=%d Sent=%d Mode=%d Last=%fA Avg1h=%fA Consumed=%fAh\"",
		//	errorCount, messageCount, azureMode,
		//	ampBuff.LastValue(), ampBuff.Avg(3600000ul), ampBuff.TotalAh());

		//{"Errors":0, "Sent" : 2, "Mode" : 2, "Amp" : -4.2, "Avg1h" : -4.266617, "ConsumedAh" : -4.287439, "b1volt" : 13.2, "b2volt" : 13.4, "RSSI" : "-55"}
		sprintf(responseMessage, "{\"Errors\":%d, \"Sent\":%d,\"Mode\":%d,\"Temp\":%f,\"RSSI\":\"%d\"}",
				errorCount, messageCount, azureMode,
				temperature1, WiFi.RSSI());

		// sprintf(responseMessage, "\"Status: Err=%d Sent:%d Mode:%d\"", errorCount, messageCount, azureMode);
		LogInfo("Requested status");
	}
	// else if (strcmp(methodName, "time2") == 0)
	//{
	//	ulong time = 0;
	//	char *expectedPayload = new char[80];
	//	memset(expectedPayload, 0, 80);
	//	if (memcpy(expectedPayload, payload, 79) > 0)
	//	{
	//		time = atol(expectedPayload);
	//	}
	//	if (time > 50000ul && time < 86400000ul)
	//	{
	//		LogInfo("Set mode 2 timer %u ms", time);
	//		timeMode2 = time;
	//	}
	//	else
	//	{
	//		LogInfo("Time %u not accepted. '%s'", time, expectedPayload);
	//		responseMessage = "\"Not accepted argument\"";
	//		result = 404;
	//	}
	// }
	// else if (strcmp(methodName, "time3") == 0)
	//{
	//	ulong time = 0;
	//	char *expectedPayload = new char[80];
	//	memset(expectedPayload, 0, 80);
	//	if (memcpy(expectedPayload, payload, 79) > 0)
	//	{
	//		time = atol(expectedPayload);
	//	}
	//	if (time > 50000ul && time < 86400000ul)
	//	{
	//		LogInfo("Set mode 3 timer %u", time);
	//		timeMode3 = time;
	//	}
	//	else
	//	{
	//		LogInfo("Time %u not accepted. '%s'", time, expectedPayload);
	//		responseMessage = "\"Not accepted argument\"";
	//		result = 404;
	//	}
	// }
	else if (strcmp(methodName, "mode") == 0 && size > 0)
	{
		switch (payload[0])
		{
		case '0':
			LogInfo("Set mode OFFLINE");
			azureMode = 0;
			break;
		case '1':
			LogInfo("Set mode ACTIVE");
			azureMode = 1;
			break;
		case '2':
			LogInfo("Set mode AVERAGE");
			azureMode = 2;
			break;
		case '3':
			LogInfo("Set mode AVERAGE3");
			azureMode = 3;
			break;
		default:
			LogInfo("No payload %d found matching mode", payload[0]);
			responseMessage = "\"No such mode found\"";
			result = 404;
			break;
		}
	}
	else
	{
		LogInfo("No method %s found", methodName);
		responseMessage = "\"No method found\"";
		result = 404;
	}

	*response_size = strlen(responseMessage) + 1;
	*response = (unsigned char *)strdup(responseMessage);

	return result;
}

extern AzureClass Azure;
