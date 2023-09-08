#include <stdio.h>
#include <string.h>

#include "usbs.h"
#include "../canaryusb.h"
#include "../utils/util.h"

UsbAttrs get_usb_attributes(sd_device *dev) 
{
        UsbAttrs usbattr = {"0000", "0000", "no", "no"};

        const char *syspath;
        sd_device_get_syspath(dev, &syspath);
        printf("syspath: %s\n", syspath);

        const char *vendor;
        sd_device_get_sysattr_value(dev, "idVendor", &vendor);
        usbattr.vendor = (char *)vendor;

        const char *product;
        sd_device_get_sysattr_value(dev, "idProduct", &product);
        usbattr.product = (char *)product; 

        const char *product_name;
        sd_device_get_sysattr_value(dev, "product", &product_name);
        usbattr.product_name = (char *)product_name;
        
        const char *serial;
        sd_device_get_sysattr_value(dev, "serial", &serial);
        printf("Tha serial: %s\n", serial);
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

