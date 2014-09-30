////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//  ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          "soundres.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   Resource constants for the AppWare sound object.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef SOUNDRES_H
#define SOUNDRES_H

////////////////////////////////////////////////////////////////////////////

// macro to easily get resource handles from integers
#ifndef ID
#ifndef RCINVOKED
#define ID(x)       MAKEINTRESOURCE(x)
#else
#define ID(x)       x
#endif
#endif

// general constants
#define IDOK                  1
#define IDCANCEL              2
#define IDHELP                998

// bitmap constants
#define BMP_N_FF              1104
#define BMP_N_PAUSE           1102
#define BMP_N_PLAY            1103
#define BMP_N_RECORD          1100
#define BMP_N_REWIND          1105
#define BMP_N_STOP            1101

#define BMP_P_FF              3104
#define BMP_P_PAUSE           3102
#define BMP_P_PLAY            3103
#define BMP_P_RECORD          3100
#define BMP_P_REWIND          3105
#define BMP_P_STOP            3101

#define BMP_F_FF              5104
#define BMP_F_PAUSE           5102
#define BMP_F_PLAY            5103
#define BMP_F_RECORD          5100
#define BMP_F_REWIND          5105
#define BMP_F_STOP            5101

// dialog constants
#define DLG_EDITSOUND         1500
#define DLG_RECORDSOUND       1501

// icon constants
#define ICON_SOUND            1600

// bitmap button constants
#define BB_RECORD             100
#define BB_STOP               101
#define BB_PAUSE              102
#define BB_PLAY               103
#define BB_FF                 104
#define BB_REWIND             105

// Edit sound dialog constants
#define PB_RECORD             110
#define PB_LOAD               111
#define PB_CLEARSOUND         112

#define ST_TYPENAME           120
#define ST_SIZE               121
#define ST_SOUNDFILE          122

#define RB_EXTERNAL           130
#define RB_INTERNAL           131

#define ED_SOUNDFILE          140

// Record sound dialog constants
#define RB_QUALITY_RADIO      200
#define RB_QUALITY_TAPE       201
#define RB_QUALITY_CD         202
#define RB_BPS_4              210
#define RB_BPS_8              211
#define RB_BPS_16             212

#define XB_STEREO             220

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef SOUNDRES_H
