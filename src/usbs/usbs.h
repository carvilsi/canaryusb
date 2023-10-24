#include <systemd/sd-device.h>

typedef struct {
        char *vendor;
        char *product;
        char *product_name;
        char *serial;
        char *syspath;
}UsbAttrs;

typedef struct {
        char *id_name;
        char *id_serial;
        char *size;
        char *blcksz_prtbltype;
        char *syspath;
}SDCardAttrs;

UsbAttrs get_usb_attributes(sd_device *dev);
void get_usb_fingerprint(UsbAttrs usb_attrs, char *usb_fingprt);
SDCardAttrs get_sdcard_attributes(sd_device *dev);
void get_sdcard_fingerprint(SDCardAttrs sdcrd_attrs, char *sdcrd_fingprt);
char *get_device_fingerprint(sd_device *dev, const char *subsystem);
