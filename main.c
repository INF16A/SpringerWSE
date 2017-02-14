// SpringerProjekt.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "stdafx.h"



// count of all fields
short fieldSize;
// = fieldSize - 1, only for optimization
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
long backTrackCount;
bool isBackTrack;
//whether the user has chosen a continuous
bool isContinuousPath;

//Enum determining which directions are possible
unsigned char* DirectionsPerField;

//describes the movement of a SPRINGER in 1D
short directions[8];
// X- value of directions
//const short directionsX[8] = { 1, 1, -1, -1, 2, 2, -2, -2 };
//const short directionsY[8] = { 2, -2, -2, 2, 1, -1, -1, 1 };
const short directionsX[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };
const short directionsY[8] = { 2, 1, -1, -2, -2, -1, 1, 2 };

short firstPos;

int main();
void resortField(short);
void initializeField();
void disposeFields();
void printField();
void printChar(char);
bool goStep(short, short);
void startStep(short, short, bool);
short generateStepList(short, short**);
short generateNeighboursStepList(short);
int main()
{
	/*	system("pause");
		for (short s = 0; s < 64; s++)
		{
		printf("\n%d:", s);
		printChar(DirectionsPerField[s]);
		}*
		system("pause");
		startStep(0, 8, true);
		system("pause");*/
	//	for (int x = 0; x < 64; x++)
	//{
	//printf("[%d] ", x);
	for (int i = 0; i < 65535; i++)
	{
		printf("%LC ", i);
		if (i % 32 == 0)
		{
			printf("\n");
		}
		if (i % (57*32) == 0)
		{
			system("pause");
		}
	}
	initializeField(8);
	printf("%LC", 9822);
	startStep(5, 8, true);
	printf("%d %d ", tryCount, backTrackCount);
	system("pause");
	return 0;
}
void printChar(char c)
{
	printf("\t %3d ", c);
	unsigned char remC = c, ctr = 0;
	for (short s = 128; s > 0; s /= 2)
	{
		printf("%d", remC / s);
		ctr += (remC / s);
		remC = remC %s;
	}
	printf("\t %2d ", ctr);
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
				//printf("%2d|%2d ->%2d|%2d, %d\n", x, y, resX, resY, DirectionsPerField[x + y*length] & (2 << s));
			}
			//	printf("\n%2d|%2d", x, y);
			//printChar(DirectionsPerField[x + y*length]);
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
	backTrackCount = 0;
	isBackTrack = false;
	initializeField(size);
	
	//setting global value to chosen solution
	isContinuousPath = isContinuous;
	if (isContinuousPath){

		firstPos = 0;
		if (goStep(0, 0))
		{
			printField();
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
	printf("\nTryCount: %d \n BacktrackCount: %d", tryCount,backTrackCount);
	disposeFields();
}
bool goStep(short position, short ctr)
{
	tryCount++;
	//printField();
	//system("pause");
	fieldArray[position] = ctr;
	if (isBackTrack){
		backTrackCount++;
		//printf("%2d , %8d , %8d \n",ctr, tryCount, backTrackCount);
		isBackTrack = false;
	}
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
		//	printf("%d", tryCount);
		if (isBackTrack == false)
		{
			//	printField();
			//		system("pause");
		}
		isBackTrack = true;
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
	{
		//get number of neighbours' adjacent fields
		stepStepList[i] = generateNeighboursStepList(stepList[i]);

		//when there's only one adjacent field, jump directly on it
		//when there's a jumpable field with 0 neighbours, jump on it (step 62)
		if (stepStepList[i] == 0 || stepStepList[i] == 1)
		{
			if (ctr != lastStepIndex - 1 && stepStepList[i] == 0){
				fieldArray[position] = -1;
				if (isBackTrack == false)
				{
					//printField();
					//	system("pause");
				}
				isBackTrack = true;
				return false;
				//printf("FEHLER!!!");
			}
			if (goStep(stepList[i], ctr + 1)){
				return true;
			}
			//printField();
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
			/*if (stepList[i] == -1)
			{
			continue;
			}*/

		}
	}
	fieldArray[position] = -1;
	if (isBackTrack == false)
	{
		//	printField();
		//	system("pause");
	}
	isBackTrack = true;
	return false;

}
// returns amount of directions and saves the resulting fields in stepList[]
short generateStepList(short pos, short* stepListRef)
{
	//short* stepList = &neighboursArray[pos*length];
	short validSteps = 0;
	for (short i = 0; i < 8; i++)
	{
		if ((DirectionsPerField[pos] & (1 << i)) && fieldArray[pos + directions[i]] == -1)
		{
			((stepListRef)[validSteps]) = (short)pos + directions[i];
			validSteps++;
		}
	}
	//*stepListRef = stepList;
	return validSteps;
}
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
bool isInBounds(short pos, short direction)
{
	return DirectionsPerField[pos] & direction;
}

void printArray(short* arrayVar, short size)
{
	printf("Array: %5d", size);
	for (short i = 0; i < size; i++)
	{
		printf(" %5d ", arrayVar[i]);
	}
	printf("\n");
}
void printArrayL(short* arrayVar, short size, short maxL)
{
	system("cls");
	printf("Array: %5d", size);
	for (short i = 0; i < size; i++)
	{
		if (i%maxL == 0){ printf("\n"); }
		printf(" %6d ", arrayVar[i]);
	}
	printf("\n");
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
