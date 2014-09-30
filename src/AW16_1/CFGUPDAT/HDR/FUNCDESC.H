#ifndef __FUNCDESC_H__
#define __FUNCDESC_H__

#include "compdesc.h"
#include "sericon.h"
#include "syntax.h"
#include "flows.h"
#include "params.h"

#define MAXDLLCHARS	12

class FUNCDESC:public COMPDESC
	{
private:
	short 					fileIndex;
	COMPUID					typeID;
	char						dll[MAXDLLCHARS+1];
	DBSTRING				entryPoint;
	SYNTAX					syntax;
	SERIUSICON			icons;
	DBSTRING				help;
	FLOWDESCARRAY		flows;
	PARAMDESCARRAY	inputs;
	PARAMDESCARRAY	outputs;
	DWORD						flags;

	FUNCDESC(				//prevent calls to the default constructor
		void)
		{
		}

public:

	FUNCDESC(
		void far*	objPtr):
			COMPDESC(objPtr),
			syntax(objPtr),
			icons(objPtr),
			help(objPtr),
			flows(objPtr),
			inputs(objPtr),
			outputs(objPtr),
			entryPoint(objPtr)
		{
		}

	FUNCDESC(
		COMPUID	id,
		TSTRING 	name);

	void ConvertToPointers(
		void);

	LPSERIUSICON	GetIcons(
		void);

	COMPUID GetTypeID(
		void)
		{
		return typeID;
		}

	BOOL SetTypeID (
		COMPUID	newTypeID)
		{
		typeID = newTypeID;
		return TRUE;
		}

	LPSTR GetLibName(
		void)
		{
		return dll;
		}

	BOOL SetLibName(
		LPSTR	pName);

	LPSTR GetEntryPointName(
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

	LPSYNTAX GetSyntax(
		void)
		{
		return &syntax;
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
		TSTRING newHelp)
		{
		help = newHelp;
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

	BOOL SetSyntax(
		LPSYNTAX	tempSyntax)
		{
		syntax = *tempSyntax;
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

	};

typedef FUNCDESC far*	LPFUNCDESC;

#endif