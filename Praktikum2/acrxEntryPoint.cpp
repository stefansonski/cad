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
class CPraktikum2App : public AcRxArxApp {

public:
	CPraktikum2App () : AcRxArxApp () {}

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


	// - CGCADPraktikum2.ktomany command (do not rename)
	static void CGCADPraktikum2ktomany(void)
	{
		AcCmColor colorGreen;
		colorGreen.setRGB(0,255,0);
		createLayer(_T("kreise"), colorGreen);

		AcCmColor colorRed;
		colorRed.setRGB(255,0,0);
		createLayer(_T("substitut"), colorRed);

		// Construct the filter 
		struct resbuf eb1; 
		ACHAR sbuf1[10]; 
		eb1.restype = 0; // Entity name 
		wcscpy(sbuf1, _T("CIRCLE")); 
		eb1.resval.rstring = sbuf1; 
		eb1.rbnext = NULL; 

		// Select All Circles 
		ads_name ss; 
		if (acedSSGet(_T("X"), NULL, NULL, &eb1, ss) != RTNORM) 
		{ 
			acutRelRb(&eb1); 
			return; 
		} 

		// Free the resbuf 
		acutRelRb(&eb1); 

		// Get the length (how many entities were selected) 
		long length = 0; 
		if ((acedSSLength( ss, &length ) != RTNORM) || (length == 0))  
		{ 
			acedSSFree( ss ); 
			return; 
		} 

		ads_name ent; 
		AcDbObjectId id = AcDbObjectId::kNull; 

		// Walk through the selection set and open each entity 
		for (long i = 0; i < length; i++)  
		{ 
			if (acedSSName(ss,i,ent) != RTNORM) continue; 
			if (acdbGetObjectId(id,ent) != Acad::eOk) continue; 
			AcDbEntity* pEnt = NULL; 
			if (acdbOpenAcDbEntity(pEnt,id,AcDb::kForWrite) != Acad::eOk) 
				continue; 
			// Change color 
			pEnt->setLayer(_T("kreise")); 
			pEnt->close(); 
		} 

		// Free selection 
		acedSSFree( ss );


		// Add your code for command CGCADPraktikum2.ktomany here
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
		   acutPrintf(_T("already exists")); //output in the status line 
		}

	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CPraktikum2App)

	ACED_ARXCOMMAND_ENTRY_AUTO(CPraktikum2App, CGCADPraktikum2, ktomany, ktoviel, ACRX_CMD_TRANSPARENT, NULL)
