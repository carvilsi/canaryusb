CC = gcc
LDFLAGS = `pkg-config --cflags --libs libsystemd`
CCFLAGS = -Werror
DEST_BIN = ~/.local/bin/
DEST_CONF = ~/.config/canaryusb/
SRC_CONF = ./configuration/config.toml
REL_PR =canaryusb_v
REL_SUF =_linux_x86_64
SHASUM_FL = sha256sum.txt

SRC := $(shell find ./src/ -name '*.c') 
NAM := $(shell awk '/_NAME_/ { gsub("\"", "", $$3); print $$3 }' src/canaryusb.h)
VERS := $(shell awk '/_VERSION_/ { gsub("\"", "", $$3); print $$3 }' src/canaryusb.h)
REL := $(REL_PR)$(VERS)$(REL_SUF)

all: cexec 

.PHONY: list
list:
	@LC_ALL=C $(MAKE) -pRrq -f $(firstword $(MAKEFILE_LIST)) : 2>/dev/null | awk -v RS= -F: '/(^|\n)# Files(\n|$$)/,/(^|\n)# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | sort | grep -E -v -e '^[^[:alnum:]]' -e '^$@$$'


debug: CCFLAGS += -DDEBUG -g 
debug: cexec 

# debug and do not call the canary token 
silence: CCFLAGS += -DSILENCE -DDEBUG 
silence: cexec

install: cexec 
	@echo '$(NAM) compiled...'
	@mv $(NAM) $(DEST_BIN)
	@echo '$(NAM) copied at: $(DEST_BIN)...'
	@mkdir -p $(DEST_CONF)
	@-cp -n $(SRC_CONF) $(DEST_CONF) ||:
	@source ~/.bashrc
	@echo '$(NAM) installed'

uninstall:
	@echo 'uninstalling $(NAM)...'
	rm  $(DEST_BIN)$(NAM)
	rm -rf $(DEST_CONF)
	@echo '$(NAM) uninstalled OK'

release: cexec
	@echo '$(NAM) compiled...'
	@mv $(NAM) $(REL) 
	@echo 'releasing $(REL)...'
	@sha256sum $(REL) > $(SHASUM_FL) 
	@echo 'sha256sum file for $(REL) created...'
	@git tag v$(VERS)
	@echo 'git tagged v$(VERS)...'
	@git push origin v$(VERS)
	@echo
	@echo '$(NAM) released OK'

cexec:
	@$(CC) $(CCFLAGS) -o $(NAM) $(SRC) $(LDFLAGS)

clean:
	@-rm $(NAM) ||:

