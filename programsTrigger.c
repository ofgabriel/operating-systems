// Student: Gabriel de Oliveira da Fonseca
// DRE: 116.049.429
// This essay was developed using MacOS Catalina (10.15.7)

// The program works as a programs "trigger". It receives two commands,
// each one regarding the call of a specific program. These programs
// are then executed upon specific signals.

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define MAX_INPUT_LENGTH 300
#define INPUT_DELIMITER " \n"
#define MAX_TOKENS_NUMBER 10
#define BASE_PATH "/sbin/"

int signalCode = 0;

char *getExecutableFromCommand(char *command);

bool isValidExecutable(char *executablePath);

void signalHandler(int signum);

int main(void) {
	char firstCommand[MAX_INPUT_LENGTH];
	char secondCommand[MAX_INPUT_LENGTH];

	// Prompts user first input.
	printf("Enter the first command to be executed\n");
	fgets(firstCommand, MAX_INPUT_LENGTH, stdin);

	// Parses first user command string adding its tokens to an array.
	// Adapted from https://stackoverflow.com/a/15472359/11998835.
	char *firstCommandTokens[MAX_TOKENS_NUMBER];
	int i = 0;

	firstCommandTokens[i] = strtok(firstCommand, INPUT_DELIMITER);

	while (firstCommandTokens[i] != NULL) {
		firstCommandTokens[++i] = strtok(NULL, INPUT_DELIMITER);

		// Exception thrown when more arguments than supported are given.
		if (i > MAX_TOKENS_NUMBER) {
			printf("Maximum number of %d arguments exceeded.\n", MAX_TOKENS_NUMBER);

			exit(EXIT_FAILURE);
		}
	}

	// Resets iterator for the second command.
	i = 0;

	char *firstExecutablePath = getExecutableFromCommand(firstCommandTokens[0]);

	// Validates the first user command.
	if (!isValidExecutable(firstExecutablePath)) {
		printf("Invalid command \"%s\"\n", firstCommandTokens[0]);

		exit(EXIT_FAILURE);
	}

	// Prompts user second input.
	printf("Enter the second command to be executed\n");
	fgets(secondCommand, MAX_INPUT_LENGTH, stdin);

	// Parses second user command string adding its tokens to an array.
	// Adapted from https://stackoverflow.com/a/15472359/11998835.
	char *secondCommandTokens[MAX_TOKENS_NUMBER];

	secondCommandTokens[i] = strtok(secondCommand, INPUT_DELIMITER);

	while (secondCommandTokens[i] != NULL) {
		secondCommandTokens[++i] = strtok(NULL, INPUT_DELIMITER);

		// Exception thrown when more arguments than supported are given.
		if (i > MAX_TOKENS_NUMBER) {
			printf("Maximum number of %d arguments exceeded.\n", MAX_TOKENS_NUMBER);

			exit(EXIT_FAILURE);
		}
	}

	char *secondExecutablePath = getExecutableFromCommand(secondCommandTokens[0]);

	// Validates the second user command.
	if (!isValidExecutable(secondExecutablePath)) {
		printf("Invalid command \"%s\"\n", secondCommandTokens[0]);

		exit(EXIT_FAILURE);
	}

	// Registers the needed signals handling.
	signal(SIGTERM, signalHandler);
	signal(SIGUSR1, signalHandler);
	signal(SIGUSR2, signalHandler);

	// Makes the program execute indefinitely, following the example presented here:
	// https://drive.google.com/file/d/1EieO-jz3AGPHuaQgZclyYWnjVl8fLV-0/view.
	while (1) {
		sleep(1);

		while (signalCode != 0) {
			// Finishes execution upon SIGTERM.
			if (signalCode == SIGTERM) {
				printf("Finishing commands trigger...\n");

				return 0;
			}

			// Fork is common to SIGUSR1 and SIGUSR2.
			int childPID = fork();

			if (signalCode == SIGUSR1 && childPID == 0) {
				execvp(firstExecutablePath, firstCommandTokens); // Executes first command upon SUGSR1.
			} else if (signalCode == SIGUSR2 && childPID == 0) {
				execvp(secondCommand, secondCommandTokens); // Executes second command upon SUGSR2.
			} else {
				wait(NULL);

				signalCode = 0;
			}
		}
	}

	return 0;
}

// Concatenates the user command with the base path to compose
// the binary absolute path.
char *getExecutableFromCommand(char *command) {
	char *executablePath = malloc(strlen(command) + strlen(BASE_PATH));

	strcpy(executablePath, BASE_PATH);
	strcat(executablePath, command);

	return executablePath;
}

// Checks if the executable exists.
// Adapted from https://stackoverflow.com/a/230068/11998835.
bool isValidExecutable(char *executablePath) {
	if (access(executablePath, X_OK) == 0) {
    	return true;
	}

	return false;
}

// Updates global state according to signal received.
void signalHandler(int signal) {
	signalCode = signal;
}
