#ifndef __EXEFLOW_H
#define __EXEFLOW_H

#include "classdef.h"
#include "flowflgs.h"
#include "exserrec.h"
#include "seruid.h"

class EXEFLOW:public EXESERIUSRECORD
{
	private:

		SERUID  flowTo;
		LONG	  signal;
		long    flags;

	public:

		EXEFLOW(
			SERUID newFlowTo,
			LONG   newSignal,
			long   newFlags):flowTo(newFlowTo), signal(newSignal), flags(newFlags)
			{
			}

		EXEFLOW(
			void * /*thisPtr*/)
			{
			//clear this flag because even though the signal may have already
			//been updated, the version may again become bad and it will again
			//need to be updated.
			flags &= FL_SIGNALUPDATED;
			}

		SERUID GetFlowToUID()
			{
			return flowTo;
			}


		void SetFlowToUID(
			SERUID newUID)
			{
			flowTo = newUID;
			}

		LONG GetSignal()
			{
			return signal;
			}

		////////////////////////////////////////////////////////////////////
		//    EXEFLOW::GetFlowTargetPtr
		//
		//  Returns a pointer to the EXEFUNC that is at the "to" side
		//  of this flow. jon 5-Aug-92.
		////////////////////////////////////////////////////////////////////
		LPEXEFUNCINST GetFlowTargetPtr()
			{
			return (LPEXEFUNCINST)GetObject(flowTo);
			}

		////////////////////////////////////////////////////////////////////
		//    EXEFLOW::DoStop
		//
		//  Returns TRUE if there is a Serius Stop set on this flowline.
		//  jon 5-Aug-92.
		////////////////////////////////////////////////////////////////////
		BOOL DoStop()
			{
			return ((flags & FL_STOP) && !(flags & FL_STOPDISABLED)) ? TRUE : FALSE;
			}

		void AddInfo(
			long info)
			{
			flags|=info;
			}

		void RemoveInfo(
			long info)
			{
			flags&= ~info;
			}

		BOOL TestFlowConnectedFlag()
			{
			return (flags & FL_SIGNALNOTLINKED)?FALSE:TRUE;
			}

	};

#endif
