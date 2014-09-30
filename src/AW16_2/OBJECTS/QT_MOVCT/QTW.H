

// ---------------------------------------------------------------------
//
// QTW.H - QuickTime for Windows C and C++ Bindings
//
//         Version 1.0
//
//         (c) 1988-1992 Apple Computer, Inc. All Rights Reserved.
//
// ---------------------------------------------------------------------


// Prevent multiple inclusion
// --------------------------
#ifndef __QTW_H__
#define __QTW_H__


// Movie Controller MCDoAction action constants
// --------------------------------------------
typedef enum
  {mcActionIdle                   =  1, // No Param
   mcActionDraw                   =  2, // No Param
   mcActionActivate               =  3, // No param
   mcActionDeactivate             =  4, // No param
   mcActionKey                    =  6, // Param is pointer to MSG
   mcActionPlay                   =  8, // Param is LFIXED play rate
   mcActionGoToTime               = 12, // Param is pointer to TimeRecord
   mcActionSetVolume              = 14, // Param is SFIXED volume
   mcActionGetVolume              = 15, // Param is pointer to SFIXED volume
   mcActionStep                   = 18, // Param is LONG number of steps
   mcActionSetLooping             = 21, // Param is a BOOL
   mcActionGetLooping             = 22, // Param is pointer to a BOOL
   mcActionSetLoopIsPalindrome    = 23, // Param is BOOL
   mcActionGetLoopIsPalindrome    = 24, // Param is pointer to a BOOL
   mcActionSetGrowBoxBounds       = 25, // Param is pointer to a Rect
   mcActionControllerSizeChanged  = 26, // No param
   mcActionSetSelectionBegin      = 29, // Param is pointer to TimeRecord
   mcActionSetSelectionDuration   = 30, // Param is pointer to TimeRecord
   mcActionSetKeysEnabled         = 32, // Param is BOOL
   mcActionGetKeysEnabled         = 33, // Param is pointer to BOOL
   mcActionSetPlaySelection       = 34, // Param is BOOL
   mcActionGetPlaySelection       = 35, // Param is pointer to BOOL
   mcActionSetUseBadge            = 36, // Param is BOOL
   mcActionGetUseBadge            = 37, // Param is pointer to BOOL
   mcActionSetFlags               = 38, // Param is LONG flags
   mcActionGetFlags               = 39, // Param is pointer to LONG flags
   mcActionSetPlayEveryFrame      = 40, // Param is BOOL
   mcActionGetPlayEveryFrame      = 41, // Param is pointer to BOOL
   mcActionGetPlayRate            = 42, // Param is pointer to LFIXED
   mcActionBadgeClick             = 44  // No param
  } mcActionType;


// Movie Controller MCDoAction, action McActionSetFlags parameter, movie
// controller structure parameter, lfActionFlags
// ---------------------------------------------------------------------
typedef enum
  {mcFlagSuppressStepButtons      = 1<<1,
   mcFlagSuppressSpeakerButton    = 1<<2,
   mcFlagsUseWindowPalette        = 1<<3
  } mcActionFlags;


// Toolbox function NewMovieController and Movie Controller function
// MCPositionController parameter controllerCreationFlags values,
// movie controller structure parameter, lfControllerStateFlags
// -----------------------------------------------------------------
typedef enum
  {mcTopLeftMovie                 = 1<<0,
   mcScaleMovieToFit              = 1<<1,
   mcWithBadge                    = 1<<2,
   mcNotVisible                   = 1<<3
  } mcControllerStateFlags;


// Movie Controller function MCGetControllerInfo parameter mcInfoFlags
// parameter, movie controller structure parameter, lfControllerInfoFlags
// ----------------------------------------------------------------------
typedef enum
  {mcInfoHasSound                 = 1<<5,
   mcInfoIsPlaying                = 1<<6,
   mcInfoIsLooping                = 1<<7,
   mcInfoIsInPalindrome           = 1<<8
  } mcControllerInfoFlags;


