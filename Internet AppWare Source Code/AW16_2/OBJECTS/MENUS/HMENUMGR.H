#ifndef _HMENUMGR_H_
#define _HMENUMGR_H_

#include "intdebug.h"

class HMENUMGR
  {
private:
  HMENU  m_hMenu;
  short  m_useCount;

  HMENUMGR(void);

public:

  HMENUMGR(
    HMENU  hMenu)
    {
    m_hMenu    = hMenu;
    m_useCount = 0;
    }

  HMENUMGR(
    long  runTimeInfo)
    {
    m_hMenu = (HMENU)LOWORD(runTimeInfo);
    m_useCount = HIWORD(runTimeInfo);
    }

  HMENU GetMenuHandle(void)
    {
    return m_hMenu;
    }

  BOOL SetMenuHandle(
    HMENU  hMenu)
    {
    if (m_useCount != 0)
      {
      ADBG_PRINT_W("HMENUMGR: SetMenuHandle called with use count > 0");
      return FALSE;
      }
    else
      {
      m_hMenu = hMenu;
      return TRUE;
      }
    }

  operator long(void)
    {
    return (long)MAKELONG(m_hMenu, m_useCount);
    }

  short GetUseCount(void)
    {
    return m_useCount;
    }

  short IncrementUseCount(void)
    {
    return (++m_useCount);
    }

  short DecrementUseCount(void)
    {
    I_ASSERT(m_useCount > 0);

    if (m_useCount > 0)
      m_useCount--;

    return m_useCount;
    }

  };

#endif