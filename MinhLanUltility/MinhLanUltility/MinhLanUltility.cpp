// MinhLanUltility.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <djvector2.h>
#include <djlink.h>

float CalculateDistanceToBase(const DJVector2& A, const DJVector2& B, const DJVector2& C, const float screenWidth)
{
	if (B.y() == A.y())
	{
		return ((C.x() - A.x())/(screenWidth - A.x()));
	}
	/*for explanation*
	float AH = B.y() - A.y();
	float AD = C.y() - A.y();
	float BH = A.x() - B.x();
	float ED = (AD)*(BH)/(AH);
	float DC = C.x() - A.x();
	float EC = ED + DC;
	return EC/screenWidth;
	/**/
	float EC = ((C.y() - A.y())*(A.x() - B.x())/(B.y() - A.y())) + (C.x() - A.x());
	return (EC/(screenWidth - C.x() + EC));
}
DJLinkedList<DJVector2> GetLessAsteroid(DJLinkedList<DJVector2> enemyPositions, const djint32 asteroidRadius, DJVector2 screenSize)
{
	DJLinkedList<DJVector2> asteroidPositions;
	if (asteroidRadius <= 0) return asteroidPositions;
	djint32 asteroidDiameter = 2*asteroidRadius;
	int rowNumber = (int)djCeiling(screenSize.y() / asteroidDiameter);
	int columnNumber = (int)djCeiling(screenSize.x() / asteroidDiameter);
	//DJDebug("row: %d  column: %d", rowNumber, columnNumber);
	int arraySize = rowNumber*columnNumber;
	int* markedArray = new int[arraySize];
	for (int i =0; i<arraySize; i++) markedArray[i] = 0;
	DJLinkedListIter<DJVector2> iter(enemyPositions);
	DJVector2* currentPosition;
	int currentColumn;
	int currentRow;
	while (currentPosition = iter.Get())
	{
		currentColumn = (int)djFloor(currentPosition->x() / asteroidDiameter);
		currentRow = (int)djFloor(currentPosition->y() / asteroidDiameter);
		markedArray[currentRow*columnNumber + currentColumn] = 1;
		iter.Next();
	}
	int asteroidCount = 0;
	for (int i =0; i<arraySize; i++)
	{
		if (markedArray[i])
		{
			asteroidCount++;
			currentColumn = i % columnNumber;
			currentRow = i / columnNumber;
			//DJDebug("%d: [%d,%d]", asteroidCount, rowNumber, columnNumber);
		}
	}
	delete (markedArray);
	return asteroidPositions;
}
int _tmain(int argc, _TCHAR* argv[])
{

	return 0;
}

