//SpringerProjekt des Teams WITTENSTEIN SE
//von Fabian Konrad und Armin Beck

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

//#include "stdafx.h"

// count of all fields
short fieldSize;
//  = fieldSize - 1, only for optimization
short lastStepIndex;

// 1D field size
short length;

//points to the begin of fields that will be used for solution finding
short* fieldArray;

//array used for heuristic step analysis
short* neighboursArray;
//array for heuristic level2 analysis
short* neighboursNeighboursArray;

//amount of entered fields until the right path could be found
long tryCount;

//defines whether the user has chosen a continuous path
bool isContinuousPath;

//Enum determining which directions are possible
unsigned char* DirectionsPerField;

//describes the movement of a knight in 1D
short directions[8];

//inputsize
const short inputSize = 8;
// X- value of directions
//const short directionsX[8] = { 1, 1, -1, -1, 2, 2, -2, -2 };
//const short directionsY[8] = { 2, -2, -2, 2, 1, -1, -1, 1 };

//movement directions in a 2d field, only the x direction is displayed
const short directionsX[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };
//movement directions in a 2d field
const short directionsY[8] = { 2, 1, -1, -2, -2, -1, 1, 2 };

//the selected start position, only the x direction is displayed
short firstPos;

int main();
void resortField(short);
void initializeField();
void disposeFields();
void printField();
bool goStep(short, short);
bool isInBounds(short, short);
void startStep(short, short, bool);
short generateStepList(short, short*);
short generateNeighboursStepList(short);
void scanParams();
void selectFieldOnBoard();
void clearScreen();
void clearBuffer();
void scanManualField();
void printSolutionClassicNotation(char);
void printSolutionOnBoard(char);
void outputControl();

int main()
{
	scanParams();//The first function scans the User Selection for the way, the algorithm is called.
	startStep(firstPos, length, isContinuousPath);//The last function Selects the path and prints it at the end.
	system("pause");//keeps the commmand prompt window alive at the end of the program.
	return 0;
}
void initializeField()
{
	//initializes the used Memory for the field and for the Neighbour Arrays
	fieldSize = length*length;
	lastStepIndex = fieldSize - 1;

	fieldArray = malloc(fieldSize*sizeof(short));
	neighboursArray = malloc(fieldSize*length*sizeof(short));
	neighboursNeighboursArray = malloc(fieldSize*length*sizeof(short));
	DirectionsPerField = malloc(fieldSize*sizeof(char));

	for (short i = 0; i < 8; i++)
	{
		directions[i] = directionsX[i] + directionsY[i] * length;
		/*The two dimensional jump directions (eg. 2 up, 1 right will be changed to the one-dimensional direction (-2*length)+1 ) is set by this sum.
		It is realized by multiplying the y-Direction by length and adding the x-Direction afterwards.
		The reason for this solution is the problem, that a changed y-Coordinate will result in a Change of a complete row.
		So for every y-Change the length of one row has to be added or subtracted. */
	}
	for (short i = 0; i < fieldSize; i++)
	{
		fieldArray[i] = -1;//every field will be initialized with -1 --> this is a value for a not-entered field
	}
	for (short y = 0; y < length; y++)
		/*This double loop generates the DirectionsPerField for every field. It is used to determine, where a neighbour can be and to
		exclude all jumps which would leave the field.*/
	{
		for (short x = 0; x < length; x++)
		{
			DirectionsPerField[x + y*length] = 0;
			for (short s = 0; s < 8; s++)
			{
				/*resX and resY are the Coordinates of the Neighbour, that would be hit by this jump.
				To generate all Coordinates, this loop iterates through all possible directions.*/
				short resX = x + directionsX[s], resY = y + directionsY[s];
				DirectionsPerField[x + y*length] |= ((-1 < resX&&resX < length)/*checks if resX is inside the field*/
					&& (-1 < resY&&resY < length))/*checks if resY is inside the field*/
					*(1 << s);/*Now puts the compare value (1 or 0)*/
				/*at the bit at the position s by multiplying them.
				This value will be or-compared with the current DirectionsPerField-Value.
				If the comparison is true, there is a neighbour at this Position and it will result with a 1 at the bit
				with the index s of the DirectionsPerField[x+y*length] variable.NOTE: because of the initial value of
				DirectionsPerField, the  Resulting Value will only depend */
			}
		}

	}
}
void disposeFields()
{
	free(fieldArray);//deletes the current fieldArray from the Memory
}
void printField()
{
	//Print the fields in a table view
	printf("\n    ");//Prints the Whitespace Head of the Table
	for (int i = 0; i < length; i++)
	{
		printf("[%3d]", i);//Print the X-Axis on top of the Grid
	}
	printf("\n");
	for (short i = 0; i < length; i++)
	{
		//                      printf("[%2d]", i);
		for (short ii = -1; ii < length; ii++)
		{
			if (ii == -1)//The left most Character: These numbers will go from 0 to length and represent the Y-Axis at the left side of the printed Grid.
			{
				printf("[%3d]", i);
				continue;
			}
			printf(" %3d ", fieldArray[i + ii * length]);//Print the value of each field from the top left to bottom right
		}
		printf("\n");
	}
}