// NewMovieFromFile flags
// ----------------------
typedef enum
  {newMovieActive                 = 1<<0,
   newMovieDontResolveDataRefs    = 1<<1,
   newMovieDontAskUnresolvedDataRefs = 1<<2,
   newMovieDontAutoAlternates     = 1<<3
  } newMovieFlags;


// Flags for SetMoviePlayHints
// ---------------------------
typedef enum
  {hintsScrubMode                 = 1<<1,
   hintsInterpolateSound          = 1<<7
  } moviePlayHints;


// Manifest constants for ostypes as flags for Component Manager calls
// -------------------------------------------------------------------
#define kAnyComponentType           0
#define kAnyComponentSubType        0
#define kAnyComponentManufacturer   0
#define kAnyComponentFlagsMask      0


// SetDefaultComponent Flags
// -------------------------
typedef enum
  {defaultComponentIdentical        = 0,
   defaultComponentAnyFlags         = 1,
   defaultComponentAnyManufacturer  = 2,
   defaultComponentAnySubType       = 4
  } defaultComponentFlags;


// RegisterComponent Flags
// -----------------------
typedef enum
  {registerComponentGlobal          = 1,
   registerComponentNoDuplicates    = 2,
   registerComponentAfterExisting   = 4
  } registerComponentFlags;


// Error codes
// -------------
enum
  {
   paramErr                       = -50,
   couldNotResolveDataRef         = -2000,
   badImageDescription            = -2001,
   badPublicMovieAtom             = -2002,
   cantFindHandler                = -2003,
   cantOpenHandler                = -2004,
   badComponentType               = -2005,
   noMediaHandler                 = -2006,
   noDataHandler                  = -2007,
   invalidMedia                   = -2008,
   invalidTrack                   = -2009,
   invalidMovie                   = -2010,
   invalidSampleTable             = -2011,
   invalidDataRef                 = -2012,
   invalidHandler                 = -2013,
   invalidDuration                = -2014,
   invalidTime                    = -2015,
   cantPutPublicMovieAtom         = -2016,
   badEditList                    = -2017,
   mediaTypesDontMatch            = -2018,
   progressProcAborted            = -2019,
   movieToolboxUninitialized      = -2020,
   wffileNotFound                 = -2021,
   cantCreateSingleForkFile       = -2022,
   invalidEditState               = -2023,
   nonMatchingEditState           = -2024,
   staleEditState                 = -2025,
   userDataItemNotFound           = -2026,
   maxSizeToGrowTooSmall          = -2027,
   badTrackIndex                  = -2028,
   trackIDNotFound                = -2029,
   trackNotInMovie                = -2030,
   timeNotInTrack                 = -2031,
   timeNotInMedia                 = -2032,
   badEditIndex                   = -2033,
   internalQuickTimeError         = -2034,
   cantEnableTrack                = -2035,
   invalidRect                    = -2036,
   invalidSampleNum               = -2037,
   invalidChunkNum                = -2038,
   invalidSampleDescIndex         = -2039,
   invalidChunkCache              = -2040,
   invalidSampleDescription       = -2041,
   dataNotOpenForRead             = -2042,
   dataNotOpenForWrite            = -2043,
   dataAlreadyOpenForWrite        = -2044,
   dataAlreadyClosed              = -2045,
   endOfDataReached               = -2046,
   dataNoDataRef                  = -2047,
   noMovieInDataFork              = -2048,
   invalidDataRefContainer        = -2049,
   badDataRefIndex                = -2050,
   noDefaultDataRef               = -2051,
   couldNotUseAnExistingSample    = -2052,
   featureUnsupported             = -2053,
   noVideoTrackInMovie            = -2054,
   noSoundTrackInMovie            = -2055,
   soundSupportNotAvailable       = -2056,
   maxControllersExceeded         = -2057,
   unableToCreateMCWindow         = -2058,
   insufficientMemory             = -2059,
   invalidUserDataHandle          = -2060,
   noPictureInFile                = -2061,
   invalidPictureFileHandle       = -2062,
   invalidPictureHandle           = -2063,
   badDisplayContext              = -2064,
   invalidComponentID             = -3000,
   validInstancesExist            = -3001,
   componentNotCaptured           = -3002,
   componentDontRegister          = -3003,
   noErr                          = 0,
   mcOK                           = 0,
   mcEventNotHandled              = 0,
   mcEventHandled                 = 1,
   codecConditionErr              = -8972,
   controllerBoundsNotExact       = -9996,
   editingNotAllowed              = -9995
  };


