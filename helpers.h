#include "variables.h"
#include <sys/types.h>
#include <sys/wait.h> // waitpid
#include <unistd.h>   // getcwd(), atoi()
#include <errno.h> // to catch errors from execvp
#include <readline/readline.h>
#include <readline/history.h>

// arguments from main
char *argv[5];
// arguments in shelly
char *args[BUFFER_LEN];
char cwd[BUFFER_LEN];
char colored_path[BUFFER_LEN];
char history[MAX_HISTORY][MAX_LENGTH_OF_STRING];
char prev[BUFFER_LEN];
char c_directory[BUFFER_LEN];
char *commands[] = {"cd", "help", "history", "shopt", "exit"};
char *text_settings[] = {"compactview", "colors", "lscolors"};

int default_settings[] = { 0, 1, 1 };
int h_pos = 0;

void launchSomething();
void getCurrentDirectory() { getcwd(cwd, sizeof(cwd)); }
void setPreviousDirectory() { getcwd(prev, sizeof(prev)); }

void getArgv(char **main_args) {
	int i = 0;
	while(main_args[i] != NULL) {	
		argv[i] = main_args[i];
		i++;
	}
	// store settings when shelly is running with params
	if (i > 1) {
		int num_settings = 3;
		for (int i = 1; i < num_settings + 1; i++) {
			if (argv[i] == NULL) { break; }
			default_settings[i - 1] = atoi(argv[i]);
		}
	}
}

void splitArgs(char arguments[]) {
	// buffer_1 - quotes, buffer_2 - single words
  char *buffer_1, *buffer_2, *end_s1, *end_s2;
  int i = 0, isQuote = 0;

	// split string to two parts (before & after quotes)
  buffer_1 = strtok_r(arguments, "\"", &end_s1); // end_s1 
  while (buffer_1 != NULL) {
		// block for quotes
		if (isQuote) {
			// allocate memory
		  args[i] = malloc(strlen(buffer_1));
		  strcpy(args[i], buffer_1);
		  i++;

			buffer_1 = strtok_r(NULL, "\"", &end_s1);

			isQuote = 0;
			continue;
		}
		buffer_2 = strtok_r(buffer_1, " ", &end_s2);
		while (buffer_2 != NULL) {
			// allocate memory
		  args[i] = malloc(strlen(buffer_2));
		  strcpy(args[i], buffer_2);
		  i++;	
		
			buffer_2 = strtok_r(NULL, " ", &end_s2);
		}
		buffer_1 = strtok_r(NULL, "\"", &end_s1);
		isQuote = 1;
  }
}

void welcomeText() {
	printf("\033c");
	// isCompactView
	if (argv[1] != NULL && !strcmp(argv[1], "1")) { return; }

	int isNoColors = 0;
	if (argv[2] != NULL && !strcmp(argv[2], "0")) { isNoColors = 1; }	

  char *username = getenv("USER");
  char *is_root = (geteuid() == 0) ? "YES" : "NO";
  int max_width = 20;

  printf("[=========== ");
 	printf(C_YELLOW);
	if (isNoColors) { printf(C_RESET); }
	printf("SHELLY" C_RESET " ===========]\n");
  printf("[=> USER: " C_GREEN);
	if (isNoColors) { printf(C_RESET); }
  printf("%*s", max_width, username);
	printf(C_RESET);
  printf(" ]\n");
  printf("[=> ROOT: ");
  printf("%*s", max_width, is_root);
  printf(" ]\n[______________________________]\n\n");
}

void pushHistory(char **command) {
  strcpy(history[h_pos], *command);
  h_pos++;
}

void flushArgs() { memset(&args[0], 0, sizeof(args)); }

void detectCommand(char *customLine) {
  if (customLine != NULL) {
    splitArgs(customLine);
  }
  // cd command
  if (!strcmp(args[0], commands[0])) {
    fireCd(args[1]);
    return;
  } else if (!strcmp(args[0], commands[1])) {
    fireHelp();
    return;
  } else if (!strcmp(args[0], commands[2])) {
    fireHistory();
    return;
	} else if (!strcmp(args[0], commands[3])) {
		fireShopt();
		return;
  } else if (!strcmp(args[0], commands[4])) {
    fireExit();
		return;
  } else {
    launchSomething();
  }
  return;
}

void formatPath() {
	memset(c_directory, 0, BUFFER_LEN);
	c_directory[0] = '[';
	strcat(c_directory, cwd);

	int length = strlen(c_directory);
	c_directory[length] = ']';
}

void getInput() {
  char *line;
  char *temp[BUFFER_LEN];

  while (1) {
    flushArgs();
    getCurrentDirectory();
		formatPath();
		
		sprintf(colored_path, "%s%s %s", C_BLUE, c_directory, C_RESET);
		// isNoColors
		if (argv[2] != NULL && !strcmp(argv[2], "0")) { sprintf(colored_path, "%s ", c_directory); }
	
		line = readline(colored_path);
		if (line[0] != 0) {
			add_history(line);
		}

    *temp = line;
    // save line to history
    pushHistory(temp);
    splitArgs(*temp);

    detectCommand(NULL);
  }
}

// fire ls with default param --color=auto
void lsColorHelper() {
	// isLsColored
	if (argv[3] != NULL && !strcmp(argv[3], "0")) { return; }	
	if(!strcmp(args[0], "ls") && args[1] == NULL) {
		execlp(args[0], args[0], "--color=auto", NULL);
	}
	return;
}

void launchSomething() {
  int status;
  pid_t pid, wait_pid;
  // create new process
  pid = fork();
  // child process
  if (pid == 0) {
		lsColorHelper();
		// catch error 
    if (execvp(args[0], args) == -1) {
			// file/directory error
			if (strstr(args[0], "/")) {
				printf("shelly: %s - %s\n", args[0], strerror(errno));
				return;
			}
			// command error
			printf("shelly: %s - command not found\n", args[0]);
			exit(1);
    }
  } else if (pid == -1) {
    printf("new process error\n");
  }
  // parent process
  else {
    do {
      // wait to execute
      wait_pid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return;
}