void startStep(short position, short size, bool isContinuous)
{
	//This function is used to startup the Algorithm, it also launches the Recursion by calling goStep.
	tryCount = 0;//Tries Will be intialized with 0 and then increment with every goStep call.
	initializeField(); //Then the field will be initialized in order to know, which neighbours are next to any field

	//setting global value to chosen solution
	isContinuousPath = isContinuous;
	if (isContinuousPath){

		firstPos = 0;//The Algorithm for a continuos path always starts at the first field of the Array. The starting point will be edited for the display at the End.
		if (goStep(0, 0))//if the recursive call of go step ends successfully, it will return true, and the path is ready to be printed.
		{
			resortField(position);//resorts the field to change the starting position from 0 0 to the user selected starting point.
			outputControl();//Shows the path in a simple way on the screen
		}
		else
		{
			puts("could not find a solution path");//Error mode, goStep has failed
		}
	}
	else{
		firstPos = position;//In the open path solution the algorithm starts at the field selected by the user.
		if (goStep(position, 0))//if the recursive call of go step ends successfully, it will return true, and the path is ready to be printed.
		{
			outputControl();//Shows the path in a simple way on the screen
		}
		else
		{
			puts("could not find a solution path");//Error mode, goStep has failed
		}
	}
	printf("\nTryCount: %d \n ", tryCount);//returns the amount af fields, that have been entered by the algorithm until goStep ended.
	disposeFields();// frees the memory again, it won't be needed any more!
}
bool goStep(short position, short ctr)
{
	/*This is the mainly used function to create the Solution Path for the Knight.
	It is a recursive function with a maximum call stack size of fieldsize minus 1*/

	tryCount++;//every time goStep is called, the counter of entered fields will be increased
	fieldArray[position] = ctr;//the current field will have the value of its position in the path through the whole board
	if (ctr == lastStepIndex)
	{
		if (!isContinuousPath){ return true; }//If the path is not continuos, the last must not be a neighbour of the starting Position
		fieldArray[firstPos] = -1;//The starting field will be set to -1 to imitate that it wasn't entered before.
		short * stepListList = &neighboursArray[position*length];
		short stepCtr = generateStepList(position, stepListList);
		if (stepCtr == 1){
			return true;//If the current field is the last field, it will only have one neighbour
		}
		fieldArray[firstPos] = 0;
		fieldArray[position] = -1;
		return false;
	}

	// points to position in Neigboursarray
	short* stepList = &neighboursArray[position*length];
	// amount of 'jumpable' adjacent fields
	short stepsCount = generateStepList(position, stepList);
	//array of stepsCount of stepList [0]
	short* stepStepList = &neighboursNeighboursArray[position* length];
	//max value in stepStepList
	short currentMaxNeighboursCount = 0;
	//genereate stepStepList
	for (short i = 0; i < stepsCount; i++)
	{           //get number of neighbours' adjacent fields
		stepStepList[i] = generateNeighboursStepList(stepList[i]);

		//when there's only one adjacent field, jump directly on it
		//when there's a jumpable field with 0 neighbours, jump on it (step 62)
		if (stepStepList[i] == 0 || stepStepList[i] == 1)
		{
			if (ctr != lastStepIndex - 1 && stepStepList[i] == 0){
				fieldArray[position] = -1;
				return false;
			}
			if (goStep(stepList[i], ctr + 1)){
				return true;
			}
			stepStepList[i] = -1;
		}
		if (stepStepList[i] > currentMaxNeighboursCount){ currentMaxNeighboursCount = stepStepList[i]; }
	}
	currentMaxNeighboursCount++;
	for (short currentVal = 2; currentVal < currentMaxNeighboursCount; currentVal++)
	{
		for (int i = 0; i < stepsCount; i++)
		{
			//if a field has two neighbours, it means that it has
			if (stepStepList[i] == currentVal)
			{
				if (goStep(stepList[i], ctr + 1)){
					return true;
				}
			}
			//field already visited or greater currentVal
		}
	}
	fieldArray[position] = -1;
	return false;

}
// returns amount of directions and saves the resulting fields in stepList[]
short generateStepList(short pos, short* stepListRef)
{
	short validSteps = 0;
	for (short i = 0; i < 8; i++)
	{
		if ((DirectionsPerField[pos] & (1 << i))/*Checks, if the end point is inside the field*/
			&& fieldArray[pos + directions[i]] == -1)//If the field was already entered, this will return false, because the fields are initialized with -1
		{
			((stepListRef)[validSteps]) = (short)pos + directions[i];//writes a reference to the neighbour in the first unused Address in steplistref
			validSteps++;//indicates the first free position in StepListRef, also works as counter for the amount of resulting fields
		}
	}
	return validSteps;
}
// returns how many 'jumpable' neighbours a field has
short generateNeighboursStepList(short pos)
{
	short validSteps = 0;
	for (short i = 0; i < 8; i++)
	{
		if ((DirectionsPerField[pos] & (1 << i))/*Checks, if the end point is inside the field*/
			&& fieldArray[pos + directions[i]] == -1)//If the field was already entered, this will return false, because the fields are initialized with -1
		{
			validSteps++;
		}
	}
	return validSteps;
}

