

#include "t_includ.h"



TEXTRUNTIME::TEXTRUNTIME()
	{
	_fmemcpy(m_szFileFilter,"TextFiles(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0", 43);
	m_tiLastActive = NULL;
	ATypeSetRuntimeInfo (OTYPE_TEXT, (LONG)this);
	}

////////////////////////////

TEXTRUNTIME::~TEXTRUNTIME()
	{
	ATypeSetRuntimeInfo (OTYPE_TEXT, 0);
  }

///////////////////
