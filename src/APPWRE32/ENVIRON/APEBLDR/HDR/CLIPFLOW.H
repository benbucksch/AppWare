#ifndef __CLIPFLOW_H
#define __CLIPFLOW_H

#include "classdef.h"
#include "serrec.h"

class CLIPFLOW:public SERIUSRECORD
	{
		private:
			SERUID flowTo;
			SERUID flowFrom;
			LONG signal;
			long status;
			LONG clipStatus;
			int order;

		public:
		 CLIPFLOW(
				FLOW * flowPtr,
				BOOL sourceSelected,
				BOOL deleteAll);

			void UpdateEnvFlow(
				SUBJECT * envSjtPtr);

	};

#endif
