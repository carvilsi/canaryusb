# Canaryusb 

Sends DNS canary tokens when a USB device is connected on a GNU/Linux system.

## Build

`$ make`

For debug mode:

`$ make debug`

## Run

`$ ./canaryusb <your DNS canary token>`

You can have a [Canary DNS token](https://canarytokens.org/generate) 

## Notes

- Inspired by [canaryfy](https://github.com/thinkst/canaryfy)
- Only works on GNU/Linux systems, and by now only tested on a 20.04.1-Ubuntu 64
- Contributions are very welcome (Pull requests, issues, comments and even a code review)  

## Thinks and things

- Adding a list of trusted USB devices.
- Exploring possibility of block some rogue devices.
- Giving a try to same concept but for macOS.
