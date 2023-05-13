# Canaryusb 

Get a mail notification via, Canary Tokens (DNS) when a USB device is connected on a GNU/Linux computer.

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


## Notes

- Inspired by [canaryfy](https://github.com/thinkst/canaryfy)
- Only works on GNU/Linux systems, and by now only tested on a 20.04.1-Ubuntu 64
- Contributions are very welcome (Pull requests, issues, comments and even a code review)  
- Also if you like it, please leave a start I would appreciate it ;)

## Thinks and things

- Exploring possibility of block some rogue devices.
