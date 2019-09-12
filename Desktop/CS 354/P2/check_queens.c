////////////////////////////////////////////////////////////////////////////////
// Main File:        Check Queens
// This File:        Check Queens
// Other Files:      None
// Semester:         CS 354 SPRING 2019
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
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h> 
#include <stdlib.h>  
#include <string.h>   
 
char *COMMA = ",";
  
/* COMPLETED:
 * Retrieves from the first line of the input file,
 * the number of rows and columns for the board.
 * 
 * fp: file pointer for input file
 * rows: pointer to number of rows
 * cols: pointer to number of columns
 */
void get_dimensions(FILE *fp, int *rows, int *cols) {  
        char *line = NULL;  
        size_t len = 0;  
        if (getline(&line, &len, fp) == -1) {  
                printf("Error in reading the file\n");  
                exit(1);  
        }  
           
        char *token = NULL; 
        token = strtok(line, COMMA);
        *rows = atoi(token); 
           
        token = strtok(NULL, COMMA); 
        *cols = atoi(token);
}      


/* 
 * Returns 1 if and only if there exists at least one pair
 * of queens that can attack each other.
 * Otherwise returns 0.
 * 
 * board: heap allocated 2D board
 * rows: number of rows
 * cols: number of columns
 */
int check_queens(int **board, int rows, int cols) {
      // Counter for counting the number of queens on the board.	
	int count = 0;
	// Counts the number of queens on the board.
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			if( *(*(board + i) + j) == 1){
				count = count + 1;
			}
		}
	}
	// List of the XPositions of the Queens.
	int* xPos = malloc(sizeof(int) * count);
	// List of the YPositions of Queens.
	int* yPos = malloc(sizeof(int) * count);
	// Keeps track of the index of the two above arrays in the next for-loop.
	int c = 0;
	// Inputs each queens X and Y position into their respective arrays.
	for(int i = 0; i < rows; i++){
                for(int j = 0; j < cols; j++){
                        if( *(*(board + i) + j)==1){
                               xPos[c] = j;
			       yPos[c] = i;
			       c = c + 1;
                        }
                }
	}
	// Checks for every queen if that queen is in range of another queen.
	for(int i = 0; i < count; i++){
                for(int j = i + 1; j < count; j++){
			// If two Queens have the same XPosition they are on the same column.
			// If two Queens have the same YPosition they are on the same row.
			// In either case they are in range of each other.
                        if(xPos[i] == xPos[j] || yPos[j] == yPos[i]){
                              return 1;
                        }
			// If the absolute value of the difference of two Queens X and Y positions
			// is equivalent then they are on the same diagonal, hence in range.
			if(abs(xPos[i]-xPos[j]) == abs(yPos[i] - yPos[j])){
				return 1;
			}	
                }
        }
	// Frees both arrays.
	free(xPos);
	xPos = NULL;
	free(yPos);
	yPos = NULL;
	// This statement will only run if no two Queens are in range of each other, hence the
	// program should return 0 by specification.
        return 0;
}     


/* PARTIALLY COMPLETED:
 * This program prints true if the input file has any pair
 * of queens that can attack each other, and false otherwise
 * 
 * argc: CLA count
 * argv: CLA value 
 */
int main(int argc, char *argv[]) {        

        //Check if number of command-line arguments is correct.
       if (argc != 2){
		printf("Error: Number of arguments does match predicted number of arguments\n");
		exit(1);
	} 

        //Open the file and check if it opened successfully.
        FILE *fp = fopen(*(argv + 1), "r");
        if (fp == NULL) {
                printf("Cannot open file for reading\n");
                exit(1);
        }


        //Declare local variables.
        int rows, cols;


        //Call get_dimensions to retrieve the board dimensions.
	get_dimensions(fp, &rows, &cols);

        //Dynamically allocate a 2D array of dimensions retrieved above.
       int** board = malloc(sizeof(int) * rows);
       for(int i = 0; i < rows; i++){
	      *(board + i) = malloc(sizeof(int) * cols);
	}
        //Read the file line by line.
        //Tokenize each line wrt comma to store the values in your 2D array.
        char *line = NULL;
        size_t len = 0;
        char *token = NULL;
        for (int i = 0; i < rows; i++) {

                if (getline(&line, &len, fp) == -1) {
                        printf("Error while reading the file\n");
                        exit(1);        
                }

                token = strtok(line, COMMA);
                for (int j = 0; j < cols; j++) {
                        //Complete the line of code below
                        //to initialize your 2D array.
                        *(*(board + i) + j) = atoi(token);
                        token = strtok(NULL, COMMA);    
                }
        }

        //Call the function check_queens and print the appropriate
        //output depending on the function's return value.
       if(check_queens(board,rows,cols) == 1){
	       printf("true\n");
	}else{
		printf("false\n");
	}
        
        //Free all dynamically allocated memory.
	free(board);
	board = NULL;

        //Close the file.
        if (fclose(fp) != 0) {
                printf("Error while closing the file\n");
                exit(1);        
        }

        return 0;
}      
