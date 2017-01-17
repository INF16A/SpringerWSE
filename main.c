#include "stdafx.h"
struct SingleField
{
	short posX;
	short posY;
	/*	 0 = unused
		-1 = Startfield
		n = jump nr. */
	short status;
	/*amount of neighbours*/
	short neighbourCount;
	/* Short array of field indexes*/
	short* FirstNeighbour;
};

struct SingleField **field;

short directions[16] =
{ 1, 2,
1, -2,
-1, -2,
-1, 2,

2, 1,
2, -1,
-2, -1,
-2, 1 };

void initializeField();
bool goStep(short, short, short);
bool isInBounds(short, short);
void printfields();

int main(void)
{
	int* resultingPointer = (int*)malloc(sizeof(int*) * 8);
	int** resultingPointerPointer = (int**)malloc(sizeof(int*) * 8);
	struct SingleField** resultingPointerStruct = (struct SingleField**)malloc(sizeof(int*) * 8);

	printf("\n");
	initializeField();
	((struct SingleField*)field[0])->status = -1;

	printf("RESULT:%d", goStep(0, 0, 0));

	//
	system("pause");
}

void initializeField()
{
	int* resultingPointer = (int*)malloc(sizeof(int*) * 8);
	field = (struct SingleField**) malloc(sizeof(int*) * 64);
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			short directionsCount = 0;

			for (int dir = 0; dir < 16; dir += 2)
			{
				short nextX = x + directions[dir];
				short nextY = y + directions[dir + 1];
				if (isInBounds(nextX, nextY))
				{
					resultingPointer[directionsCount++] = (nextX + 8 * nextY);
				}
			}
			struct SingleField* recentField = malloc(sizeof(struct SingleField));
			recentField->neighbourCount = directionsCount;
			recentField->posX = x;
			recentField->posY = y;
			recentField->status = 0;
			recentField->FirstNeighbour = (short*)malloc(directionsCount*sizeof(short*));
			//Initialisierung erfolgt zwei Zeilen später

			field[x + 8 * y] = recentField;
			for (short i = 0; i < directionsCount; i++)
			{
				recentField->FirstNeighbour[i] = (int)resultingPointer[i];
				//printf("[%d|%x|%d]\n", x + 8 * y, &field[x + 8 * y], (((struct SingleField*)field[x + 8 * y])->FirstNeighbour[0]));
			}

		}
	}
	for (int i = 0; i < 64; i++)
	{
		printf("[%d|%x|%x]\n", i, &field[i], (((struct SingleField*)field[i])->FirstNeighbour[0]));
	}
}


bool isInBounds(short x, short y)
{
	return (-1 < x && 8 > x&&-1 < y && 8 > y);
}

bool goStep(short x, short y, short count)
{
	if (count == 63)
	{
		((struct SingleField*)field[(x + 8 * y)])->status = count;
		return true;
	}
	printfields();
	struct SingleField* currentField = ((struct SingleField*)field[(x + 8 * y)]);
	if (count>0)currentField->status = count;
	short neighbourCount = currentField->neighbourCount;

	//Array for sorting, will contain the amount of further possible fields
	short *neighbours = (short*)malloc(currentField->neighbourCount*sizeof(short));
	short smallestAmountofNeighbours = 999, smallestNeighboursIndex = 0;

	// forwarded array, representing Index at field of currentfields's neighbours
	short* firstNeighbour = currentField->FirstNeighbour;
	//printf("Neighbours:");
	for (short i = 0; i < neighbourCount; i++)
	{
		// forwarded neighbour
		struct SingleField* neighbour = field[firstNeighbour[i]];
		if (neighbour->status != 0)
		{
			// 0 means it's not possible to jump there
			neighbours[i] = 0;
			continue;
		}
		// 1 means it's possible to jump there, but there are NO free fields possible from there
		neighbours[i] = 1;
		short neighboursNeighbourMaxCount = neighbour->neighbourCount;
		for (short ii = 0; ii < neighboursNeighbourMaxCount; ii++)
		{
			if (neighbour->status)
			{
				neighbours[i]++;
			}
		}
		//printf(" %d ", neighbours[i]);
		if (smallestAmountofNeighbours > neighboursNeighbourMaxCount)
		{
	//		printf("\n %d > %d", smallestAmountofNeighbours, neighbours[i]);
			smallestAmountofNeighbours = neighboursNeighbourMaxCount;//neighbours[i];
			smallestNeighboursIndex = i;
		}
	}
	//printf("\nSmallestIdx: %d SmallestAmount: %d", smallestNeighboursIndex, smallestAmountofNeighbours);
	if (smallestAmountofNeighbours == 999){
		currentField->status = 0;
		return false;
	}
	struct SingleField* nextField = field[firstNeighbour[smallestNeighboursIndex]];// firstNeighbour[smallestNeighboursIndex];

	//printf("\n%2d|%2d,%2d -> %2d,%2d | %2d\n", count, x, y,nextField->posX,nextField->posY,nextField->status);
	if (goStep(nextField->posX, nextField->posY, count + 1))return true;
	neighbours[smallestNeighboursIndex] = 0;

	for (short k = 0; k < neighbourCount; k++)
	{
		for (short i = 0; i < neighbourCount; i++)
		{
			if (!neighbours[i]){
				continue;
			}
			if (smallestAmountofNeighbours == neighbours[i])
			{
				if (goStep(field[firstNeighbour[i]]->posX, field[firstNeighbour[i]]->posY, count + 1))
				{
					return true;
				}
				neighbours[i] = 0;
			}
		}
		smallestAmountofNeighbours++;
	}
	currentField->status = 0;
	return false;
}

void printfields()
{
	system("cls");
	printf("\n\n");
	for (short x = -1; x < 8; x++)
	{
		printf("\n[%3d]", x);
		for (short y = 0; y < 8; y++)
		{
			if (x < 0){
				printf("[%3d]", y);
				continue;
			}
			printf(" %3d ", field[x + 8 * y]->status);
		}
	}
}
