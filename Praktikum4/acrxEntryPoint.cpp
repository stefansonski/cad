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

		AcDbFace* faces[2];
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

		AcGePoint3d points[2][4];	
		AcGeVector3d normal[2];

		for(int i = 0; i < 2; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				faces[i]->getVertexAt(j, points[i][j]);
			}

			AcGeVector3d direction01;
			AcGeVector3d direction03;
			direction01 = points[i][1] - points[i][0];
			direction03 = points[i][3] - points[i][0];
			normal[i] = direction01.crossProduct(direction03);
			normal[i] = normal[i].normalize();
		}

		AcGeVector3d crossVec;
		crossVec = normal[0].crossProduct(normal[1]);
		crossVec = crossVec.normalize();

		double result[2];
		AcGePoint3d cross;
		for(int i = 0; i < 2; i++) 
		{
			result[i] = points[i][0].asVector().negate().dotProduct(crossVec);
		}

		if(normal[0].x != 0.0 && (normal[1].x * normal[0].y / normal[0].x) + normal[1].y != 0.0)
		{
			cross.y = -1 * ((result[1] - (normal[1].x * result[0] / normal[0].x)) / ((normal[1].x * normal[0].y / normal[0].x) + normal[1].y));
			cross.x = (result[0] - normal[0].y * cross.y) / normal[0].x;
			cross.z = 0;
		}
		else if(normal[0].y != 0.0 && (normal[1].y * normal[0].z / normal[0].y) + normal[1].z != 0.0)
		{
			cross.z = -1 * ((result[1] - (normal[1].y * result[0] / normal[0].z)) / ((normal[1].y * normal[0].z / normal[0].y) + normal[1].z));
			cross.y = (result[0] - normal[0].z * cross.z) / normal[0].y;
			cross.x = 0;
		}
		else if(normal[0].z != 0.0 && (normal[1].z * normal[0].x / normal[0].z) + normal[1].x != 0.0)
		{
			cross.x = -1 * ((result[1] - (normal[1].z * result[0] / normal[0].x)) / ((normal[1].z * normal[0].x / normal[0].z) + normal[1].x));
			cross.z = (result[0] - normal[0].x * cross.x) / normal[0].z;
			cross.y = 0;
		}

		AcGePoint3d crossPoint[2];
		int counter = 0;
		//dirty shit is now comming
		for(int i = 0; i < 4; i++) {
			int next = (i + 1) % 4;
			AcGeVector3d tmp = points[1][next] - points[1][i];
			boolean found = false;
			double a1, a2, a3, cross1, cross2, cross3, crossVec1, crossVec2, crossVec3, tmp1, tmp2, tmp3;
			if(tmp.x != 0.0 && tmp.y != 0.0)
			{
				a1 = points[1][i].x;
				a2 = points[1][i].y;
				a3 = points[1][i].z;
				cross1 = cross.x;
				cross2 = cross.y;
				cross3 = cross.z;
				crossVec1 = crossVec.x;
				crossVec2 = crossVec.y;
				crossVec3 = crossVec.z;
				tmp1 = tmp.x;
				tmp2 = tmp.y;
				tmp3 = tmp.z;
				found = true;
			}
			else if(tmp.x != 0.0 && tmp.z != 0.0)
			{
				a1 = points[1][i].x;
				a2 = points[1][i].z;
				a3 = points[1][i].y;
				cross1 = cross.x;
				cross2 = cross.z;
				cross3 = cross.y;
				crossVec1 = crossVec.x;
				crossVec2 = crossVec.z;
				crossVec3 = crossVec.y;
				tmp1 = tmp.x;
				tmp2 = tmp.z;
				tmp3 = tmp.y;
				found = true;
			}
			else if(tmp.y != 0.0 && tmp.z != 0.0) 
			{
				a1 = points[1][i].y;
				a2 = points[1][i].z;
				a3 = points[1][i].x;
				cross1 = cross.y;
				cross2 = cross.z;
				cross3 = cross.x;
				crossVec1 = crossVec.y;
				crossVec2 = crossVec.z;
				crossVec3 = crossVec.x;
				tmp1 = tmp.y;
				tmp2 = tmp.z;
				tmp3 = tmp.x;
				found = true;
			}

			if(found)
			{
				double mue = (a3 - cross3 + ((cross2 * tmp3 - a2 * tmp3) / tmp2)) / (crossVec3 - ((crossVec2 * tmp3) / tmp2));
				double lambda = (cross2 + mue * crossVec2 - a2) / tmp2;
				//if(a3 + lambda * tmp3 - cross3 + mue * crossVec3 <= 0.000000000000000000001 && a3 + lambda * tmp3 - cross3 + mue * crossVec3 >= -0.000000000000000000001)
				{
					crossPoint[counter].x = points[1][i].x + lambda * tmp.x;
					crossPoint[counter].y = points[1][i].y + lambda * tmp.y;
					crossPoint[counter].z = points[1][i].z + lambda * tmp.z;
					counter++;
					if(counter == 2)
					{
						acutPrintf(_T("2 Points found"));
						break;
					}
				}
			}
		}

		counter = 0;
		for(int i = 0; i < 4; i++)
		{
			if((normal[1].dotProduct(points[0][i].asVector()) - result[1])  < 0.0)
			{
				faces[0]->setVertexAt(i, crossPoint[counter++]);
			}
		}

		for(int i = 0; i < 2; i++) {
			faces[i]->close();
		}
		reactivateSnap(&oldOsnap);
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
