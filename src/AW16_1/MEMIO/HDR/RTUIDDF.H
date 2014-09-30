/////////////////////////////////////////////////////////////////////////
//  This file defines the RUNTIMEUID structure.  Inorder to avoid the
//  circular include of SERUID.H the implementation of the inline
//  constructor RUNTIMEUID(SERUID &seriusUID)  is placed in the file
//  rtuid.h.  Users of the class RUNTIMEUID should include the file
//  rtuid.h.
////////////////////////////////////////////////////////////////////////

#ifndef __RTUIDDF_H__
#define __RTUIDDF_H__

#include "classdef.h"

union RUNTIMEUID
	{
	long int 	   id;
	LPRUNTIMEREC ptr;

	int	operator ==(
		const RUNTIMEUID far& uid2)
		{
		return (this->id == uid2.id);
		}

	int	operator !=(
		const RUNTIMEUID far& uid2)
		{
		return (this->id != uid2.id);
		}

	RUNTIMEUID(
		void)
		{
		}

	RUNTIMEUID(
		long int	id)
		{
		this->id = id;
		}

  RUNTIMEUID(
    SERUID  seriusUID);

	};

typedef RUNTIMEUID far*  LPRUNTIMEUID;

#endif
