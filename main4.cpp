#include <iostream>
#include "node.h"
#include <queue>
#include <string>
#include "CompareNode.h"
#include "tsp.h"
#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <sys/timeb.h>

using namespace std;

int getMilliCount(){
	timeb tb;
	ftime(&tb);
	int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
	return nCount;
}

int getMilliSpan(int nTimeStart){
	int nSpan = getMilliCount() - nTimeStart;
	if(nSpan < 0)
		nSpan += 0x100000 * 1000;
	return nSpan;
}


int main(int argc, char* argv[]){

	/*int c[10][10]={{-1,5,7,6,1,4,5,3,7,7},
					{5,-1,9,9,8,1,4,6,8,9},
					{7,9,-1,10,2,9,4,5,5,6},
					{6,9,10,-1,6,10,8,4,3,10},
					{1,8,2,6,-1,4,9,8,6,7},
					{4,1,9,10,4,-1,5,10,5,9},
					{5,4,4,8,9,5,-1,6,9,10},
					{3,6,5,4,8,10,6,-1,6,7},
					{7,8,5,3,6,5,9,6,-1,1},
					{7,9,6,10,7,9,10,7,1,-1}};*/

					int c[13][13] = {{-1, 24, 29, 22, 34, 85, 21, 67, 20, 34, 26, 68, 98},
									 {24, -1, 39, 83, 55, 40, 26, 49, 68, 89, 42, 43,  9},
									 {29, 39, -1, 83, 21, 28, 62, 23, 19, 18,  4, 15, 10},
									 {22, 83, 83, -1, 78, 99, 29, 43, 49, 21, 10, 39, 22},
									 {34, 55, 21, 78, -1, 91, 70, 51, 27,  1, 16, 64,  6},
									 {85, 40, 28, 99, 91, -1, 4, 67, 56, 91, 63, 16, 40},
									 {21, 26, 62, 29, 70,  4, -1, 98, 69,  3, 19, 79, 22},
									 {67, 49, 23, 43, 51, 67, 98, -1, 73, 50,  2, 31, 87},
									 {20, 68, 19, 49, 27, 56, 69, 33, -1,  2, 13, 75, 45},
									 {34, 89, 18, 21,  1, 91,  3, 22, 71, -1, 33, 32, 66},
									 {26, 42,  4, 10, 16, 63, 19,  2, 13, 33, -1, 92, 25}, 
									 {68, 43, 15, 39, 64, 16, 79,  2, 75, 32, 92, -1, 44},
									 {98,  9, 10, 22,  6, 40, 22, 87, 45, 66, 25, 44, -1}};

	int length = sizeof(*c) / sizeof(int);				

	int myid, numprocs, temp[length][length];	

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);

	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	int start, end;
	if(myid == 0)
	{
		start = getMilliCount();
	}
	MPI_Request req, send_req[numprocs], recv_req[numprocs];
	MPI_Status stat[numprocs], stats;

	Node *node = new Node;
	
	priority_queue<Node*, vector<Node*>, CompareNode> *pq = new priority_queue<Node*, vector<Node*>, CompareNode>;	

	//Assign first dimension
	node->map = new int*[length];
	//Assign second dimension
	for(int i = 0; i < length; i++)
		node->map[i] = new int[length];		

	for(int i=0;i<length;i++)
	{
		for(int j=0;j<length;j++)
		{
			node->map[i][j]=c[i][j];		
		}		
	}
	
	Tsp *tsp = new Tsp(node,pq, length); //map, pq, and length

	if(myid==0)
	{
		node = new Node; //origin condition;
		node->map = new int*[length];	
		for(int i = 0; i < length; i++)
			node->map[i] = new int[length];	
		
		for(int i=0;i<length;i++)
		{
			for(int j=0;j<length;j++)
			{
				if(i==j)
				{
					node->map[i][j]=-1;
				}
				else
				{
					node->map[i][j]=0;	
				}			
				
			}			
			
		}	
		node->hValue = tsp->hFunc(node->map);	
	
		pq->push(node);
		int count = numprocs - 1;
		while(count > 0)
		{
			count--;
			node = pq->top();
			pq->pop();
			tsp->popChildren(node);
		}
		
		for(int i=1;i<numprocs;i++)
		{

			node = pq->top();
			pq->pop();
			int temp[length][length];

			for(int row=0;row<length;row++)
			{
				for(int col=0;col<length;col++)
				{
					temp[row][col] = node->map[row][col];
				}
			}
			
			
			MPI_Send(&temp, length*length, MPI_INT, i, 0, MPI_COMM_WORLD);
		}		

	}

	if(myid != 0)
		MPI_Recv(&temp, length*length,MPI_INT, 0, 0, MPI_COMM_WORLD, 0);
	
	node = new Node;
		node->map = new int*[length];	
		for(int i = 0; i < length; i++)
			node->map[i] = new int[length];
		
	for(int i=0;i<length;i++)
	{
		for(int j=0;j<length;j++)
		{
			node->map[i][j] = temp[i][j];				
		}						
	}

	node->hValue = tsp->hFunc(node->map);		
	pq->push(node);

	while(pq->empty() == false)
	{
	
		int flag = 0;
		MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &stats);
		while(flag == 1)
		{
			

			MPI_Recv(&temp, length*length,MPI_INT, stats.MPI_SOURCE, 0, MPI_COMM_WORLD, &stats);				
			int **temp2 = new int*[length];
			for(int c=0; c<length;c++)
				temp2[c] = new int[length];
			
			//cout << "receiving " << myid << " from " << stats.MPI_SOURCE << endl;
			for(int row=0;row<length;row++)
			{
				for(int col=0;col<length;col++)
				{
					temp2[row][col] = temp[row][col];
				
				}			
				
			}
			//cout << tsp->hFunc(temp2) << endl;

			//cout << "compare " << myid << ": "<< tsp->getLowestNode().hValue << " and " << stats.MPI_SOURCE<< ": " << tsp->hFunc(temp2) << endl;
			if(tsp->hFunc(temp2) < tsp->getLowestNode().hValue)
			{			
			//	cout << "take " << tsp->hFunc(temp2) << endl;
				tsp->setLowestNode(temp2);	
			//	cout << "confirm " << tsp->getLowestHVal() << endl;
			}
			 
			MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &stats);

		}	
		

		Node* currentNode = pq->top();		
		pq->pop();


		if(tsp->isFinal(currentNode->map))
		{
			
			if(currentNode->hValue < tsp->getLowestHVal())
			{
				for(int c=0;c<numprocs;c++)
				{
					
					//cout << "SENDING " << myid << " " << currentNode-> hValue << endl;
					for(int row=0;row<length;row++)
					{
						for(int col=0;col<length;col++)
						{
							temp[row][col] = currentNode->map[row][col];						
						
						}
						
					}

					MPI_Isend(&temp, length*length, MPI_INT, c, 0, MPI_COMM_WORLD, &send_req[c]);

				}
				
			}			
			
		}
	
		tsp->popChildren(currentNode);
	
	}


	MPI_Barrier(MPI_COMM_WORLD);
	
	if(myid == 0)
	{	
		int flag =0;
		MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &stats);
		while(flag == 1)
		{
			
			MPI_Recv(&temp, length*length,MPI_INT, stats.MPI_SOURCE, 0, MPI_COMM_WORLD, &stats);				
			int **temp2 = new int*[length];
			for(int c=0; c<length;c++)
				temp2[c] = new int[length];
			
			//cout << "receiving " << myid << " from " << stats.MPI_SOURCE << endl;
			for(int row=0;row<length;row++)
			{
				for(int col=0;col<length;col++)
				{
					temp2[row][col] = temp[row][col];
				}			
			}

			//cout << tsp->hFunc(temp2) << endl;

			//cout << "compare " << myid << ": "<< tsp->getLowestNode().hValue << " and " << stats.MPI_SOURCE<< ": " << tsp->hFunc(temp2) << endl;
			if(tsp->hFunc(temp2) < tsp->getLowestNode().hValue)
			{			
			//	cout << "take " << tsp->hFunc(temp2) << endl;
				tsp->setLowestNode(temp2);	
			//	cout << "confirm " << tsp->getLowestHVal() << endl;
			}
			 
			MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &stats);

		}	
		
		cout << "Result !" << endl;
		tsp->printResult();	
		end=getMilliSpan(start);
		printf("Elapsed time = %u millisecond\n", end);
	}	

	MPI_Finalize();

	delete tsp;
	delete node;	
	delete pq;
	return 0;
	
}