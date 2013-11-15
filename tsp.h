#ifndef TSP_H
#define TSP_H

#include "node.h"
#include <queue>
#include "CompareNode.h"
#include <iostream>

using namespace std;


class Tsp{
public:
	Tsp(){};

	Tsp(Node* map, priority_queue<Node*, vector<Node*>, CompareNode> *pq, int leng)
	{
		originalMap = map;
		_pq = pq;
		length =leng;
		lowestNode.map = new int*[length];
	//Assign second dimension
		for(int i = 0; i < length; i++)
			lowestNode.map[i] = new int[length];
		lowestNode.hValue = 999999;	
		lowestHVal = 999999;
	};

	~Tsp()
	{
		delete originalMap;
		
	};

	void printResult();
	void popChildren(Node* node);
	void run();
	double hFunc(int **map);
	bool isFinal(int **map);
	void verify(int **map);
	void copyMap(int **curr, int **child);
	Node getLowestNode();
	void setLowestNode(int **condition);
	double getLowestHVal();

private:
	priority_queue<Node*, vector<Node*>, CompareNode> *_pq;
	Node *originalMap;
	int length;
	double lowestHVal;
	Node lowestNode;

};

#endif