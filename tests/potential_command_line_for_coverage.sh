find {src/canary/,src/usbs/} -type f -name *.[c] | xargs ctags -x | awk { print } | xargs -I func grep -wc  func tests/canaryusb_test.c
