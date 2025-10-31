<div align="center">

<h1>Canaryusb</h1> 

<img src="https://img.shields.io/github/actions/workflow/status/carvilsi/canaryusb/c.yml?logo=github&label=tests" alt="test">
<img src="https://img.shields.io/github/v/release/carvilsi/canaryusb?maxAge=3600&logo=github" alt="release">

Get a mail notification via, **Canary Tokens (DNS)** when a **USB** or **SDCard** device is connected on a Linux computer.
Could be useful when you leave the laptop unattended or for a server on a remote location, will not prevent to being 
breached, but at least you'll notice; this is the principle behind [Thinkst Canary](https://canary.tools/#why).
Here we are thinking about removable media threats like BadUSB or physical attacks to extract data.

Also it is possible to de-authorize via [kernel](https://www.kernel.org/doc/html/v5.15/usb/authorization.html) an USB that is not present on trusted devices list.

</div>

---

1. [Command Line Options](#command-line-options)
2. [Buid](#build)
    1. [Dependencies](#dependencies)
3. [Run](#run)
4. [Install](#install)
    1. [Uninstall](#uninstall)
    2. [Install from binary](#install-from-binary)
5. [Install as a service](#install-as-a-service)
    1. [Remove the service](#remove-the-service)
    2. [Install and run service as root (de-authorize mode)](#service-as-root) /> 
6. [Tests](#tests)
7. [Examples](#examples)
8. [Stop the daemon](#stop-the-daemon)
9. [Logs](#logs)
10. [Notes](#notes)

---

## Command Line Options<a name="commad-line-options" />

**Options**:

**-v, --version**
                this prints the canaryusb version

**-c, --canary-token [DNS canary token]**
                created token from Canary token site: https://canarytokens.org/generate
                you'll receive the notification based on the created DNS token
                to know more about it check: https://docs.canarytokens.org/guide/dns-token.html

**-f, --fingerprint-device**
                this prints the fingerprint related with a USB or SDCard device that is plugged into computer
                and could be used to create the list for trust_list option.
                In this mode, will not be any call to Canary Tokens, only the usb fingerprint will be printed.
                Also will not start a daemon, to quit **ctrl+c**.

**-u, --usb-monitor**
                will only monitor USB devices.

**-s, --sdcard-monitor**
                will only monitor SDCard devices.
                Note: that the USB storage devices will be also monitored.

**-t, --trust-list [comma separated usb_fingerprint list]**
                list of usb fingerprints, comma seprated, to not notify when the related deviced is connected
                check usb_fingerprint option to retrieve device fingerprint for connected USB device.

-d, --de-authorize-device
                de-authorize a connected device not present on the trust list
                requires to be executed as sudoer (root).

**-k, --kill**
                kills the daemon, if it's running.

**Note:**
If any option is not provided the default behaviour is try to retrieve the options from the a config file located at `~/.config/canaryusb/config.toml`.
An example of this configuration file is under `configuration/` directory at the repo.

The canaryusb uses the system logger for logging purposes.

## Build

`$ make`

For debug mode:

`$ make debug`

Special build mode called `silence` allows to debug without calling canarytokens service:

`$ make silence`

Note that in general you can run `make clean` before any make option, e.g.

`$ make clean; make`

### Dependencies

In general you'll need:

* gcc
* make
* pkg-config

It's quite possible that on **Ubuntu** you'll need to install the `libsystem-dev` package. At least was my case for **Ubuntu 23.04** (lunar)

`sudo apt install libsystemd-dev`

Seems that on **Arch Linux** is already installed.

## Run

`$ ./canaryusb [options]`

**Receive a mail for any device that will be connected to USB or SDCard slot**

`$ ./canaryusb -c <your DNS canary token>`

**Receive a mail for any device that is not at the trust list and  will be connected to USB**

`$ ./canaryusb -c <your DNS canary token> -t <comma separated list of trusted devices fingerprint>`

**Get the fingerprint of devices for trusted list, does not send any mail notification**

`$ ./canaryusb -f`

**Will monitor only USB devices**

`$ ./canaryusb -u`

**Will monitor only SDCard, note that USB with storage capabilities will be also detected**

`$ ./canaryusb -s`

**USB devices not present on trusted list will be de-authorizated, requires root**

`$ ./canaryusb -d`


Here you can create your [DNS token](https://canarytokens.org/generate)

## Install

`$ make clean; make install`

This will install the binary at `~/.local/bin/` and the example configuration file at `~/.config/canaryusb/config.toml`

In order to run:

`$ canaryusb [options]`

### Uninstall

If you already installed canaryusb, you can uninstall it with:

`$ make uninstall`

### Install from binary<a name="install-from-binary" />

You can download the binary file at [repo releases](https://github.com/carvilsi/canaryusb/releases)
Notice that if you want to run *canaryusb* with config file, it will expect that this is at: 

`~/.config/canaryusb/config.toml`. 

I guess that be able to config with cli params the location for the config file would it be something to add (TODO). 
Also the *make* commands like *uninstall, add_service* expects that the binary is at `~/.local/bin/`, change this is just edit the binary location of *canaryusb.service* under *configuration* directory though.

## Install as a service<a name="install-as-a-service" />

`$ make clean; make add_service`

This will install canaryusb and enable a service for the **current user**
If is not an update you'll need to set the right configuration at `~/.config/canaryusb/config.toml` before start the service:

`$ systemctl --user start canaryusb.service` 

### Remove the service<a name="remove-the-service" />

`$ make remove_service`

Will remove the canaryusb service, this will **not** uninstall canaryusb.

### Install and run service as root (de-authorize mode)<a name="service-as-root" /> 

You'll require to run canaryusb as a sudoer (root) in order to enable the de-authorize feature, any USB device not listed on trusted devices list, appart to trigger the CanaryToken will de-authorize the device on the system, meaning that will not possible to use it. Check [this](https://www.kernel.org/doc/html/v5.15/usb/authorization.html) to know how it works behind the scene.

Since We do not like to deal with your system outside the user space there is not a make rule to do this; you'll need to **do it manually**:

- Compile `$ make clean; make` or download the binary file at [repo releases](https://github.com/carvilsi/canaryusb/releases)
- Copy the binary to your `/root/.local/bin/` folder (create it if not exists).
- Copy the [configuration file](https://github.com/carvilsi/canaryusb/blob/main/configuration/config.toml) to `/root/.config/canaryusb` folder and for the de-authorization feature to be anable check that `deauth_devices` configuration is set to **true**.
- Copy the [Sevice Unit File](https://github.com/carvilsi/canaryusb/blob/main/configuration/canaryusb.service) to `/etc/systemd/system/` folder and check that **line 14 is uncommented** (User=root) under `[Service]` config section.
- Reload the services: `$ sudo systemctl daemon-reload`
- Start the service: `$ sudo systemctl start canaryusb`
- Check the status: `$ sudo systemctl status canaryusb`

## Tests

The tests should be run from the `tests/` directory.

`$ cd tests/`
`$ make clean; make test`

## Examples

* Receive a mail for any device that will be connected to USB

`$ ./canaryusb -c 555whateverYouGetFrom.canarytokens.com`

* Receive a mail for any device that is not at the trust list and  will be connected to USB

`$ ./canaryusb -c 5555whateverYouGetFrom.canarytokens.com -t 1af3:0001-ZOWIE_Gaming_mouse-no,594d:604d-YD60MQ-no`

* Kill the daemon (if it's already running)

`$ ./canaryusb -k`

* Get the fingerprint of devices for trusted list, do not send any mail

`$ ./canaryusb -f`

**Output** when a USB device is connected:

`usb_fingerprint: 1af3:0001-ZOWIE_Gaming_mouse-no`

* Execute installed and use config file

`$ canaryusb`

## Stop the daemon<a name="stop-the-daemon" />

If **installed**:

`$ canaryusb -k`

If is a **service**:

`$ systemctl --user stop canaryusb.service`

If **running from src**:

`$ ./canaryusb -k`

## Logs<a name="logs" />

**canaryusb** uses the *system logger* so you can check these by:

- `$ journalctl -t canaryusb`

In case that is runing as a service also possible to:

- `$ journalctl --user`

## Notes

- Inspired by [canaryfy](https://github.com/thinkst/canaryfy)
- Only works on Linux systems, and by now only tested on a 23.04 Ubuntu and Arch Linux.

---

Feedback from usage and contributions are very welcome.
Also if you like it, please leave a :star: I would appreciate it ;)

<3 & Hack the Planet!

