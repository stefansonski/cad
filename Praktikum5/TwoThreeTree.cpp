// Autor: Stefan Wiesmann nach einem Skript von Steffen Lange
//////////////////////////////////////////////////////////////////////

#include "TwoThreeTree.h"
#include <iostream>
#include <iomanip>
#define DEBUG false
using namespace std;

TwoThreeTree::TwoThreeTree(void)
{
	root = 0;
}

TwoThreeTree::~TwoThreeTree(void)
{}

void TwoThreeTree::browse()
{
	cout << "----------------------- 2-3 Baum ------------------------------" << endl;
	if (root != 0) 
		browse(root,0);
	cout << "---------------------------------------------------------------" << endl;
}

/* Einfuegen
- suche den Schlüssel x und markiere die besuchten inneren Knoten
- bestimme den Vater v des Blatts, bei dem bei der Suche gestoppt wurde, 
	und füge links bzw. rechts davon ein Blatt mit dem Schlüssel x ein
- falls v drei Söhne hat, so folge dem Weg zur Wurzel und passe die Schlüssel an
- falls v vier Söhne hat, so spalte v in zwei innere Knoten, passe die Schlüssel dieser neuen Knoten an 
	und mache beide Knoten zu Söhnen des Vaters v‘ von v; anschließend gehe zu v‘ 
	- falls v‘ gar nicht existiert, neue Wurzel erzeugen
	- falls v‘ drei Söhne hat, folge dem Weg zur Wurzel und passe die Schlüssel an
	- falls v‘ vier Söhne hat, setze rekursiv fort
*/
void TwoThreeTree::insertAngle(int angle, int edge) //------------------------------------------------------------------------------------
{
	TreeElement * found;
	bool founded;
	TreeElement * n = new TreeElement(angle, edge); 
	if (DEBUG) cout << "Einzufuegendes Element: angle=" << angle << endl;
	
	if (root == 0) // special case if root empty
	{
		root = new TreeElement();
		root->first = n;
		root->setAngle(angle); // here smarter than adaptKeys
		root->numberChilds = 1;

		n->parent = root;	
		n->whichChildIam = 1;
	}
	else // no empty root
	{
		found = searchAngle(angle, &founded);
	    if (founded)
			found->setEdge(edge); // fuer unsere Anwendung kann hier der neue Kantenwert eingetragen werden
		
		else // not founded
		{
			if (DEBUG) cout << "Gefundenes Element: angle=" << found->getAngle() << endl;
			
			TreeElement * v = found->parent;
			n->parent = v;
			
			if (v->numberChilds == 1) // only one child (in the root)!
			{
				if (v->first->getAngle() < angle) // n will be append right
				{
					v->second = n;
					n->last = v->first;
					v->first->next = n;
					n->whichChildIam = 2;
					adaptKeys(v, angle); // passe Schluessel an //WI
				}
				else // n have to append left
				{
					v->second = v->first;
					v->second->whichChildIam = 2;
					v->first = n;
					n->last = v->second->last;
					n->next = v->second;
					n->whichChildIam = 1;
					v->second->last = n;
				}
				v->numberChilds = 2;
			}

			else if (v->numberChilds == 2) // third child can append without problems
			{
				if (DEBUG) cout << "Knoten hat bisher nur zwei Kinder" << endl;

				if (v->first->getAngle() > angle) // n have to append absolutly left
				{
					v->third = v->second;
					v->third->whichChildIam =3;
					v->second = v->first;
					v->second->whichChildIam = 2;
					v->first = n;
					n->last = v->second->last;
					n->next = v->second;
					n->whichChildIam = 1;
					if (v->second->last != 0)
						v->second->last->next = n;
					v->second->last = n;					
				}
				else if (v->second->getAngle() > angle) // n have to append in the middle
				{
					v->third = v->second;
					v->third->whichChildIam = 3;
					v->second = n;
					n->last = v->third->last;
					n->next = v->third;
					n->whichChildIam = 2;
					v->first->next = n;
					v->third->last = n;	
				}
				else // n will be the third child
				{
					v->third = n;
					n->last = v->second;
					n->next = v->second->next;
					n->whichChildIam = 3;
					v->second->next = n;	
					adaptKeys(v,angle); //WI
				}
				v->numberChilds = 3;
			}	
			else // v has three childs already; temporaly we make a fourth child
			{
				if (DEBUG) cout << "Knoten hat bereits drei Kinder" << endl;

				if (v->first->getAngle() > angle) // n have to append absolutly left
				{
					v->fourth = v->third;
					v->fourth->whichChildIam = 4;
					v->third = v->second;
					v->third->whichChildIam = 3;
					v->second = v->first;
					v->second->whichChildIam = 2;
					v->first = n; 
					n->last = v->second->last;
					n->next = v->second;
					n->whichChildIam = 1;
					if (v->second->last != 0)
						v->second->last->next = n;
					v->second->last = n; 
				}
				else if (v->second->getAngle() > angle) // n will be the second child
				{
					v->fourth = v->third;
					v->fourth->whichChildIam = 4;
					v->third = v->second;
					v->third->whichChildIam = 3;
					v->second = n;
					n->last = v->third->last;
					n->next = v->third;
					n->whichChildIam = 2;
					v->first->next = n;
					v->third->last = n;	
				}
				else if (v->third->getAngle() > angle) // n will be the third child
				{
					v->fourth = v->third;
					v->fourth->whichChildIam = 4;
					v->third = n;
					n->last = v->fourth->last;
					n->next = v->fourth;
					n->whichChildIam = 3;
					v->fourth->last = n;
					v->second->next = n;
				}
				
				else // n will be append as fourth child
				{
					v->fourth = n;
					n->last = v->third;
					n->next = v->third->next;
					n->whichChildIam = 4;
					v->third->next = n;	
				}
				v->numberChilds = 4;
				normalizeTreeAdd(v);
			} // end parent has already three childs
		} // end not founded
	} // end no root
}
				
