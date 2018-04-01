ifeq ($(PREFIX),)
				PREFIX := /usr/local/bin
endif

EXEC: program

default: program

program:
				@if (! ls /root); then echo "[!] You're not a root user. Use sudo command"; echo ""; exit 1; fi
				@if ! dpkg -l | grep libreadline-dev -c >> /dev/null; then apt-get install libreadline-dev; fi
				@gcc main.c -lreadline -o shelly

install:
				@echo "Installing Shelly to $(PREFIX)"
				@echo "Make sure to have needed permissions to this directory"
				@echo ""

				@test ! -f shelly && echo "[!] ERROR: Use 'make' command first!"; echo ""

				@if ! [ -w /usr/local/bin ]; then echo "[!] ERROR: You don't have permission to write in $(PREFIX)"; else cp -f shelly $(PREFIX); echo "Installation success!"; fi


clean: 
				@rm shelly
