// Autor: Stefan Wiesmann nach einem Skript von Steffen Lange
//////////////////////////////////////////////////////////////////////

/*
a) jeder innere Knoten hat mindestens zwei und höchstens drei Söhne
b) alle Blätter haben dieselbe Tiefe
c) jedes Blatt enthält einen Schlüssel, wobei die Schlüssel den
Blättern in aufsteigender Reihenfolge von links nach rechts
zugewiesen sind
d) jeder innere Knoten enthält den größten Schlüssel, der in dem an
diesem Knoten beginnenden Teilbaum gespeichert ist
*/
#pragma once
class TreeElement
{
public:
	TreeElement();
	TreeElement(int angle, int edge);
	~TreeElement(void){}

	int getAngle();
	int getEdge();
	void setAngle(int angle);
	void setEdge(int edge);
	void browse(int tab);
	void browseLeaf(int tab);

private:
	TreeElement * first;
	TreeElement * second;
	TreeElement * third;
	TreeElement * fourth;
	TreeElement * parent;
	TreeElement * last;
	TreeElement * next;

	int angle;
	int edge;

	// only for a better code
	int numberChilds;
	int whichChildIam;

	bool isLeaf();

friend class TwoThreeTree;
};