//checks whether it's valid to move from a field in a certain direction
bool isInBounds(short pos, short direction)
{
	return DirectionsPerField[pos] & direction;
}
void resortField(short firstPos)
{
	fieldArray[0] = 0;
	short stepValue = fieldArray[firstPos];
	for (short i = 0; i < fieldSize; i++)
	{
		fieldArray[i] = (fieldArray[i] - stepValue);
		if (fieldArray[i] < 0){ fieldArray[i] += fieldSize; }
	}
}

void clearBuffer()
{
	while (getchar() != '\n');
}
void scanParams()
{
	puts("\nWelcome to the Knight Problem Solver, made by Employees of WITTENSTEIN SE Company.\n");
	puts("This is the main menu. You can choose between following options:\nNote: Always press Enter twice.\n");
	puts("1. Solution with an open path...\n");
	puts("2. Solution with a closed path...\n");
	bool closedPath;
	//choose mode
	while (true){
		char input = 0;
		scanf("%c", &input);
		clearBuffer();
		int i = input - '0';
		if (i == 1)
		{
			closedPath = false;
			break;
		}
		if (i == 2)
		{
			closedPath = true;
			break;
		}
		puts("\r Invalid Input, please try again.");
	}
	clearScreen();
	puts("Please enter the desired field size");
	if (closedPath)
	{
		puts("\nNote: uneven field sizes don't have closed solutions");
	}

	while (true){
		length = -1;//Sets the Value of length to -1 at the Beginning of the loop to guarantee
		scanf("%hd", &length);
		clearBuffer();
		if (length < 5){ puts("There is no solution existent for your desired Field Size. Try again."); continue; }
		if (length < 14){ break; }
		if (length > 14){
			char decision = 'n';
			puts("\nSure to use such a large field?(y/n)");//Warns the user, that it may take almost forever to find a Solution
			scanf("%c", &decision);//reads the user's decision into the decision char; everything after the first character will be ignored.
			clearBuffer();
			if (decision == 'y' || decision == 'Y'){
				break;//It will only break the loop, if the User types Y or y
			}
		}
	}

	clearScreen();
	puts("\n Select your favourite input method:\n");
	puts("1.Enter a field manually");
	puts("2.Choose a field on the board");
	puts("3.Let the computer choose by random");
	int inputMethod = 0;

	//choose input mode
	while (true){
		char input = 0;
		scanf("%c", &input);
		clearBuffer();
		inputMethod = input - '0';
		if (0 < inputMethod&&inputMethod < 4){//Checks, if the user has typed a value between 0 and 4 otherwise it will ask the user for his value again.
			break;
		}
		puts("\r Invalid Input, please try again.");
	}

	switch (inputMethod)//defines, which mode for selection is used, in dependence of the user input before. Returns and not breaks in any switch case, because in this function nothing follows after the function call.
	{
	case 1:
		scanManualField();//Call manual command line input function
		return;
	case 2:
		selectFieldOnBoard();//Call semi-graphical input function
		return;
	case 3:
		srand(time(NULL));//Generate new random Seed by time
		firstPos = rand() % (length*length);//defines the startingfield by random
		return;
	}

}
bool parseClassicNotation(char input[])//This function is used to turn the Classic Chess Style Notation into a single value field Index
{
	int idx = 0, x = 0, y = 0;
	while (input[idx] == ' ' && (idx++) < inputSize);//jumps through the Whitespaces, up to the first digit or until the length of the input String is reached
	if (idx >= inputSize)
	{
		return false;//If there was no letter that differed from whitespaces, this function will return the error Value false
	}
	if (islower(input[idx]))//double if block to determine if the letter at position idx is lower or upper
	{
		x = input[idx++] - 'a';//if the letter is lower, a will be subtracted and x will be the letter's index in the alphabet (a=0; b=1; etc.).
	}
	else if (isupper(input[idx]))
	{
		x = input[idx++] - 'A';//if the letter is upper, A will be subtracted and x will be the letter's index in the alphabet (A=0; B=1; etc.).
	}
	//jump over spaces
	while (input[idx] == ' ' && (idx++) < inputSize);
	//read number
	while (isdigit(input[idx]))
	{
		y = y * 10 + (input[idx++] - '0');//reads the current digit and multiplies it by 10 if another digit comes
	}
	if (y == 0){ return false; }//return false, if the user has written A0 or b0 etc. --> ask the User to input again
	firstPos = (y - 1)*length + x;//The single-dimensional firstpos is calculated by multiplying y by length and adding x (in graphical mode y starts with 1, so a subtraction is needed)
	return true;
}
bool parseCartesianNotation(char input[])
{
	int x = 0, y = 0, idx = 0;
	//jump over spaces
	while (input[idx] == ' ' && (idx++) < inputSize);
	if (idx >= inputSize){ return false; }
	//read first number
	while (isdigit(input[idx]) && idx < inputSize)
	{
		x = x * 10 + (input[idx++] - '0');
	}
	//jump over spaces
	while (input[idx] == ' ' && (idx++) < inputSize);

	if (idx >= inputSize){
		//1D idx detected
		if (-1 < x && x < length*length)
		{
			firstPos = x;
			return true;
		}
		return false;
	}
	//read second number if it exists
	while (isdigit(input[idx]) && idx < inputSize)
	{
		y = y * 10 + (input[idx++] - '0');
	}
	//2d idx detected
	if (y > -1 && x > -1 && x < length&& y < length)
	{
		firstPos = y*length + x;
		return true;
	}
	return false;

}
void scanManualField()
{
	clearScreen();
	while (true){
		puts("Please enter the desired start field\nPossible input formats:\n");
		puts("->'classic' notation e.g. A1\n");
		puts("->zero based 2D coordinates X Y,  e.g. 1 2\n");
		puts("->zero based 1D coordinates e.g. 17\n");
		//                      int const  inputSize = 8;
		char* input = malloc(sizeof(char)*inputSize);
		for (int i = 0; i < 8; i++){
			input[i] = 0;
		}

		scanf("%8[^\n]s", input);
		clearBuffer();
		int idx = 0;
		//jump over spaces
		while (' ' == (input[idx]) && (idx++) < inputSize);

		//classic mode detected
		if (isalpha(input[idx]) && parseClassicNotation(input))
		{
			return; // breaks the loop and the function beacause the Input has been successfully parsed.
		}
		//1D or 2D mode detected
		else if (isdigit(input[idx]) && parseCartesianNotation(input))
		{
			return;// breaks the loop and ends the function beacause the Input has been successfully parsed.
		}
		puts("\r Invalid Input, please try again.");//Error Warning to the User. The Loop will start again.
	}
}

