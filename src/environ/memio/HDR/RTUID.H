#ifndef __RTUID_H__
#define __RTUID_H__

#include "rtuiddf.h"
#include "seruid.h"

inline RUNTIMEUID::RUNTIMEUID(
  SERUID  seriusUID)
  {
  this->id = seriusUID.id;
  }

#endif
