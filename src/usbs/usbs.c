#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
        SDCardAttrs sdcrd_attrs = {"noname", "noserial", "nosize", 
                "noblocksizenorparttabletype", "no-path"};

        int res;
        const char *syspath;
        res = sd_device_get_syspath(dev, &syspath);
        if (res >= 0)
                sdcrd_attrs.syspath = (char *)syspath;

        const char *name;
        res = sd_device_get_property_value(dev, "ID_NAME", &name);
        if (res >= 0)
                sdcrd_attrs.id_name = (char *)name;

        //XXX: Trying to maintain the backward compatibility related with ID_SERIAL
        //     but not shure about this... I think UUID is better
        const char *uuid;
        res = sd_device_get_property_value(dev, "ID_SERIAL", &uuid);
        if (res >= 0) {
                sdcrd_attrs.uuid = (char *)uuid; 
        } else {
                res = sd_device_get_property_value(dev, "ID_FS_UUID", &uuid);
                if (res >= 0) {
                        sdcrd_attrs.uuid = (char *)uuid; 
                } else {
                        res = sd_device_get_property_value(dev, "ID_PART_TABLE_UUID", &uuid);
                        if (res >= 0)
                                sdcrd_attrs.uuid = (char *)uuid;
                }
        }

        const char *size;
        res = sd_device_get_sysattr_value(dev, "size", &size);
        if (res >= 0)
                sdcrd_attrs.size = (char *)size;
        
        const char *blcksz_prtbltype;
        res = sd_device_get_property_value(dev, "ID_FS_BLOCKSIZE", &blcksz_prtbltype);
        if (res >= 0)
                sdcrd_attrs.blcksz_prtbltype = (char *)blcksz_prtbltype;
        else if (sd_device_get_property_value(dev, "ID_PART_TABLE_TYPE:", 
                                &blcksz_prtbltype) >= 0)
                sdcrd_attrs.blcksz_prtbltype = (char *)blcksz_prtbltype;
        
        return sdcrd_attrs;
}

void get_usb_fingerprint(UsbAttrs usb_attrs, char *usb_fingprt) 
{
        dprintf("\n-- USB device connected --\n");
        dprintf("\tattributes vendor: %s\n", usb_attrs.vendor);
        dprintf("\tattributes product: %s\n", usb_attrs.product);
        dprintf("\tattributes product name: %s\n", usb_attrs.product_name);
        dprintf("\tattributes serial: %s\n", usb_attrs.serial);
        dprintf("\tat: %s\n\n", usb_attrs.syspath);

        sprintf(usb_fingprt, "%s:%s-%s-%s", usb_attrs.vendor, usb_attrs.product, 
                        usb_attrs.product_name, usb_attrs.serial);
        if (strchr(usb_fingprt, REPLACE_THIS) != NULL)
                replace_in_string(usb_fingprt, REPLACE_THIS, REPLACE_WITH);
        dprintf("usb fingerprint: %s\n", usb_fingprt);
}

void get_sdcard_fingerprint(SDCardAttrs sdcrd_attrs, char *sdcrd_fingprt) 
{
        dprintf("\n-- SDCard connected --\n");
        dprintf("\tid_name: %s\n", sdcrd_attrs.id_name);
        dprintf("\tid_serial: %s\n", sdcrd_attrs.uuid);
        dprintf("\tsize: %s\n", sdcrd_attrs.size);
        dprintf("\tblock size or part table type: %s\n", sdcrd_attrs.blcksz_prtbltype);
        dprintf("\tat: %s\n\n", sdcrd_attrs.syspath);
        
        sprintf(sdcrd_fingprt, "%s:%s-%s:%s", sdcrd_attrs.id_name, sdcrd_attrs.uuid, 
                        sdcrd_attrs.size, sdcrd_attrs.blcksz_prtbltype);
        if (strchr(sdcrd_fingprt, REPLACE_THIS) != NULL)
                replace_in_string(sdcrd_fingprt, REPLACE_THIS, REPLACE_WITH);
        dprintf("sdcard fingerprint: %s\n", sdcrd_fingprt);
}

char *get_device_fingerprint(sd_device *dev, const char *subsystem)
{
        char *fngrprnt;
        if (strcmp(USB_SUBSYSTEM, subsystem) == 0) {
                UsbAttrs usb_attrs = get_usb_attributes(dev);
                size_t usb_fngrp_len = strlen(usb_attrs.vendor) + 
                        strlen(usb_attrs.product) + 
                        strlen(usb_attrs.product_name) + 
                        strlen(usb_attrs.serial) + 5;
                fngrprnt = (char*) malloc(usb_fngrp_len);
                check_memory_allocation(fngrprnt);
                get_usb_fingerprint(usb_attrs, fngrprnt);
        } 
 
        if (strcmp(SDCARD_SUBSYSTEM, subsystem) == 0) {
                SDCardAttrs sdcrd_attrs = get_sdcard_attributes(dev);
                size_t sdcrd_fngrp_len = strlen(sdcrd_attrs.id_name) +
                        strlen(sdcrd_attrs.uuid) +
                        strlen(sdcrd_attrs.size) +
                        strlen(sdcrd_attrs.blcksz_prtbltype) + 5;
                fngrprnt = (char*) malloc(sdcrd_fngrp_len);
                check_memory_allocation(fngrprnt);
                get_sdcard_fingerprint(sdcrd_attrs, fngrprnt);
        }

        return fngrprnt;
}

