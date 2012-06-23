// Autor: Stefan Wiesmann nach einem Skript von Steffen Lange
//////////////////////////////////////////////////////////////////////
#include "TreeElement.h"
#include <iostream>
#include <iomanip>

using namespace std;
using namespace Tree;

TreeElement::TreeElement()
{
              first = second = third = fourth= parent = last = next = 0;
              numberChilds = whichChildIam= 0;             
//eingefuegt WI 19062012
              this->angle = -1;
}

TreeElement::TreeElement(int angle, int edge)
{
	this->angle = angle;
	this->edge = edge;
	first = second = third = fourth = parent = last = next = 0;
	numberChilds = whichChildIam = 0;
}

int TreeElement::getAngle()
{
	return this->angle;
}

int TreeElement::getEdge()
{
	return this->edge;
}

void TreeElement::setAngle(int angle)
{
	this->angle = angle;
}

void TreeElement::setEdge(int edge)
{
	this->edge = edge;
}

bool TreeElement::isLeaf()
{
	//if (this->first == 0 && this->second == 0 && this->third == 0)
	if (this->numberChilds == 0)	
		return true;
	else
		return false;
}

void TreeElement::browse(int tab, std::ofstream& outFile)
{
	outFile << setiosflags(ios::left);
	outFile << setw(tab) << setfill(' ') << " " << setw(3) << angle  << "[" << numberChilds << "]" << endl;
}

void TreeElement::browseLeaf(int tab, std::ofstream& outFile)
{
	int a = 0;
	if (next != 0)
		a = next->getAngle();
	outFile << setiosflags(ios::left);
	outFile << setw(tab) << setfill(' ') << " " << setw(3) << angle << "(" << edge << ")" << "{-->" << a << "}" << endl;
}

