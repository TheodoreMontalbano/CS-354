////////////////////////////////////////////////////////////////////////////////
// Main File:        intdate.c
// This File:        intdate.c
// Other Files:      sendsig.c, division.c
// Semester:         CS 354 Spring 2019
//
// Author:           Theodore Montalbano
// Email:            tmontalbano@wisc.edu
// CS Login:         montlabano
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
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
int set_time = 3;
int count = 0;

/*
 * Signal Handler of the program
 */
void handler_ALARM(int sig){
  time_t currtime;
  if (time(&currtime) == -1) {
    perror("Time error");
    exit(0);
  }
  // Prints current time and PID of the program.
  printf("PID: %d", getpid());
  printf(" | Current time: %s", ctime(&currtime));
  if (alarm(set_time) != 0) {
    printf("Alarm error.\n");
    exit(0);
  }
}
/*
 * Catches SIGUSR then prints and increments the count.
 */
void handler_SIGUSR(int sig) {
  printf("SIGUSR1 caught!\n");
  count++;
}
/*
 * In order to exit the program if CTRL C is input.
 */
void  handler_SIGINT(int sig) {
  printf("\nSIGINT received.\n");
  printf("SIGUSR1 was received %i times. Exiting now.\n", count);
  exit(0);
}

/* Main function - where all signal Alarms are set up.
 *
 * argc:the number of arguments
 * argv: use the char pointer to store each arguments
 */
int main(int argc, char *argv[]) {
  printf("Pid and time will be printed every 3 seconds.\n");
  printf("Enter ^C to end the program.\n");
  // Sets up signal handling and alarm for while loop
  struct sigaction alarm;
  memset (&alarm, 0, sizeof(alarm));
  alarm.sa_handler = handler_ALARM;
  if (sigaction(SIGALRM, &alarm, NULL) < 0){
    perror("Error handling");
    exit(0);
  }
  if (alarm(set_time) != 0) {
    printf("Alarm error.\n");
    exit(0);
  }
  // Sets up signal for SIGUSR
  struct sigaction signal1;
  memset (&signal1, 0, sizeof(act2));
  signal1.sa_handler = handler_SIGUSR;
  if (sigaction(SIGUSR1, &signal1, NULL) < 0) {
    perror("Error handling");
    exit(0);
  }
  // Sets up signal for SIGINT
  struct sigaction signal2;
  memset (&signal2, 0, sizeof(signal2));
  signal2.sa_handler = handler_SIGINT;
  if (sigaction(SIGINT, &signal2, NULL) < 0) {
    perror("Error handling");
    exit(0);
  }

  while (1) {
  }
  return (0);
}
