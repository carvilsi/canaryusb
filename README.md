# Canaryusb 

Get a mail notification via, **Canary Tokens (DNS)** when a **USB** device is connected on a GNU/Linux computer.

**Options**:
-c, --canary_token [DNS canary token]
                created token from Canary token site: https://canarytokens.org/generate
                you'll receive the notification based on the created DNS token
                to know more about it check: https://docs.canarytokens.org/guide/dns-token.html
-u, --usb_fingerprint
                this prints the fingerprint related with a USB device that is plugged into computer
                and could be used to create the list for trust_list option.
                In this mode, will not be any call to Canary Tokens, only the usb fingerprint will be printed.
-t, --trust_list [comma separated usb_fingerprint list]
                list of usb fingerprints, comma seprated, to not notify when the related deviced is connected
                check usb_fingerprint option to retrieve device fingerprint for connected USB device

## Build

`$ make`

For debug mode:

`$ make debug`

## Run

`$ ./canaryusb <your DNS canary token>`

Here you can create your [DNS canary token](https://canarytokens.org/generate)

## Stop the daemon

Get the `PID` for canaryusb process and kill it:

`$ ps -ef | grep canaryusb`
`$ kill [PID]`

or just:

`$ kill $(pgrep canaryusb)`

## Notes

- Inspired by [canaryfy](https://github.com/thinkst/canaryfy)
- Only works on GNU/Linux systems, and by now only tested on a 20/22.04.1-Ubuntu 64 and Linux 6.3.2-arch1-1
- Contributions are very welcome.
- Also if you like it, please leave a start I would appreciate it ;)

## Thinks and things

- Exploring possibility of block some rogue devices.
