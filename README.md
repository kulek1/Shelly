
# Shelly

Shelly is an own, basic version of shell from Linux.
It's written in C. You can learn how it works!

<p align="center">
  <img src="https://raw.githubusercontent.com/kulek1/ReadmeImages/master/shelly.png?token=AHQNCt5WiWLYa90QWj7ymW35Iyk_mjNdks5aye6jwA%3D%3D" alt="Size Limit example"
       width="654" height="450">
</p>

Write down `help` argument to show supported commands in terminal.


## Built-in commands


|command| description |
|--|--|
| cd [PATH] | change directory |
| cd .. | change directory to parent |
| cd / | change directory to home path |
| cd - | back to previous directory |
| history | display history of typed commands|
| history -n [N] | display the last of N typed commands |
| help | display help of shelly |
| shopt | display all settings of shelly |
| shopt -s [S] | turn on S-name option (It will reload shelly!) |
| shopt -u [S] | turn off S-name option (It will reload shelly!) |
| exit | close shelly immediately |
| exit [N] | close shelly with specific exit code (0-255) |


## Shelly options

Using  `shopt` command you can:

 - toggle compact view
 - toggle colors in shelly
 - toggle colors in `ls`

## Usage

First, compile `shelly`:

```sh
$ make && make install && make clean
```

If you are root user, `shelly` will install to `/usr/local/bin` path giving you possibility to run the program from any directory.

To compile on macOS:

* install readline via Homebrew
* edit makefile or compile manually via terminal;

```@if (! ls /root); then echo "[!] You're not a root user. Use sudo command"; echo ""; exit 1; fi

@if ! dpkg -l | grep libreadline-dev -c >> /dev/null; then apt-get install libreadline-dev; fi

@gcc main.c -lreadline -o shelly
```
