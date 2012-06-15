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

#define PI 3.14159265358979323846

//-----------------------------------------------------------------------------
#define szRDS _RXST("CGCAD")

typedef std::pair<AcGePoint3d, AcGePoint3d> Edge;

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
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


	// - CGCADPraktikum5.triangularize command (do not rename)
	static void CGCADPraktikum5triangularize(void)
	{
		struct resbuf polyline;
		polyline.restype = 0;
		polyline.resval.rstring = _T("LWPOLYLINE");
		polyline.rbnext = NULL;

		Greedy(&polyline);
	}

	static void Greedy(struct resbuf *polyline)
	{
		AcCmColor color;
		color.setRGB(0,0,255);
		createLayer(_T("TRIANG"),color);
		ads_name ssname;
		long length = 0;
		int res = acedSSGet(_T("X"), NULL, NULL, NULL, ssname);
		if(res != RTNORM) return;
		acedSSLength(ssname, &length);
		for(int k = 0; k < length; k++)
		{
			ads_name ent;
			AcDbObjectId id = AcDbObjectId::kNull;
			AcDbEntity* tmp;

			if(acedSSName(ssname,k,ent) != RTNORM) return;
			if(acdbGetObjectId(id,ent) != Acad::eOk) return; 
			if(acdbOpenAcDbEntity(tmp,id,AcDb::kForRead) != Acad::eOk) return;
			AcDbPolyline* polyline = (AcDbPolyline*)tmp;
			std::vector<Edge> edges;
			std::vector<Edge> allEdges;
			std::vector<Edge> newEdges;
			std::vector<AcGePoint3d> points;
			std::vector<TwoThreeTree> trees;
			for(int i = 0; i < polyline->numVerts(); i++)
			{
				AcGePoint3d pt;
				polyline->getPointAt(i, pt);
				points.push_back(pt);
				TwoThreeTree tree;
				tree.insertAngle(360,0);
				trees.push_back(tree);
			}
			polyline->close();

			for(int i = 0; i < points.size(); i++)
			{
				for(int j = i + 1; j < points.size(); j++)
				{
					if(i + 1 == j || (i == 0 && j == points.size() - 1))
						edges.push_back(Edge(points[i], points[j]));
					else
						allEdges.push_back(Edge(points[i], points[j]));
				}
			}

			std::sort(allEdges.begin(), allEdges.end(), compareEdgesBySize);
			for(int i = 0; i < allEdges.size() && newEdges.size() < points.size() - 3; i++)
			{
				Edge currentSmallestEdge = allEdges[i];
				bool cross[2] = {false};
				int index = 0;
				for(int j = 0; j < trees.size() && index < 2; j++)
				{
					AcGePoint3d* currentPoint = NULL;
					if(points[j] == currentSmallestEdge.first)
						currentPoint = &currentSmallestEdge.second;
					else if(points[j] == currentSmallestEdge.second)
						currentPoint = &currentSmallestEdge.first;
					if(currentPoint != NULL)
					{
						int angle = (int)((atan2(points[j].y - currentPoint->y, points[j].x - currentPoint->x)) * 180 / PI);
						angle < 0 ? angle += 360: angle;

						int edge = trees[j].getEdgeForAngle(angle);
						if(edge != 0)
						{
							cross[index] = true;
							index++;
						}
					}
				}
				if(!(cross[0] && cross[1]))
				{
					newEdges.push_back(currentSmallestEdge);

					int edgePoint[2];
					index = 0;
					for(int k = 0; k < points.size(); k++)
					{
						if(points[k] == currentSmallestEdge.first || points[k] == currentSmallestEdge.second)
						{
							edgePoint[index] = k;
						}
					}

					for(int j = 0; j < trees.size(); j++)
					{
						if(currentSmallestEdge.first != points[j] && currentSmallestEdge.second != points[j])
						{
							int angle[2];
							angle[0] = (int)(atan2(currentSmallestEdge.first.y - points[j].y, currentSmallestEdge.first.x - points[j].x) * 180 / PI);
							angle[1] = (int)(atan2(currentSmallestEdge.second.y - points[j].y, currentSmallestEdge.second.x - points[j].x) * 180 / PI);
							
							angle[0] < 0 ? angle[0] += 360: angle[0];
							angle[1] < 0 ? angle[1] += 360: angle[1];

							int tmp = angle[1];
							if(angle[1] < angle[0])
								tmp += 360;

							int diff = tmp - angle[0];
							if(diff > 180)
								trees[j].setBlockingEdge(angle[1], angle[0], newEdges.size());
							else
								trees[j].setBlockingEdge(angle[0], angle[1], newEdges.size());
						}
					}
				}
			}
			//if(newEdges.size() == points.size() - 3)
			{
				//database connect
				AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
		
				//blocktable init
				AcDbBlockTable* pBlockTable = NULL;
				pDB->getSymbolTable(pBlockTable, AcDb::kForRead);
		
				//blocktable record init
				AcDbBlockTableRecord* pBlockTableRecord = NULL;
				pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
		
				pBlockTable->close();

/*				for(int i = 0; i < points.size(); i++)
				{
					AcDbFace* face;
					for(int j = 0; j < newEdges.size(); j++)
					{
						if(points[i] == newEdges[j].first)
						{
							face = new AcDbFace(newEdges[j].first, newEdges[j].second, points[i == 0 ? (points.size() - 1) : i - 1], TRUE, TRUE, TRUE);
							face->setLayer(_T("TRIANG"));
							pBlockTableRecord->appendAcDbEntity(face);
							face->close();
						}
					}
				}
*/
				AcDbLine* line;
					for(int j = 0; j < newEdges.size(); j++)
					{
						line = new AcDbLine(newEdges[j].first, newEdges[j].second);
						line->setLayer(_T("TRIANG"));
						pBlockTableRecord->appendAcDbEntity(line);
						line->close();
					}
				pBlockTableRecord->close();

			}
		}
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
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CPraktikum5App)

ACED_ARXCOMMAND_ENTRY_AUTO(CPraktikum5App, CGCADPraktikum5, triangularize, triangulieren, ACRX_CMD_TRANSPARENT, NULL)