// QTInitialize error codes
// ------------------------
enum
  {QTI_OK                         = 0,
   QTI_FAIL_NOEXIST               = 1,
   QTI_FAIL_CORRUPTDLL            = 2,
   QTI_FAIL_286                   = 3,
   QTI_FAIL_WIN30                 = 4
  };


// Component Manager error codes
// -----------------------------
#define badComponentInstance    0x80008001
#define badComponentSelector    0x80008002


// Special MCI Interface
// ---------------------
#define MCI_QTW_PLAY_WINDOW     0x01000000L
#define MCI_QTW_PLAY_FULLSCREEN 0x02000000L
#define MCI_QTW_SET_USE_MC      0x00100000L
#define MCI_QTW_SET_PAL_ASSERT  0x00200000L
#define MCI_QTW_SET_GROWBOX     0x00400000L


// Declared handles
// ----------------
DECLARE_HANDLE(UserData);              // Hungarian: ud
DECLARE_HANDLE(PicHandle);             // Hungarian: ph
DECLARE_HANDLE(DIBHandle);             // Hungarian: dh
DECLARE_HANDLE(PicFile);               // Hungarian: pic
DECLARE_HANDLE(MovieFile);             // Hungarian: mov


// Typedef's
// ---------
typedef LONG ComponentResult;          // Hungarian: cr
typedef LONG OSErr;                    // Hungarian: oserr
typedef LONG OSType;                   // Hungarian: ostype
typedef VOID FAR *ProgressProcRecordPtr; // Hungarian: pprp
typedef LONG TimeScale;                // Hungarian: ts
typedef DWORD TimeValue;               // Hungarian: tv
typedef LONG LFIXED;                   // Hungarian: lfx
typedef short SFIXED;                  // Hungarian: sfx

typedef struct _tagInt64               // Hungarian: qw (quad word)
  {DWORD dwLo;
   LONG  dwHi;
  } Int64;

typedef struct _tagOpenCPicParms       // Hungarian: ocp
  {RECT   rect;
   LFIXED hRes;
   LFIXED vRes;
   WORD   wVersion;
   WORD   wReserved1;
   DWORD  dwReserved2;
  } OpenCPicParams;

typedef struct _tagTimeBaseRecord      // Hungarian: tbr
  {DWORD FAR *p;
  } TimeBaseRecord;

typedef TimeBaseRecord FAR *TimeBase;  // Hungarian: tb

#define TIMEBASE_DEFAULT ((TimeBase) -1L)

typedef struct _tagTimeRecord          // Hungarian: tr
  {Int64     value;
   TimeScale scale;
   TimeBase  base;
  } TimeRecord;

typedef struct _tagSoundDescription    // Hungarian: sd (SoundDescription)
  {LONG  descSize;                     // structure size
   DWORD dataFormat;                   // always 'raw '
   DWORD resvd1;                       // always 0
   WORD  resvd2;                       // always 0
   WORD  dataRefIndex;                 // always 1
   WORD  version;                      // always 0
   WORD  revLevel;                     // always 0
   DWORD vendor;                       // always 0
   WORD  numChannels;                  // 1=mono; 2=stereo
   WORD  sampleSize;                   // 8=8bit sound; 16=16bit sound
   WORD  compressionID;                // always 0
   WORD  packetSize;                   // always 0
   LFIXED sampleRate;                  // eg: 44100.0000 per second
  } SoundDescription;

