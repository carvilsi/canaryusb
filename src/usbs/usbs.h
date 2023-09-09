#include <systemd/sd-device.h>

typedef struct 
{
        char *vendor;
        char *product;
        char *product_name;
        char *serial;
        char *syspath;
} UsbAttrs;
UsbAttrs get_usb_attributes(sd_device *dev);
char *get_usb_fingerprint(UsbAttrs usbattrs, char *usb_fingprt);

