// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "TwoThreeTree.h"
#include <vector>
#include <algorithm>
#include <math.h>
#include <limits>

#define PI 3.14159265358979323846

using namespace std;
using namespace Tree;

//-----------------------------------------------------------------------------
#define szRDS _RXST("CGCAD")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
//-----------------------------------------------------------------------------



// Typedef --------------------------------------------------------------------
typedef pair<AcGePoint3d, AcGePoint3d> Edge;

// Globale Variablen ----------------------------------------------------------
vector<AcGePoint3d> polyPoints;					// alle Punkte der Polyline p(0) bis p(n-1)
vector<TwoThreeTree> trees;						// zu jedem Punkt ein Tree
vector<Edge> outerEdges;
vector<Edge> innerEdges;
vector<AcGePoint3d> intersections;
vector<AcGePoint3d> finalIntersections;
AcGePoint3d intersectionPoint;

class CPraktikum5App : public AcRxArxApp {

	public:
	CPraktikum5App () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;

		// TODO: Add your initialization code here

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}

	//-----------------------------------------------------------------------------
	// Los gehts!
	//-----------------------------------------------------------------------------
	
	static void CGCADPraktikum5triangularize(void){
		Greedy();
	}

	static void Greedy(){
			
			setPolyPoints();					// Es wird davon ausgegangen, dass tatsächlich eine Polyline existiert
			setTrees();							// initialisiere für jeden Knoten einen Tree mit Winkel 360°
			setOuterAngles();
			setOuterEdges();
			setInnerEdges();
			sortInnerEdges();

			createPolygons();

			drawIntersections();
			drawFinalIntersections();

			housekeeping();
	}

	static void housekeeping() {
		polyPoints.clear();
		trees.clear();
		outerEdges.clear();
		innerEdges.clear();
	}

	static void setPolyPoints(){
		struct resbuf polylineFilter;
		polylineFilter.restype = 0;
		polylineFilter.resval.rstring = _T("LWPOLYLINE");
		polylineFilter.rbnext = NULL;
		
		AcCmColor color;
		color.setRGB(0,150,255);
		createLayer(_T("trianguliert"),color);

		color.setRGB(255,0,0);
		createLayer(_T("crosspoints"),color);

		color.setRGB(0,255,0);
		createLayer(_T("finalCrosspoints"),color);

		ads_name ssname;
		long length = 0;
		int res = acedSSGet(_T("X"), NULL, NULL, &polylineFilter, ssname);
		if(res != RTNORM) return;
		acedSSLength(ssname, &length);
		for(int k = 0; k < length; k++){
			ads_name ent;
			AcDbObjectId id = AcDbObjectId::kNull;
			AcDbEntity* tmp;

			if(acedSSName(ssname,k,ent) != RTNORM) return;
			if(acdbGetObjectId(id,ent) != Acad::eOk) return; 
			if(acdbOpenAcDbEntity(tmp,id,AcDb::kForRead) != Acad::eOk) return;
			
			// gib mir die Polyline
			AcDbPolyline* polyline = (AcDbPolyline*)tmp;
						
			for(int i = 0; i < polyline->numVerts(); i++)	// gehe alle Punkte durch 
			{
				AcGePoint3d pt;
				polyline->getPointAt(i, pt);				// gib mir den punkt im index i
				bool exists = false;
				for(int j = 0; j < polyPoints.size(); j++) {
					if(polyPoints[j].x == pt.x && polyPoints[j].y == pt.y && polyPoints[j].z == pt.z) {
						exists = true;
					}
				}
				if(!exists){
					polyPoints.push_back(pt);					// und steck ihn in polyPoints
				}
				//	TwoThreeTree tree;							
				//	tree.insertAngle(360,0);					// Tree init für jeden Punkt
				//	trees.push_back(tree);						// leg für jeden Punkt Tree mit 0°-360° an
				//}
			}

			polyline->close();
		}
	}

	static void setTrees(){
		for(int i=0; i < polyPoints.size(); i++){
			TwoThreeTree tree;							
			tree.insertAngle(360,0);					// Tree init für jeden Punkt
			trees.push_back(tree);						// leg für jeden Punkt Tree mit 360° an
		}
	}

	static void setOuterAngles(){
		//----------------------------------------------------------------------------------------
		// bestimmt die anfangs und endwinkel eines knotens und fügt sie zu trees hinzu
		//----------------------------------------------------------------------------------------

		for(int i = 0; i < polyPoints.size(); i++){
															
			int previous = i - 1;
			int next = i + 1;
			if(previous < 0)
				previous += polyPoints.size();
			if(next >= polyPoints.size())
				next = 0;

			int angle[2];
															// previous	= kante davor
															// next		= kante danach
															// angle[2]	= winkel von previous = angle[0] und von next = angle[1]

			angle[0] = getAngle(polyPoints, i, previous);	// angle[0] = start winkel
			angle[1] = getAngle(polyPoints, i, next);		// angle[1] = end winkel
			
															
			angle[0] < 0 ? angle[0] += 360: angle[0];		// atan2 liefert zahlen zwischen (-180) - (180)...
			angle[1] < 0 ? angle[1] += 360: angle[1];		// ... daher +=360 wenn negativ
			
			//acutPrintf(_T("polyPoints[%d]: startangle: %d endangle: %d\n"), i, angle[0], angle[1]);
			trees[i].setBlockingEdge(angle[0], angle[1], -1);
		}
		//acutPrintf(_T("________________________________"));
	}

	static void createLayer(const ACHAR* name, AcCmColor &color)
	{
		//database connect
		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		//layer table
		AcDbLayerTable* pLayerTbl = NULL; 

		// Get the LayerTable for write because we will create a new entry 
		pDB->getSymbolTable(pLayerTbl,AcDb::kForWrite); 

		// Check if the layer is already there 
		if (!pLayerTbl->has(name)) // _T for char Unicode convert  
		{ 
			// Instantiate a new object and set its properties 
			AcDbLayerTableRecord *pLayerTblRcd = new AcDbLayerTableRecord; 
			pLayerTblRcd->setName(name); 
			pLayerTblRcd->setIsFrozen(0); // layer set to THAWED 
			pLayerTblRcd->setIsOff(0);    // layer set to ON 
			pLayerTblRcd->setIsLocked(0); // layer un-locked 
			pLayerTblRcd->setColor(color);
			// Now, add the new layer to its container 
			pLayerTbl->add(pLayerTblRcd); 

			// Close the container 
			pLayerTblRcd->close();
			pLayerTbl->close();

		}  
		else  
		{ 
			// If our layer is already there, just close the container and continue 
			pLayerTbl->close(); 
			acutPrintf(_T("\n"));
			acutPrintf(name);
			acutPrintf(_T(" already exists")); //output in the status line 
		}
	}

	static bool compareEdgesBySize(Edge edge1, Edge edge2)
	{
		if(edge1.first.distanceTo(edge1.second) <= edge2.first.distanceTo(edge2.second))
			return true;
		else
			return false;
	}

	static void setOuterEdges(){
		for(int i = 0; i < polyPoints.size(); i++)
		{
			for(int j = i + 1; j < polyPoints.size(); j++)
			{
				if(i + 1 == j || (i == 0 && j == polyPoints.size() - 1))
					outerEdges.push_back(Edge(polyPoints[i], polyPoints[j]));
				else
					continue;
			}
		}
	}
	
	static void setInnerEdges(){
		for(int i = 0; i < polyPoints.size(); i++)
		{
			for(int j = i + 1; j < polyPoints.size(); j++)
			{
				if(i + 1 == j || (i == 0 && j == polyPoints.size() - 1))
					continue;
				else
					innerEdges.push_back(Edge(polyPoints[i], polyPoints[j]));
			}
		}
	}

	static void sortInnerEdges(){
		std::sort(innerEdges.begin(), innerEdges.end(), compareEdgesBySize);
	}

	// ab hier wirds spannend
	static void createPolygons()
	{
		std::vector<Edge> newEdges;

		// angefangen bei der kleinsten innerEdge wird nun geprüft ob das setzen erlaubt ist
		for(int i = 0; i < innerEdges.size() && newEdges.size() < polyPoints.size() - 3; i++){	
			
			// wenn innerEdge[i] erlaubt ist füge sie zu newEdges hinzu
			if(!isEdgeBlocked(innerEdges[i], polyPoints, trees)){
				appendEdge(innerEdges[i], newEdges, polyPoints, trees);					
			}
		}

		drawPolygons(newEdges, outerEdges);
	}

	static void appendEdge(Edge edge, std::vector<Edge> &newEdges, std::vector<AcGePoint3d> &polyPoints, std::vector<TwoThreeTree> &trees){
		
		// fügt die Kante zu newEdges hinzu
		newEdges.push_back(edge);

		/* setze im tree die blockingEdges 
		 * wobei der Winkel von innerEdge[i] zu polyPoint[i] um 1° reduziert wird
		 * damit der Punkt selbst nicht fälschlicher weise auch geblockt wird
		 */
		for(int i = 0; i < trees.size(); i++){
			if(edge.first != polyPoints[i] && edge.second != polyPoints[i]){
				int angle[2];
				bool isMajorAngle = true;
				
				angle[0] = (int)(atan2(edge.first.y - polyPoints[i].y, edge.first.x - polyPoints[i].x) * 180 / PI);
				angle[1] = (int)(atan2(edge.second.y - polyPoints[i].y, edge.second.x - polyPoints[i].x) * 180 / PI);
				angle[0] < 0 ? angle[0] += 360: angle[0];
				angle[1] < 0 ? angle[1] += 360: angle[1];

				double distanceToEdgeFirst = polyPoints[i].distanceTo(edge.first);
				double distanceToEdgeSecond = polyPoints[i].distanceTo(edge.second);

				int tmp = angle[1];
				
				if(angle[0] > angle[1]){
					tmp += 360;
				}
				
				int diff = tmp - angle[0];
				int* startAngle;
				int* endAngle;
				if(diff > 180){
					startAngle = &angle[1];
					endAngle = &angle[0];
				} else{
					startAngle = &angle[0];
					endAngle = &angle[1];
				}

				// Test ob bei aktuellem Knoten eine Kante vor der appendEdge liegt, wenn ja muss der Winkel verkleinert werden
				for(int j = 0; j < outerEdges.size(); j++){
					//angle[0] = Winkel zu IKS.first
					//angle[1] = Winkel zu IKS.second
					
					// wenn aktuellerKnoten-edge.first und aktuellerKnoten-edge.second die Außenkante schneidet angle[0] = angle[1] = 0
					if(hasIntersection(Edge(polyPoints[i], edge.first), outerEdges[j]) && hasIntersection(Edge(polyPoints[i], edge.second), outerEdges[j])){
						angle[0] = angle[1] = 0;
						break;
					}

					if(hasIntersection(Edge(polyPoints[i], edge.first), outerEdges[j])){
						// Wenn die Distanz kleiner ist setze neuen Winkel
						if(polyPoints[i].distanceTo(intersectionPoint) < distanceToEdgeFirst){
							int closerAngle[2];
							closerAngle[0] = (int)(atan2(outerEdges[j].first.y - polyPoints[i].y, outerEdges[j].first.x - polyPoints[i].x) * 180 / PI);
							closerAngle[1] = (int)(atan2(outerEdges[j].second.y - polyPoints[i].y, outerEdges[j].second.x - polyPoints[i].x) * 180 / PI);
							closerAngle[0] < 0 ? closerAngle[0] += 360: closerAngle[0];
							closerAngle[1] < 0 ? closerAngle[1] += 360: closerAngle[1];
							//int tmpCloser = closerAngle[0];
							//if(tmpCloser < *startAngle)
							//	tmpCloser += 360;
							int tmpStart = *startAngle;
							if(*startAngle < *endAngle)
								tmpStart += 360;

							if(tmpStart < closerAngle[0] && closerAngle[0] < *endAngle)
								angle[0] = closerAngle[0];
							else
								angle[0] = closerAngle[1];
						}	
					}
					
					if(hasIntersection(Edge(polyPoints[i], edge.second), outerEdges[j])){
						// Wenn die Distanz kleiner ist setze neuen Winkel
						if(polyPoints[i].distanceTo(intersectionPoint) < distanceToEdgeSecond){
							int closerAngle[2];
							closerAngle[0] = (int)(atan2(outerEdges[j].first.y - polyPoints[i].y, outerEdges[j].first.x - polyPoints[i].x) * 180 / PI);
							closerAngle[1] = (int)(atan2(outerEdges[j].second.y - polyPoints[i].y, outerEdges[j].second.x - polyPoints[i].x) * 180 / PI);
							closerAngle[0] < 0 ? closerAngle[0] += 360: closerAngle[0];
							closerAngle[1] < 0 ? closerAngle[1] += 360: closerAngle[1];
							//int tmpCloser = closerAngle[0];
							//if(tmpCloser < *startAngle)
							//	tmpCloser += 360;
							int tmpStart = *startAngle;
							if(*startAngle < *endAngle)
								tmpStart += 360;

							if(tmpStart < closerAngle[0] && closerAngle[0] < *endAngle)
								angle[1] = closerAngle[0];
							else
								angle[1] = closerAngle[1];
						}
					}
				}

				*startAngle += 1;
				*endAngle -= 1;
				*startAngle > 359 ? *startAngle -= 360: *startAngle;
				*endAngle < 0 ? *endAngle += 360: *endAngle;
				//acutPrintf(_T("polyPoints[%d]: startangle: %d endangle: %d\n"), i, angle[1], angle[0]);
				trees[i].setBlockingEdge(*startAngle, *endAngle, newEdges.size());

				/*
				if(angle[0] < angle[1]){
					angle[0] += 1;
					angle[1] -= 1;
					angle[0] > 359 ? angle[0] -= 360: angle[0];
					angle[1] < 0 ? angle[1] += 360: angle[1];
					trees[i].setBlockingEdge(angle[0], angle[1], newEdges.size());	
				} else if (angle[0] < angle[1]){ 
					angle[0] -= 1;
					angle[1] += 1;
					angle[0] < 0 ? angle[0] += 360: angle[0];
					angle[1] > 359 ? angle[1] -= 360: angle[1];
					trees[i].setBlockingEdge(angle[1], angle[0], newEdges.size());
				}*/

			}
		}
	}

	static void drawPolygons(std::vector<Edge> &innerEdges, std::vector<Edge> &outerEdges){
		
		//database connect
		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		//blocktable init
		AcDbBlockTable* pBlockTable = NULL;
		pDB->getSymbolTable(pBlockTable, AcDb::kForRead);

		//blocktable record init
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);

		pBlockTable->close();

		/*				for(int i = 0; i < polyPoints.size(); i++)
		{
		AcDbFace* face;
		for(int j = 0; j < newEdges.size(); j++)
		{
		if(polyPoints[i] == newEdges[j].first)
		{
		face = new AcDbFace(newEdges[j].first, newEdges[j].second, polyPoints[i == 0 ? (polyPoints.size() - 1) : i - 1], TRUE, TRUE, TRUE);
		face->setLayer(_T("TRIANG"));
		pBlockTableRecord->appendAcDbEntity(face);
		face->close();
		}
		}
		}
		*/

		/*
		 * TODO: statt lines faces verwenden!
		 */
		AcDbLine* line;
		for(int j = 0; j < innerEdges.size(); j++)
		{
			line = new AcDbLine(innerEdges[j].first, innerEdges[j].second);
			line->setLayer(_T("trianguliert"));
			pBlockTableRecord->appendAcDbEntity(line);
			line->close();
			ads_real result;
			//acedGetReal(_T("Block"), &result);
		}


		pBlockTableRecord->close();
	}

	static void drawIntersectionLines(AcGePoint3d startPoint, AcGePoint3d endPoint){
		
		//database connect
		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		//blocktable init
		AcDbBlockTable* pBlockTable = NULL;
		pDB->getSymbolTable(pBlockTable, AcDb::kForRead);

		//blocktable record init
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);

		pBlockTable->close();

		AcDbLine* line;
		for(int j = 0; j < innerEdges.size(); j++)
		{
			line = new AcDbLine(startPoint, endPoint);
			line->setLayer(_T("finalCrosspoints"));
			pBlockTableRecord->appendAcDbEntity(line);
			line->close();
			ads_real result;
			//acedGetReal(_T("Block"), &result);
		}


		pBlockTableRecord->close();
	}

	static void drawIntersections(){
		
		//database connect
		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		//blocktable init
		AcDbBlockTable* pBlockTable = NULL;
		pDB->getSymbolTable(pBlockTable, AcDb::kForRead);

		//blocktable record init
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);

		pBlockTable->close();

		
		AcDbCircle* circle;
		for(int i = 0; i < intersections.size(); i++)
		{
			circle = new AcDbCircle(intersections[i],AcGeVector3d(0,0,1), 5);
			circle->setLayer(_T("crosspoints"));
			pBlockTableRecord->appendAcDbEntity(circle);
			circle->close();
			//ads_real result;
			//acedGetReal(_T("Block"), &result);
		}


		pBlockTableRecord->close();
	}

	static void drawFinalIntersections(){
		
		//database connect
		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();

		//blocktable init
		AcDbBlockTable* pBlockTable = NULL;
		pDB->getSymbolTable(pBlockTable, AcDb::kForRead);

		//blocktable record init
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);

		pBlockTable->close();

		
		AcDbCircle* circle;
		for(int i = 0; i < finalIntersections.size(); i++)
		{
			circle = new AcDbCircle(finalIntersections[i],AcGeVector3d(0,0,1), 5);
			circle->setLayer(_T("finalCrosspoints"));
			pBlockTableRecord->appendAcDbEntity(circle);
			circle->close();
			//ads_real result;
			//acedGetReal(_T("Block"), &result);
		}


		pBlockTableRecord->close();
	}

	static int getAngle(std::vector<AcGePoint3d> &polyPoints, int& current, int& neighbour){
		 return (int)(atan2(polyPoints[neighbour].y - polyPoints[current].y, polyPoints[neighbour].x - polyPoints[current].x) * 180 / PI);
	}

	static bool isEdgeBlocked(Edge &innerEdge, std::vector<AcGePoint3d> &polyPoints, std::vector<TwoThreeTree> &trees){
		/*
		 * suche den anfangspunkt von innerEdge.first
		 * und untersuche ob die innerEdge im blockingAngle des Anfangspunktes liegt
		 */
		for(int i = 0; i < polyPoints.size(); i++){
			
			if(polyPoints[i] == innerEdge.first){

				// gib mir den Winkel der innenkante
				int angle = (int)((atan2(innerEdge.second.y - polyPoints[i].y,innerEdge.second.x - polyPoints[i].x)) * 180 / PI);
				angle < 0 ? angle += 360: angle;

				/* testet am anfangspunkt ob winkel für innerEdge zulässig ist
				 * gibt 0 zurück wenn der Winkel noch frei ist
				 */
				int edge = trees[i].getEdgeForAngle(angle);						
				if(edge != 0){
					return true;
				}
			}
		}

		// wenn Winkel noch frei kann noch ein Schnitt zwischen den Außenkanten vorliegen
		// HIER LIEGT DAS PROBLEM!
		for(int i = 0; i < outerEdges.size(); i++) {
			if(innerEdge.first != outerEdges[i].first && innerEdge.first != outerEdges[i].second && innerEdge.second != outerEdges[i].first && innerEdge.second != outerEdges[i].second) {
				if(hasIntersection(innerEdge, outerEdges[i])) {
						return true;
				}
			}
		}
		return false;
	}

	static bool hasIntersection(Edge &innerEdge, Edge &outerEdge){
		
		// setze homogene koordinaten für schnelleren schnittpunkttest
		AcGePoint3d innerEdgeStartPoint(innerEdge.first);
		innerEdgeStartPoint.z = 1;
		AcGePoint3d innerEdgeEndPoint(innerEdge.second);
		innerEdgeEndPoint.z = 1;

		AcGePoint3d outerEdgeStartPoint(outerEdge.first);
		outerEdgeStartPoint.z = 1;
		AcGePoint3d outerEdgeEndPoint(outerEdge.second);
		outerEdgeEndPoint.z = 1;

		// normale = kreuzprodukt 2er punkte auf der geraden
		AcGeVector3d innerEdgeNormal = crossProduct(innerEdgeStartPoint, innerEdgeEndPoint);
		AcGeVector3d outerEdgeNormal = crossProduct(outerEdgeStartPoint, outerEdgeEndPoint);

		AcGeVector3d crossPoint = crossProduct(outerEdgeNormal, innerEdgeNormal);

		if(crossPoint.z == 0){
			return false;
		}

		// homogener schnittpunkt mit z = 1
		crossPoint.x = crossPoint.x / crossPoint.z;
		crossPoint.y = crossPoint.y / crossPoint.z;
		crossPoint.z = 1;

		// 
		// intersections.push_back(AcGePoint3d(crossPoint.x, crossPoint.y, 0));

		/*ads_real t;
		if(innerEdgeEndPoint.x - innerEdgeStartPoint.x != 0)
			t = (crossPoint.x - innerEdgeStartPoint.x) / (innerEdgeEndPoint.x - innerEdgeStartPoint.x);
		else if(innerEdgeEndPoint.y - innerEdgeStartPoint.y != 0)
			t = (crossPoint.y - innerEdgeStartPoint.y) / (innerEdgeEndPoint.y - innerEdgeStartPoint.y);
		
		// liegt Punkt auf der innerEdge && liegt der Punkt nicht im Polygon, dann gibts einen Schnitt mit einer Außenkante
		if((t > 1 || t < 0 || CompareDoubles(t, 1.0) || CompareDoubles(t, 0.0))){		
			return false;
		}

		*/

		ads_real innerT;
        if(innerEdgeEndPoint.x - innerEdgeStartPoint.x != 0){
                innerT = (crossPoint.x - innerEdgeStartPoint.x) / (innerEdgeEndPoint.x - innerEdgeStartPoint.x);
		} else if(innerEdgeEndPoint.y - innerEdgeStartPoint.y != 0){
                innerT = (crossPoint.y - innerEdgeStartPoint.y) / (innerEdgeEndPoint.y - innerEdgeStartPoint.y);
		}
               
        ads_real outerT;
        if(outerEdgeEndPoint.x - outerEdgeStartPoint.x != 0){
                outerT = (crossPoint.x - outerEdgeStartPoint.x) / (outerEdgeEndPoint.x - outerEdgeStartPoint.x);
		} else if(outerEdgeEndPoint.y - outerEdgeStartPoint.y != 0){
                outerT = (crossPoint.y - outerEdgeStartPoint.y) / (outerEdgeEndPoint.y - outerEdgeStartPoint.y);
		}
               
        //if((innerT > 1 || innerT < 0 || CompareDoubles(innerT, 1.0) || CompareDoubles(innerT, 0.0)) && (outerT > 1 || outerT < 0 || CompareDoubles(outerT, 1.0) || CompareDoubles(outerT, 0.0)) ){
        
		if((innerT > 0.0 && innerT < 1.0) && (outerT > 0.0 && outerT < 1.0)){
			intersectionPoint = AcGePoint3d(crossPoint.x, crossPoint.y, 1);
			return true;
		}
		//acutPrintf(_T(" t = %f\n"), t);

		//finalIntersections.push_back(AcGePoint3d(crossPoint.x, crossPoint.y, 0));
		//drawIntersectionLines(innerEdgeStartPoint, innerEdgeEndPoint);
		//drawIntersectionLines(outerEdgeStartPoint, outerEdgeEndPoint);

		return false;
	}

	/* wird nicht benötigt
	static bool isInPolygon(AcGeVector3d &crossPoint){
		
		Edge xToInf;	// Strecke von crossPoint nach rechts bis 10.000
		AcGePoint3d inf(10.000,crossPoint.y,0);
		return true;
	}
	*/

	static AcGeVector3d crossProduct(AcGeVector3d &a, AcGeVector3d &b){
		AcGeVector3d result;
		result.x = a.y * b.z - a.z * b.y;
		result.y = a.z * b.x - a.x * b.z;
		result.z = a.x * b.y - a.y * b.x;
		return result;
	}

	static AcGeVector3d crossProduct(AcGePoint3d &a, AcGePoint3d &b){
		return crossProduct(a.asVector(), b.asVector());
	}

	static bool CompareDoubles(double a, double b) {
	   return std::fabs(a - b) < 0.0001;
	   //return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CPraktikum5App)

	ACED_ARXCOMMAND_ENTRY_AUTO(CPraktikum5App, CGCADPraktikum5, triangularize, triangulieren, ACRX_CMD_TRANSPARENT, NULL)
