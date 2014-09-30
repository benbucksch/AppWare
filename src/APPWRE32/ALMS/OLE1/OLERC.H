/*//////////////////////////////////////////////////////////////
//
//					Copyright 1994, Novell, Inc.  All rights reserved
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
// FILE:					OLERC.H
//
// OWNER:					Scott McCarty
//
// DESCRIPTION:		
//
///////////////////////////////////////////////////////////////*/

#ifndef OLERC_H
#define OLERC_H

////////////////////////////////////////////////////////////////
//
// If we are not in the RC compiler, force all ids to be MAKEINTRESOURCE
// so we can reference them as real IDs instead of strings.
//
////////////////////////////////////////////////////////////////

#ifndef RCINVOKED
#define ID(x)							MAKEINTRESOURCE(x)
#else
#define	ID(x)							x
#endif

////////////////////////////////////////////////////////////////
//
// Definitions for all resource ids.
//
////////////////////////////////////////////////////////////////

#define OLEDLG						ID(100)
#define OLEICON						ID(101)

#define IDD_NAME					10				/* blank area to draw name              */
#define IDD_OBJECTTYPES		11				/* combobox to list OLE server classes  */
#define IDD_EMBEDDED			12				/* radio button:  specifies embedded    */
#define IDD_LINKED				13				/* radio button:  specifies linked      */
#define IDD_FILECUE				14				/* static text:  "File Name:"           */
#define IDD_FILENAME			15				/* edit:  holds file to link to         */
#define IDD_ITEMCUE				16				/* static text:  "Item Name:"           */
#define IDD_ITEMNAME			17				/* edit:  holds item name to link to    */
#define IDD_HELP					18				/* help button                          */
#define IDD_MENUOBJECTS		19				/* combobox to list available menus     */
#define IDD_EDITMENU			20				/* button to edit current menu object   */
#define IDD_TRACKSIZE			21				/* checkbox to track server-size of obj */
#define IDD_MENUITEMPOS		22				/* numeric edit:  pos in menu to insert */

#endif	// !OLERC_H
