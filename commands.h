#include <unistd.h>
#include <stdlib.h> // system
#include "variables.h"
#include <errno.h> // error codes

extern char *args[BUFFER_LEN];
extern char prev[BUFFER_LEN];
extern char history[MAX_HISTORY][MAX_LENGTH_OF_STRING];
extern char *text_settings[];
extern char *argv[5];

extern int default_settings[];
extern int h_pos;

void getCurrentDirectory();
void setPreviousDirectory();
void detectCommand(char *customLine);


// CD COMMAND
void fireCd(char *dir) {
  if (args[1] == NULL && !strcmp(args[1], "~") || !strcmp(args[1], "$HOME")) {
    // go to home directory
    chdir(getenv("HOME"));
  } else if (!strcmp(args[1], "-")) {
    chdir(prev);
  } else {
    // success - changing directory
		setPreviousDirectory();
    if (chdir(args[1]) != 0) {
      perror(args[1]);
    };
  }
    getCurrentDirectory();
}

// HELP COMMAND
void fireHelp() {
  printf("\n[=========== SHELLY ===========]\n");
  printf("Author: Mateusz Kulinski, version: 0.1\n");
  printf("List of basic commands implemented in shelly:\n\n");
  printf("%-30s %-30s\n", "cd [PATH]", "change directory");
  printf("%-30s %-30s\n", "cd ..", "change to parent");
  printf("%-30s %-30s\n", "cd /", "change directory to home path");
  printf("%-30s %-30s\n", "cd -", "back to previous directory");
  printf("%-30s %-30s\n", "history", "display history of typed commands");
  printf("%-30s %-30s\n", "history -n [N]",
         "display the last of N typed commands");
  printf("%-30s %-30s\n", "help", "display help of shelly");
  printf("%-30s %-30s\n", "shopt", "display all settings of shelly");
  printf("%-30s %-30s\n", "shopt -s [S]", "turn on S-name option (It will reload shelly!)");
  printf("%-30s %-30s\n", "shopt -u [S]", "turn off S-name option (It will reload shelly!)");
  printf("%-30s %-30s\n", "exit", "close shelly immediately");
  printf("%-30s %-30s\n", "exit [N]",
         "close shelly with specific exit code (0-255)");
}

void fireHistoryCommand(int pos) {
  detectCommand(history[pos]);
}

// HISTORY COMMAND
void fireHistory() {
  int helper = 0;

  if (args[1] != NULL && !strcmp(args[1], "-n")) {
    // catch exception when user pass parametr without number
    if (args[2] == NULL) {
      printf("Incorrect param - try again\n");
      return;
    }
    int n = atoi(args[2]);
    if (n > 0 && n < 101) {
      helper = h_pos - n - 1;
    } else {
      printf("Incorrect number - try again\n");
      return;
    }
  } else if (args[1] != NULL) {
    int x = atoi(args[1]);
    if (x > 0 && x < 101) {
      fireHistoryCommand(x - 1);
    } else {
      printf("Incorrect number - try again\n");
    }
    return;
  }
  printf("\n[=========== SHELLY HISTORY ===========]\n");
  for (int i = helper; i < h_pos - 1; i++) {
    printf("%s%d%s%s\n", "[=> ", i + 1, " ", history[i]);
  }
  printf("\n");
}

void displaySettingError() {
	printf("Shelly: invalid shell option name\n");
}	

// return pos of element in array (Error code: -1)
int findSetting(char *setting_name) {
	int i = 0;
	while (text_settings[i] != NULL) {
		if (!strcmp(text_settings[i], setting_name)) {
			return i;
		}
		i++;
	}
	return -1;
}

// display all setings of shelly
void displayShoptSettings() {
	char *isTurnOn;
	int length = sizeof(*text_settings)/sizeof(int);

	for (int i = 0; i <= length; i++) {
		isTurnOn = default_settings[i] == 1 ? "on" : "off";
		printf("%-20s %-20s\n", text_settings[i], isTurnOn);
	}
}

void fireShoptOption(int pos, int isTurnOn) {
	default_settings[pos] = isTurnOn;
	char temp[sizeof(int)*3+2];
	// generate char copy array of shopt settings
	int length = sizeof(argv)/sizeof(argv[0]);
	for (int i = 1; i < length - 1; i++) {
		// tricky part - convert each number from array to char then char to array of char
		snprintf(temp, sizeof temp, "%d", default_settings[i-1]);
		argv[i] = malloc(1*sizeof(char));
		strcat(argv[i], (char *) temp);
	}
	// NULL is necessary for execv()
	argv[length+1] = NULL;

	printf("\033c");
	// RESTART PROGRAM
	if (execv(argv[0], argv) == -1) {
		execvp(argv[0], argv);
	};
}

// SHOPT COMMAND
void fireShopt() {
	int pos;
	if (args[1] == NULL) { 
		displayShoptSettings(); 
	} else if (!strcmp(args[1], "-s") && args[2] != NULL) {
		pos = findSetting(args[2]);
		pos >= 0 ? fireShoptOption(pos, 1) : displaySettingError();
	} else if (!strcmp(args[1], "-u") && args[2] != NULL) {
		pos = findSetting(args[2]);
		pos >= 0 ? fireShoptOption(pos, 0) : displaySettingError();
	} else {
		displayShoptSettings();
	}
}

// EXIT COMMAND
void fireExit() {
  if (args[1]) {
    int code = atoi(args[1]);
    if (code > 0 && code < 255) {
			// exit immediately
      exit(code);
    } else {
      printf("Incorrect error status - try again\n");
    }
  } else {
    exit(0);
  }
}