void TwoThreeTree::setBlockingEdge(int startAngle, int endAngle, int edgeNumber) //--------------------------------------------------------
{
	if (endAngle < startAngle)
	{
		setBlockingEdge(startAngle, 360, edgeNumber);
		setBlockingEdge(0, endAngle, edgeNumber);
	}
	else
	{
		cout << "\nBlockingEdge von " << startAngle << " to " << endAngle << endl;
		
		int edgeNumberRight = -1;

		bool existingAngle = false;
		TreeElement * t;

		t = searchAngle(root, startAngle, &existingAngle);
		if (existingAngle)
		{
			if (DEBUG) cout << "Startwinkel vorhanden" << endl;
		}
		else
		{
			if (DEBUG) cout << "Startwinkel NICHT vorhanden" << endl;
			edgeNumberRight = getEdgeForAngle(t->getAngle());
			insertAngle(startAngle, edgeNumberRight);
		}
		
		t = searchAngle(root, endAngle, &existingAngle);
		if (existingAngle)
		{
			if (DEBUG) cout << "Endwinkel vorhanden" << endl;
			t->setEdge(edgeNumber);
		}
		else
		{
			if (DEBUG) cout << "Endwinkel NICHT vorhanden" << endl;
			insertAngle(endAngle, edgeNumber);
		}

		t = searchAngle(root, startAngle, &existingAngle);
		t = findNextLeaf(t);
		do 
		{
			if ( t!= 0 && t->getAngle() < endAngle)
			{
				int w = t->getAngle();
				if (DEBUG) cout << "geloescht werden soll Winkel " << w << endl;
				t = findNextLeaf(t);
				deleteAngle(w);
			}
		} while ( t != 0 && t->getAngle() < endAngle);
	}
	if (DEBUG) browse();
}

int TwoThreeTree::getEdgeForAngle(int angle) //-----------------------------------------------------------------------------------------
{
	TreeElement * t;
	bool founded;
	t = searchAngle(root, angle, &founded);
	return t->getEdge();
}

// Korrektur, um bei Treffen eines Winkels genau -1 zurückzugeben
int TwoThreeTree::getEdgeForAngleFinal(int angle) //-------------------------------------------------------------------------------------
{
	TreeElement * t;
	bool founded;
	t = searchAngle(root, angle, &founded);
	if (DEBUG) cout << "founded= " << founded << endl;
	if (DEBUG) cout << "t->Angle= " << t->getAngle() << endl;
	if (founded)
		return -1;
	else
		return t->getEdge();
}