void selectFieldOnBoard() //This function is used for the interactive Selection of the Starting point
{
	int curX = 0, curY = length - 1;
	while (true)
	{
		clearScreen();
		puts("Select the startpoint\n");
		puts("Use w (up), a (left),  s(right), d(down) for moving. Press enter after every input\n");
		puts("Use q to confirm selection\n");
		//Prints everything, that is displayed on top of the interactive field
		printf("\t\t%c", 201);//corner top left
		for (int i = 0; i < length; i++)printf("%c", 205);//top frame
		printf("%c\n", 187);//corner top right
		//Prints all the fields now, this is a two dimensional loop
		for (int y = 0; y < length; y++)
		{
			printf("\t\t%c", 186);//left border
			for (int x = 0; x < length; x++)
			{
				if (x == curX&&y == curY){ printf("%c", 2); continue; }
				printf("%c", (((x + y) % 2) ? 176 : 178));//If the current field to be printed is an even number, character 176 will be printed, if it is uneven, character 178 will be printed.
			}
			printf("%c\n", 186);//right border
		}
		printf("\t\t%c", 200);//corner top left
		for (int i = 0; i < length; i++)printf("%c", 205);//top frame
		printf("%c", 188);//corner top right
		printf("\n\tCurrent Position: %c%d", 'A' + curX, length - curY);
		char c;
		scanf("%c", &c);//scans the User input, if he wants to go upwards downwards etc. or if he wants to quit.
		clearBuffer();
		if (c == 'w' && curY - 1 >= 0){ curY--; }
		if (c == 'a' && curX - 1 >= 0){ curX--; }
		if (c == 'd' && curX + 1 < length){ curX++; }
		if (c == 's' && curY + 1 < length){ curY++; }
		if (c == 'q'){
			firstPos = curX*length + curY; return;//quits the whole input procedure and saves the currently highlighted field as starting point for the algorithm.
		}
		//every wrong input will return in nothing to change
	}
}
void outputControl()
{
	char outputMethod = 'y';
	char input = '\n';
	while (true)
	{
		clearScreen();
		puts("Press y for Classic Notation, x for interactive Board and c for Number Board\n");
		switch (outputMethod)
		{
		case 'y':{printSolutionClassicNotation(input); break; }
		case 'x':{printSolutionOnBoard(input); break; }
		case 'c':{printField(); break; }
		}
		puts("\n");
		scanf("%c", &input);
		clearBuffer();
		if (input == 'q'){ break; }
		// chars to change outputmethod: y x c 
		if (input == 'y' || input == 'x' || input == 'c'){ outputMethod = input; }
	}
}
void printSolutionOnBoard(char c){

	if (length > 25)//Disables classical stile Print of fields bigger than 25x25
	{
		puts("Classic notation is not supported for the selected size");
		return;
	}
	static int CurrentStep = 0;
	int curX = 0, curY = length - 1;
	for (int i = 0; i < fieldSize; i++)
	{
		if (fieldArray[i] == CurrentStep)
		{
			curX = i / length;
			curY = i%length;
			break;
		}
	}

	if (c == 'a' && CurrentStep > 0){ CurrentStep--; }
	if (c == 'd' && CurrentStep < fieldSize){ CurrentStep++; }
	puts("Use a(previous step) and d(next step) for navigating through the solution. Press enter after every input\n");
	puts("Use q to exit\n");

	//Prints everything, that is displayed on top of the interactive field
	printf("\t\t%c", 201);//corner top left
	for (short i = 0; i < length; i++)
	{
		printf("%c", 205);//top frame
	}
	printf("%c\n", 187);//corner top right
	//Prints all the fields now, this is a two dimensional loop
	for (short y = 0; y < length; y++)
	{
		printf("\t\t%c", 186);//left border
		for (short x = 0; x < length; x++)
		{
			if (x == curX&&y == curY){ printf("%c", 2); continue; }
			printf("%c", ((x + y) & 1) ? 176 : 178);//If the current field to be printed is an even number, character 176 will be printed, if it is uneven, character 178 will be printed.
		}
		printf("%c\n", 186);//right border
	}
	printf("\t\t%c", 200);//corner top left
	for (short i = 0; i < length; i++)
	{
		printf("%c", 205);//top frame
	}
	printf("%c", 188);//corner top right
	printf("\n\tCurrent Position: %c%d", 'A' + curX, length - curY);
}
void printSolutionClassicNotation(char c)
{
	if (length > 25)//Disables classical stile Print of fields bigger than 25x25
	{
		puts("Classic notation is not supported for the selected size");
		return;
	}
	puts("Use a(previous steps) or d(next steps) to navigate\n");
	static int currentStep = 0;
	if (c == 'd'&&currentStep < (fieldSize - fieldSize % 8 - 8)){ currentStep += 8; }
	if (c == 'a'&&currentStep >= 8){ currentStep -= 8; }
	int lastIndex = -1;
	for (int lines = 0; lines <= ((fieldSize - currentStep>8) ? 8 : fieldSize - currentStep); lines++)
	{
		for (int idx = 0; idx < fieldSize; idx++)
		{
			if (fieldArray[idx] == currentStep + lines)
			{
				if (lastIndex == -1)
				{
					lastIndex = idx;
					break;
				}
				printf("%4d. %c%d - %c%d\n", currentStep + lines, 'A' + (lastIndex / length), (length - lastIndex%length),
					'A' + (idx / length), (length - idx%length));
				lastIndex = idx;
			}
		}
	}
}

void clearScreen(){
	//Used to clear the screen in dependence of the Operating System. The selection of the right clear mode depends on the compiler.
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
	system("clear"); // the Standard command on a unix based OS to clear the screen
#elif defined(_WIN32) || defined(_WIN64)
	system("cls"); //the Windows command to clear the screen
#else
	for (int i = 0; i < 30; i++){ puts("\n"); } //The alternative. It will be used, if the upper defines do not exist.
#endif
}
