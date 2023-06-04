#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <libudev.h>
#include <unistd.h>

#include "usbs.h"
#include "canaryusb.h"
#include "util.h"

UsbAttrs get_usb_attributes(struct udev_device *dev) 
{
        UsbAttrs usbattr = {"0000", "0000", "no", "no"};
        const char *vendor = udev_device_get_sysattr_value(dev, "idVendor");
        if (vendor)
                usbattr.vendor = (char*)vendor;
        const char *product = udev_device_get_sysattr_value(dev, "idProduct");
        if (product)
                usbattr.product = (char*)product; 
        const char *product_name = udev_device_get_sysattr_value(dev, "product");
        if (product_name)
                usbattr.product_name = (char*)product_name;
        const char *serial = udev_device_get_sysattr_value(dev, "serial");
        if (serial)
                usbattr.serial = (char*)serial;
        return usbattr;
}

char *get_usb_fingerprint(UsbAttrs usbattrs, char *usb_fingprt) 
{
        sprintf(usb_fingprt, "%s:%s-%s-%s", usbattrs.vendor, usbattrs.product, usbattrs.product_name, usbattrs.serial);
        if (strchr(usb_fingprt, REPLACE_THIS) != NULL)
                replace_in_string(usb_fingprt, REPLACE_THIS, REPLACE_WITH);
        dprintf("usb fingerprint: %s\n", usb_fingprt);
        return usb_fingprt;
}