void TwoThreeTree::normalizeTreeAdd(TreeElement * v) //-----------------------------------------------------------------------------------
{
	if (DEBUG) cout << "\nAufruf normalizeTreeAdd" << endl;
	
	TreeElement * v1 = new TreeElement();
	v1->first = v->first;
	v1->first->parent = v1;
	v1->first->whichChildIam = 1;
	if (DEBUG) cout << "in normalizeTreeAdd: v firstAngle= " << v->first->getAngle() << endl;
	v1->second = v->second;
	v1->second->parent = v1;
	v1->second->whichChildIam = 2;
	if (DEBUG) cout << "in normalizeTreeAdd: v secondAngle= " << v->second->getAngle() << endl;
	v1->numberChilds = 2;
	if (DEBUG) cout << "in normalizeTreeAdd: v1 mit 2 Kindern" << endl;
	adaptKeys(v1,v1->second->getAngle());
				
	TreeElement * v2 = new TreeElement();
	v2->first = v->third;
	v2->first->parent = v2;
	v2->first->whichChildIam = 1;
	v2->second = v->fourth;
	v2->second->parent = v2;
	v2->second->whichChildIam = 2;
	v2->numberChilds = 2;
	if (DEBUG) cout << "in normalizeTreeAdd: v2 mit 2 Kindern" << endl;
	adaptKeys(v2,v2->second->getAngle());

	if (DEBUG) cout << "Zwei neue Knoten angelegt" << endl;

	TreeElement * vStrich = v->parent;
		
	if (vStrich == 0) // v' don't exist
	{
		vStrich = new TreeElement();
		vStrich->first = v1;
		v1->parent = vStrich;
		v1->whichChildIam = 1;
		vStrich->second = v2;
		v2->parent = vStrich;
		v2->whichChildIam = 2;
		vStrich->numberChilds = 2;
		if (DEBUG) cout << "in normalizeTreeAdd: vStrich existiert nicht" << endl;
		adaptKeys(v2,v2->second->getAngle());
		this->root = vStrich;
	}
	else
	{
		if (DEBUG) cout << "numberChild in vStrich= " << vStrich->numberChilds << endl;		
		// delete v from vStrich
		if (v->whichChildIam == 1)
		{
			vStrich->first = vStrich->second;
			vStrich->first->whichChildIam = 1;
			vStrich->second = vStrich->third;
			if (vStrich->second != 0)
				vStrich->second->whichChildIam = 2;
			vStrich->third = 0;
		}
		else if (v->whichChildIam == 2)
		{
			vStrich->second = vStrich->third;
			if (vStrich->second != 0)
				vStrich->second->whichChildIam = 2;
			vStrich->third = 0;
		}
		else if (v->whichChildIam == 3)
		{
			vStrich->third = 0;
		}
		
		vStrich->numberChilds = vStrich->numberChilds-1;
		
		delete v;
		
		if (vStrich->numberChilds == 1) // insert two new elements without problems
		{
			if (DEBUG) cout << "drei Kinder in v'" << endl;
			if (vStrich->first->getAngle() < v1->getAngle()) // append right both elements
			{
				vStrich->second = v1;
				v1->whichChildIam = 2;
				vStrich->third = v2;
				v2->whichChildIam = 3;
			}
			else if (vStrich->first->getAngle() < v2->getAngle()) // v1 before and v2 behind existing element
			{
				vStrich->second = vStrich->first;
				vStrich->second->whichChildIam = 2;
				vStrich->first = v1;
				v1->whichChildIam = 1;
				vStrich->third = v2;
				v2->whichChildIam = 3;
			}
			else // v1 and v2 append absolutly left
			{
				vStrich->third = vStrich->first;
				vStrich->third->whichChildIam = 3;
				vStrich->first = v1;
				v1->whichChildIam = 1;
				vStrich->second = v2;
				v2->whichChildIam = 2;
			}
			v1->parent = vStrich;
			v2->parent = vStrich;
			vStrich->numberChilds = 3;
			if (DEBUG) cout << "in normalizeTreeAdd: numberChilds =1" << endl;
			adaptKeys(vStrich,vStrich->third->getAngle());
		}
		else // vStrich has already two childs; now we will have four childs
		{
			if (DEBUG) cout << "vier Kinder in v'" << endl;
			if (vStrich->first->getAngle() > v2->getAngle()) // append absolutly left, v1 will be first, v2 the second child
			{
				if (DEBUG) cout << "in normalizeTreeAdd: Case 1" << endl; 
				vStrich->third = vStrich->first;
				vStrich->third->whichChildIam = 3;
				vStrich->fourth = vStrich->second;
				vStrich->fourth->whichChildIam = 4;
				vStrich->first = v1;
				v1->whichChildIam = 1;
				vStrich->second = v2;
				v2->whichChildIam = 2;
			}
			else if (vStrich->second->getAngle() < v1->getAngle()) // append absolutly right, v1 will be third, v2 the fourth child
			{
				if (DEBUG) cout << "in normalizeTreeAdd: Case 2" << endl; 
				vStrich->third = v1;
				v1->whichChildIam = 3;
				vStrich->fourth = v2;
				v2->whichChildIam = 4;
			}
			else if (vStrich->first->getAngle() < v1->getAngle()
				     && vStrich->second->getAngle() < v2->getAngle()) // v1 will be the second and v2 the fourth child
			{
				if (DEBUG) cout << "in normalizeTreeAdd: Case 3" << endl; 
				vStrich->third = vStrich->second;
				vStrich->third->whichChildIam = 3;
				vStrich->second = v1;
				v1->whichChildIam = 2;
				vStrich->fourth = v2;
				v2->whichChildIam = 4;
			}
			else if (vStrich->first->getAngle() < v1->getAngle()
				     && vStrich->second->getAngle() > v2->getAngle()) // v1 will be the second and v2 the third child
			{
				if (DEBUG) cout << "in normalizeTreeAdd: Case 4" << endl; 	
				vStrich->fourth = vStrich->second;
				vStrich->fourth->whichChildIam = 4;
				vStrich->second = v1;
				v1->whichChildIam = 2;
				vStrich->third = v2;
				v2->whichChildIam = 3;
			}
			else if (vStrich->first->getAngle() > v1->getAngle()
				     && vStrich->second->getAngle() > v2->getAngle()) // v1 will be the first and v2 the third child
			{
				if (DEBUG) cout << "in normalizeTreeAdd: Case 5" << endl; 	
				vStrich->fourth = vStrich->second;
				vStrich->fourth->whichChildIam = 4;
				vStrich->second = vStrich->first;
				vStrich->second->whichChildIam = 2;
				vStrich->first = v1;
				v1->whichChildIam = 1;
				vStrich->third = v2;
				v2->whichChildIam = 3;
			}
			else if (vStrich->first->getAngle() > v1->getAngle()
				     && vStrich->second->getAngle() < v2->getAngle()) // v1 will be the first and v2 the fourth child
			{
				if (DEBUG) cout << "in normalizeTreeAdd: Case 6" << endl; 	
				vStrich->third = vStrich->second;
				vStrich->third->whichChildIam = 3;
				vStrich->second = vStrich->first;
				vStrich->second->whichChildIam = 2;
				vStrich->first = v1;
				v1->whichChildIam = 1;
				vStrich->fourth = v2;
				v2->whichChildIam = 4;
			}
			v1->parent = vStrich;
			v2->parent = vStrich;
			vStrich->numberChilds = 4;
			if (DEBUG) cout << "in normalizeTreeAdd: numberChilds in vStrich=4" << endl;
			adaptKeys(vStrich,vStrich->fourth->getAngle());
			if (DEBUG) cout << "\nRekursiver Aufruf normalizeTreeAdd" << endl;
			normalizeTreeAdd(vStrich);
		}
	}
}

