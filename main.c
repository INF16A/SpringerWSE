#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


struct SingleField
{
	/*
	 0 = unused
	-1 = Startfield
	 n = jump nr.
	*/
	short posX;
	short posY;
	short status;
	short neighbourCount;
	struct SingleField **FirstNeighbour;
//	struct SingleField *neighbours[];
};

struct SingleField *field[8][8];

short directions[16]=
{	1,2,
	1,-2,
	-1,-2,
	-1,2,

	2,1,
	2,-1,
	-2,-1,
	-2,1};

void initializeField();
bool goStep(short,short,short);
bool isInBounds(short,short);
void printfields();

int main(void)
{
	printf("\n");
	initializeField();
	(field[0][0])->status =-1;
	goStep(0,0,0);
}

void initializeField()
{
    struct SingleField*** resultingPointer = malloc(sizeof(int*) * 8);
	for(int x=0;x<8;x++)
	{
		for (int y = 0; y < 8; y++)
		{
			short directionsCount = 0;

			for (int dir = 0; dir < 16; dir += 2)
			{
				short nextX = x + directions[dir];
				short nextY = y + directions[dir + 1];
				if (isInBounds(nextX, nextY))
				{
					resultingPointer[directionsCount++] = &field[nextX][nextY];
				}
			}
			printf("\t%d", directionsCount);
			//realloc((void *)resultingPointer,directionsCoun*sizeof(int *));
			//realloc(resultingPointer, directionsCount * sizeof(int));
			//SingleField definieren
			//malloc(sizeof(struct SingleField **),;
			//resultingPointer=realloc(resultingPointer,directionsCount*sizeof(struct SingleField **));
			struct SingleField recentField = {x,y,0,directionsCount};//Initialisierung erfolgt zwei Zeilen später
			field[x][y] = memcpy(malloc(sizeof(recentField)), &recentField, sizeof(recentField));
			field[x][y]-> FirstNeighbour = memcpy( malloc(directionsCount*sizeof(int*)), resultingPointer, directionsCount*sizeof(int*));
		}
		printf("\n");
	}
}


bool isInBounds(short x,short y)
{
	return (-1<x&&8>x&&-1<y&&8>y);
}

bool goStep(short x, short y,short count)
{
	if(count == 63)
	{
		field[x][y]->status = count;
		return true;
	}
	printfields();
	struct SingleField* currentField = field[x][y];
	currentField->status = count;
	short neighbourCount=currentField->neighbourCount;
	short *neighbours = malloc(currentField->neighbourCount*sizeof(short));
	short smallestAmountofNeighbours=999, smallestNeighboursIndex=0;
	struct SingleField** firstNeighbour=currentField->FirstNeighbour;
	for(short i =0; i< neighbourCount;i++)
	{
		struct SingleField* neighbour= firstNeighbour[i];
		if(neighbour->status>0)
		{
			neighbours[i]=0;
			continue;
		}
		neighbours[i]=1;
		short neighboursNeighbourMaxCount=neighbour->neighbourCount;
		for( short ii=0;ii<neighboursNeighbourMaxCount;ii++)
		{
			if(neighbour->status)
			{
				neighbours[i]++;
			}
		}
		if(smallestAmountofNeighbours>neighbours[i])
		{
			smallestAmountofNeighbours=neighbours[i];
			smallestNeighboursIndex=i;
		}
	}

	struct SingleField* nextField = (struct SingleField*) firstNeighbour[smallestNeighboursIndex];
	if(goStep(nextField->posX, nextField->posY,count+1))return true;
	neighbours[smallestNeighboursIndex]=0;

	for(short k =0; k <neighbourCount; k++ )
	{
		for(short i = 0;i<neighbourCount;i++)
		{
			if(!neighbours[i]){
				continue;
			}
			if(smallestAmountofNeighbours==neighbours[i])
			{
				if(goStep( ((struct SingleField*)firstNeighbour[i])->posX, ( (struct SingleField*)firstNeighbour[i])->posY ,count+1))
                    {return true;}
				neighbours[i]=0;
			}
		}
		smallestAmountofNeighbours++;
	}
	currentField->status = 0;
	return false;
}

void printfields()
{
    for ( short i = 0 ; i< 8 ; i ++)
    {
        for ( short ii = 0 ; ii < 8; ii++)
        {
            printf("Feld an der Stelle X:%d Y:%d hat den Status: %d den Neigbourcount: %d und sein erster Nachbar liegt bei %d\n",
         field[i][ii]->posX, field[i][ii]->posY, field[i][ii]->status, field[i][ii]->neighbourCount, field[i][ii]->FirstNeighbour);

        }
    }
}


