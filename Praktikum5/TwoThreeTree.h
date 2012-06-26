// Autor: Stefan Wiesmann nach einem Skript von Steffen Lange
//////////////////////////////////////////////////////////////////////

#pragma once

#include "TreeElement.h"

class TwoThreeTree
{
public:
	TwoThreeTree(void);
	~TwoThreeTree(void);

	void browse();
	void insertAngle(int angle, int edge);

	void setBlockingEdge(int startAngle, int endAngle, int edgeNumber);
	int getEdgeForAngle(int angle);
	int getEdgeForAngleFinal(int angle);
	
	void deleteAngle(int angle);	 // only for test in public

private:
	TreeElement * root;

	TreeElement * searchAngle(int angle, bool * founded);
	TreeElement * searchAngle(TreeElement * r, int angle, bool * founded);
	
	void normalizeTreeAdd(TreeElement * v);
	void normalizeTreeDel(TreeElement * v);
	void adaptKeys(TreeElement * p, int angle);
	void adaptKeysReduce(TreeElement * p, int angle);
	TreeElement * findNextLeaf(TreeElement * sourceLeaf);
	void browse(TreeElement * te, int tab);
};

