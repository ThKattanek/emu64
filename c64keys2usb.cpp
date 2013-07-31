#include "c64keys2usb.h"

C64Keys2USB::C64Keys2USB()
{
    dev = NULL;
    isUSBOpen = false;
}

C64Keys2USB::~C64Keys2USB()
{
    usbhidCloseDevice(dev);
}

char *C64Keys2USB::usbErrorMessage(int errCode)
{
static char buffer[80];

    switch(errCode){
        case USBOPEN_ERR_ACCESS:      return (char*)"Access to device denied";
        case USBOPEN_ERR_NOTFOUND:    return (char*)"The specified device was not found";
        case USBOPEN_ERR_IO:          return (char*)"Communication error with device";
        default:
            sprintf(buffer, "Unknown USB error %d", errCode);
            return buffer;
    }
    return NULL;    /* not reached */
}

usbDevice_t *C64Keys2USB::openDevice(void)
{
usbDevice_t     *dev = NULL;
unsigned char   rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
char            vendorName[] = {USB_CFG_VENDOR_NAME, 0}, productName[] = {USB_CFG_DEVICE_NAME, 0};
int             vid = rawVid[0] + 256 * rawVid[1];
int             pid = rawPid[0] + 256 * rawPid[1];
int             err;

    if((err = usbhidOpenDevice(&dev, vid, vendorName, pid, productName, 0)) != 0){
        fprintf(stderr, "error finding %s: %s\n", productName, usbErrorMessage(err));
        return NULL;
    }
    return dev;
}

bool C64Keys2USB::readKeys()
{
    int err;
    int len = sizeof(KeyBuffer);

    if((err = usbhidGetReport(dev, 0, (char*)KeyBuffer, &len)) != 0)
    {
        fprintf(stderr, "error reading data: %s\n", usbErrorMessage(err));
        return false;
    }
    else
    {
        return true;
    }
}

bool C64Keys2USB::Open()
{
    dev = openDevice();
    if(dev == NULL)
    {
        isUSBOpen = false;
    }
    else
    {
        isUSBOpen = true;
    }

    return isUSBOpen;
}

void C64Keys2USB::Update()
{
    if(!readKeys())
    {
        for(uint8_t i=0;i<10;i++)
        {
            KeyBuffer[i] = 0x00;
        }
    }
}

uint8_t C64Keys2USB::GetKeysRow(uint8_t col)
{
    if(col > 7) return 0x00;
    return KeyBuffer[col+1];
}

uint8_t C64Keys2USB::GetKeysCol(uint8_t row)
{
    if(row > 7) return 0x00;

    uint8_t ret = 0x00;

    for(uint8_t i=0;i<8;i++)
    {
        ret |= ((KeyBuffer[i+1]>>row)&0x01)<<i;
    }
    return ret;
}

bool C64Keys2USB::GetRestoreStatus()
{
    if(KeyBuffer[8+1]==0x01) return true;
    else return false;
}
