#include <iostream>
#include "tsp.h"
#include "node.h"
#include <queue>
#include "CompareNode.h"
#include <string>
#include <sstream>

using namespace std;

double Tsp::getLowestHVal()
{
	return lowestHVal;
}

void Tsp::setLowestNode(int** temp)
{
	copyMap(temp, lowestNode.map);
	lowestNode.hValue = hFunc(temp);
	lowestHVal = lowestNode.hValue;
}

Node Tsp::getLowestNode()
{
	return lowestNode;
}

void Tsp::printResult()
{
	for(int i=0;i<length;i++)
	{
		for(int j=0;j<length;j++)
		{
			cout << lowestNode.map[i][j] << " ";
		}
		cout << endl;
	}

	stringstream result2;
	int numberOfRow = length;
	int previousRow =0;
	int currentRow =0;
	while(numberOfRow != 0)
	{
		for(int g=0;g<length;g++)
		{
			if(lowestNode.map[currentRow][g] == 1 && previousRow != g)
			{
				result2 << g+1 << " ";
				previousRow = currentRow;
				currentRow = g;
				numberOfRow--;
				break;
			}			
		}
	}

	cout << result2.str() << endl;
	cout << "cost: " << lowestNode.hValue << endl;
	
}

void Tsp::copyMap(int **curr, int **child)
{
	for(int i=0;i<length;i++)
	{
		for(int j=0;j<length;j++)
		{
			child[i][j] = curr[i][j];
		}
	}
}

void Tsp::run()
{
	int ran = 0;
	while(_pq->empty() != true)
	{
		ran++;
		Node* currentNode = _pq->top();
		_pq->pop();
		popChildren(currentNode);
	}


}

void Tsp::popChildren(Node *node)
{	
			//load current map condition		
			Node* currentNode = node;
			//If current node is lower than lowest heuristic value, generate below, else prune. 
			if(currentNode->hValue < lowestHVal)
			{
				//create new nodes for left and right
				Node *leftNode = new Node;
				leftNode->map = new int*[length];
				for(int i = 0; i < length; i++)
						leftNode->map[i] = new int[length];


				Node *rightNode = new Node;	
				rightNode->map = new int*[length];
				for(int i = 0; i < length; i++)
						rightNode->map[i] = new int[length];			
				
				//load their map conditions
				copyMap(currentNode->map, leftNode->map);
				copyMap(currentNode->map, rightNode->map);				
				
				int saveRow = 0;									
				
				//check if the node is a last node.
				
				if(isFinal(currentNode->map))
				{					
					
					if(currentNode->hValue < lowestHVal)
					{
						lowestHVal = currentNode->hValue;
						lowestNode.hValue = lowestHVal;
						copyMap(currentNode->map, lowestNode.map);
					}
					delete leftNode;
					delete rightNode;
				}
				else
				{
					//Create two node with constraints					
					
					bool changeMade = false; //to check any changes have made.
					int numberOfOne = 0; //to check how many 1 in a row.
					//find the current map has a row with one 1.
					
					//Checking any row consists only 1 one. and save the index of that row.
					//this is to not make a cycle consists not all the cities. 
					for(int i=0;i<length;i++)
					{
						numberOfOne=0;
						for(int j=0;j<length;j++)
						{
							if(numberOfOne == 2)
							{
								break;
							}							
							if(currentNode->map[i][j]==1)
							{
								numberOfOne++;								
							}
						}
						
						if(numberOfOne == 1)
						{
							saveRow = i;							
							break;
						}						
					}
					
					

					//if any row consists only 1 one, only generates the index of the row that consists only 1 one.
					if(numberOfOne == 1)
					{							
						for(int i=0;i<length;i++)
						{
							//finds the first 0.
							if(currentNode->map[saveRow][i]==0)
							{
								//set the constraint of left child
								leftNode->map[saveRow][i]=1;
								leftNode->map[i][saveRow]=1;
								
								//set the constraint of right child
								rightNode->map[saveRow][i]=-1;
								rightNode->map[i][saveRow]=-1;
								
								//verify left child and right child
								verify(leftNode->map);
								
								verify(rightNode->map);
								
								//calculate the heuristic value of left child and right child
								leftNode->hValue = hFunc(leftNode->map);
								rightNode->hValue = hFunc(rightNode->map);

								//put them into the priority queue
								_pq->push(leftNode);
								_pq->push(rightNode);
								break;
							}
										
						}
					}
					else
					{
						//Case where the current condition has no row with only 1 one.
						for(int i = 0;i<length;i++)
						{		
							numberOfOne = 0;
							for(int j = 0;j<length;j++)
							{	
								//if the current row has 2 ones, then skip to the next row. 
								if(currentNode->map[i][j]==1)
								{
									numberOfOne++;
									if(numberOfOne == 2)
									{
										numberOfOne = 0;
										break;
									}
								}							
								else if(currentNode->map[i][j]==0) //if finds 0(first available slot)
								{						
									
									changeMade=true;
																		
									//set the constraint of left child.
									leftNode->map[i][j] = 1;
									leftNode->map[j][i] = 1;
									
									
									//set the constraint of right child
									rightNode->map[i][j] = -1;
									rightNode->map[j][i] = -1;		
									
									//verify them
									verify(leftNode->map);
									verify(rightNode->map);
									
									//set heuristic value for them
									leftNode->hValue=hFunc(leftNode->map);								
									rightNode->hValue=hFunc(rightNode->map);
									
									//add them into priority queue
									_pq->push(leftNode);
									_pq->push(rightNode);								
									
									break;						
								}									
							}						
							
							//if change has made, break and go pop the next node
							if(changeMade)
								break;
							
						}		
					
					}
					
				}		
			}
			else
			{
				//System.out.println("Pruned");
			}		

}



