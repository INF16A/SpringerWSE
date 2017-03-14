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

//amount tried steps
long tryCount;

//whether the user has chosen a continuous
bool isContinuousPath;

//Enum determining which directions are possible
unsigned char* DirectionsPerField;

//describes the movement of a knight in 1D
short directions[8];
// X- value of directions
//const short directionsX[8] = { 1, 1, -1, -1, 2, 2, -2, -2 };
//const short directionsY[8] = { 2, -2, -2, 2, 1, -1, -1, 1 };

//movement directions in a 2d field
const short directionsX[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };
//movement directions in a 2d field
const short directionsY[8] = { 2, 1, -1, -2, -2, -1, 1, 2 };

//the selected start position
short firstPos;

int main();
void resortField(short);
void initializeField();
void disposeFields();
void printField();
bool goStep(short, short);
void startStep(short, short, bool);
short generateStepList(short, short**);
short generateNeighboursStepList(short);
void scanParams();
void selectFieldOnBoard();
void clearScreen();
int main()
{
	scanParams();
	initializeField(length);
	startStep(firstPos, length, isContinuousPath);
	system("pause");
	return 0;
}
void printFieldWithSpringerSymbol(int pos)
{
	char corners[] = { 201, 187, 188, 202 };
	/*for (int i = 0; i < fieldSize; i++)
	{
	printf()
	for (int ii = 0; ii < fieldSize; ii++)
	{

	}
	}*/
}
void initializeField(short inputlength)
{
	length = inputlength;
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
			printf(" %2d ", fieldArray[ii + i * 8]);
		}
		printf("\n");
	}
}

void startStep(short position, short size, bool isContinuous)
{
	tryCount = 0;
	initializeField(size);

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
	tryCount++;
	fieldArray[position] = ctr;
	if (ctr == lastStepIndex)
	{
		fieldArray[firstPos] = -1;
		short * stepListList = &neighboursArray[position*length];
		short stepCtr = generateStepList(position, (stepListList));
		if (stepCtr == 1){
			return true;
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
		if ((DirectionsPerField[pos] & (1 << i)) && fieldArray[pos + directions[i]] == -1)
		{
			((stepListRef)[validSteps]) = (short)pos + directions[i];
			validSteps++;
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
		if ((DirectionsPerField[pos] & (1 << i)) && fieldArray[pos + directions[i]] == -1)
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

void clearBuffer(void)
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
		int i = fgetc(stdin) - '0';
		clearBuffer();
		if (i == 1){ closedPath = false; break; }
		if (i == 2){ closedPath = true; break; }
		puts("\r Invalid Input, please try again.");
	}

	clearScreen();
	puts("Please enter the desired field size");
	while (true){
		length = -1;
		scanf("%d", length);
		clearBuffer();
		if (length < 5){ puts("There is no solution existent for your desired fieldsize. Try again."); }
	}

	clearScreen();
	puts("\n Select your favourite input method:\n");
	puts("1.Enter a field manually");
	puts("2.Choose a field on the board");
	puts("3.Let the computer choose by random");
	int inputMethod = 0;
	//choose input mode
	while (true){
		inputMethod = fgetc(stdin) - '0';
		clearBuffer();
		if (0 < inputMethod&&inputMethod < 4){
			break;
		}
		puts("\r Invalid Input, please try again.");
	}
	switch (inputMethod)
	{
	case 1:scanParams();
	case 2:
	default:break;
	}
}
void scanManualField(){
	int x = -1, y = -1;
	while (true){
		scanf("%c%d", x, y);
		x -= x < 'H'&&x >= 'A' ? 'A' : 'a';
		y--;
		if (-1 < x&&-1 < y && x < 8 && y < 8){
			break;
		}
		puts("\r Invalid Input, please try again.");
		firstPos = x*length + y;
	}

}
void selectFieldOnBoard(){
	int curX = 0, curY = length - 1;
	char lastDir = 0;
	while (true)
	{
		clearScreen();
		puts("Select the startpoint\n");
		puts("Use w (up), a (left),  s(right), d(down) for moving. Press enter after every input\n");
		puts("Use q to confirm selection\n");
		printf("\t\t%c", 201);//corner top left
		for (int i = 0; i < length; i++)printf("%c", 205);//top frame
		printf("%c\n", 187);//corner top right
		for (int y = 0; y < length; y++)
		{
			printf("\t\t%c", 186);//left border
			for (int x = 0; x < length; x++)
			{
				if (x == curX&&y == curY){ printf("%c", 2); continue; }
				printf("%c", ((x + y*(length + 1)) & 1) ? 176 : 178);
			}
			printf("%c\n", 186);//right border
		}
		printf("\t\t%c", 200);//corner top left
		for (int i = 0; i < length; i++)printf("%c", 205);//top frame
		printf("%c", 188);//corner top right
		printf("\n\tCurrent Position: %c%d", 'A' + curX, length - curY);
		char c = getchar(stdin);
		clearBuffer();
		if (c == '\n'){ c = lastDir; }
		if (c == 'w' && curY - 1>0){ curY--; }
		if (c == 'a' && curX - 1 > 0){ curX--; }
		if (c == 'd' && curX + 1 < length){ curX++; }
		if (c == 's' && curY + 1 < length){ curY++; }
		if (c == 'q'){ firstPos = curX*length + curY; return; }
		lastDir = c;
	}
}
void clearScreen(){
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
	system("clear");
#elif defined(_WIN32) || defined(_WIN64)
	system("cls");
#else
	for(int i=0;i<30;i++){puts("\n");}
#endif
}
