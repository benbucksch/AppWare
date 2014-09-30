#ifndef __FUNCDESC_H__
#define __FUNCDESC_H__

#include "classdef.h"
#include "typeary.h"
#include "flows.h"
#include "params.h"
#include "dbstring.h"
#include "compuid.h"
#include "compdesc.h"
#include "sericon.h"

#define MAXDLLCHARS	12

class AFUNCDESC:public ACOMPDESC
	{
private:
	short 					fileIndex_UNUSED;
	COMPUID					typeID_UNUSED;
	char						dll[MAXDLLCHARS+1];
	DBSTRING				entryPoint;
	TYPEIDARRAY			requiredTypes;
	SERIUSICON			icons;
	DBSTRING				help;
	FLOWDESCARRAY		flows;
	PARAMDESCARRAY	inputs;
	PARAMDESCARRAY	outputs;
	DWORD						flags;

	AFUNCDESC(				//prevent calls to the default constructor
		void)
		{
		}

public:

	AFUNCDESC(
		void *	objPtr):
			ACOMPDESC(objPtr),
			requiredTypes(objPtr),
			icons(objPtr),
			help(objPtr),
			flows(objPtr),
			inputs(objPtr),
			outputs(objPtr),
			entryPoint(objPtr)
		{
		}

	AFUNCDESC(
		COMPUID	id,
		TSTRING 	name);

	void ConvertToPointers(
		void);

	LPSERIUSICON	GetIcons(
		void);

	LPSTR GetLibName(
		void)
		{
		return dll;
		}

	BOOL SetLibName(
		LPSTR	pName);

	LPCSTR GetEntryPointName(
		void)
		{
		return entryPoint;
		}

	LPSTR pGetEntryPointName(
		void)
		{
		return entryPoint.pString;
		}

	BOOL SetEntryPointName(
		LPSTR name)
		{
		entryPoint = name;
		return TRUE;
		}

	LPPARAMDESCARRAY GetInputs(
		void)
		{
		return &inputs;
		}

	LPPARAMDESCARRAY GetOutputs(
		void)
		{
		return &outputs;
		}

	LPFLOWDESCARRAY GetFlows(
		void)
		{
		return &flows;
		}

	TSTRING GetHelp(
		void)
		{
		return help;
		}

	LPSTR pGetHelpFileName(
		void)
		{
		return help.pString;
		}

	BOOL SetHelp(
		const TSTRING& newHelp)
		{
		help = (TSTRING&) newHelp;
		return TRUE;
		}

	BOOL SetInputs(
		LPPARAMDESCARRAY	tempInputs)
		{
		inputs = *tempInputs;
		return TRUE;
		};

	BOOL SetOutputs(
		LPPARAMDESCARRAY	tempOutputs)
		{
		outputs = *tempOutputs;
		return TRUE;
		}

	BOOL SetFlows(
		LPFLOWDESCARRAY	tempFlows)
		{
		flows = *tempFlows;
		return TRUE;
		}

	int GetInputsCount(
		void);

	int GetOutputsCount(
		void);

	int GetFlowsCount(
		void);

	LPPARAMDESC GetInput(
		int index);

	LPPARAMDESC GetOutput(
		int index);

	LPFLOWDESC GetFlow(
		int index);

	void		SetRequiredType(
		ATYPEID		t);

	ATYPEID	GetRequiredType(
		void);

	ATYPEID	pGetRequiredType(
		void);

	};

#endif
