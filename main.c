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
int main()
{
	scanParams();//The first function scans the User Selection for the way, the algorithm is called.

	startStep(firstPos, length, isContinuousPath);//The last function Selects the path and prints it at the end.
	system("pause");
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

	for (short i = 0; i < length; i++)
	{
		directions[i] = directionsX[i] + directionsY[i] * length;
	}
	for (short i = 0; i < fieldSize; i++)
	{
		fieldArray[i] = -1;
	}
	for (short y = 0; y < length; y++)
	{
		for (short x = 0; x < length; x++)
		{
			DirectionsPerField[x + y*length] = 0;
			for (short s = 0; s < 8; s++)
			{
				short resX = x + directionsX[s], resY = y + directionsY[s];
				DirectionsPerField[x + y*length] |= ((-1 < resX&&resX < length) && (-1 < resY&&resY < length))*(1 << s);
			}
		}

	}
}
void disposeFields()
{
	free(fieldArray);
}
void printField()
{
	printf("\n");
	for (short i = 0; i < length; i++)
	{
		printf("[%2d]", i);
		for (short ii = -1; ii < length; ii++)
		{
			if (ii == -1)
			{
				printf("[%2d]", i);
				continue;
			}
			printf(" %2d ", fieldArray[ii + i * length]);
		}
		printf("\n");
	}
}

void startStep(short position, short size, bool isContinuous)
{
	tryCount = 0;
	initializeField(); //Then the field will be initialized in order to know, which neighbours are next to any field

	//setting global value to chosen solution
	isContinuousPath = isContinuous;
	if (isContinuousPath){

		firstPos = 0;
		if (goStep(0, 0))
		{
			resortField(position);
			printField();
		}
		else
		{
			puts("could not find a solution path");
		}
	}
	else{
		firstPos = position;
		if (goStep(position, 0))
		{
			printField();
		}
		else
		{
			puts("could not find a solution path");
		}
	}
	printf("\nTryCount: %d \n ", tryCount);
	disposeFields();
}
bool goStep(short position, short ctr)
{

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
	{	//get number of neighbours' adjacent fields
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
void scanParams(){
	/*
	TBD:
	1. closed/open
	2. size
	3. input method
	*/
	puts("\nWelcome to the Knight Problem Solver, made by Employees of WITTENSTEIN SE Company.\n");
	puts("This is the main menu. You can choose between following options:\nNote: Always press Enter twice.\n");
	puts("1. Solution with an open path...\n");
	puts("2. Solution with a closed path...\n");
	bool closedPath;
	//choose mode
	while (true){
		int i = getchar() - '0';
		clearBuffer();
		if (i == 1){ closedPath = false; break; }
		if (i == 2){ closedPath = true; break; }
		puts("\r Invalid Input, please try again.");
	}

	clearScreen();
	puts("Please enter the desired field size");
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
		inputMethod = getchar() - '0';
		clearBuffer();
		if (0 < inputMethod&&inputMethod < 4){//Checks, if the user has typed a value between 0 and 4 otherwise it will ask the user for his value again.
			break;
		}
		puts("\r Invalid Input, please try again.");
	}

	if (inputMethod == 1){ scanManualField(); }
	else if (inputMethod == 2){ selectFieldOnBoard(); }
	if (inputMethod == 3){ firstPos = rand() % (length*length); }
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
		x = input[idx++] - 'a';//if the letter is lower a will be subtracted and x will be the letter's index in the alphabet (a=0; b=1; etc.).
	}
	else if (isupper(input[idx]))
	{
		x = input[idx++] - 'A';//if the letter is upper a will be subtracted and x will be the letter's index in the alphabet (A=0; B=1; etc.).
	}
	//jump over spaces
	while (input[idx] == ' ' && (idx++) < inputSize);
	//read number
	while (isdigit(input[idx]))
	{
		y = y * 10 + (input[idx++] - '0');
	}
	firstPos = (y - 1)*length + x;
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
		int const  inputSize = 8;
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
			return;
		}
		//1D or 2D mode detected
		else if (isdigit(input[idx]) && parseCartesianNotation(input))
		{
			return;
		}
		puts("\r Invalid Input, please try again.");
	}
}

void selectFieldOnBoard() //This function is used for the interactive Selection of the Starting point
{
	int curX = 0, curY = length - 1;
	char lastDir = 0;
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
				printf("%c", ((x + y*(length + 1)) & 1) ? 176 : 178);//If the current field to be printed is an even number, character 176 will be printed, if it is uneven, character 178 will be printed.
			}
			printf("%c\n", 186);//right border
		}
		printf("\t\t%c", 200);//corner top left
		for (int i = 0; i < length; i++)printf("%c", 205);//top frame
		printf("%c", 188);//corner top right
		printf("\n\tCurrent Position: %c%d", 'A' + curX, length - (curY + 1));
		char c;
		scanf("%c", &c);//scans the User input, if he wants to go upwards downwards etc. or if he wants to quit.
		clearBuffer();
		if (c == '\n'){ c = lastDir; }
		if (c == 'w' && curY > 0){ curY--; }
		if (c == 'a' && curX > 0){ curX--; }
		if (c == 'd' && curX < length){ curX++; }
		if (c == 's' && curY < length){ curY++; }
		if (c == 'q'){
			firstPos = curX*length + curY; return;//quits the whole input procedure and saves the currently highlighted field as starting point for the algorithm.
		}
		//every wrong input will return in nothing to change
		lastDir = c; //at the end of the loop lastDir will be overwritten with the index of the Current field
	}
}
void clearScreen(){
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
	system("clear");
#elif defined(_WIN32) || defined(_WIN64)
	system("cls");
#else
	for (int i = 0; i < 30; i++){ puts("\n"); }
#endif
}