/*  Suche:
-beginne bei der Wurzel
- es seien y1, y2 und y3 die in den Söhnen gespeicherten Schlüssel
	- falls die Söhne Blätter sind, so prüfe, ob x = y1, x = y2 bzw.x = y3 gilt
	- falls ja, stoppe und gib „gefunden“ zurück 
	- falls nein,so stoppe beim Sohn mit dem Schlüssel y1 (y1 > x),	beim Sohn mit dem Schlüssel y2, (y2 > x), bzw.
		y3 (sonst) und gib „nicht gefunden“ zurück)
- falls die Söhne keine Blätter sind, gehe wie folgt vor:
	- falls x ≤ y1, so suche rekursiv im linken Teilbaum weiter
	- falls y1 < x ≤ y2, so suche im mittleren Teilbaum weiter
	- falls y2 < x, so suche im rechten Teilbaum weiter
*/
TreeElement * TwoThreeTree::searchAngle(int angle, bool * founded) //-----------------------------------------------------------------------
{
	if (root == 0)
	{
		*founded = false;
		return 0;
	}
	return searchAngle(root, angle, founded);
}

TreeElement * TwoThreeTree::searchAngle(TreeElement * r, int angle, bool * founded) //------------------------------------------------------
{
	if (DEBUG) cout << "searchAngle: angle=" << angle << endl;
	*founded = false;
	
	if (r->first != 0) 
		if (r->first->isLeaf())
		{
			if(r->first->getAngle() == angle) // y1 == x
			{
				*founded = true;
				return r->first;
			}
			else if (r->first->getAngle() > angle) // y1 > x
				return r->first;
		}
		else if (r->first->getAngle() >= angle || r->numberChilds == 1) // x <= y1
			return searchAngle(r->first, angle, founded);
		
	if (r->second != 0) 
	{	
		if (r->second->isLeaf())
		{
			if(r->second->getAngle() == angle) // y2 == x
			{
				*founded = true;
				return r->second;
			}
			else if (r->second->getAngle() > angle) // y2 > x
				return r->second;
		}
		else if (r->second->getAngle() >= angle || r->numberChilds == 2)
			return searchAngle(r->second, angle, founded);
	}
	else
	{
		if (DEBUG) cout << "zweites Blatt ist 0" << endl;
		return r->first;
	}
	
	if (r->third != 0) 
	{
		if (r->third->isLeaf())
		{
			if(r->third->getAngle() == angle) // y3 == x
			{
				*founded = true;
				return r->third;
			}
			else 
				return r->third; // sonst
		}
		else 
			return searchAngle(r->third, angle, founded); // sonst
	}
	else
	{
		if (DEBUG) cout << "drittes Blatt ist 0" << endl;
		return r->second;
	}
}

