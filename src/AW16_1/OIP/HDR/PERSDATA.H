// persdata.h -- persistent data file stuff

#ifndef __PERSISTENT_DATA__
#define __PERSISTENT_DATA__

  #include <time.h>

  #define PERSDATA_EXTENSION  ".PDT"
  #define PERSDATA_TAG        1701
  #define PERSDATA_INIKEY     "PersistentData"

  class PERSISTENTDATAHEADER
	  {
  private:

    long	 tag;
    time_t secs;
    long   ticks;

  public:

    PERSISTENTDATAHEADER()
      {
      tag = 0;
      secs = 0;
      ticks = 0;
      }

    BOOL Init()
      {
      time_t   mySecs;

      tag = 0xDEFACED1L;
      ::time(&mySecs);
      this->secs = mySecs;
      ticks = ::GetTickCount();
      return TRUE;
      }

    long GetTag()
      {
      return tag;
      }
    };

  typedef PERSISTENTDATAHEADER far * LPPERSISTENTDATAHEADER;

#endif
