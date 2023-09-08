#include <stdio.h>
#include <string.h>

#include "usbs.h"
#include "../canaryusb.h"
#include "../utils/util.h"

UsbAttrs get_usb_attributes(sd_device *dev) 
{
        UsbAttrs usbattr = {"0000", "0000", "no", "no", "no-path"};

        int res;
        const char *syspath;
        res = sd_device_get_syspath(dev, &syspath);
        if (res >= 0)
                usbattr.syspath= (char *)syspath;

        const char *vendor;
        res = sd_device_get_sysattr_value(dev, "idVendor", &vendor);
        if (res >= 0)
                usbattr.vendor = (char *)vendor;

        const char *product;
        res = sd_device_get_sysattr_value(dev, "idProduct", &product);
        if (res >= 0)
                usbattr.product = (char *)product; 

        const char *product_name;
        res = sd_device_get_sysattr_value(dev, "product", &product_name);
        if (res >= 0)
                usbattr.product_name = (char *)product_name;
        
        const char *serial;
        res = sd_device_get_sysattr_value(dev, "serial", &serial);
        if (res >= 0)
                usbattr.serial = (char *)serial;
        
        return usbattr;
}

char *get_usb_fingerprint(UsbAttrs usbattrs, char *usb_fingprt) 
{
        dprintf("usbattrs vendor: %s\n", usbattrs.vendor);
        dprintf("usbattrs product: %s\n", usbattrs.product);
        dprintf("usbattrs product name: %s\n", usbattrs.product_name);
        dprintf("usbattrs.serial: %s\n", usbattrs.serial);
        sprintf(usb_fingprt, "%s:%s-%s-%s", usbattrs.vendor, usbattrs.product, usbattrs.product_name, usbattrs.serial);
        if (strchr(usb_fingprt, REPLACE_THIS) != NULL)
                replace_in_string(usb_fingprt, REPLACE_THIS, REPLACE_WITH);
        dprintf("usb fingerprint: %s\n", usb_fingprt);
        return usb_fingprt;
}

