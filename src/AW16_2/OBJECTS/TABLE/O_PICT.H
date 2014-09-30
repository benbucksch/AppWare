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

#ifndef O_PICT_H
#define O_PICT_H

/* ***********************************************************************
 * TYPEID for Picture Object
 * *********************************************************************** */

#define OTYPE_PICTURE                 MAKETYPEID('P', 'i', 'c', 't')

/* ***********************************************************************
 * EVENTIDs for Picture Object
 * *********************************************************************** */

#define OPICT_EVT_OBJECTTOBITMAP      MAKEEVENTID('o', 't', 'o', 'B')
#define OPICT_EVT_BITMAPTOOBJECT      MAKEEVENTID('b', 't', 'o', 'O')

#define OPICT_EVT_OBJECTTOMETAFILE    MAKEEVENTID('o', 't', 'o', 'M')
#define OPICT_EVT_METAFILETOOBJECT    MAKEEVENTID('m', 't', 'o', 'O')

/* ***********************************************************************
 * API Function prototypes for Picture Object
 * *********************************************************************** */

/* Get the picture handle for any visual instance */
extern "C" BOOL ALMAPI  OPictGetHandle(OBJECTID itemID, LPLONG lHandle);

#endif  /* !O_PICT_H */
