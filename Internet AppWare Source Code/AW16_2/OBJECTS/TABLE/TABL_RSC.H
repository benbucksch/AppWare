////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//  WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//  OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//  HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:  "tabl_rsc.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __TABL_RSC_H  // prevent multiple includes
#define __TABL_RSC_H

// Alignment constants /////////////////////////////////////////////////////

#ifndef ALIGN_xxxx
  #define ALIGN_xxxx
  #define ALIGN_ERROR           (-1)
  #define ALIGN_LEFT            310
  #define ALIGN_CENTER          309
  #define ALIGN_RIGHT           311
  #define ALIGN_TITLELEFT         2
  #define ALIGN_TITLECENTER       1
  #define ALIGN_TITLERIGHT        3
#endif

// Object Defines //////////////////////////////////////////////////////////

#define TABLEDIALOG             1000
#define TABLEATTRIBUTES         1100
#define TABLEICON               1200
#define RESIZECURSOR            1300
#define BORDERBITMAP            1400

// TableDialog constants ///////////////////////////////////////////////////

// static text fields           
#define IDD_OBJECT              200
#define IDD_STATICTITLE         201

// group boxes
#define IDD_DEFCOLUMNATTRIB     204
#define IDD_SELCOLUMNATTRIB     205
#define IDD_ALIGNMENT           206
#define IDD_TITLEALIGNMENT      207
#define IDD_WIDTH               208

// list boxes                   
#define IDD_COLUMNS             210
#define IDD_AVAILOBJS           211

// standard push buttons        
#define IDD_ADD                 220
#define IDD_REMOVE              221
#define IDD_EDIT                222
#define IDD_TITLEFONT           230
#define IDD_CELLFONT            231
#define IDOK                    1
#define IDCANCEL                2
#define IDD_OPTIONS             240
#define IDHELP                  998

// text edit controls           
#define IDD_TITLE               250
#define IDD_PIXELS              251
#define IDD_CHARACTERS          252
#define IDD_INCHES              253

// check boxs
#define IDD_EDITABLE            260

// radio buttons
#define IDD_LEFT                ALIGN_LEFT
#define IDD_CENTER              ALIGN_CENTER
#define IDD_RIGHT               ALIGN_RIGHT
#define IDD_TITLELEFT           (ALIGN_TITLELEFT  + 500)
#define IDD_TITLECENTER         (ALIGN_TITLECENTER + 500)
#define IDD_TITLERIGHT          (ALIGN_TITLERIGHT + 500)
#define IDD_ALL                 280
#define IDD_TEXT                281
#define IDD_NUMBER              282
#define IDD_DATE                283
#define IDD_TIME                284
#define IDD_SOURCEFIELDS        285
#define IDD_RBPIXELS            290
#define IDD_RBCHARACTERS        291
#define IDD_RBINCHES            292

// Table Attributes Dialog Constants ///////////////////////////////////////

// check boxes
#define IDD_AUTOFILL            400
#define IDD_MULTISELECT         401
#define IDD_SHOWTITLES          402
#define IDD_CANACTIVATE         403
#define IDD_VSCROLL             404
#define IDD_HSCROLL             405
#define IDD_ROWLINES            406
#define IDD_COLUMNLINES         407
#define IDD_CANRESIZE           408
#define IDD_SHOWCOLSEL          409
#define IDD_SHOWCELLSEL         410
#define IDD_CLAIRVOYANTSEL      411
#define IDD_ADHOC               412
// push buttons
#define IDOK                    1
#define IDCANCEL                2
#define IDHELP                  998

// combo boxes
#define IDD_ROWINDEX            420
#define IDD_DATASOURCE          421
#define IDD_INDEXEDFIELD        422

// text edit controls
#define IDD_FIXFIRST            430
#define IDD_NUMBUFFERED         431

// static text fields
#define IDD_OBJECT              200
#define IDD_FIXFIRSTSTATIC      441
#define IDD_COLUMNSTATIC        442
#define IDD_DATASOURCESTATIC    443
#define IDD_INDEXEDFIELDSTATIC  444
#define IDD_ROWINDEXSTATIC      445
#define IDD_NUMBUFFEREDSTATIC   446

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef __TABL_RSC_H
