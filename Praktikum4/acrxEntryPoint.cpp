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
#include <math.h>

//-----------------------------------------------------------------------------
#define szRDS _RXST("CGCAD")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CPraktikum4App : public AcRxArxApp {

public:
	CPraktikum4App () : AcRxArxApp () {}

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


	// - CGCADPraktikum4.cutFace command (do not rename)
	static void CGCADPraktikum4cutFace(void)
	{
		struct resbuf oldOsnap;
		deactivateSnap(&oldOsnap);

		struct resbuf filter;
		filter.restype = 0;
		filter.resval.rstring = _T("3DFACE");
		filter.rbnext = NULL;

		// hier kommen die 2 Ebenen rein
		AcDbFace* faces[2];

		// Filter der nur faces liefert
		for(int i = 0; i < 2; i++) {
			ads_name ssname, ent;
			AcDbObjectId id = AcDbObjectId::kNull;
			AcDbEntity* tmp;
			long length = 0;
			if(acedSSGet(_T("_:S"), NULL, NULL, &filter, ssname) != RTNORM) return;
			acedSSLength(ssname, &length);
			if(length == 0) return;
			if(acedSSName(ssname,0,ent) != RTNORM) return;
			if(acdbGetObjectId(id,ent) != Acad::eOk) return; 
			if(acdbOpenAcDbEntity(tmp,id,AcDb::kForWrite) != Acad::eOk) return;
			faces[i] = (AcDbFace*)tmp;
		}

		/*-Deklaration-----------------------*/
												// 2 Ebenenen, 4 Eckpunkte + homogene Koordinate
		ads_real sFaceCorners[4][4] = {};		// sface = small Face
		ads_real bFaceCorners[4][4] = {};		// bface = bigface

		ads_real sDirection[2][4] = {};			// Ebenen: 2 Richtungsvektoren à 4 Koordinaten
		ads_real bDirection[2][4] = {};

		ads_real sNormal[4] = {};				// Normale...
		ads_real bNormal[4] = {};


		/*-Zuweisung Ebene-------------------*/

		getFacePoints(faces[0],sFaceCorners);
		getFacePoints(faces[1],bFaceCorners);

		
		getFaceDirection(sFaceCorners, sDirection);
		getFaceDirection(bFaceCorners, bDirection);

		getNormal(sFaceCorners[0], sDirection, sNormal);
		getNormal(bFaceCorners[0], bDirection, bNormal);

		getNormalizedNormal(sNormal);
		getNormalizedNormal(bNormal);

		
		/*-Auf gehts-------------------------*/
				
		int index[2] = {}; 
		getPointsToChange(bNormal, sFaceCorners, index);	// Welche Punkte müssen geändert werden?
		
		int indexDirection = -1;
		getDirection(index, indexDirection);				// Welcher Richtungsvektor wird benötigt? 


		ads_real intersection[4] = {};
		getIntersection(sFaceCorners[index[0]],sDirection[indexDirection],bNormal,intersection);			// Neuen Punkt ermitteln
		faces[0]->setVertexAt(index[0],AcGePoint3d (intersection[0],intersection[1],intersection[2]));		// Neuen Punkt setzen

		getIntersection(sFaceCorners[index[1]],sDirection[indexDirection],bNormal,intersection);			// Neuen Punkt ermitteln
		faces[0]->setVertexAt(index[1],AcGePoint3d (intersection[0],intersection[1],intersection[2]));		// Neuen Punkt setzen

		
		for(int i = 0; i < 2; i++) {
			faces[i]->close();
		}
		reactivateSnap(&oldOsnap);
	}
	
	
	
	static void getDirection(int (&index)[2], int (&indexDirection)){
		
		if(index[0] + index[1] == 3){		// Index addiert == 3 -> strecke [0]:[1] bzw [2]:[3], 
			indexDirection = 0;				// damit direction 0
		} else {							// direction 0 zeigt weg von 0 und zeigt richtung 1 
			indexDirection = 1;				// direction 1 zeigt weg von 0 und zeigt richtung 3 
		}

	}

	static void getIntersection(ads_real (&refPoint)[4], ads_real (&direction)[4], ads_real (&normal)[4], ads_real (&intersection)[4]){
		
		ads_real result = 0;
		ads_real top = (normal[0] * refPoint[0]) + (normal[1] * refPoint[1]) + (normal[2] * refPoint[2]) + (normal[3] * refPoint[3]);
		
		// wenn kein schnittpunkt wird down = 0
		ads_real down = (normal[0] * direction[0]) + (normal[1] * direction[1]) + (normal[2] * direction[2]) + (normal[3] * direction[3]);

		result = (-1) * (top / down); 

		intersection[0] = refPoint[0] + result * direction[0];
		intersection[1] = refPoint[1] + result * direction[1];
		intersection[2] = refPoint[2] + result * direction[2];
		intersection[3] = refPoint[3] + result * direction[3];
	}

	static void getPointsToChange(ads_real (&bNormal)[4], ads_real (&sFaceCorners)[4][4], int (&index)[2]){	

		for(int i= 0; i < 4; i++){

			ads_real firstPoint = bNormal[0] * sFaceCorners[i%4][0] + bNormal[1] * sFaceCorners[i%4][1] + bNormal[2] * sFaceCorners[i%4][2] + bNormal[3] * sFaceCorners[i%4][3];
			ads_real secondPoint = bNormal[0] * sFaceCorners[(i+1)%4][0] + bNormal[1] * sFaceCorners[(i+1)%4][1] + bNormal[2] * sFaceCorners[(i+1)%4][2] + bNormal[3] * sFaceCorners[(i+1)%4][3];
			
			ads_real lot[4] = {};
			ads_real lotDirection = (-1) *(firstPoint / (bNormal[0] * bNormal[0] + bNormal[1] * bNormal[1] + bNormal[2] * bNormal[2] + bNormal[3] * bNormal[3]));

			lot[0] = sFaceCorners[i][0] + (lotDirection * bNormal[0]);
			lot[1] = sFaceCorners[i][1] + (lotDirection * bNormal[1]);
			lot[2] = sFaceCorners[i][2] + (lotDirection * bNormal[2]);
			lot[3] = sFaceCorners[i][3] + (lotDirection * bNormal[3]);

			if(firstPoint * secondPoint > 0 && sFaceCorners[i][2] < lot[2]){
				index[0] = i;
				index[1] = (i+1)%4;
				return;
			}
		}

		/*int counter = 0;

		for(int i=0; i < 4; i++){
			
			ads_real pointIs = bNormal[0] * sFaceCorners[i][0] + bNormal[1] * sFaceCorners[i][1] + bNormal[2] * sFaceCorners[i][2] + bNormal[3] * sFaceCorners[i][3];
			
			if(pointIs < 0){			// liegt unterhalb der größeren Ebene
				index[counter++] = i;
			}
		}*/
	}

	static void getNormalizedNormal(ads_real (&normal)[4]) {
		
		ads_real length = sqrt (normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2] + normal[3] * normal[3]);

		if(length == 0) {return;}

		normal[0] = normal[0] / length;
		normal[1] = normal[1] / length;
		normal[2] = normal[2] / length;
		normal[3] = normal[3] / length;

	}

	static void getFaceDirection(ads_real (&facePoints)[4][4], ads_real (&direction)[2][4]) {
		
		// 1. Richtungsvektor zeigt von [0][0] nach [1][0] 
		direction[0][0] = ((-1) * facePoints[0][0]) + facePoints[1][0];
		direction[0][1] = ((-1) * facePoints[0][1]) + facePoints[1][1];
		direction[0][2] = ((-1) * facePoints[0][2]) + facePoints[1][2];
		direction[0][3] = ((-1) * facePoints[0][3]) + facePoints[1][3];

		// 2. Richtungsvektor zeigt von [0][0] nach [3][0]
		direction[1][0] = ((-1) * facePoints[0][0]) + facePoints[3][0];
		direction[1][1] = ((-1) * facePoints[0][1]) + facePoints[3][1];
		direction[1][2] = ((-1) * facePoints[0][2]) + facePoints[3][2];
		direction[1][3] = ((-1) * facePoints[0][3]) + facePoints[3][3];
	}

	static void getNormal(ads_real (&refPoint)[4], ads_real (&direction)[2][4], ads_real (&normal)[4]) {
		
		normal[0] = ((direction[0][1] * direction[1][2]) - (direction[0][2] * direction[1][1]));
		
		normal[1] = ((direction[0][0] * direction[1][2]) - (direction[0][2] * direction[1][0])) * (-1);
		
		normal[2] = ((direction[0][0] * direction[1][1]) - (direction[0][1] * direction[1][0]));
		
		normal[3] = (-1) * (refPoint[0] * ((direction[0][1] * direction[1][2]) - (direction[0][2] * direction[1][1]))) - (refPoint[1] * ((direction[0][0] * direction[1][2]) - (direction[0][2] * direction[1][0]))) + (refPoint[2] * ((direction[0][0] * direction[1][1]) - (direction[0][1] * direction[1][0])));
	}

	static void getFacePoints(AcDbFace*& face, ads_real (&facePoints)[4][4]) {
		
		AcGePoint3d points3d[4];
		
		for(int i = 0; i < 4; i++){		// 4 Eckpunkte
					
			face->getVertexAt(i, points3d[i]);	// zuweisung zu points3d

			facePoints[i][0] = points3d[i].x; 
			facePoints[i][1] = points3d[i].y; 
			facePoints[i][2] = points3d[i].z; 
			facePoints[i][3] = 1;					// setze homohene koordinaten = 1
		}	
	}



	static void deactivateSnap(resbuf* oldOsnap) {
		struct resbuf newOsnap;
		// Get current OSNAP
		acedGetVar(_T("OSMODE"), oldOsnap);

		// Chage OSNAP settings
		newOsnap.restype = RTSHORT;
		newOsnap.resval.rint = 0;
		acedSetVar(_T("OSMODE"), &newOsnap);
	}

	static void reactivateSnap(resbuf* oldOsnap) {
		acedSetVar(_T("OSMODE"), oldOsnap);
	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CPraktikum4App)

ACED_ARXCOMMAND_ENTRY_AUTO(CPraktikum4App, CGCADPraktikum4, cutFace, schneideFace, ACRX_CMD_MODAL, NULL)