/* Delete
- suche den Schlüssel x und markiere die besuchten inneren Knoten
- bestimme den Vater v des Blatts mit dem Schlüssel x und lösche dieses Blatt
- falls v zwei Söhne hat, so folge dem Weg zur Wurzel und passe die Schlüssel an
- falls v einen Sohn hat, prüfe ob ein Bruder von v drei Söhne hat
- falls ja, stehle einen Sohn vom Bruder, mache ihn zu einem Sohn von v, folge dem Weg zur Wurzel und passe die Schlüssel an
- falls nein, verschmelze v mit einem seiner Brüder, passe den Schlüssel von v an und gehe zum Vater v‘ von v
- falls v‘ zwei Söhne hat, folge dem Weg zur Wurzel und passe die Schlüssel an
- falls v‘ einen Sohn hat, setze rekursiv fort (falls v‘ die Wurzel ist, so lösche v‘)
*/
void TwoThreeTree::deleteAngle(int angle) //-----------------------------------------------------------------------------------------------
{
	if (DEBUG) cout << "Zu loeschendes Element: angle=" << angle << endl;
	
	TreeElement * found;
	bool founded;

	found = searchAngle(angle, &founded);
	if (!founded)
	{
		cout << "zu loeschendes Element mit angle= " << angle << " nicht vorhanden!" << endl;
		return;
	}
	else //bestimme den Vater v des Blatts mit dem Schlüssel x und lösche dieses Blatt
	{
		TreeElement * v = found->parent;
		
		if (found->whichChildIam == 1)
		{
			v->first = v->second;
			v->first->whichChildIam = 1;
			v->second = v->third;
			if (v->second != 0)
				v->second->whichChildIam = 2;
		}
		else if (found->whichChildIam == 2)
		{
			v->second = v->third;
			if (v->second != 0)
				v->second->whichChildIam = 2;
		}
		v->third = 0;
			
		v->numberChilds--;
		if (DEBUG) cout << "nun NumberChilds von v=" << v->numberChilds << endl;
		
		found->last->next = found->next;
		if (found->next != 0)
			found->next->last = found->last;
		delete found;

		if (v->numberChilds == 2) //falls v zwei Söhne hat, so folge dem Weg zur Wurzel und passe die Schlüssel an
		{
			v->setAngle(v->second->getAngle());
			adaptKeys(v->second, v->second->getAngle());
		}
	    else //falls v nur einen Sohn hat, prüfe ob ein Bruder von v drei Söhne hat usw.
		{
			v->setAngle(v->first->getAngle());
			adaptKeys(v, v->first->getAngle());
			if (v->parent != 0)
				normalizeTreeDel(v);
		}
	}
}

