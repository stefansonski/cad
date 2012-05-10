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
#include <iostream>
#include <math.h>

using namespace std;

#define PI 3.14159265358979323846

//-----------------------------------------------------------------------------
#define szRDS _RXST("CGCAD")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CPrakitkum3App : public AcRxArxApp {

public:
	CPrakitkum3App () : AcRxArxApp () {}

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


	// ----- CGCADPraktikum3.round command
	static void CGCADPraktikum3round(void)
	{
		struct resbuf OldOsnap, NewOsnap;
		// Get current OSNAP
		acedGetVar(_T("OSMODE"), &OldOsnap);

		// Chage OSNAP settings
		NewOsnap.restype = RTSHORT;
		NewOsnap.resval.rint = 0;
		acedSetVar(_T("OSMODE"), &NewOsnap);

		ads_name ssname[2] = {};
		ads_point points[2] = {};
		ads_real radius = 0;

		ads_name ent; 
		AcDbObjectId id = AcDbObjectId::kNull; 
		AcDbEntity* lines[2];

		// Walk through the selection set and open each entity 
		for (long i = 0; i < 2; i++)  
		{ 

			if (acedGetPoint(NULL, _T("Objekt wählen"), points[i]) != RTNORM) return;

			// get line
			acedSSGet(NULL, points[i], NULL, NULL, ssname[i]);

			long length = 0;
			acedSSLength(ssname[i], &length);
			if(length == 0) return;

			if (acedSSName(ssname[i],0,ent) != RTNORM) continue; 
			if (acdbGetObjectId(id,ent) != Acad::eOk) continue; 
			if (acdbOpenAcDbEntity(lines[i],id,AcDb::kForWrite) != Acad::eOk) 
				continue;  	
		} 
		if (acedGetReal(_T("Radius wählen"), &radius) != RTNORM) return;

		AcGePoint3d* intersection = new AcGePoint3d();

		// calc Intersection Point
		getIntersection((AcDbLine*)lines[0],(AcDbLine*)lines[1],intersection);

		cutLines((AcDbLine*&)lines[0],(AcDbLine*&)lines[1],intersection,points[0],points[1]);

		curveLines((AcDbLine*)lines[0],(AcDbLine*)lines[1], radius);

		for (int i = 0; i < 2; i++)
		{
			lines[i]->close();
		}
		// Get back the old OSNAP
		acedSetVar(_T("OSMODE"), &OldOsnap);
	}


	static void cutLines(AcDbLine*& line1,AcDbLine*& line2, AcGePoint3d*& intersection, ads_point& point1, ads_point& point2){

		//find out how to cut the lines
		if(isInLine(point1,line1->startPoint(),*intersection)){
			AcGePoint3d tmpPoint(line1->startPoint());
			line1->setStartPoint(line1->endPoint());
			line1->setEndPoint(tmpPoint);
		}

		line1->setStartPoint(AcGePoint3d(intersection->x,intersection->y,intersection->z));

		if(isInLine(point2,line2->startPoint(),*intersection)){
			AcGePoint3d tmpPoint(line2->startPoint());
			line2->setStartPoint(line2->endPoint());
			line2->setEndPoint(tmpPoint);
		}

		line2->setStartPoint(AcGePoint3d(intersection->x,intersection->y,intersection->z));

		//cout << "point is in line: " <<isInLine(point1,line1->startPoint(),intersection) << endl;
	}

	static bool isInLine(ads_point& point,AcGePoint3d& linePoint, AcGePoint3d& intersection){

		ads_real result;
		if(0 != intersection.x - linePoint.x)
			result = (point[0] - linePoint.x)/(intersection.x - linePoint.x);
		else
			result = (point[1] - linePoint.y)/(intersection.y - linePoint.y);
		if ( result <= 1 && result >= 0){
			return true;
		}
		return false;
	}

	static void getIntersection(AcDbLine* line1,AcDbLine* line2, AcGePoint3d*& intersection){

		AcGeVector3d* directionVector1 = new AcGeVector3d();
		AcGeVector3d* directionVector2 = new AcGeVector3d();
		ads_real multiplier = 0;

		getDirectionVector(line1, directionVector1);
		getDirectionVector(line2, directionVector2);

		//calc intersection
		ads_real nenner = (directionVector2->x * directionVector1->y) - (directionVector2->y * directionVector1->x);
		try{
			if (nenner == 0){
				char string[] = "nenner = 0";
				throw string;
			}
		}catch(char* e){
			cout << "An exception occurred. Exception: " << e << endl;
			return;
		}

		ads_real zaehler = (directionVector1->x * (line2->startPoint().y - line1->startPoint().y) - ((line2->startPoint().x - line1->startPoint().x) * directionVector1->y));

		multiplier = zaehler / nenner;

		intersection->x = line2->startPoint().x + multiplier * directionVector2->x;
		intersection->y = line2->startPoint().y + multiplier * directionVector2->y;
		//intersection->z = line2->startPoint().z + multiplier * directionVector2->z;
	}

	static void getDirectionVector(AcDbLine*& line, AcGeVector3d*& result){
		AcGePoint3d* tmpStart = new AcGePoint3d();
		AcGePoint3d* tmpEnd = new AcGePoint3d();

		*tmpStart = line->startPoint();
		*tmpEnd = line->endPoint(); //liefert mir einen ungenauen wert wenn endpunkt mit festem wert statt mit fadenkreuz angegeben wird

		result->x = tmpEnd->x - tmpStart->x;
		result->y = tmpEnd->y - tmpStart->y;
		result->z = tmpEnd->z - tmpStart->z;
	}

	static ads_real compareDistance(AcGePoint3d& point, AcGePoint3d*& intersection) {
		return sqrt(pow(point.x - intersection->x, 2) + pow(point.y - intersection->y, 2));
	}

	static void curveLines(AcDbLine* line1, AcDbLine* line2, ads_real& radius)
	{
		AcGeVector3d* direction1 = new AcGeVector3d();
		AcGeVector3d* direction2 = new AcGeVector3d();
		getDirectionVector(line1, direction1);
		getDirectionVector(line2, direction2);

		ads_real zaehler = direction1->x * direction2->x + direction1->y * direction2->y + direction1->z * direction2->z;
		ads_real nenner = sqrt(pow(direction1->x, 2) + pow(direction1->y, 2) + pow(direction1->z, 2)) * sqrt(pow(direction2->x, 2) + pow(direction2->y, 2) + pow(direction2->z, 2));
		ads_real alpha = acos(zaehler / nenner);
		ads_real length = radius / tan(alpha);

		line1->setStartPoint(line1->startPoint() + (*direction1 * length));
		line2->setStartPoint(line2->startPoint() + (*direction2 * length));
		acedCommand(RTSTR,_T("_arc"),RTPOINT, line2->startPoint(),RTSTR, _T("_e"), RTPOINT, line1->startPoint(), RTSTR, _T("_a"), RTREAL, alpha * 180 / PI, 0);
	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CPrakitkum3App)

	ACED_ARXCOMMAND_ENTRY_AUTO(CPrakitkum3App, CGCADPraktikum3, round, runden, ACRX_CMD_TRANSPARENT, NULL)
