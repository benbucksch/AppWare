/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:					OLE_OBJ.CPP
//
// AUTHOR:				Scott McCarty
//
// DESCRIPTION:		Implements the AppWare OLE Object.
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#define STRICT
#define ASTRICT
#include <string.h>
#include <windows.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>
#include <o_menus.h>

#include "srole.h"
#include "srolefun.h"
#include "srolemod.h"
#include "sroleapp.h"
#include "sroledoc.h"
#include "sroleobj.h"
#include "srreg.h"
#include "olerc.h"
#include "ole_obj.h"
#include "ole_data.h"					// data block definitions
#include "ole_wip.h"

////////////////////////////////////////////////////////////////
//
// Signal definition
//
////////////////////////////////////////////////////////////////

typedef struct
	{
	LPSTR				pszName;
	SOLE_EVENT	nId;
	}	OLESIGNAL;

const int						nSignalCount								= 5;
static OLESIGNAL		SketchSignals[nSignalCount]	=
												{
													{ "Created",     SE_CREATED     },
													{ "Deleted",     SE_DELETED     },
													{ "Activated",   SE_ACTIVATED   },
													{ "Deactivated", SE_DEACTIVATED },
													{ "Size Change", SE_SIZECHANGE  },
												};

//****************************************************************
//
//											 SROLEObject methods

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::SROLEObject
//
// DESC:			Determines some runtime information.
//
////////////////////////////////////////////////////////////////

