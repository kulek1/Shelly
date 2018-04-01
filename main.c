#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <sys/types.h>

#include "commands.h"
#include "helpers.h"

int main(int argc, char **argv) {
	getArgv(argv);
  welcomeText();
  // Run command loop
  getInput();

  return 0;
}
