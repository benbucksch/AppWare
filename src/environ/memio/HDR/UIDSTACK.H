#ifndef __UIDSTACK_H__
#define __UIDSTACK_H__

#include "seruid.h"
#include "classdef.h"

class UIDSTACK
  {
private:

  RUNTIMEUID  blockID;
  LPSERUID    dataPointer;
  short       initialSize;
  short       growthSize;
  short       maxIndex;
  short       topIndex;

  UIDSTACK();

public:

  UIDSTACK(
    short   initialSize,
    short   growthSize = 10);

  void Push(
    SERUID  item);

  SERUID Pop(
    void);

  SERUID GetTop(
    void)
    {

    if (this->topIndex > -1)
      return this->dataPointer[topIndex];
    else
      return 0;

    }

  };

#endif
