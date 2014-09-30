/*//////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#include <stdio.h>
#include <string.h>

int		main(
	int			argc,
	char**	argv
	)
	{
	char*		pScan;

	if (argc == 2)
		{
		pScan = argv[1] + strlen(argv[1]) - 1;

		for (;pScan >= argv[1]; pScan--)
			{
			if (*pScan == '/' || *pScan == '\\')
				break;
			}

		pScan++;

		fprintf(stdout, "%s\n", pScan);
		}
	else
		fprintf(stderr, "Usage:  basename <filename|filepath>\n");

	return 0;
	}
