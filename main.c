#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

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
    short* FirstNeighbour;
    //    struct SingleField *neighbours[];
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
    int* resultingPointer =(int*)malloc(sizeof(int*) * 8);
    int** resultingPointerPointer = (int**)malloc(sizeof(int*) * 8);
    struct SingleField** resultingPointerStruct = (struct SingleField**)malloc(sizeof(int*) * 8);

    printf("\n");
    initializeField();
    ((struct SingleField*)field[0])->status = -1;

    printf("RESULT:%d", goStep(0, 0, 0));

    system("PAUSE");
}

void initializeField()
{
    //struct SingleField***
        int* resultingPointer =/* (SingleField***)*/(int*)malloc(sizeof(int*) * 8);
        //struct SingleField*** resultingPointerStruct = (SingleField***)malloc(sizeof(int*) * 8);
    field =(struct SingleField**) malloc(sizeof(int*) * 64);
    for (int i = 0; i < 64; i++)
    {
        //printf("[%d|%d]\n", i, &field[i]);
    }
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
                    //printf("[%d|%d]->%d | %x \t [%d|%d]->%d | %x \n", x, y, x + y 8, &field[x + 8 y], nextX, nextY, nextX + 8 nextY, &field[nextX + 8 nextY]);
                    //resultingPointer[directionsCount++] = ((SingleField**) &field[nextX+8*nextY]);

                    resultingPointer[directionsCount++] = /*(SingleField**)*/ (nextX + 8 * nextY);// (field[nextX + 8 * nextY]);
                    //resultingPointerStruct[directionsCount] =(SingleField**) (nextX + 8 * nextY);
            /*        printf("Next:%d | Cast1:%d| Cast2:%d | BCast:%d | rInt:%d | rStruct:%d \n",
                        nextX + 8 * nextY,
                        (SingleField*)(nextX + 8 * nextY),
                        (SingleField**)(nextX + 8 * nextY),
                        (int)((SingleField**)(nextX + 8 * nextY)),
                        resultingPointer[directionsCount],
                        resultingPointerStruct[directionsCount]);*/
                    //directionsCount++;
                }
            }
            //printf("\t%d", directionsCount);
            //realloc((void *)resultingPointer,directionsCoun*sizeof(int *));
            //realloc(resultingPointer, directionsCount * sizeof(int));
            //SingleField definieren
            //malloc(sizeof(struct SingleField **),;
            //resultingPointer=realloc(resultingPointer,directionsCount*sizeof(struct SingleField **));
            struct SingleField* recentField = malloc(sizeof(struct SingleField));
            //{ x, y, 0, directionsCount };
            recentField->neighbourCount = directionsCount;
            recentField->posX = x;
            recentField->posY = y;
            recentField->status = 0;
            recentField->FirstNeighbour = (short*)malloc(directionsCount*sizeof(short*));
        //Initialisierung erfolgt zwei Zeilen später
            //field[(x+8*y)] =(SingleField**) memcpy(malloc(sizeof(recentField)), &recentField, sizeof(recentField));
            //((SingleField*)field[(x+8*y)])->FirstNeighbour = (SingleField**)memcpy(malloc(directionsCount*sizeof(int*)), &resultingPointer, directionsCount*sizeof(int*));
            //field[x+8*y] = (SingleField**)memcpy(malloc(sizeof(recentField)), &recentField, sizeof(recentField));


            //((SingleField*)field[x+8*y])->FirstNeighbour =;
            field[x + 8 * y] = recentField;
            for (short i = 0; i< directionsCount; i++)
            {

((struct SingleField*)field[x + 8 * y])->FirstNeighbour[i] = (int)resultingPointer[i];
                printf("[%d|%x|%d]\n", x + 8 * y, &field[x + 8 * y], (((struct SingleField*)field[x + 8 * y])->FirstNeighbour[0]));
                /*printf("[%d|%d]->%d | %x \t %x \t %x \n",
                    x, y,
                    x + y * 8,
                    &field[x + 8 * y],
                    i,
                    ((SingleField*)field[x + 8 * y])->FirstNeighbour[i],
                    *resultingPointer[i],
                    resultingPointer[i]);*/
            }

        }
        //printf("\n");
    }
    for (int i = 0; i < 64; i++)
    {
        printf("[%d|%x|%x]\n", i, &field[i], (((struct SingleField*)field[i])->FirstNeighbour[0]));
    }
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            for (int i = 0; i <((struct SingleField*) field[(x + 8 * y)])->neighbourCount; i++)
            {
                //printf("Feld:[%d|%d], Neighbour %d:[%d|%d]\n", ((SingleField*)field[(x+8*y)])->posX, ((SingleField*)field[(x+8*y)])->posY, ((SingleField*)field[(x+8*y)])->FirstNeighbour[i]->posX, ((SingleField*)field[(x+8*y)])->FirstNeighbour[i]->posY);
            }
        }
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
    currentField->status = count;
    short neighbourCount = currentField->neighbourCount;

    //Array for sorting
    short *neighbours = (short*)malloc(currentField->neighbourCount*sizeof(short));
    short smallestAmountofNeighbours = 999, smallestNeighboursIndex = 0;
    //struct SingleField* firstNeighbour = field[(short)(currentField->FirstNeighbour)];


    // forwarded array, representing Index at field of currentfields's neighbours
    short* firstNeighbour = currentField->FirstNeighbour;
    for (short i = 0; i < neighbourCount; i++)
    {
        struct SingleField* neighbour = field[firstNeighbour[i]];
        if (neighbour->status > 0)
        {
            neighbours[i] = 0;
            continue;
        }
        neighbours[i] = 1;
        short neighboursNeighbourMaxCount = neighbour->neighbourCount;
        for (short ii = 0; ii < neighboursNeighbourMaxCount; ii++)
        {
            if (neighbour->status)
            {
                neighbours[i]++;
            }
        }
        if (smallestAmountofNeighbours > neighbours[i])
        {
            smallestAmountofNeighbours = neighbours[i];
            smallestNeighboursIndex = i;
        }
    }

    struct SingleField* nextField = field[firstNeighbour[smallestNeighboursIndex]];// firstNeighbour[smallestNeighboursIndex];
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
                if (goStep(field[firstNeighbour[i]]->posX,field[ firstNeighbour[i]]->posY, count + 1))
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
    printf("\n\n");
    for (short x = -1; x < 8; x++)
    {
        printf("\n[%3d]",x);
        for (short y = 0; y < 8; y++)
        {
            if (x<0){
                printf("[%3d]",y);
                continue;
            }
            printf(" %3d ", field[x + 8 * y]->status);
        /*    printf("Feld an der Stelle X:%d Y:%d hat den Status: %d den Neigbourcount: %d und sein erster Nachbar liegt bei %d\n",

((struct SingleField*)field[(x+8*y)])->posX,
                ((struct SingleField*)field[(x + 8 * y)])->posY,
                ((struct SingleField*)field[(x + 8 * y)])->status,
                ((struct SingleField*)field[(x + 8 * y)])->neighbourCount,
                ((struct SingleField*)field[(x + 8 * y)])->FirstNeighbour);*/
        }
    }
}
