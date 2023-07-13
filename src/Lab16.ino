SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

#include "oled-wing-adafruit.h"


BleAdvertisingData datas;
OledWingAdafruit display;

const size_t UART_TX_BUF_SIZE = 20;

void onDataReceived(const uint8_t *datas, size_t len, const BlePeerDevice &peer, void *context);

const BleUuid serviceUuid("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");

const BleUuid rxUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
BleCharacteristic rxCharacteristic("rx", BleCharacteristicProperty::WRITE_WO_RSP, rxUuid, serviceUuid, onDataReceived, NULL);

const BleUuid txUuid("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");
BleCharacteristic txCharacteristic("tx", BleCharacteristicProperty::NOTIFY, txUuid, serviceUuid);

uint8_t txBuf[10]; // an array of 10 bytes we can send

String charToStr;
bool disconnectControl = false;
bool refreshControl = false;

void setup()
{
    display.setup();
    BLE.on();
    RGB.control(true);
    Serial.begin(9600);
    
    refreshDisplay();
    BLE.addCharacteristic(txCharacteristic);
    BLE.addCharacteristic(rxCharacteristic);

    txCharacteristic.setValue(txBuf, 10); // here we are sending all 10 bytes

    datas.appendServiceUUID(serviceUuid);
    BLE.advertise(&datas);
    BLE.setDeviceName("Shoubhit!");
}

void loop()
{
    display.loop();
    if (!BLE.connected())
    {
        RGB.color(255, 255, 0);
    }
    else if(BLE.connected())
    {
        if (refreshControl == false)
        {
            refreshDisplay();
            display.display();
            refreshControl = !refreshControl;
        }
        RGB.color(0, 0, 255);
        uint8_t txBuf[UART_TX_BUF_SIZE];
        String message = "Yo!\n";
        message.toCharArray((char *)txBuf, message.length() + 1);
        txCharacteristic.setValue(txBuf, message.length() + 1);
    }
    delay(5000);
    if (disconnectControl)
    {
        BLE.disconnect();
        disconnectControl = false;
        RGB.color(255, 255, 0);
        refreshDisplay();
        display.println("Disconnected");
        display.display();
        delay(5000);
        refreshControl = false;
    }
    
    
   
}

void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
{
    for (size_t i = 0; i < len; i++)
    {
        charToStr = String(data[i]);
        int strToInt = charToStr.toInt();
        Serial.write(data[i]);
        if (strToInt == 49) //Entering a 1
        {
            refreshDisplay();
            display.println("1!");
            display.display();
        }
        if (strToInt == 48) //Entering a 0
        {
            disconnectControl = true;
        }
    }
}

void refreshDisplay()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
}