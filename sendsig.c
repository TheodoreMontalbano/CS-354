////////////////////////////////////////////////////////////////////////////////
// Main File:        intdate.c
// This File:        sendsig.c
// Other Files:      division.c
// Semester:         CS 354 Spring 2019
//
// Author:           Theodore Montalbano
// Email:            tmontalbano@wisc.edu
// CS Login:         montalbano
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
//////////////////////////// 80 columns wide ///////////////////////////////////

#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
/**
 * Main - main routine
 */
int main(int argc, char* argv[]) {
  // Checks to see whether the inputs to the
  // program is correct
  if (argc != 3) {
    printf("Improper Inputs\n");
    exit(0);
  }
  // Gets the pid of the process the signal is
  // to be sent to
  pid_t pid = atoi(argv[2]);
  // Which signal to be sent
  int signal = strcmp(argv[1], "-i");
  if (signal > 0) {
    // Checks if signal was SIGUSR1
    if (kill(pid, SIGUSR1) == -1) {
      perror("Kill process has errored");
      exit(0);
    }
  } else if (signal == 0) { 
    // Checks if signal sent was SIGINT
    if (kill(pid, SIGINT) ==-1) {
      perror("Kill process has errored");
      exit(0);
    }
  } else {
    // Checks if signal sent was correct
    perror("Invalid Entry");
    exit(0);
  }
  return 0;
}


