# Canaryusb 

Get a mail notification via, **Canary Tokens (DNS)** when a **USB** device is connected on a GNU/Linux computer.
Could be useful when you leave the laptop unattended or for a server on a remote location, will not prevent to being 
breached, but at least you'll notice; this is the principle behind [Thinkst Canary](https://canary.tools/#why).
Here we are thinking about removable media threats like BadUSB or data theft.

**Options**:

**-c, --canary_token [DNS canary token]**
                created token from Canary token site: https://canarytokens.org/generate
                you'll receive the notification based on the created DNS token
                to know more about it check: https://docs.canarytokens.org/guide/dns-token.html

**-u, --usb_fingerprint**
                this prints the fingerprint related with a USB device that is plugged into computer
                and could be used to create the list for trust_list option.
                In this mode, will not be any call to Canary Tokens, only the usb fingerprint will be printed.

**-t, --trust_list [comma separated usb_fingerprint list]**
                list of usb fingerprints, comma seprated, to not notify when the related deviced is connected
                check usb_fingerprint option to retrieve device fingerprint for connected USB device

**Note:**
If any option is not provided the default behaviour is try to retrieve the options from the a config file located at `~/.config/canaryusb/config.toml`.
An example of this configuration file is under `configuration/` directory at the repo.

The canaryusb uses the system logger for logging purposes.

## Build

`$ make`

For debug mode:

`$ make debug`

Special build mode called `silence` allows to edebug without calling canarytokens service:

`$ make silence`

Note that in general you can run `make clean` before any make option, e.g.

`$ make clean; make`

## Run

`$ ./canaryusb [options]`

**Receive a mail for any device that will be connected to USB**

`$ ./canaryusb -c <your DNS canary token>`

**Receive a mail for any device that is not at the trust list and  will be connected to USB**

`$ ./canaryusb -c <your DNS canary token> -t <comma separated list of trusted devices fingerprint>`

**Get the fingerprint of devices for trusted list, do not send any mail**

`$ ./canaryusb -u`


Here you can create your [DNS token](https://canarytokens.org/generate)

## Install

First build it:

`$ make build`

And then:

`$ make install`

This will install the binary at `~/.local/bin/` and the example configuration file at `~/.config/canaryusb/config.toml`

In order to run:

`$ canaryusb [options]`

### Uninstall

If you already installed canaryusb, you can uninstall it with:

`$ make uninstall`

## Examples

**Receive a mail for any device that will be connected to USB**

`$ ./canaryusb -c 555whateverYouGetFrom.canarytokens.com`

**Receive a mail for any device that is not at the trust list and  will be connected to USB**

`$ ./canaryusb -c 5555whateverYouGetFrom.canarytokens.com -t 1af3:0001-ZOWIE_Gaming_mouse-no,594d:604d-YD60MQ-no`

**Get the fingerprint of devices for trusted list, do not send any mail**

`$ ./canaryusb -u`

**Output** when a USB device is connected:

`usb_fingerprint: 1af3:0001-ZOWIE_Gaming_mouse-no`

**Execute installed and use config file**

`$ canaryusb`

## Stop the daemon

`kill $(pgrep canaryusb)`

## Notes

- Inspired by [canaryfy](https://github.com/thinkst/canaryfy)
- Only works on GNU/Linux systems, and by now only tested on a 20/22.04.1-Ubuntu 64 and Linux 6.3.2-arch1-1
- Contributions are very welcome.
- Also if you like it, please leave a start I would appreciate it ;)

### Thinks and things

- Exploring possibility of block some rogue devices.

