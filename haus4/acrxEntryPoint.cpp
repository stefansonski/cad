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
class Chaus4App : public AcRxArxApp {

public:
	Chaus4App () : AcRxArxApp () {}

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

	// - CGCADhaus4.house4 command (do not rename)
	static void CGCADhaus4house4(void)
	{
		// Add your code for command CGCADhaus4.house4 here
		//ground
		AcGePoint3d groundPoint1(0.0, 0.0, 0.0);
		AcGePoint3d groundPoint2(10.0, 0.0, 0.0);
		AcGePoint3d groundPoint3(10.0, 10.0, 0.0);
		AcGePoint3d groundPoint4(0.0, 10.0, 0.0);
		//ceiling	
		AcGePoint3d ceilingPoint1(0.0, 0.0, 10.0);
		AcGePoint3d ceilingPoint2(10.0, 0.0, 10.0);
		AcGePoint3d ceilingPoint3(10.0, 10.0, 10.0);
		AcGePoint3d ceilingPoint4(0.0, 10.0, 10.0);
		//roof
		AcGePoint3d roofPoint1(5.0, 0.0, 15.0);
		AcGePoint3d roofPoint2(5.0, 10.0, 15.0);

		//initialize 3dFaces
		AcDbFace* ground = new AcDbFace(groundPoint1, groundPoint2, groundPoint3, groundPoint4, TRUE, TRUE, TRUE, TRUE);
		AcDbFace* wall1 = new AcDbFace(groundPoint1, groundPoint2, ceilingPoint2, ceilingPoint1, TRUE, TRUE, FALSE, TRUE);
		AcDbFace* wall11 = new AcDbFace(ceilingPoint1, ceilingPoint2, roofPoint1, FALSE, TRUE, TRUE, FALSE);
		AcDbFace* wall2 = new AcDbFace(groundPoint2, groundPoint3, ceilingPoint3, ceilingPoint2, TRUE, TRUE, TRUE, TRUE);
		AcDbFace* wall3 = new AcDbFace(groundPoint3, groundPoint4, ceilingPoint4, ceilingPoint3, TRUE, TRUE, FALSE, TRUE);
		AcDbFace* wall31 = new AcDbFace(ceilingPoint3, ceilingPoint4, roofPoint2, FALSE, TRUE, TRUE, FALSE);
		AcDbFace* wall4 = new AcDbFace(groundPoint4, groundPoint1, ceilingPoint1, ceilingPoint4, TRUE, TRUE, TRUE, TRUE);
		AcDbFace* roof1 = new AcDbFace(ceilingPoint1, roofPoint1, roofPoint2, ceilingPoint4, TRUE, TRUE, TRUE, TRUE);
		AcDbFace* roof2 = new AcDbFace(ceilingPoint2, ceilingPoint3, roofPoint2, roofPoint1, TRUE, TRUE, TRUE, TRUE);

		AcDbBlockTable* pBlockTable = NULL;

		AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
		pDB->getSymbolTable(pBlockTable, AcDb::kForRead);
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
		pBlockTable->close();

		pBlockTableRecord->appendAcDbEntity(ground);
		pBlockTableRecord->appendAcDbEntity(wall1);
		pBlockTableRecord->appendAcDbEntity(wall11);
		pBlockTableRecord->appendAcDbEntity(wall2);
		pBlockTableRecord->appendAcDbEntity(wall3);
		pBlockTableRecord->appendAcDbEntity(wall31);
		pBlockTableRecord->appendAcDbEntity(wall4);
		pBlockTableRecord->appendAcDbEntity(roof1);
		pBlockTableRecord->appendAcDbEntity(roof2);

		pBlockTableRecord->close();
		ground->close();
		wall1->close();
		wall11->close();
		wall2->close();
		wall3->close();
		wall31->close();
		wall4->close();
		roof1->close();
		roof2->close();
	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(Chaus4App)
ACED_ARXCOMMAND_ENTRY_AUTO(Chaus4App, CGCADhaus4, house4, haus4, ACRX_CMD_TRANSPARENT, NULL)