typedef struct _tagImageDescription    // Hungarian: id (ImageDescription)
  {LONG  idSize;                       // structure size
   DWORD CodecType;                    // 'rpza', 'jpeg', 'rle ', 'raw ', 'smc'
   DWORD resvd1;                       //
   WORD  resvd2;                       // always 0
   WORD  dataRefIndex;                 // always 1
   WORD  version;                      //
   WORD  revLevel;                     //
   DWORD vendor;                       // 'appl' or other vendor
   DWORD temporalQuality;              //
   DWORD spatialQuality;               //
   WORD  width;                        // Source image width in pixels
   WORD  height;                       // Source image height in pixels
   LFIXED hRes;                        // Horizontal resolution (eg: 72.0)
   LFIXED vRes;                        // Vertical resolution (eg: 72.0)
   DWORD dataSize;                     // Memory required for image data
   WORD  frameCount;                   // always 0
   char  name[32];                     // Compression algorithm (eg: Animation)
   WORD  depth;                        // Pixel depth of source image
   WORD  clutID;                       // Macintosh ROM Color table ID
  } ImageDescription;

typedef ComponentResult (_far _cdecl *ComponentRoutine) (VOID);
typedef DWORD (FAR CDECL *ENTRYFUNC) (VOID);
typedef VOID (FAR PASCAL *FIXUPFUNC) (ENTRYFUNC);
typedef LONG Component;
typedef LONG ComponentInstance;

typedef struct tagComponentDescription { // Hungarian: cd
  OSType  ostypeComponentType;         // component identifier
  OSType  ostypeComponentSubType;      // sub-type
  OSType  ostypeComponentManufacturer; // vendor ID
  DWORD   dwComponentFlags;            // control flags
  DWORD   dwComponentFlagsMask;        // mask for control flags
  ComponentRoutine   crEntryPoint;     // pointer to entry point
  HRSRC   hrsrcName;                   // handle to name string
  HRSRC   hrsrcInfo;                   // handle to info string
  HRSRC   hrsrcIcon;                   // handle to icon
} ComponentDescription, FAR *LPCD;




// Macros
// ------
#define MAKELFIXED(integ, fract) ((LONG)(((WORD)(fract)) | (((DWORD)((WORD)(integ))) << 16)))

#define MAKESFIXED(integ, fract) ((int)(((BYTE)(fract)) | (((WORD)((BYTE)(integ))) << 8)))

