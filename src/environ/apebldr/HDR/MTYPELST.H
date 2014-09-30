#include "memclass.h"
#include "tstring.h"
#include "bitree.h"
#include "intalm.h"
#include "classdef.h"

#ifndef 	MADMNLST_H
#define MADMNLST_H
																	
class ADMNNODE:public BITREEMEMBER
	{
	public:
		ATYPEID 		m_typeID;
		ADMNPROC 	m_admnProc;
		HINSTANCE m_libHandle;
		long 			m_status;
		DWORD			m_errorValue;
		TSTRING		m_typeName;
		TSTRING		m_dllName;
		TSTRING		m_procName;

		ADMNNODE(
			ATYPEID typeID,
			LPCSTR typeName,
			LPCSTR dllName,
			LPCSTR procName):m_typeID(typeID), m_typeName(typeName),
				m_dllName(dllName), m_procName(procName), 
				m_admnProc(NULL), m_libHandle(NULL), m_status(0), m_errorValue(ERROR_SUCCESS)
			{
			}

		~ADMNNODE()
			{
			if (m_libHandle && (m_errorValue == ERROR_SUCCESS))
				FreeLibrary(m_libHandle);
			}

		int Compare(
			LPBITREEMEMBER node)
			{
			if(m_typeID < ((LPADMNNODE)node)->m_typeID)
				return -1;
			else if(m_typeID > ((LPADMNNODE)node)->m_typeID)
				return 1;
			else
				return 0;
			}

		int MatchCriteria(
			long criteria)
			{
			if(m_typeID < (ATYPEID)criteria)
				return -1;
			else if(m_typeID > (ATYPEID)criteria)
				return 1;
			else
				return 0;
			}


	};


class MASTERTYPELIST:public MEMORY
	{
	private:
		LPBITREE m_admnProcList;
		LPADMNNODE m_nodeKey;
		long m_status;

		BOOL LoadAdmnProc(
			LPADMNNODE nodePtr);

		void ShowErrorMessage(
			LPADMNNODE nodePtr);

	public:

		MASTERTYPELIST();

		~MASTERTYPELIST();

		void PrepareNodes();

		ADMNPROC GetAdmnProc(
			ATYPEID typeID);

		BOOL LoadNextDLL();
	
		void AddTypeNode(
			ATYPEID typeID,
			LPCSTR typeName,
			LPCSTR dllName,
			LPCSTR procName);

		LPADMNNODE GetNextNode(
			LPADMNNODE node)
			{
			return (LPADMNNODE)m_admnProcList->GetNextSortedMember(node);
			}
				 
		LPADMNNODE GetNode(
			ATYPEID typeID);

	};
#endif
