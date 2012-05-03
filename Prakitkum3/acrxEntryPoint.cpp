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
	// ----- CGCADPraktikum3.round command
	static void CGCADPraktikum3round(void)
	{
		
		ads_name ssname[2];
		ads_point points[2];

		ads_name ent; 
		AcDbObjectId id = AcDbObjectId::kNull; 
		AcDbEntity* lines[2];

		// Walk through the selection set and open each entity 
		for (long i = 0; i < 2; i++)  
		{ 
			
			if (acedGetPoint(NULL, _T("Objekt wählen"), points[i]) != RTNORM) return;
			
			// get line
			acedSSGet(NULL, points[i], NULL, NULL, ssname[i]);
			
			if (acedSSName(ssname[i],0,ent) != RTNORM) continue; 
			if (acdbGetObjectId(id,ent) != Acad::eOk) continue; 
			if (acdbOpenAcDbEntity(lines[i],id,AcDb::kForWrite) != Acad::eOk) 
				continue;  	
			AcCmColor color;
			color.setRGB(255,0,0);
			lines[i]->setColor(color);
		} 
		



		for (int i = 0; i < 2; i++)
		{
			lines[i]->close();
		}
	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CPrakitkum3App)

ACED_ARXCOMMAND_ENTRY_AUTO(CPrakitkum3App, CGCADPraktikum3, round, runden, ACRX_CMD_TRANSPARENT, NULL)
