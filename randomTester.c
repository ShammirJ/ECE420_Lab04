/* A program used to create random test cases for main.c 
 *      Usage: ./randomTester lowerBound upperBound numTests
 *      Output: randomTests.log -> An output files that contains the test resutls 
 * 
 * IMPORTANT: Make sure DEBUG is OFF in main
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "timer.h"

#define OUTPUT "randomTests.log"
#define MAX_PROCESS 4

/* Generate a randome number from lowerBound to UpperBound */
int random_number(int lowerBound, int UpperBound) {
    return (rand() % (UpperBound - lowerBound + 1)) + lowerBound;
}

/* Execute terminal command */
void Run_Command(FILE *out, const char *command) {
    char buffer[1024];
    memset(buffer, '\0', 1024);
    FILE *pipe = popen(command, "r");
    
    if (pipe == NULL) {
        fprintf(stderr, "Failed to run command: %s\n", command);
        return;
    }
    
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        fprintf(out, "%s", buffer);  // Write command output to log file
    }
    
    fprintf(out, "\n");
    fflush(out);  // Ensure it is written to disk

    pclose(pipe);
}

int main(int argc, char* argv[]) {
    srand(time(NULL)); // Set a random seed

    if (argc != 4) {
        fprintf(stderr, "USAGE: ./randomTester lowerBound upperBound numTests");
        exit(EXIT_FAILURE);
    }

    char *endptr1, *endptr2, *endptr3;
    long lowerBound, upperBound, numTests;

    lowerBound = strtol(argv[1], &endptr1, 10);
    upperBound = strtol(argv[2], &endptr2, 10);
    numTests = strtol(argv[3], &endptr3, 10);

    if (*endptr1 != '\0' || *endptr2 != '\0' || *endptr3 != '\0') {
        fprintf(stderr, "Invalid arguments\n");
        exit(EXIT_FAILURE);
    }

    if (upperBound  <= 0 || lowerBound <= 0 || numTests <= 0) {
        fprintf(stderr, "Args must be nonzero\n");
        exit(EXIT_FAILURE);
    }

    if (lowerBound > upperBound) {
        long temp;
        temp = lowerBound;
        lowerBound = upperBound;
        upperBound = temp;
    }

    FILE *out = fopen(OUTPUT, "w");
    if (out == NULL) {
        fprintf(stderr, "Could not open \'%s\'\n", OUTPUT);
        exit(EXIT_FAILURE);
    }

    fprintf(out, "%ld test cases, lowerBound = %ld, upperBound = %ld\n\n", numTests, lowerBound, upperBound);

    for (long i = 0; i < numTests; i++) {
        int np = random_number(1, MAX_PROCESS);
        int bound = random_number((int) lowerBound, (int) upperBound);

        printf("Test %ld of %ld\n", i+1, numTests);

        fprintf(out, "\n------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        fprintf(out, "Test %ld: np = %d, bound = %d\n\n", i+1, np, bound);

        fflush(out);

        char datagen[100], execute[100], verify[100];
        memset(datagen, '\0', 100);
        memset(execute, '\0', 100);
        memset(verify, '\0', 100);

        sprintf(datagen, "./datatrim -b %d", bound);
        sprintf(execute, "mpirun -np %d main", np);
        sprintf(verify, "./serialtester");

        Run_Command(out, datagen);
        Run_Command(out, execute);
        Run_Command(out, verify);

        fprintf(out, "\n------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        //fflush(out);
    }

    fclose(out);

    return 0;
}