void TwoThreeTree::normalizeTreeDel(TreeElement * v) //------------------------------------------------------------------------------------
{
	if (DEBUG) cout << "\nAufruf normalizeTreeDel" << endl;
	
	TreeElement * vStrich = v->parent;
	
	switch (v->whichChildIam)
	{
	case 1: // then the brother of v is the second in vStrich, but v the first
	{
		if (DEBUG) cout << "der 2. Bruder wird geprueft numberChilds=" << vStrich->second->numberChilds << endl;
		
		if (vStrich->second != 0 && vStrich->second->numberChilds == 3) //falls ja, stehle einen Sohn vom Bruder, mache ihn zu einem Sohn von v
		{
			v->second = vStrich->second->first;
			v->second->whichChildIam = 2;
			v->second->parent = v;
			v->numberChilds = 2;
			vStrich->second->first = vStrich->second->second;
			vStrich->second->first->whichChildIam = 1;
			vStrich->second->second = vStrich->second->third;
			vStrich->second->second->whichChildIam = 2;
			vStrich->second->third = 0;
			vStrich->second->numberChilds = 2;
			adaptKeys(v, v->second->getAngle()); //folge dem Weg zur Wurzel und passe die Schlüssel an
			return;
		}
		break;
	}
	case 2: // here v has two brothers, the first and possibly the third
	{
		if (DEBUG) cout << "der 1. Bruder wird geprueft numberChilds=" << vStrich->first->numberChilds << endl;
		
		if (vStrich->first->numberChilds == 3) //falls ja, stehle einen Sohn vom Bruder, mache ihn zu einem Sohn von v
		{
			v->second = v->first;
			v->second->whichChildIam = 2;
			v->first = vStrich->first->third;
			v->first->whichChildIam = 1;
			v->first->parent = v;
			v->numberChilds = 2;
			vStrich->first->third = 0;
			vStrich->first->numberChilds = 2;
			
			adaptKeysReduce(vStrich->first, vStrich->first->second->getAngle()); //folge dem Weg zur Wurzel und passe die Schlüssel an //WI
			
			adaptKeys(v, v->second->getAngle()); //folge dem Weg zur Wurzel und passe die Schlüssel an
			return;
		}
		else if (vStrich->third != 0)
			if (vStrich->third->numberChilds == 3)
			{
				if (DEBUG) cout << "der 3. Bruder hat 3 Kinder" << endl;
				v->second = vStrich->third->first;
				v->second->whichChildIam = 2;
				v->second->parent = v;
				v->numberChilds = 2;
				vStrich->third->first = vStrich->third->second;
				vStrich->third->first->whichChildIam = 1;
				vStrich->third->second = vStrich->third->third;
				vStrich->third->second->whichChildIam = 2;
				vStrich->third->third = 0;
				vStrich->third->numberChilds = 2;
				adaptKeys(v, v->second->getAngle()); //folge dem Weg zur Wurzel und passe die Schlüssel an
				return;
			}
		break;
	}
	case 3: // then v's brother is the second in vStrich, but v is the third in vStrich
	{
		if (DEBUG) cout << "der 2. Bruder wird geprueft numberChilds=" << vStrich->second->numberChilds << endl;
		
		if (vStrich->second != 0 && vStrich->second->numberChilds == 3) //falls ja, stehle einen Sohn vom Bruder, mache ihn zu einem Sohn von v
		{
			v->second = v->first;
			v->second->whichChildIam = 2;
			v->first = vStrich->second->third;
			v->first->whichChildIam = 1;
			v->first->parent = v;
			v->numberChilds = 2;
			vStrich->second->third = 0;
			vStrich->second->numberChilds = 2;
			adaptKeys(v, v->second->getAngle()); //folge dem Weg zur Wurzel und passe die Schlüssel an
			return;
		}
		break;
	}
	} // end switch
	
	// falls nein, verschmelze v mit einem seiner Brüder, passe den Schlüssel von v an und gehe zum Vater v‘ von v
	
	switch (v->whichChildIam)
	{
	case 1:
	{
		if (DEBUG) cout << "verschmelze v mit rechtem Bruder" << endl;
	
		vStrich->first = vStrich->second;
		vStrich->first->whichChildIam = 1;
		vStrich->second = vStrich->third;
		if (vStrich->second != 0)
			vStrich->second->whichChildIam = 2;
		vStrich->third = 0;
		vStrich->numberChilds--;
		vStrich->first->third = vStrich->first->second;
		vStrich->first->third->whichChildIam = 3;
		vStrich->first->second = vStrich->first->first;
		vStrich->first->second->whichChildIam =2;
		vStrich->first->first = v->first;
		vStrich->first->first->whichChildIam = 1;
		vStrich->first->first->parent = vStrich->first;
		vStrich->first->numberChilds++;
		delete v;
		adaptKeys(vStrich->first, vStrich->first->first->getAngle()); //folge dem Weg zur Wurzel und passe die Schlüssel an
		break;
	}
	case 2:
	{
		if (DEBUG) cout << "verschmelze v mit linkem Bruder" << endl;
			
		vStrich->second = vStrich->third;
		if (vStrich->second != 0)
			vStrich->second->whichChildIam = 2;
		vStrich->third = 0;
		vStrich->numberChilds--;
		vStrich->first->third = v->first;
		vStrich->first->third->whichChildIam = 3;
		vStrich->first->third->parent = vStrich->first;
		vStrich->first->numberChilds++;
		delete v;
		adaptKeys(vStrich->first, vStrich->first->third->getAngle()); //folge dem Weg zur Wurzel und passe die Schlüssel an
		break;
	}
	case 3:
	{
		if (DEBUG) cout << "verschmelze v mit linkem Bruder" << endl;
			
		vStrich->third = 0;
		vStrich->numberChilds--;
		vStrich->second->third = v->first;
		vStrich->second->third->whichChildIam = 3;
		vStrich->second->third->parent = vStrich->second;
		vStrich->second->numberChilds++;
		delete v;
		adaptKeys(vStrich->second, vStrich->second->third->getAngle()); //folge dem Weg zur Wurzel und passe die Schlüssel an
	}
	} // end switch
	
	if (DEBUG) cout << "Anzahl der Kinder in v' =" << vStrich->numberChilds << endl;
	
	if (vStrich->numberChilds < 2) // falls v‘ einen Sohn hat, setze rekursiv fort (falls v‘ die Wurzel ist, so lösche v‘)
	{
		if (vStrich->parent == 0) // falls v‘ die Wurzel ist, so lösche v‘
		{
			if (DEBUG) cout << "jetzt sind wir in der Wurzel" << endl;

			root = vStrich->first;
			root->parent = 0;
			delete vStrich;
			return;
		}
		if (DEBUG) cout << "\nRekursiver Aufruf normalizeTreeAdd" << endl;
		normalizeTreeDel(vStrich);
	}
}