#define QTFOURCC(ch0, ch1, ch2, ch3)                                \
                 ((DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |  \
                 ((DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ))


// ToolBox Callbacks and Handles
// -----------------------------
typedef LPVOID Movie;
typedef OSErr (CALLBACK *MovieRgnCoverProc) (Movie, HDC, LONG);


// Movie Controller Callbacks and Handles
// --------------------------------------
typedef ComponentInstance MovieController;
typedef BOOL (CALLBACK *MCActionFilter) (MovieController, UINT, LPVOID, LONG);


// Support C++ Function Invocations
// --------------------------------
#ifdef __cplusplus
  extern "C" {
#endif


// Functions
// ---------
#define         QTAPI _far _cdecl

OSErr           QTAPI QTInitialize (LPLONG);
VOID            QTAPI QTTerminate (VOID);
VOID            QTAPI  AddTime ( TimeRecord FAR *, const TimeRecord FAR *);
VOID            QTAPI  ClearMoviesStickyError ( VOID);
OSErr           QTAPI  CloseMovieFile ( MovieFile);
OSErr           QTAPI  ClosePictureFile ( PicFile);
VOID            QTAPI  ConvertTimeScale ( TimeRecord FAR *, TimeScale);
LONG            QTAPI  CountUserDataType ( UserData, OSType);
OSErr           QTAPI  DeleteMovieFile ( LPCSTR);
VOID            QTAPI  DisposeMovie ( Movie);
VOID            QTAPI  DisposeMovieController ( MovieController);
VOID            QTAPI  DisposePicture ( PicHandle);
OSErr           QTAPI  DrawPicture ( HDC, PicHandle, const LPRECT, ProgressProcRecordPtr);
OSErr           QTAPI  DrawPictureFile ( HDC, PicFile, const LPRECT, ProgressProcRecordPtr);
OSErr           QTAPI  EnterMovies ( VOID);
VOID            QTAPI  ExitMovies ( VOID);
BOOL            QTAPI  GetMovieActive ( Movie);
VOID            QTAPI  GetMovieActiveSegment ( Movie, TimeValue FAR *, TimeValue FAR *);
VOID            QTAPI  GetMovieBox ( Movie, LPRECT);
LONG            QTAPI  GetMovieCreationTime ( Movie);
LONG            QTAPI  GetMovieDataSize ( Movie, TimeValue, TimeValue);
TimeValue       QTAPI  GetMovieDuration ( Movie);
LONG            QTAPI  GetMovieModificationTime ( Movie);
PicHandle       QTAPI  GetMoviePict ( Movie, TimeValue);
PicHandle       QTAPI  GetMoviePosterPict ( Movie);
TimeValue       QTAPI  GetMoviePosterTime ( Movie);
LFIXED          QTAPI  GetMoviePreferredRate ( Movie);
SFIXED          QTAPI  GetMoviePreferredVolume ( Movie);
OSErr           QTAPI  GetMoviesError ( VOID);
OSErr           QTAPI  GetMoviesStickyError ( VOID);
ComponentResult QTAPI  GetMovieStatus ( Movie, LPVOID);
TimeValue       QTAPI  GetMovieTime ( Movie, TimeRecord FAR *);
TimeScale       QTAPI  GetMovieTimeScale ( Movie);
UserData        QTAPI  GetMovieUserData ( Movie);
OSType          QTAPI  GetNextUserDataType ( UserData, OSType);
OSErr           QTAPI  GetPictureFileHeader ( PicFile, LPRECT, OpenCPicParams FAR *);
OSErr           QTAPI  GetPictureFileInfo ( PicFile, ImageDescription FAR *);
PicHandle       QTAPI  GetPictureFromFile ( PicFile);
OSErr           QTAPI  GetPictureInfo ( PicHandle, ImageDescription FAR *);
HPALETTE        QTAPI  GetPicturePalette ( PicHandle);
OSErr           QTAPI  GetSoundInfo ( Movie, SoundDescription FAR *);
OSErr           QTAPI  GetUserData ( UserData, LPHANDLE, OSType, LONG, LPLONG);
OSErr           QTAPI  GetUserDataText ( UserData, LPHANDLE, OSType, LONG, UINT, LPLONG);
OSErr           QTAPI  GetVideoInfo ( Movie, ImageDescription FAR *);
VOID            QTAPI  KillPicture ( PicHandle);
MovieController QTAPI  NewMovieController ( Movie, const LPRECT, LONG, HWND);
OSErr           QTAPI  NewMovieFromDataFork ( Movie FAR *, HFILE, LONG, UINT);
OSErr           QTAPI  NewMovieFromFile ( Movie FAR *, MovieFile, LPINT, LPSTR, UINT, BOOL FAR *);
VOID            QTAPI  NormalizeRect ( LPRECT);
OSErr           QTAPI  OpenMovieFile ( LPCSTR, MovieFile FAR *, UINT);
OSErr           QTAPI  OpenPictureFile ( LPCSTR, PicFile FAR *, UINT);
DIBHandle       QTAPI  PictureToDIB ( PicHandle);
OSErr           QTAPI  PrerollMovie ( Movie, TimeValue, LFIXED);
BOOL            QTAPI  PtInMovie ( Movie, POINT);
VOID            QTAPI  SetMovieActive ( Movie, BOOL);
VOID            QTAPI  SetMovieBox ( Movie, const LPRECT);
VOID            QTAPI  SetMovieCoverProcs ( Movie, MovieRgnCoverProc, MovieRgnCoverProc, LONG);
VOID            QTAPI  SetMoviePlayHints ( Movie, LONG, LONG);
VOID            QTAPI  SubtractTime ( TimeRecord FAR *, const TimeRecord FAR *);
OSErr           QTAPI  UpdateMovie ( Movie);
ComponentResult QTAPI  MCActivate ( MovieController, HWND, BOOL);
ComponentResult QTAPI  MCDoAction ( MovieController, UINT, LPVOID);
ComponentResult QTAPI  MCDraw ( MovieController, HWND);
ComponentResult QTAPI  MCDrawBadge ( MovieController, HRGN, HRGN FAR *);
ComponentResult QTAPI  MCGetControllerBoundsRect ( MovieController, LPRECT);
ComponentResult QTAPI  MCGetControllerInfo ( MovieController, LPLONG);
TimeValue       QTAPI  MCGetCurrentTime ( MovieController, TimeScale FAR *);
Movie           QTAPI  MCGetMovie ( MovieController);
ComponentResult QTAPI  MCGetVisible ( MovieController);
ComponentResult QTAPI  MCIdle ( MovieController);
ComponentResult QTAPI  MCIsControllerAttached ( MovieController);
ComponentResult QTAPI  MCIsPlayerMessage ( MovieController, HWND, UINT, WPARAM, LPARAM);
ComponentResult QTAPI  MCKey ( MovieController, WPARAM, LPARAM);
OSErr           QTAPI  MCNewMovieController ( MovieController, ComponentInstance, Movie, const LPRECT, LONG, HWND);
ComponentResult QTAPI  MCNewAttachedController ( MovieController, Movie, HWND, POINT);
ComponentResult QTAPI  MCPositionController ( MovieController, LPRECT, LPRECT, LONG);
ComponentResult QTAPI  MCRemoveMovie ( MovieController);
ComponentResult QTAPI  MCSetActionFilter ( MovieController, MCActionFilter, LONG);
ComponentResult QTAPI  MCSetControllerAttached ( MovieController, BOOL);
ComponentResult QTAPI  MCSetControllerBoundsRect ( MovieController, const LPRECT);
ComponentResult QTAPI  MCSetMovie ( MovieController, Movie, HWND, POINT);
ComponentResult QTAPI  MCSetVisible ( MovieController, BOOL);
Component       QTAPI  CaptureComponent ( Component cCaptured, Component cCapturing);
OSErr           QTAPI  CloseComponent ( ComponentInstance ci);
OSErr           QTAPI  CloseComponentResFile ( short sFileNum);
LONG            QTAPI  ComponentFunctionImplemented ( ComponentInstance ci, short sFunctionNumber);
LONG            QTAPI  CountComponentInstances ( Component c);
LONG            QTAPI  CountComponents ( ComponentDescription FAR *lpcdLooking);
Component       QTAPI  FindNextComponent ( Component c, ComponentDescription FAR *lpcdLooking);
HINSTANCE       QTAPI  GetComponentResFileInstance ( short sFileNum);
OSErr           QTAPI  GetComponentInfo ( Component c, ComponentDescription FAR *lpcd);
OSErr           QTAPI  GetComponentInstanceError ( ComponentInstance ci);
LPVOID          QTAPI  GetComponentInstanceStorage ( ComponentInstance ci);
LONG            QTAPI  GetComponentListModSeed ( void);
LONG            QTAPI  GetComponentRefcon ( Component c);
LONG            QTAPI  GetComponentVersion ( ComponentInstance ci);
ComponentInstance QTAPI  OpenComponent ( Component c);
short           QTAPI  OpenComponentResFile ( Component c);
ComponentInstance QTAPI  OpenDefaultComponent ( OSType ostypeComponentType, OSType ostypeComponentSubType);
Component       QTAPI  RegisterComponent ( LPCD lpcd, WORD wGlobal, FIXUPFUNC lpfnTBFixup, FIXUPFUNC lpfnCMFixup);
OSErr           QTAPI  RegisterComponentResourceFile ( LPCSTR lpszDLL, WORD wGlobal);
VOID            QTAPI  SetComponentInstanceError ( ComponentInstance ci, OSErr oserr);
VOID            QTAPI  SetComponentInstanceStorage ( ComponentInstance ci, LPVOID lpvStorage);
VOID            QTAPI  SetComponentRefcon ( Component c, LONG lRefcon);
OSErr           QTAPI  SetDefaultComponent ( Component c, short sFlags);
OSErr           QTAPI  UncaptureComponent ( Component c);
OSErr           QTAPI  UnregisterComponent ( Component c);
OSErr           QTAPI  UnregisterComponentResourceFile ( LPCSTR lpszDLL);

// End of Functions
// ----------------
#ifdef __cplusplus
  }
#endif


// End of QTW.H
// ------------
#endif // __QTW_H__
