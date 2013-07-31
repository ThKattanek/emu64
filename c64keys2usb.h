#ifndef C64KEYS2USB_H
#define C64KEYS2USB_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
using namespace std;


#include "hiddata.h"
#include "firmware/usbconfig.h"  /* for device VID, PID, vendor name and product name */

class C64Keys2USB
{

public:
    C64Keys2USB();
    ~C64Keys2USB();
    bool Open();
    void Update();
    uint8_t GetKeysRow(uint8_t col);
    uint8_t GetKeysCol(uint8_t row);
    bool GetRestoreStatus();

private:
    usbDevice_t *dev;
    usbDevice_t *openDevice(void);
    char *usbErrorMessage(int errCode);
    bool readKeys();
    bool isUSBOpen;
    uint8_t KeyBuffer[11];
};

#endif // C64KEYS2USB_H
