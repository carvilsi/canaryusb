#include <stdio.h>
#include <string.h>

#include "usbs.h"
#include "../canaryusb.h"
#include "../utils/util.h"

UsbAttrs get_usb_attributes(sd_device *dev) 
{
        UsbAttrs usb_attr = {"0000", "0000", "no", "no", "no-path"};

        int res;
        const char *syspath;
        res = sd_device_get_syspath(dev, &syspath);
        if (res >= 0)
                usb_attr.syspath = (char *)syspath;

        const char *vendor;
        res = sd_device_get_sysattr_value(dev, "idVendor", &vendor);
        if (res >= 0)
                usb_attr.vendor = (char *)vendor;

        const char *product;
        res = sd_device_get_sysattr_value(dev, "idProduct", &product);
        if (res >= 0)
                usb_attr.product = (char *)product; 

        const char *product_name;
        res = sd_device_get_sysattr_value(dev, "product", &product_name);
        if (res >= 0)
                usb_attr.product_name = (char *)product_name;
        
        const char *serial;
        res = sd_device_get_sysattr_value(dev, "serial", &serial);
        if (res >= 0)
                usb_attr.serial = (char *)serial;
        
        return usb_attr;
}

SDCardAttrs get_sdcard_attributes(sd_device *dev) 
{
        SDCardAttrs sdcrd_attrs = {"noname", "noserial", "nosize", "noblocksizenorparttabletype", "no-path"};

        int res;
        const char *syspath;
        res = sd_device_get_syspath(dev, &syspath);
        if (res >= 0)
                sdcrd_attrs.syspath = (char *)syspath;

        const char *name;
        res = sd_device_get_sysattr_value(dev, "ID_NAME", &name);
        if (res >= 0)
                sdcrd_attr.id_name= (char *)name;

        const char *serial;
        res = sd_device_get_sysattr_value(dev, "ID_SERIAL", &serial);
        if (res >= 0)
                sdcrd_attr.id_serial= (char *)serial; 

        const char *size;
        res = sd_device_get_sysattr_value(dev, "size", &size);
        if (res >= 0)
                sdcrd_attr.size= (char *)size;
        
        const char *blcksz_prtbltype;
        res = sd_device_get_sysattr_value(dev, "ID_FS_BLOCKSIZE", &blcksz_prtbltype);
        if (res >= 0)
                sdcrd_attr.blcksz_prtbltype = (char *)blcksz_prtbltype;
        else if (sd_device_get_sysattr_value(dev, "ID_PART_TABLE_TYPE:", &blcksz_prtbltype) >= 0)
                sdcrd_attr.blcksz_prtbltype = (char *)blcksz_prtbltype;
        
        return sdcrd_attr;
}

char *get_usb_fingerprint(UsbAttrs usb_attrs, char *usb_fingprt) 
{
        dprintf("-- USB device connected --\n");
        dprintf("\tattributes vendor: %s\n", usb_attrs.vendor);
        dprintf("\tattributes product: %s\n", usb_attrs.product);
        dprintf("\tattributes product name: %s\n", usb_attrs.product_name);
        dprintf("\tattributes serial: %s\n", usb_attrs.serial);
        dprintf("\tat: %s\n\n", usb_attrs.syspath);

        sprintf(usb_fingprt, "%s:%s-%s-%s", usb_attrs.vendor, usb_attrs.product, usb_attrs.product_name, usb_attrs.serial);
        if (strchr(usb_fingprt, REPLACE_THIS) != NULL)
                replace_in_string(usb_fingprt, REPLACE_THIS, REPLACE_WITH);
        dprintf("usb fingerprint: %s\n", usb_fingprt);
        return usb_fingprt;
}

char *get_sdcard_fingerprint(SDCardAttrs sdcrd_attrs, char *sdcrd_fingprt) 
{
        dprintf("-- SDCard connected --\n");
        dprintf("\tid_name: %s\n", sdcrd_attrs.id_name);
        dprintf("\tid_serial: %s\n", sdcrd_attrs.id_serial);
        dprintf("\tsize: %s\n", sdcrd_attrs.size);
        dprintf("\tblock size or part table type: %s\n", sdcrd_attrs.blcksz_prtbltype);
        dprintf("\tat: %s\n\n", sdcrd_attrs.syspath);
        
        sprintf(sdcrd_fingprt, "%s:%s-%s:%s", sdcrd_attrs.id_name, sdcrd_attrs.id_serial, sdcrd_attrs.size, sdcrd_attrs.blcksz_prtbltype);
        if (strchr(sdcrd_fingprt, REPLACE_THIS) != NULL)
                replace_in_string(sdcrd_fingprt, REPLACE_THIS, REPLACE_WITH);
        dprintf("sdcard fingerprint: %s\n", sdcrd_fingprt);
        return sdcrd_fingprt;
}

