#ifndef CNode_H
#define CNode_H

using namespace std;
class CompareNode{
	public:
		bool operator()(Node* n1, Node* n2)
		{
			if(n1->hValue >= n2->hValue) return true;

			return false;


		}


};

#endif