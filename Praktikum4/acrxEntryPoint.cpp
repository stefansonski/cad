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

		AcGeVector3d direction003;
		AcGeVector3d direction103;
		AcGeVector3d direction012;
		AcGeVector3d direction112;

		getDirectionVector(0, 3, faces[0], direction003);
		getDirectionVector(0, 3, faces[1], direction103);
		getDirectionVector(1, 2, faces[0], direction012);
		getDirectionVector(1, 2, faces[1], direction112);

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

	static void getDirectionVector(int startIndex, int endIndex, AcDbFace* face, AcGeVector3d &result) {
		AcGePoint3d startPoint;
		AcGePoint3d endPoint;
		face->getVertexAt(startIndex, startPoint);
		face->getVertexAt(endIndex, endPoint);
		result = endPoint - startPoint;
	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CPraktikum4App)

ACED_ARXCOMMAND_ENTRY_AUTO(CPraktikum4App, CGCADPraktikum4, cutFace, schneideFace, ACRX_CMD_MODAL, NULL)
