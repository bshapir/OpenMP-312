#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <iomanip>

using namespace std;



int main()
{
	ifstream inputFile;
	int sizeRow;
	int sizeCol;
	inputFile.open("sample.txt");
	inputFile >> sizeRow;
	inputFile >> sizeCol;
	inputFile.get();

	char *matrix = new char[sizeRow*sizeCol];
	for (int row = 0; row < sizeRow; row++)
	{
		for (int col = 0; col < sizeCol; col++)
		{
			inputFile >> *(matrix + sizeCol * row + col);
		}
	}

	for (int row = 0; row < sizeRow; row++) 
	{
		for (int col = 0; col < sizeCol; col++)
		{
			cout << *(matrix + sizeCol * row + col) << " ";
		}
		cout << endl;
	}

	system("pause");

	delete[]matrix;

		return 0;

		
}