double Tsp::hFunc(int** current)
{
		double sum = 0;	
		int min1;
		int min2;
		int numberOfOne;	
		
		for(int i=0;i<length;i++)		
		{
			min1=9999999;
			min2=9999999;
			numberOfOne = 0;
			
			//in each row, get the minimum values. If the cell has 1, this has to be the minimum.
			for(int j=0;j<length;j++)
			{							
				//if 1 is found in a row
				if(current[i][j] == 1)
				{
					if(numberOfOne == 0)
					{
						min1 = originalMap->map[i][j]; //first 1 is saved into min1
					}
					else if(numberOfOne == 1)
					{
						min2 = originalMap->map[i][j]; //second 1 is saved into min2
					}
					numberOfOne++;	//increment when it sees the 1
				}
				else if(current[i][j] == 0)
				{
					//if numberOf1 is 0 in a current row
					if(numberOfOne == 0)
					{						
						//put the min values
						if(min1 > originalMap->map[i][j])
						{
							min1 = originalMap->map[i][j];
						}
						else if(min2 > originalMap->map[i][j])
						{
							min2 = originalMap->map[i][j];
						}
						
						//if min1 has lower value, swap the min1 and min2.						
						if(min1 < min2)
						{
							int tempo = min2;
							min2 = min1;
							min1 = tempo;
						}
						
					}
					//if numberOf1 is 1 in a current row, save only into min2
					else if(numberOfOne == 1)
					{
						if(min2 > originalMap->map[i][j])
						{
							min2 = originalMap->map[i][j];
						}
					}
					
				}				
			}
			//after finding the minimum of each row
			
			//add them into sum
			sum = sum+min1+min2;			
		}		
		
		//return sum/2
	return sum/2;
	
}

bool Tsp::isFinal(int **map)
{

	for(int i=0;i<length;i++)
		{
			for(int j =0;j<length;j++)
			{
				if(map[i][j]==0)
				{
					return false;
				}
			}
		}
		
	return true;

}

void Tsp::verify(int **map)
{

	bool changeMade=true;
		while(changeMade==true)
		{
			//set nothing has changed
			changeMade = false;
			
			
			int numberOfCompletedRow = 0; //count the number of completed rows
			int savedRowWithOne1[2];

			int counter = 0;

			for(int i=0;i<length;i++)
			{
				
				int availableOne = 2; // number of available ones
				int numberOfZero = 0; // number of zeros
				
				for(int j=0; j<length; j++)
				{	
					//if see 1, decrement availableOne
					if(map[i][j]==1)
					{
						availableOne--;						
						
					}
					//if see 0, increment numberOfZero
					if(map[i][j]==0)
					{
						numberOfZero++;
					}				
				
				}				
	 
				
				//if two edges are already selected, set the rest as unavailable.
				if(availableOne == 0)
				{
					numberOfCompletedRow++; //increment the completed row
					
					//set the rest as unavailable.
					for(int j=0;j<length;j++)
					{
						if(map[i][j]==0)
						{

							map[i][j]=-1;
							map[j][i]=-1;
							changeMade=true; //set change has made

						}
					}
					
				}				
				else if(availableOne == numberOfZero) //if available edges are the same as edges that can be assigned
				{
					//make all the available edges to be selected.
					for(int j=0;j<length;j++)
					{
						if(map[i][j]==0)
						{

							map[i][j]=1;
							map[j][i]=1;
							availableOne--;
							changeMade=true;//set change has made

						}
					}				
					
				}
				
				//if the row with the number of one is 1, save and increment counter
				if(availableOne == 1)
				{
					savedRowWithOne1[counter] = i;
					counter++;
				}
			}
			
			//This makes sure there cannot be a cycle if the completed row is bigger than 1
			//and less then cities - 1.
			//meaning that if there are 6 Cities: A,B,C,D,E,F,
			//then if we have C A B D C, this doesn't cover all the cities. This shouldn't happen.

			if(numberOfCompletedRow > 0 && numberOfCompletedRow< length-1)
			{
	 		
				if(savedRowWithOne1[0] < length && savedRowWithOne1[0] >= 0 && savedRowWithOne1[1] < length && savedRowWithOne1[1] >= 0)
				{				
					//so from the above condition,
					//if C A B D, then C and D shouldn't connect each other.
					if(map[savedRowWithOne1[0]][savedRowWithOne1[1]] == 0)
					{

						map[savedRowWithOne1[0]][savedRowWithOne1[1]] = -1;
						changeMade = true;


					}
					
					if(map[savedRowWithOne1[1]][savedRowWithOne1[0]] == 0)
					{

						map[savedRowWithOne1[1]][savedRowWithOne1[0]] = -1;
						changeMade = true;

					}
				}		
			}
			
		}
}
