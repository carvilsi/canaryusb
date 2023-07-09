typedef struct 
{
        char *vendor;
        char *product;
        char *product_name;
        char *serial;
} UsbAttrs;
UsbAttrs get_usb_attributes(struct udev_device *dev);
char *get_usb_fingerprint(UsbAttrs usbattrs, char *usb_fingprt);
