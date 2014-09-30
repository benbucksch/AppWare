/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** */

#ifndef O_PICT_H
#define O_PICT_H

/* ***********************************************************************
 * ATYPEID for Picture Object
 * *********************************************************************** */

#define OTYPE_PICTURE                 ATYPE_MAKEID('P', 'i', 'c', 't')

/* ***********************************************************************
 * EVENTIDs for Picture Object
 * *********************************************************************** */

/*
* Description:	In response to this event the picture ALM will
*								return the Bitmap stored in the picture object
*		            in a DIB format.  Currently a METAFILE type picture does not return
*								anything.
*               The Caller should create a 0 sized MemblockID and store it in the
*								Details field of the pAEvtInfo Structure.  The Piture Method will resize
*								the Memblock to store the DIB data.
*								The Memblock will contain	: BITMAPINFOHEADER
*																					: RGBTABLE
*																					: IMAGE BITS
*
*
*/

#define OPICT_EVT_OBJECTTOBITMAP      AEVENT_MAKEID('o', 't', 'o', 'B')

/*
* Description:  In response to this event the picture ALM will set the datas of the
*								picture object in a DIB format.The details field of the pAEvtInfo Structure
*								should contain the AMEMBLOCKID of the DIB data.
*								The Memblock will contain	: BITMAPINFOHEADER
*																					: RGBTABLE
*																					: IMAGE BITS

*
*/
#define OPICT_EVT_BITMAPTOOBJECT      AEVENT_MAKEID('b', 't', 'o', 'O')


// Not Supported
#define OPICT_EVT_OBJECTTOMETAFILE    AEVENT_MAKEID('o', 't', 'o', 'M')

// Not Supported
#define OPICT_EVT_METAFILETOOBJECT    AEVENT_MAKEID('m', 't', 'o', 'O')

/* ***********************************************************************
 * API Function prototypes for Picture Object
 * *********************************************************************** */

/* Get the picture handle for any visual instance of the picture Object (id = itemID) .
	 The handle is returned in the lHandle param */

extern "C" BOOL ALMAPI  OPictGetHandle(AOBJECTID itemID, LPLONG lHandle);

#endif  /* !O_PICT_H */
