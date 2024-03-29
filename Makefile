CC = gcc
LDFLAGS = `pkg-config --cflags --libs libsystemd`
CCFLAGS = -Werror -Wall
DEST_BIN = ~/.local/bin/
DEST_CONF = ~/.config/canaryusb/
SRC_CONF = ./configuration/config.toml
REL_PR =canaryusb_v
REL_SUF =_linux_x86_64
SHASUM_FL = sha256sum.txt
DEST_SERVICE = ~/.config/systemd/user/
SRC_SERVICE = ./configuration/canaryusb.service
NAM_SERVICE = canaryusb.service

SRC := $(shell find ./src/ -name '*.c') 
NAM := $(shell awk '/NAME/ { gsub("\"", "", $$3); print $$3 }' src/canaryusb.h)
VERS := $(shell awk '/VERSION/ { gsub("\"", "", $$3); print $$3 }' src/canaryusb.h)
REL := $(REL_PR)$(VERS)$(REL_SUF)

all: cexec 

.PHONY: list
list:
	@LC_ALL=C $(MAKE) -pRrq -f $(firstword $(MAKEFILE_LIST)) : 2>/dev/null | awk -v RS= -F: '/(^|\n)# Files(\n|$$)/,/(^|\n)# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | sort | grep -E -v -e '^[^[:alnum:]]' -e '^$@$$'


debug: CCFLAGS += -DDEBUG -g 
debug: cexec 

# debug and do not call the canary token 
silence: CCFLAGS += -DSILENCE -DDEBUG -g 
silence: cexec

install: cexec 
	@echo '$(NAM) compiled...'
	@mv $(NAM) $(DEST_BIN)
	@echo '$(NAM) copied at: $(DEST_BIN)...'
	@mkdir -p $(DEST_CONF)
	@-cp -n $(SRC_CONF) $(DEST_CONF) ||:
	@. ~/.bashrc
	@echo '$(NAM) installed'

uninstall:
	@echo 'uninstalling $(NAM)...'
	rm $(DEST_BIN)$(NAM)
	rm -rf $(DEST_CONF)
	@echo '$(NAM) uninstalled OK'

# installs and creates a user service
add_service: install 
	@echo 'going to create the $(NAM_SERVICE)...'
	@mkdir -p $(DEST_SERVICE)
	@-cp -n $(SRC_SERVICE) $(DEST_SERVICE) ||:
	@systemctl --user daemon-reload
	@systemctl --user enable $(NAM_SERVICE) 
	@echo '$(NAM_SERVICE) enabled in order to start it'
	@echo 'first set correct $(NAM) configuration at $(DEST_CONF)$(SRC_CONF) and run:' 
	@echo 'systemctl --user start $(NAM_SERVICE)'

# only removes the user service
remove_service: 
	@echo 'going to remove the service: $(NAM_SERVICE)...'
	@systemctl --user stop $(NAM_SERVICE) 
	systemctl --user disable $(NAM_SERVICE)
	@rm $(DEST_SERVICE)$(NAM_SERVICE)
	@systemctl --user daemon-reload
	@echo '$(NAM_SERVICE) removed'
	@echo 'notice that $(NAM) still installed, to uninstall it (make uninsntall)'

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

# to remove also files generated for releases
clean_all: clean 
	@-rm $(NAM)_* ||: 
	@-rm $(SHASUM_FL) ||:

