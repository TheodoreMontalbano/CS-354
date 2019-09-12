////////////////////////////////////////////////////////////////////////////////
// Main File:        intdate.c
// This File:        division.c
// Other Files:      sendsig.c
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
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
// The number of completed success
int count;

/*
 * Checks for division by 0
 * arg sig: the signal being broadcast
 */
void handler_SIGFPE(int sig) {
  printf("Error: a division by 0 operation was attemped.\n");
  printf("Total number of operations completed successfully: %i\n", count);
  printf("The program will be terminated.\n");
  exit(0);
}

/*
 * Checks for program termination by CTRL C
 * arg sig: the signal being broadcast
 */
void  handler_SIGINT(int sig) {
  printf("\nTotal number of operations successfully completed: %i\n", count);
  printf("The program will be terminated.\n");
  exit(0);
}

/*
 * Main method
 */
int main() {
  count = 0;
  // Sets up processes for catching signal 1
  struct sigaction signal1;
  memset(&signal1, 0, sizeof(signal1));
  signal1.sa_handler = handler_SIGFPE;
  if (sigaction(SIGFPE, &signal1, NULL) < 0) {
    perror("Error handling");
    exit(0);
  }
  // Sets up processes for catching signal 2
  struct sigaction signal2;
  memset(&signal2, 0, sizeof(signal2));
  signal2.sa_handler = handler_SIGINT;  
  if (sigaction(SIGINT, &signal2, NULL) < 0) {
    perror("Error handling");
    exit(0);
  }
  // Processes for division
  while (1) {
    char buffer [100];
    // Checks for first integer
    printf("Enter first integer: ");
    fgets(buffer, 100, stdin);
    int firstNum = atoi(buffer);
    
    // Checks for second integer
    printf("Enter second integer: ");
    fgets(buffer, 100, stdin);
    int secondNum = atoi(buffer);

    // Computes division of the two numbers
    int quotient = firstNum / secondNum;
    int remainder = firstNum % secondNum;
    printf("%i / %i is %i with a remainder of %i\n",
        firstNum, secondNum, quotient, remainder);
    count++;
  }
  return (0);
}
