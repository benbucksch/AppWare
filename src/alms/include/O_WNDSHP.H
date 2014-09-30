/* ***********************************************************************
 *
 * Copyright (c) 1993-1994 Novell, Inc.  All rights reserved.
 *
 * THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
 * TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
 * COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
 * CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
 * TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
 * NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
 * AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
 * LIABILITY.
 *
 * *********************************************************************** */

#ifndef O_WNDSHP_H
#define O_WNDSHP_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Shape Library Admin Procedure */

typedef LONG (ALMAPI* OShapeAdmin) (UINT, pAObjMessage);
typedef LONG (ALMAPI* OLayoutAdmin)(UINT, pAObjMessage);

/* Messages sent to Admin Procedure */

/* General library info and setup:  called once only; sent before all other messages               */
#define OSHP_SETLAYOUTINFO      0       /* .msg1 is layout handle, msg2 is pfn                     */
#define OSHP_QUERYLIBINFO       1       /* return createfn in msg1, ATYPEID in .msg2, caps in .msg3 */
#define OSHP_MENUSTRING         2       /* return string in char* msg1, buflen in msg2             */

/* Tool and shape info:  called many times */
#define OSHP_QUERYTOOLINFO      3       /* .msg1 == tool id; return capabilities in .msg2; return HCURSOR in .msg3 */
#define OSHP_GETSHAPENAME       4       /* message1 = VI, msg2 = psz, msg3 = len                                   */
#define OSHP_QUERYVERSIONANDID  15      /* message1 = VI, msg2 = (short)version, msg3 = (short)shape id            */

/* Palette control:  palette handle passed in message1 (except for create)        */
#define OSHP_CREATEPALETTE      5       /* returns handle in message1             */
#define OSHP_DESTROYPALETTE     6       /* destroys palette                       */
#define OSHP_SHOWPALETTE        7       /* shows palette                          */
#define OSHP_HIDEPALETTE        8       /* hides palette                          */
#define OSHP_ENABLEPALETTE      9       /* deadens or resurrects palette          */
#define OSHP_SETPALETTEPOS      10      /* moves palette to msg2, msg3            */
#define OSHP_GETPALETTEPOS      11      /* returns window rect in (LPRECT) msg2   */
#define OSHP_GETPALETTESTATUS   12      /* returns palette status in msg2         */

/* Tool state changes, etc. */
#define OSHP_SETTOOLSTATE       13      /* some tool selected                     */
#define OSHP_SHAPECREATED       14      /* message1 = palette, msg2 = VI of shape */

/* Legal Return Values from Admin Procedure are:
    A_OK, A_NOTHANDLED, A_ERROR */



/* OSHP_QUERYLIBINFO .msg3 bits */

#define OSHP_LIBCAP_HASPALETTE    0x01L   /* maintains a palette for shape selection */


/* OSHP_QUERYTOOLCAPS bits */

#define OSHP_CAP_CREATEONDOWN     0x01L   /* call createfn on button down in layout (per tool) */
#define OSHP_CAP_CREATEONUP       0x02L   /* call createfn on button up (after rect drag)      */
#define OSHP_CAP_CREATEIMMEDIATE  0x04L   /* call create as soon as tool is active             */
#define OSHP_CAP_RESIZEONCREATE   0x08L   /* go into resize mode immediately after create      */
#define OSHP_CAP_NOTIFYONCREATE   0x10L   /* notify with OSHP_SHAPECREATED                     */

/* OSHP_GETPALETTESTATUS bits */

#define OSHP_PS_VISIBLE           0x01L   /* palette is visible          */


/* OSHP_SETTOOLSTATE .msg3 bits */

#define OSHP_TS_ACTIVE            0x01L   /* change tool to be active    */
#define OSHP_TS_INACTIVE          0x02L   /* change tool to be inactive  */
#define OSHP_TS_ENABLED           0x04L   /* change tool to be enabled   */
#define OSHP_TS_DISABLED          0x08L   /* change tool to be disabled  */
#define OSHP_TS_UP                0x10L   /* change tool to be up        */
#define OSHP_TS_DOWN              0x20L   /* change tool to be down      */

/* *****************  API back to layout *************** */

typedef struct _OShpStdToolInfo
  {
  HBITMAP     hImage;     /* layout owns bitmap--shape library shouldn't delete */
  LONG        lStyle;     /* toggle, pushbutton, ... */
  }   OShpStdToolInfo;

/* OShpStdToolInfo.lStyle bits */
#define OSHP_STDCAP_TOGGLE            0x01
#define OSHP_STDCAP_PUSH              0x02
#define OSHP_STDCAP_RADIO             0x04    /* => peer with library-defined tools */

/* Valid .msg2 results for GETCURRENTUNITS */
#define OSHP_UNITS_ENGLISH_IN         0       /* inches */
#define OSHP_UINTS_METRIC_MM          1       /* millimiters */
#define OSHP_UNITS_POINTS_PT          2       /* true points (1 pt == 1/72 in) */
#define OSHP_UNITS_POINTS_TWIPS       3       /* twips (1 pt == 20 twips) */


/* Message IDs back to layout admin procedure */

#define OSHP_MSG_SETACTIVETOOL        1
  /* .msg1 == layout handle
     .msg2 == ATYPEID of shape library       0 => standard tool
     .msg3 == tool id (or OSHP_STDTOOL_*)
     .msg4 == HCURSOR to use (layout does not destroy it)
  */

#define OSHP_MSG_CREATESHAPE          2
  /* .msg1 == layout handle
     .msg2 == x position of new shape (-1 for default position)
     .msg3 == y position of new shape (-1 for default position)
  */

#define OSHP_MSG_GETSTDTOOLINFO       3
  /* .msg1 == layout handle
     .msg2 == std tool #
     .msg3 == OShpStdToolInfo*
  */

#define OSHP_MSG_GETCURRENTUNITS      4
  /* .msg1 == layout handle
     .msg2 == return OSHP_* flag for current units
  */

#define OSHP_MSG_ENABLEPALETTES       5
  /* .msg1 == layout handle
     .msg2 == BOOL (TRUE == enable / FALSE == disable)
     .msg3 == ATYPEID of shape library
  */


/* Predefined shape ids for inserting standard tools in a shape palette */

#define OSHP_STDTOOL_ANCHOR           1     /* anchor objects           */
#define OSHP_STDTOOL_ARROW            2     /* select tool              */
#define OSHP_STDTOOL_BACKGROUND       3     /* select window background */
#define OSHP_STDTOOL_GROUPSTOP        4     /* set groupstop            */
#define OSHP_STDTOOL_LOCK             5     /* set lock status          */
#define OSHP_STDTOOL_REORDER          6     /* order items              */
#define OSHP_STDTOOL_SELECTALL        7     /* select all               */
#define OSHP_STDTOOL_TABSTOP          8     /* set tabstop              */
#define OSHP_STDTOOL_VISIBILITY       9     /* set visibility           */
#define OSHP_STDTOOL_USERTOOL       100     /* reserve 100 std tools    */

/* shapes are required to have the following structure be the first 
   field in their data (extra data).
 */

typedef struct _OShpBase
  {
  short       sVer;    /* shape library version                         */
  short       sType;   /* shape item type (different than library type) */
  }   OShpBase;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* !O_WNDSHP_H */