/* Adapt Key
 - wenn der Winkel des neu hinzugefügten Kind-Knotens grösser ist als der Winkel des Vaters, muss dieser rekursiv
   auf diesen Wert angepasst werden
*/
void TwoThreeTree::adaptKeys(TreeElement * p, int angle) //------------------------------------------------------------------------------
{
	if (DEBUG) 
	{
		cout << "in adaptKeys: angle=" << angle << " p->getAngle= " << p->getAngle() << endl;
		system("Pause");
	}
	if (p->getAngle() < angle)
	//if (p->getAngle() != angle) //WI
		p->setAngle(angle);
	
	if (p->parent != 0)
		adaptKeys(p->parent, angle);
}

void TwoThreeTree::adaptKeysReduce(TreeElement * p, int angle) //-------------------------------------------------------------------------
{
	if (DEBUG) 
	{
		cout << "in adaptKeysReduce: angle=" << angle << " p->getAngle= " << p->getAngle() << endl;
		system("Pause");
	}
	if (p->getAngle() != angle)
		p->setAngle(angle);
	
	if (p->parent != 0 && p->whichChildIam == p->parent->numberChilds-1)
		adaptKeysReduce(p->parent, angle);
}

TreeElement * TwoThreeTree::findNextLeaf(TreeElement * source) //-------------------------------------------------------------------------
{
	return source->next;
}

void TwoThreeTree::browse(TreeElement * te, int tab) //-----------------------------------------------------------------------------------
{
	if (te == 0) return;
	
	if (te->isLeaf())
	{
		te->browseLeaf(tab);
		return;
	}

	if (te->first != 0)
		browse(te->first, tab+8);
		
	te->browse(tab);
	
	if (te->second != 0)
		browse(te->second, tab+8);
	else
		cout << setw(tab+8) << setfill(' ') << " " << " nul\n";
	cout << endl;	
	
	if (te->third != 0) 
		browse(te->third, tab+8);
	else
		cout << setw(tab+8) << setfill(' ') << " " << " nul\n";
}