SROLEObject::SROLEObject(
	OBJECTID		oi
	) : m_Object(oi)
	{
	Init();
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::Init
//
// DESC:			Sets up runtime info flags based on datablock information.
//
////////////////////////////////////////////////////////////////

void	SROLEObject::Init(
	void
	)
	{
	OLECFGDATA*		pCfg		= new (this) OLECFGDATA(this);

	if (AObjGetDataSize(m_Object, OLEDATA_OLEBLOCK) != 0)
		SetRuntimeInfo(ORI_HASDATA);

	if (pCfg)
		{
		if (pCfg->ObjectType() != NULL)
			SetRuntimeInfo(ORI_VALID);
		pCfg->Destroy(this);
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::BuildSignals
//
// DESC:			Fills in the memblock with the signal names.
//
////////////////////////////////////////////////////////////////

VOID	SROLEObject::BuildSignals(
	MEMBLOCKID			mbID								// mod:  filled in with signal info
	)
	{
	OLESIGNAL*					pSignals			= SketchSignals;
	int									nSignal;
	AObjSignalRecord*		pSignal;

	AMemSetSize(mbID, nSignalCount * sizeof(AObjSignalRecord));

	pSignal = (AObjSignalRecord*) AMemLock(mbID);

	if (pSignal != NULL)
		{
		for (nSignal = 0; nSignal < nSignalCount; nSignal++)
			{
			pSignal->theID = (LONG) OLEMODULE->OleEventType(pSignals->nId);
			lstrcpy(pSignal->theName, pSignals->pszName);

			pSignals++;
			pSignal++;
			}
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::DlgLoadMenuObjects
//
// DESC:			Loads the list of available menu objects into the combobox.
//
////////////////////////////////////////////////////////////////

VOID	SROLEObject::DlgLoadMenuObjects(
	HWND					hDlg
	)
	{
	OLECFGDATA*		pOleData			= new (this) OLECFGDATA(this);
	HWND					hCombo				= GetDlgItem(hDlg, IDD_MENUOBJECTS);
	LONG					lEnumKey			= AOBJ_GETFIRSTKEY;
	short					nIdx;
	short					nIdxToSelect;
	char					szName[32];
	OBJECTID			oId;

	I_ASSERT(hCombo != NULL);

	nIdxToSelect = SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) "<None>");

	I_ASSERT(nIdxToSelect == 0);

	while ((oId = AObjGetNextObject(OTYPE_MENU, &lEnumKey)) != 0)
		{
		AObjGetName(oId, szName, sizeof(szName));

		nIdx = SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) szName);

		if (nIdx != CB_ERR)
			{
			SendMessage(hCombo, CB_SETITEMDATA, nIdx, (LPARAM) oId);

			if (oId == pOleData->MenuObject())
				{
				nIdxToSelect = nIdx;
				}
			}
		}

	SendMessage(hCombo, CB_SETCURSEL, nIdxToSelect, 0);
	EnableWindow(GetDlgItem(hDlg, IDD_EDITMENU), (nIdxToSelect != 0));

	pOleData->Destroy(this);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::DlgInit
//
// DESC:			Initializes the dialog box.
//
////////////////////////////////////////////////////////////////

BOOL	SROLEObject::DlgInit(
	HWND		hDlg
	)
	{
	OLECFGDATA*		pOleData		= new (this) OLECFGDATA(this);
	HWND					hCombo			= GetDlgItem(hDlg, IDD_OBJECTTYPES);
	HWND					hFile				= GetDlgItem(hDlg, IDD_FILENAME);
	HWND					hItem				= GetDlgItem(hDlg, IDD_ITEMNAME);
	WORD					wCmd;

	I_ASSERT(hCombo != NULL);

	// Fill with OLE class descriptions
	FillClassList(hCombo);

	// Fill with available menu objects
	DlgLoadMenuObjects(hDlg);

	SendMessage(hCombo, CB_LIMITTEXT, MAXKEY, 0);
	SendMessage(hFile,  EM_LIMITTEXT, _MAX_PATH, 0);
	SendMessage(hItem,  EM_LIMITTEXT, _MAX_PATH, 0);

	if (!(RuntimeInfo() & ORI_VALID))							// first time:  set defaults
		{
		SendMessage(hCombo, CB_SETCURSEL, 0, 0);		// select first one
		SetDlgItemInt(hDlg, IDD_MENUITEMPOS, 0, FALSE);
		CheckDlgButton(hDlg, IDD_EMBEDDED, 1);
		wCmd = IDD_EMBEDDED;
		}

	else													// set controls from info
		{
		if (SendMessage(hCombo, CB_SELECTSTRING, (WPARAM) -1,
								(LPARAM) pOleData->ObjectType()) == CB_ERR)
			{
			// string wasn't in list:  set the text explicitly
			SendMessage(hCombo, WM_SETTEXT, 0, (LPARAM) pOleData->ObjectType());
			}

		if (pOleData->Options() & SOLE_EMBEDDED)
			{
			CheckDlgButton(hDlg, IDD_EMBEDDED, 1);
			wCmd = IDD_EMBEDDED;
			}
		else
			{
			CheckDlgButton(hDlg, IDD_LINKED, 1);

			SetWindowText(hFile, pOleData->LinkFile());
			SetWindowText(hItem, pOleData->LinkItem());
			wCmd = IDD_LINKED;
			}

		if (pOleData->Options() & SOLE_SVRRESIZE)
			CheckDlgButton(hDlg, IDD_TRACKSIZE, 1);

		SetDlgItemInt(hDlg, IDD_MENUITEMPOS, pOleData->MenuPosition(), FALSE);
		}

	SetFocus(hCombo);
	// select all
	SendMessage(hCombo, CB_SETEDITSEL, 0, MAKELPARAM(0, -1));

	PostMessage(hDlg, WM_COMMAND, wCmd, 0);

	pOleData->Destroy(this);

	return TRUE;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::DlgGetData
//
// DESC:			Grabs data from the dialog controls
//
////////////////////////////////////////////////////////////////

VOID	SROLEObject::DlgGetData(
	HWND		hDlg
	)
	{
	HWND					hCombo			= GetDlgItem(hDlg, IDD_OBJECTTYPES);
	HWND					hMenus			= GetDlgItem(hDlg, IDD_MENUOBJECTS);
	OLECFGDATA*		pData0;
	OBJECTID			oMenuObject;
	char					szType[MAXKEY];
	char					szFile[_MAX_PATH];
	char					szItem[_MAX_PATH];
	LPSTR					pszFile;
	LPSTR					pszItem;
	OLEOPTIONS		oOptions;
	short					nMenuInsertPos;
	int						iTmp;
	BOOL					fTranslated;

	SendMessage(hCombo, WM_GETTEXT, sizeof(szType), (LPARAM) (LPCSTR) szType);

	iTmp = (int) SendMessage(hMenus, CB_GETCURSEL, 0, 0);
	if (iTmp != CB_ERR)
		{
		oMenuObject = (OBJECTID) SendMessage(hMenus, CB_GETITEMDATA, iTmp, 0);
		I_ASSERT((iTmp == 0) || (oMenuObject != NULL));
		}
	else
		{
		oMenuObject = NULL;
		}

	nMenuInsertPos = (int) GetDlgItemInt(hDlg, IDD_MENUITEMPOS,
														&fTranslated, FALSE);

	if (IsDlgButtonChecked(hDlg, IDD_EMBEDDED))
		{
		oOptions = SOLE_EMBEDDED;
		pszFile  = NULL;
		pszItem  = NULL;
		}
	else
		{
		oOptions = SOLE_LINKED;
		GetWindowText(GetDlgItem(hDlg, IDD_FILENAME), szFile, sizeof(szFile));
		GetWindowText(GetDlgItem(hDlg, IDD_ITEMNAME), szItem, sizeof(szItem));
		pszFile = szFile;
		pszItem = szItem;
		}

	if (IsDlgButtonChecked(hDlg, IDD_TRACKSIZE))
		oOptions |= SOLE_SVRRESIZE;

	iTmp = OLECFGDATA::SizeNeeded(szType, pszFile, pszItem);

	AObjResizeData(m_Object, OLEDATA_CFGBLOCK, iTmp);
	pData0 = new (this) OLECFGDATA(this);

	I_ASSERT(pData0 != NULL);

	pData0->Init(oOptions, szType, pszFile, pszItem);

	if (nMenuInsertPos >= 0)
		pData0->SetMenuInsertPos(nMenuInsertPos);

	pData0->SetMenuObject(oMenuObject);

	pData0->Destroy(this);

	SetRuntimeInfo(ORI_VALID);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::DlgProc
//
// DESC:			Dialog proc for OLE object definition dialog.
//
////////////////////////////////////////////////////////////////

BOOL ALMCALLBACK _export		SROLEObject::DlgProc(
	HWND					hDlg,
	UINT					uMsg,
	WPARAM				wParam,
	LPARAM				lParam
	)
	{
	BOOL					fRet				= FALSE;
	BOOL					fEnable;
	SROLEObject*	pObj;
	char					szObjName[64];

	switch (uMsg)
		{
		case WM_INITDIALOG:
			
			AUtlSetTitleFont(OTYPE_OLE1CLIENT, NULL,
												GetDlgItem(hDlg, IDD_NAME));
			AUtlCenterDialog(hDlg, 0);

			pObj = (SROLEObject*) lParam;
			SetWindowLong(hDlg, DWL_USER, lParam);

			I_ASSERT(pObj != NULL);

			AObjGetName(pObj->ObjectId(), szObjName, sizeof(szObjName));
			SetWindowText(hDlg, szObjName);

			fRet = pObj->DlgInit(hDlg);
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDD_EMBEDDED:					// Disable link controls
				case IDD_LINKED:						// Enable link controls
					fEnable = (wParam == IDD_LINKED);
					EnableWindow(GetDlgItem(hDlg, IDD_FILECUE),  fEnable);
					EnableWindow(GetDlgItem(hDlg, IDD_FILENAME), fEnable);
					EnableWindow(GetDlgItem(hDlg, IDD_ITEMCUE),  fEnable);
					EnableWindow(GetDlgItem(hDlg, IDD_ITEMNAME), fEnable);
					break;

				case IDD_MENUOBJECTS:
					{
					BOOL			fEnable;

					if (HIWORD(lParam) == CBN_SELCHANGE)
						{
						fEnable = ((int) SendMessage((HWND) LOWORD(lParam),
																					CB_GETCURSEL, 0, 0) != 0);
						EnableWindow(GetDlgItem(hDlg, IDD_EDITMENU), fEnable);
						}

					break;
					}

				case IDD_EDITMENU:
					{
					HWND			hMenus;
					short			nSel;
					OBJECTID	oiMenu;

					hMenus = GetDlgItem(hDlg, IDD_MENUOBJECTS);
					nSel   = (short) SendMessage(hMenus, CB_GETCURSEL, 0, 0);
					oiMenu = (OBJECTID) SendMessage(hMenus, CB_GETITEMDATA, nSel, 0);

					I_ASSERT(oiMenu != NULL);
					AObjEdit(oiMenu);
					break;
					}

				case IDOK:
					pObj = (SROLEObject*) GetWindowLong(hDlg, DWL_USER);
					pObj->DlgGetData(hDlg);
					EndDialog(hDlg, TRUE);
					break;
					
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;
				}
			break;
		}

	return fRet;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::CopyData
//
// DESC:			Copies data from datablock in 'this' to the datablock
//						in oDst.
//
////////////////////////////////////////////////////////////////

VOID	SROLEObject::CopyData(
	OBJECTID		oDst,
	LONG				lBlock
	)
	{
	VOID*							pSrc;
	VOID*							pDst;
	LONG							lSize;

	lSize = AObjGetDataSize(m_Object, lBlock);
	AObjResizeData(oDst, lBlock, lSize);

	pDst = AObjLockData(oDst,     lBlock);
	pSrc = AObjLockData(m_Object, lBlock);

	if (pSrc && pDst && lSize > 0)
		hmemcpy(pDst, pSrc, lSize);

	AObjUnlockData(m_Object, lBlock);
	AObjUnlockData(oDst,     lBlock);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::Create
//
// DESC:			Initializes an OLE Object.  Since this is a runtime-only object,
//						we can't load it from memory.
//
////////////////////////////////////////////////////////////////

VOID	SROLEObject::Create(
	SROleClientDoc*		pDoc,							// in:  document that owns object
	BOOL							fRuntime					// in:  runtime or design time
	)
	{
	int								nSize;
	OLECFGDATA*				pOleData;

	I_ASSERT(m_Object != 0);
	I_ASSERT(AObjGetDataCount(m_Object) == 0);

	AObjSetDataCount(m_Object, OLEDATA_BLOCKCOUNT);

	// Initialize cfgblock of data
	nSize = OLECFGDATA::SizeNeeded(NULL, NULL, NULL);

	AObjResizeData(m_Object, OLEDATA_CFGBLOCK, nSize);
	pOleData = new (this) OLECFGDATA(this);

	if (pOleData != NULL)
		pOleData->Init(SOLE_EMBEDDED, NULL, NULL, NULL);

	pOleData->Destroy(this);

	if (fRuntime)
		{
		I_ASSERT(pDoc != NULL);

		// Create object and associate AppWare Object ID with it so we can
		// later retrieve it

		pDoc->CreateObjById(m_Object);
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::Assign
//
// DESC:			Assigns one OLE Object to another OLE Object.
//
////////////////////////////////////////////////////////////////

VOID	SROLEObject::Assign(
	SROleClientDoc*		pDoc,
	OBJECTID					oDst
	)
	{
	// 'this' is the source object

	I_ASSERT(pDoc != NULL);

	if (m_Object != oDst)						// not assigning to ourselves
		{
		SROleClientObj*		pObj	= pDoc->GetObjById(oDst);

		// Delete runtime information
		if (pObj)
			pDoc->DeleteObj(pObj);

		// Copy over datablocks
		AObjSetDataCount(oDst, AObjGetDataCount(m_Object));

		CopyData(oDst, OLEDATA_CFGBLOCK);
		CopyData(oDst, OLEDATA_OLEBLOCK);

		// Create new runtime manager and do Load if there is data
		pObj = pDoc->CreateObjById(oDst);
		if (pObj)
			{	
			SROLEObject*		psrDst		= (SROLEObject*) AObjGetRuntimeInfo(oDst);

			psrDst->Init();		// reinit runtime flags

			if (psrDst->RuntimeInfo() & ORI_HASDATA)
				pObj->Load();
			}
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::PreWrite
//
// DESC:			If the OLE object is open, then the user is asked if it
//						should be updated for the save.  If 'yes' then update it.
//
////////////////////////////////////////////////////////////////

VOID	SROLEObject::PreWrite(
	VOID
	)
	{
	SROleClientDoc*		pDoc		= SRCurrentDoc();

	if (pDoc != NULL)
		pDoc->Save();
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::Export
//
// DESC:			Exports the object via UPSF
//
////////////////////////////////////////////////////////////////

LONG	SROLEObject::Export(
	VOID
	)
	{
	OLECFGDATA*		pCfg		= new (this) OLECFGDATA(this);

	if (pCfg != NULL)
		{
		OLEOPTIONS	nOptions			= pCfg->Options();
		OBJECTID		oiMenu				= pCfg->MenuObject();
		LPCSTR			pszClass			= pCfg->ObjectType();
		LPCSTR			pszLinkFile		= pCfg->LinkFile();
		LPCSTR			pszLinkItem		= pCfg->LinkItem();
		WORD				nMenuPos			= pCfg->MenuPosition();

		if (pszClass == NULL)
			pszClass = "";
		if (pszLinkFile == NULL)
			pszLinkFile = "";
		if (pszLinkItem == NULL)
			pszLinkItem = "";

		AUpsfExportField("Options", 0, AUPSF_NATV_SHORT, AUPSF_UNIV_INTEGER,
										&nOptions, sizeof(OLEOPTIONS));
		AUpsfExportField("Menu", 0, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF,
										&oiMenu, sizeof(OBJECTID));
		AUpsfExportField("Class", 0, AUPSF_NATV_STRING, AUPSF_UNIV_STRING,
										(void*) pszClass, strlen(pszClass));
		AUpsfExportField("LinkFile", 0, AUPSF_NATV_STRING, AUPSF_UNIV_STRING,
										(void*) pszLinkFile, strlen(pszLinkFile));
		AUpsfExportField("LinkItem", 0, AUPSF_NATV_STRING, AUPSF_UNIV_STRING,
										(void*) pszLinkItem, strlen(pszLinkItem));
		AUpsfExportField("MenuPos", 0, AUPSF_NATV_WORD, AUPSF_UNIV_INTEGER,
										&nMenuPos, sizeof(WORD));

		pCfg->Destroy(this);
		}

	return A_OK;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::Import
//
// DESC:			Imports the object via UPSF
//
////////////////////////////////////////////////////////////////

LONG	SROLEObject::Import(
	VOID
	)
	{
	OLEOPTIONS		nOptions;
	OBJECTID			oiMenu;
	char					szClass[MAXKEY];
	char					szLinkFile[_MAX_PATH];
	char					szLinkItem[_MAX_PATH];
	WORD					nMenuPos;
	WORD					nSize;

	AUpsfImportField("Options", 0, AUPSF_NATV_SHORT,
									&nOptions, sizeof(OLEOPTIONS));
	AUpsfImportField("Menu", 0, AUPSF_NATV_OBJECTREF,
									&oiMenu, sizeof(OBJECTID));
	AUpsfImportField("Class", 0, AUPSF_NATV_STRING,
									szClass, sizeof(szClass));
	AUpsfImportField("LinkFile", 0, AUPSF_NATV_STRING,
									szLinkFile, sizeof(szLinkFile));
	AUpsfImportField("LinkItem", 0, AUPSF_NATV_STRING,
									szLinkItem, sizeof(szLinkItem));
	AUpsfImportField("MenuPos", 0, AUPSF_NATV_WORD,
									&nMenuPos, sizeof(WORD));

	nSize = OLECFGDATA::SizeNeeded(szClass, szLinkFile, szLinkItem);

	if (AObjResizeData(m_Object, OLEDATA_CFGBLOCK, nSize))
		{
		OLECFGDATA*		pCfg;

		pCfg = new (this) OLECFGDATA(this);

		if (pCfg != NULL)
			{
			pCfg->Init(nOptions, szClass, szLinkFile, szLinkItem);

			pCfg->SetMenuObject(oiMenu);
			pCfg->SetMenuInsertPos(nMenuPos);

			pCfg->Destroy(this);
			}
		}

	Init();		// reinit runtime flags

	return A_OK;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::Edit
//
// DESC:			Brings up the edit dialog box on the OLE object.
//
////////////////////////////////////////////////////////////////

BOOL	SROLEObject::Edit(
	VOID
	)
	{
	return DialogBoxParam(OLEMODULE->Instance(), OLEDLG, GetActiveWindow(),
													DlgProc, (LPARAM) this);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::CheckState
//
// DESC:			Performs the compile-time check of the object.
//
////////////////////////////////////////////////////////////////

LONG	SROLEObject::CheckState(
	void
	)
	{
	LONG				lState;

	if (RuntimeInfo() & ORI_VALID)
		lState = A_OK;

	else
		{
		lState = AObjReportError(m_Object, AOBJ_CHECKOBJECT, A_WARNING,
															"Object has not been initialized.  Continue compilation?",
															0);
		}

	return lState;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::ValidateIds
//
// DESC:			Performs validation on object Ids
//
////////////////////////////////////////////////////////////////

VOID	SROLEObject::ValidateIds(
	AObjMessage*		pSysMessage
	)
	{
	OLECFGDATA*		pCfg		= new (this) OLECFGDATA(this);
	OBJECTID			oiMenu;

	if (pCfg)
		{
		oiMenu = pCfg->MenuObject();

		AObjValidateID(&oiMenu, pSysMessage);

		pCfg->SetMenuObject(oiMenu);

		pCfg->Destroy(this);
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::QueryProtocol
//
// DESC:			Performs the QueryProtocol behavior
//
////////////////////////////////////////////////////////////////

LONG	SROLEObject::QueryProtocol(
	ApQueryInfo*		pqi
	)
	{
	LONG						lRet;

	if (pqi->protocol == APROT_WND)
		{
		lRet = AP_SUPPORTED;
		pqi->pfnCreate = (APPCFN) OLEObjectVICreate;
		}
	else
		{
		lRet = AP_NOTSUPPORTED;
		}

	return lRet;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::DeleteData
//
// DESC:			Deletes the OLEDATA_OLEBLOCK information
//
////////////////////////////////////////////////////////////////

VOID	SROLEObject::DeleteData(
	VOID
	)
	{
	// This effectively kills the data
	AObjResizeData(m_Object, OLEDATA_OLEBLOCK, 0);

	ClrRuntimeInfo(ORI_HASDATA);
	}


////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::ObjectType
//
// DESC:			Copies the object type (ASCII string) of the OLE object.
//
////////////////////////////////////////////////////////////////

VOID		SROLEObject::ObjectType(
	LPSTR		pszBuf,
	int			nBufSize
	)
	{
	OLECFGDATA*		pData			= new (this) OLECFGDATA(this);
	LPCSTR				pszType;

	I_ASSERT(pszBuf != NULL);

	pszType = pData->ObjectType();

	if (pszType != NULL)
		strncpy(pszBuf, pszType, nBufSize);
	else
		*pszBuf = '\0';

	pData->Destroy(this);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::LinkFile
//
// DESC:			Copies the link file of the OLE object.
//
////////////////////////////////////////////////////////////////

VOID		SROLEObject::LinkFile(
	LPSTR		pszBuf,
	int			nBufSize
	)
	{
	OLECFGDATA*		pData			= new (this) OLECFGDATA(this);
	LPCSTR				pszFile;

	I_ASSERT(pszBuf != NULL);

	pszFile = pData->LinkFile();
	if (pszFile != NULL)
		strncpy(pszBuf, pszFile, nBufSize);
	else
		*pszBuf = '\0';

	pData->Destroy(this);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::LinkItem
//
// DESC:			Copies the linked item within the link file of the OLE
//						object.
//
////////////////////////////////////////////////////////////////

VOID	SROLEObject::LinkItem(
	LPSTR		pszBuf,
	int			nBufSize
	)
	{
	OLECFGDATA*		pData			= new (this) OLECFGDATA(this);
	LPCSTR				pszItem;

	I_ASSERT(pszBuf != NULL);

	pszItem = pData->LinkItem();
	if (pszItem != NULL)
		strncpy(pszBuf, pszItem, nBufSize);
	else
		*pszBuf = '\0';

	pData->Destroy(this);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::Options
//
// DESC:			Returns the options that the OLE object was created with.
//
////////////////////////////////////////////////////////////////

OLEOPTIONS	SROLEObject::Options(
	VOID
	)
	{
	OLECFGDATA*		pData			= new (this) OLECFGDATA(this);
	OLEOPTIONS		pOptions	= pData->Options();

	pData->Destroy(this);

	return pOptions;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::MenuObject
//
// DESC:			Returns the menu object that is associated with the OLE object.
//
////////////////////////////////////////////////////////////////

OBJECTID	SROLEObject::MenuObject(
	VOID
	)
	{
	OLECFGDATA*		pData			= new (this) OLECFGDATA(this);
	OBJECTID			oiMenu		= pData->MenuObject();

	pData->Destroy(this);

	return oiMenu;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::MenuInsertPos
//
// DESC:			Returns the menu position that is manipulated by the OLE object.
//
////////////////////////////////////////////////////////////////

short	SROLEObject::MenuInsertPos(
	VOID
	)
	{
	OLECFGDATA*		pData			= new (this) OLECFGDATA(this);
	short					nPos			= pData->MenuPosition();

	pData->Destroy(this);

	return nPos;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::CreateDataBlock
//
// DESC:			Creates the OLE datablock with the given size.  The
//						pointer is returned (the block is locked).
//
////////////////////////////////////////////////////////////////

VOID*		SROLEObject::CreateDataBlock(
	DWORD			dwSize
	)
	{
	VOID*			pData		= NULL;

	if (AObjGetDataCount(ObjectId()) < OLEDATA_BLOCKCOUNT)
		AObjSetDataCount(ObjectId(), OLEDATA_BLOCKCOUNT);

	if (AObjGetDataCount(ObjectId()) == OLEDATA_BLOCKCOUNT)
		AObjResizeData(ObjectId(), OLEDATA_OLEBLOCK, (LONG) dwSize);

	if (AObjGetDataSize(ObjectId(), OLEDATA_OLEBLOCK) == dwSize)
		pData = AObjLockData(ObjectId(), OLEDATA_OLEBLOCK);

	return pData;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::LockDataBlock
//
// DESC:			Returns a pointer to the locked OLE data datablock.
//
////////////////////////////////////////////////////////////////

VOID*		SROLEObject::LockDataBlock(
	VOID
	)
	{
	VOID*			pData		= NULL;

	if (AObjGetDataCount(ObjectId()) == OLEDATA_BLOCKCOUNT)
		pData = AObjLockData(ObjectId(), OLEDATA_OLEBLOCK);

	return pData;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::UnlockDataBlock
//
// DESC:			Unlocks the OLE data datablock.
//
////////////////////////////////////////////////////////////////

VOID	SROLEObject::UnlockDataBlock(
	VOID
	)
	{
	AObjUnlockData(ObjectId(), OLEDATA_OLEBLOCK);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::ObjectEvent
//
// DESC:			Handles general events.
//
////////////////////////////////////////////////////////////////

VOID	SROLEObject::ObjectEvent(
	AEvtInfo*			pEvent
	)
	{
	I_ASSERT(pEvent != NULL);

	switch ((short) pEvent->details)
		{
		case AEVT_G_ACTIVATE:
			{
			SROleClientApp*		pApp;
			SROleClientDoc*		pDoc;
			SROleClientObj*		pObj;

			pApp = (SROleClientApp*) ATypeGetRuntimeInfo(OTYPE_OLE1CLIENT);

			if ((pApp != NULL) && ((pDoc = pApp->GetDoc()) != NULL))
				{
				pObj = pDoc->GetObjById(pEvent->targetObject);

				// There is a small possibility that pObj is null at this point:
				// if the user uses Delete Ole Object function and then has
				// a breakpoint in the function chain after that, then we'll
				// get an Activate on the window item after the object has been
				// deleted.

				if (pObj != NULL &&
						pObj->OleObject() != NULL &&
						MenuObject() != NULL)
					pObj->ChangeMenu(MenuObject(), MenuInsertPos());
				}

			break;
			}


		case AEVT_G_RESETMENU:
			{
			SROleClientApp*		pApp;
			SROleClientDoc*		pDoc;
			SROleClientObj*		pObj;

			pApp = (SROleClientApp*) ATypeGetRuntimeInfo(OTYPE_OLE1CLIENT);

			if ((pApp != NULL) && ((pDoc = pApp->GetDoc()) != NULL))
				{
				pObj = pDoc->GetObjById(pEvent->targetObject);

				if (pObj != NULL && MenuObject() != NULL)
					pObj->ResetMenu(MenuObject(), MenuInsertPos());
				}
			break;
			}

		default:
			I_ASSERT(0);
			break;
		}
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::PostObjectEvent
//
// DESC:			Posts general events.
//
////////////////////////////////////////////////////////////////

VOID	SROLEObject::PostObjectEvent(
	LONG		lDetails
	)
	{
	AEvtPostStandard(ObjectId(), AOLE_EVT_GENERAL, lDetails, AEVT_ATTAIL);
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::Draw
//
// DESC:			Draws the OLE object (if it exists) to the given DC.
//
////////////////////////////////////////////////////////////////

BOOL	SROLEObject::Draw(
	HDC			hDC,
	RECT*		prcItem								// bounding box of visual instance
	)
	{
	BOOL							fDrawn	= FALSE;
	SROleClientDoc*		pDoc		= SRCurrentDoc();
	SROleClientObj*		pObj;

	// The object is drawn both at design time and at runtime.  If at
	// design time, then pDoc will be NULL, else we search for
	// the object.

	if (pDoc)
		pObj = pDoc->GetObjById(ObjectId());
	else
		pObj = NULL;

	if (pDoc == NULL)				// if at design time
		{
		Rectangle(hDC, prcItem->left, prcItem->top,
										prcItem->right - 1, prcItem->bottom - 1);
		fDrawn = TRUE;
		}
	else if (pObj != NULL)	// not always non-null; deleting obj will do this
		{
		fDrawn = pObj->Draw(hDC, prcItem);
		}

	return fDrawn;
	}

////////////////////////////////////////////////////////////////
//
// METHOD:		SROLEObject::Activate
//
// DESC:			Activates the OLE object (if it exists) with the default verb.
//
////////////////////////////////////////////////////////////////

VOID	SROLEObject::Activate(
	VOID
	)
	{
	SROleClientDoc*		pDoc		= SRCurrentDoc();
	SROleClientObj*		pObj;

	// The object is drawn both at design time and at runtime.  If at
	// design time, then pDoc will be NULL, else we search for
	// the object.

	if (pDoc)
		pObj = pDoc->GetObjById(ObjectId());
	else
		pObj = NULL;

	if (pObj)
		{
		pObj->Activate(OLEVERB_PRIMARY);
		}
	}

//****************************************************************
//			 AppWare-required ADMIN routine

////////////////////////////////////////////////////////////////
//
// FUNCTION:	OleAdmin
//
// DESC:			Admin function for the AppWare OLE Object.  Called during
//						design time and runtime.
//
////////////////////////////////////////////////////////////////

extern "C"
LONG ALMCALLBACK		OleAdmin(
	OBJECTID					theObject,		
	pAObjMessage			theSystem
	)
	{
	LONG							lRet			= A_OK;
	SROleClientDoc*		pDoc;
	BOOL							fRuntime;
	SROLEObject*			pObject;
	

	I_ASSERT(theSystem != NULL);
	I_ASSERT(theObject != 0 ||
						(theSystem->message1 == AOBJ_QUERYPROTOCOL ||
						 theSystem->message1 == AOBJ_REPORTREQUIREDFILES));

	if (theObject != 0)
		pObject = (SROLEObject*) AObjGetRuntimeInfo(theObject);
	else
		pObject = NULL;

	fRuntime = (theSystem->message4 & AOBJ_TEMPORARY) ? TRUE : FALSE;

	switch (theSystem->message1)
		{
		case AOBJ_AWAKENED:
			I_ASSERT(pObject == NULL);

			pObject = new SROLEObject(theObject);
			AObjSetRuntimeInfo(theObject, (LONG) pObject);
			lRet = A_CONTINUE;
			break;

		case AOBJ_CREATED:
			I_ASSERT(pObject == NULL);

			pObject = new SROLEObject(theObject);
			AObjSetRuntimeInfo(theObject, (LONG) pObject);

			I_ASSERT(pObject != NULL);

			if (fRuntime)
				pDoc = SRCurrentDoc();
			else
				pDoc = NULL;

			pObject->Create(pDoc, fRuntime);
			lRet = A_OK;
			break;

		case AOBJ_ASSIGNOBJECT:
			I_ASSERT(pObject != NULL);
			// Reverse the assignment since we destroy the destination's 'this'
			// object
			pObject = (SROLEObject*) AObjGetRuntimeInfo((OBJECTID) theSystem->message3);
			pObject->Assign(SRCurrentDoc(), theObject);
			break;

		case AOBJ_PREWRITE:
			// Check for open embedded objects and save if directed to.
			I_ASSERT(pObject != NULL);
			pObject->PreWrite();
			lRet = A_CONTINUE;
			break;

		case AOBJ_ASLEEP:
		case AOBJ_DESTROYED:
			I_ASSERT(pObject != NULL);
			delete pObject;
			AObjSetRuntimeInfo(theObject, 0);
			lRet = A_CONTINUE;
			break;

		case AOBJ_EXPORTOBJECT:
			I_ASSERT(pObject != NULL);
			lRet = pObject->Export();
			break;
			
		case AOBJ_IMPORTOBJECT:
			I_ASSERT(pObject != NULL);
			lRet = pObject->Import();
			break;
			
		case AOBJ_EDITOBJECT:								// configure object
			I_ASSERT(pObject != NULL);
			if (pObject->Edit())
				lRet = A_MODIFIED;
			break;

		case AOBJ_VALIDATEOBJECTIDS:				// update object ids within OLE object
			I_ASSERT(pObject != NULL);
			pObject->ValidateIds(theSystem);
			lRet = A_OK;
			break;

		case AOBJ_BUILDSIGNALS:							// return signal names
			I_ASSERT(pObject != NULL);
			pObject->BuildSignals((MEMBLOCKID) theSystem->message2);
			break;

		case AOBJ_CHECKOBJECT:							// check validity of object
			I_ASSERT(pObject != NULL);
			lRet = pObject->CheckState();
			break;

		case AOBJ_QUERYPROTOCOL:
			lRet = SROLEObject::QueryProtocol((ApQueryInfo*) theSystem->message4);
			break;

		default:
			lRet = A_NOTHANDLED;
			break;
		}

	return lRet;
	}

//****************************************************************
//								Event-handlers for OLE object type

////////////////////////////////////////////////////////////////
//
// FUNCTION:	OleStartupEvent
//
// DESC:			Handles the startup event.  This event is sent for every
//						application that uses the OLE Object.
//
////////////////////////////////////////////////////////////////

VOID ALMCALLBACK _export		OleStartupEvent(
	pAEvtInfo					theEvent,
	pAObjMessage			theSystem
	)
	{
	SROleClientDoc*		pDoc				= NULL;
	SROleClientApp*		pApp;
	MEMBLOCKID				mbAppName;
	char*							pszAppName;
	BOOL							fUnlockBlock;

	I_UNUSED(theEvent);
	I_UNUSED(theSystem);

	I_ASSERT(theEvent != NULL && theSystem != NULL);

	// A new application object should be created at this time.
	// Since we force one document onto the application, we also create
	// the ole document here.

	mbAppName = AMemAllocate(0);
	if (mbAppName)
		{
		AAppGetName(mbAppName);
		pszAppName = (char*) AMemLock(mbAppName);
		fUnlockBlock = TRUE;
		}
	else
		{
		pszAppName = NULL;
		fUnlockBlock = FALSE;
		}

	if (pszAppName == NULL)
		pszAppName = "An AppWare Application";			//!! move to resource

	pApp = new SROleClientApp(pszAppName);

	if (pApp != NULL && pApp->Init())
		{
		if ((pDoc = new SROleClientDoc(pApp)) != NULL && pDoc->Register(pszAppName))
			{
			ATypeSetRuntimeInfo(OTYPE_OLE1CLIENT, (LONG) pApp);
			pApp->AddDoc(pDoc);

			// Enumerate through all SOLE Objects and do
			// initialization on them (mainly create SROleClientObj objects)

			if (pDoc->InitObjects())
				{
				pDoc->LoadObjects();
				pDoc->ResetMenus();
				}
			}
		else
			{
			if (pDoc != NULL)
				delete pDoc;

			pApp->Cleanup();
			delete pApp;
			}
		}

	else
		{
		//!! critical error occurred here!
		if (pApp != NULL)
			delete pApp;
		}

	if (fUnlockBlock)
		AMemUnlock(mbAppName);

	if (mbAppName)
		AMemFree(mbAppName);
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	OleShutdownEvent
//
// DESC:			Handles the shutdown event.  This event is sent for every
//						application that uses the OLE Object.
//
////////////////////////////////////////////////////////////////

VOID ALMCALLBACK _export		OleShutdownEvent(
	pAEvtInfo					theEvent,
	pAObjMessage			theSystem
	)
	{
	SROleClientApp*		pApp;

	I_UNUSED(theEvent);
	I_UNUSED(theSystem);

	I_ASSERT(theEvent != NULL && theSystem != NULL);

	pApp = (SROleClientApp*) ATypeGetRuntimeInfo(OTYPE_OLE1CLIENT);

	if (pApp == NULL)
		{
		// probably occurred because of failure during StartupEvent
		}
	else
		{
		ATypeSetRuntimeInfo(OTYPE_OLE1CLIENT, 0);

		pApp->Cleanup();

		delete pApp;
		}
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	OleMenuEvent
//
// DESC:			Handles the MENUITEMCHOSE event.  This event is sent when
//						a menu item that was created by the OLE system is chosen
//						by the user.
//
////////////////////////////////////////////////////////////////

VOID ALMCALLBACK _export		OleMenuEvent(
	pAEvtInfo					theEvent,
	pAObjMessage			theSystem
	)
	{
	SROleClientApp*		pApp;
	SROleClientDoc*		pDoc;
	SROleClientObj*		pObj;

	I_UNUSED(theSystem);

	I_ASSERT(theEvent != NULL && theSystem != NULL);

	pApp = (SROleClientApp*) ATypeGetRuntimeInfo(OTYPE_OLE1CLIENT);

	if (pApp == NULL)
		{
		// probably occurred because of failure during StartupEvent
		}
	else
		{
		pDoc = pApp->GetDoc();

		if (pDoc != NULL)
			{
			I_ASSERT(theEvent->targetObject != NULL);

			pObj = pDoc->GetObjById(theEvent->targetObject);

			if (pObj != NULL)
				pObj->ActivateByMenu((short) theEvent->details);
			}
		}
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	OleGenEvent
//
// DESC:			Handles OLE-internal events.  These events are typically
//						time-shifted actions.
//
////////////////////////////////////////////////////////////////

VOID ALMCALLBACK _export		OleGenEvent(
	pAEvtInfo					theEvent,
	pAObjMessage			theSystem
	)
	{
	SROLEObject*			psrObject;

	I_UNUSED(theSystem);

	I_ASSERT(theEvent != NULL && theSystem != NULL);

	switch (theEvent->details)
		{
		default:
			I_ASSERT(theEvent->targetObject != NULL);

			psrObject = (SROLEObject*) AObjGetRuntimeInfo(theEvent->targetObject);

			I_ASSERT(psrObject != NULL);

			psrObject->ObjectEvent(theEvent);
			break;
		}
	}

