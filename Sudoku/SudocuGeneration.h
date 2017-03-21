#pragma once


class SudocuGeneration
{
public:
	SudocuGeneration();
	~SudocuGeneration();
		
	int generationMas(int **data);


private:

	int checkCell(int cell);
				
};

