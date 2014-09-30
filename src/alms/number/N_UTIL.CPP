
#include "n_includ.h"

////////////////////////////////////////////////
// Returns the index of the first number found.
// If no number is found in the string, returns -1
////////////////////////////////////////////////
static int FindFirstNumber(
	LPCSTR									str)
	{
	int											i;

	for(i=0; *str; str++, i++)
		if (isdigit(*str)) return i;

  return -1;
	}

//////////////////////////////////////////////////
//  Returns the index of the first occurance of
//  a character in a string.  If the character is
//  not found, it returns -1
//////////////////////////////////////////////////
static int FindCharIdx(
	LPCSTR									str,
	char										ch)
	{
	int					i;

	for(i=0; *str; str++, i++)
		if (ch == *str) return i;
  return -1;
	}

//////////////////////////////////////////////////
//  Returns the index of the first occurance of
//  of a string within another string.  If the
//  string is not found, it returns -1
//////////////////////////////////////////////////

static int FindSubstringIdx(
	LPCSTR									s1, //search in this 
	LPCSTR									s2) //for this
	{
	LPCSTR									cp1=s1;
	LPCSTR									cp2=s2;
	int											i, rVal;

	for(i=0, rVal=-1; *cp1 && *cp2; cp1++, i++)
		{
		if ( (*cp1|0x20) == (*cp2|0x20) )
			{
			if (rVal == -1) rVal = i;
			cp2++;
			}
		else
			{
			rVal = -1;
			cp2 = s2;
			}
		}

	return rVal;
	}
/////////////////////////////////////////////////////////////////////
//  Returns TRUE if the character passed in is part
//  of the a number .,%-+  etc...
//////////////////////////////////////////////
static BOOL IsNumberPart(
	char										ch)
	{

	if (ch == g_chDecSep)			return TRUE;
	if (ch == g_chThouSep)		return TRUE;
	if (ch == g_chCurrency) 	return TRUE;

///	if (_fstrchr(g_szNumberParts, ch))
	if (strchr(g_szNumberParts, ch))
		return TRUE;
		
  return FALSE;
	}

///////////////////////////////////////////////
///  fills filledString with the character
//   in source until a numberPart is found 
///////////////////////////////////////////////
static void FillNonNumberString(
	LPCSTR									source,
	LPSTR										filledString,
	int											filledStringSize)
	{
	LPSTR										cpMax = &filledString[filledStringSize-1];

	while ( *source										&&
					filledString < cpMax			&&
					!isdigit(*source)		 			&&
					!IsNumberPart(*source) )
		{
		*(filledString++) = *(source++);
		}
  *filledString = 0;
	}


static LPSTR	IncrementToNonQuote(
	LPCSTR									str)
	{
	if (*str == '\'')  // this single quote character
		{
		if (*str) str++;
		while ( *str != '\'' && *str) str++;
		if (*str) str++;
		}
	else str++;

	return (LPSTR)str;
	}

/////////////////////////////////////////

static LPSTR SearchForSubstring(
	LPCSTR									s1,
	LPCSTR									s2)
	{
	LPCSTR									cp1=s1;
	LPCSTR									cp2=s2;
	LPCSTR									cpReturn=NULL;

	while(*cp1)
		{
		if (!*cp2) return (LPSTR)cpReturn;

		if ( (*cp1|0x20) == (*cp2|0x20) )
			{
			if (!cpReturn) cpReturn = cp1;
			cp1++;
			cp2++;
			}
		else
			{
			cpReturn = NULL;
			cp1++;
			cp2 = s2;
			}
		}
	return (LPSTR)cpReturn;
	}


//////////////////////////////////////////////////
// Validates the number format string, and fills
// in the information for NFORM_INFO
//////////////////////////////////////////////////
void ValidateNumberFormatString(
	LPSTR										str,
	LPNFORM_INFO						formatInfo,
	BOOL										bDropDecimal)
	{
	LPSTR										cp=str;
	BOOL										forcingBefore;
	BOOL										allowingAfter;
	BOOL										afterDecimal;
	LPNFORM									nForm;
	int											i;

	for(i=0; i<3 && *cp ;i++)
		{
		if (i==0) 			nForm = &(formatInfo->positive);
		else if (i==1)  nForm = &(formatInfo->negitive);
		else						nForm = &(formatInfo->zero);

		forcingBefore = FALSE;
		allowingAfter = FALSE;
		afterDecimal = FALSE;

		nForm->sFormatIdx = (short)(cp-str);
		nForm->forceDecimal = FALSE;
		nForm->usesPercent = FALSE;
		nForm->forceSign = (BYTE)((!i) ? TRUE:FALSE);
		nForm->widthWhole = nForm->widthFloat = 0;

		while(*cp)
			{
			if (*cp == ';')
				{
        cp++;
				break; //Do the next number type format parsing
				}

			else if (*cp == '#')
				{
				if (afterDecimal)
					{
					allowingAfter = TRUE;
					nForm->widthFloat++;
					cp++;
					}
				else
					{
					if (forcingBefore) *cp = '0';
					nForm->widthWhole++;
          cp++;
          }
				}
			else if (*cp == '0')
				{
				if (afterDecimal)
					{
					if (allowingAfter) *cp = '#';
					else nForm->forceDecimal = TRUE;
					nForm->widthFloat++;
					cp++;
					}
				else
					{
					forcingBefore = TRUE;
					nForm->widthWhole++;
					cp++;
					}
				}
			else if (*cp == g_chDecSep)
				{
				if (bDropDecimal)
					*cp = 0; //if it is for an integer, kill it here
				else
					{
					if (afterDecimal) lstrcpy(cp, cp+1);		//remove extra decimals
					else cp++;
					afterDecimal = TRUE;
					}
				}
			else if (*cp == 0x27) //the single quote
				{
				cp = IncrementToNonQuote(cp);
				}
			else if (*cp == '%')
      	{
				nForm->usesPercent = TRUE;
				cp++;
        }
			else if (IsNumberPart(*cp) )
				cp++;
			else
				lstrcpy(cp, cp+1);		//remove unknown stuff
			}
		}

	switch (i)
		{
		case 0:  //no formats supplied
			ZeroMemory(&formatInfo->positive, sizeof(NFORM)); //now the positive format has non-bogus info
			//fall through
		case 1: //only the positive entered
			CopyMemory(&formatInfo->negitive, &formatInfo->positive, sizeof(NFORM));
			//fall through
		case 2: //positive and negitive entered
			CopyMemory(&formatInfo->zero, &formatInfo->positive, sizeof(NFORM));
			break;
		}

	}

/////////////////////////////////////////////////////////////////////
//  Formats a number occording to the format string.  If formatInfo
//  is NULL, is validates it first
/////////////////////////////////////////////////////////////////////
unsigned short FormatFloatNumber(
	long double							num,
	LPSTR										format,
	LPSTR										str,
	unsigned short 					len,
	LPNFORM_INFO						formatInfo)//Defaults to NULL
	{
	LPSTR										fp;
	LPSTR										cp = str;
	LPSTR										cpMax = &str[len-1];
	LPNFORM									nForm;
	NFORM_INFO							defaultFormatInfo;
	char										szNumber[255];
	LPSTR										np;
	BOOL										bPlacedNumber;

	if (!formatInfo)
		{
		ValidateNumberFormatString(format, &defaultFormatInfo, FALSE);
		formatInfo = &defaultFormatInfo;
		}

	if (num<0.0)
		{
		nForm = &(formatInfo->negitive);
		if (nForm->forceSign)
			*(cp++) = '-';
		num *= -1.0;
		}
	else if (num==0.0)	nForm = &(formatInfo->zero);
	else              	nForm = &(formatInfo->positive);

	if ( num)
  	{
		if (nForm->usesPercent)
			num *= 100.0;

///		if (num >= pow10l(nForm->widthWhole) )
		if (num >= pow(10l, nForm->widthWhole) )
			{//num is too big
      lstrcpy(str, "*****");
			//sprintf(str,"%.2Le", num*sign);
			return (USHORT)(lstrlen(str)+1);
			}
		}

	if (nForm->widthFloat)
		{
		sprintf(szNumber, "%*.*Lf", (int)nForm->widthWhole + (int)nForm->widthFloat + 1, (int)nForm->widthFloat, num); 
		//padded with trailing '0' -- make them spaces
		np = &szNumber[lstrlen(szNumber)-1];
		while (*np == '0')
			*(np--) = ' ';
		}
	else
		sprintf(szNumber, "%*.Lf", (int)nForm->widthWhole, num); 
		
	np = szNumber;
	fp = format + nForm->sFormatIdx;
	bPlacedNumber = FALSE;

	while(*fp && *fp!=';' &&  cp<cpMax)
		{
		if (*fp == '#')
			{
			if (*np >= '0'  &&  *np <= '9')
				{
				*cp = *np;
				cp++;
				bPlacedNumber = TRUE;
				}
			np++;
			}
	
		else if (*fp == '0')
			{
			if (*np >= '0'  &&  *np <= '9')
				*cp = *np;
			else
				*cp = '0';
			cp++;
			bPlacedNumber = TRUE;
			np++;
			}
	
		else if (*fp == g_chDecSep)
			{
			*cp = *fp;
			I_ASSERT(*np == *fp);
			np++;
			cp++;
			}
	
		else if (*fp == g_chThouSep  &&  bPlacedNumber)
			{
			*cp = *fp;
			cp++;
			}

		else if (*fp == 0x27) //the single quote
			{
			fp++; //skip the first quote
			while(*fp  &&  *fp != 0x27  &&  cp<cpMax)
				*(cp++) = *(fp++);
			}

		else if (*fp == '+' || *fp == '-')
			*(cp++) = *fp;

		fp++;
		}

	if (nForm->usesPercent)
  	*(cp++) = '%';

	*cp = 0;
	return (USHORT)(lstrlen(str)+1);
	}
				
				
/////////////////////////////////////////////////////////////////////
/// Scans a string for a decimal seperator or a percent sign  
/////////////////////////////////////////////////////////////////////
BOOL DoesStringContainRealNumberThingies(
	LPCSTR 									str)
	{

	for( ;*str; str++)
		{
		if (*str == g_chDecSep  ||  *str == '%')
			return TRUE;
		}
	
	return FALSE;
	}
																
																
/////////////////////////////////////////////////////////////////////
///  Parses a number occording to the format string.  
/////////////////////////////////////////////////////////////////////
long double ParseFormatedNumber(
	LPCSTR				str,
	LPSTR					format,
	LPNFORM_INFO	formatInfo,			//Defaults to NULL
	LPCSTR				endIdx)         //Defaults to NULL
	{
	BOOL					afterDecimal = FALSE;
	LPCSTR				cp = str;
	int						whereFound;
	long double		num = 0.0;
	long double		floatWidth=10.0;
	int						sign = 1;
	NFORM_INFO		localFormatInfo;
	char					nonNumStr[40];
	BOOL					hadAPercent = FALSE;
	BOOL					notZeroDamnit = FALSE;

	if (!str)
		return 0.0;

	if (SearchForSubstring(str, "e+") || SearchForSubstring(str, "e-"))
		{ //Handle the parsing of an exponential number
		sscanf(str, "%Le", &num);
		return num;
		}

	if (!formatInfo)
		{
		ValidateNumberFormatString(format, &localFormatInfo, FALSE);
		formatInfo = &localFormatInfo;
		}

	while(*cp && (!endIdx ? TRUE : cp<=endIdx) )
		{
		if (isdigit(*cp) )
			{
			if (!afterDecimal)
				{
				num *= 10.0;
				num += *cp - '0';
				}
			else
				{
				num += (*cp - '0')/floatWidth;
				floatWidth *= 10;
				}
			}

		else if (*cp == g_chDecSep)
			afterDecimal = TRUE;

		else if (*cp == g_chThouSep|| *cp == g_chCurrency)
			;//Nada

		else if (*cp == '%')
			hadAPercent = TRUE;

		else if (IsNumberPart(*cp))
			{
			whereFound = FindCharIdx(format, *cp);
			if (whereFound != -1)
				{
				if ( (formatInfo->negitive.sFormatIdx != formatInfo->positive.sFormatIdx)  &&
						 (formatInfo->zero.sFormatIdx != formatInfo->positive.sFormatIdx) )
					{
					if (whereFound >= formatInfo->zero.sFormatIdx)						sign = 0;
					else if (whereFound >= formatInfo->negitive.sFormatIdx)	sign = -1;
					else 																										sign = 1;
					}
				else if (formatInfo->negitive.sFormatIdx != formatInfo->positive.sFormatIdx)
        	{
					if (whereFound >= formatInfo->negitive.sFormatIdx) 			sign = -1;
					else 																										sign = 1;
					}
				}//wherefound
			else if (*cp == '-') sign *= -1; //if no format contain '-' .....
			}//IsNumberPart

		else //Something else
			{
			FillNonNumberString(cp, nonNumStr, 40);
			whereFound = FindSubstringIdx(format, nonNumStr);
			if (whereFound != -1)
				{
				if ( (formatInfo->negitive.sFormatIdx != formatInfo->positive.sFormatIdx)  &&
						 (formatInfo->zero.sFormatIdx != formatInfo->positive.sFormatIdx) )
					{
					if (whereFound >= formatInfo->zero.sFormatIdx && !notZeroDamnit)
						{
						if (FindFirstNumber(str) != -1)
							notZeroDamnit = TRUE;
						else
							sign = 0;
            }
					else if (whereFound >= formatInfo->negitive.sFormatIdx)	sign = -1;
					else 																										sign = 1;
					}
				else if (formatInfo->negitive.sFormatIdx != formatInfo->positive.sFormatIdx)
        	{
					if (whereFound >= formatInfo->negitive.sFormatIdx) 			sign = -1;
					else 																										sign = 1;
					}
				}//wherefound
      cp += lstrlen(nonNumStr)-1;
			}//something else

		cp++;
		}//while

	if (hadAPercent)
		num /= 100.0;

	return num*sign;
	}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

long double GetDlgEditFormatedReal(
	HWND										hWndDlg,
	int											item,
	LPSTR										format)
	{
	char										tmpStr[100];
	long double 						num;

	GetDlgItemText(hWndDlg, item, tmpStr, sizeof(tmpStr) );
	num = ParseFormatedNumber(tmpStr, format);
	SetDlgEditFormatedReal(hWndDlg, item, num, format);
	return num;
	}

void SetDlgEditFormatedReal(
	HWND										hWndDlg,
	int											item,
	long double							num,
	LPSTR										format)
	{
	char										tmpStr[100];

	FormatFloatNumber(num, format, tmpStr, sizeof(tmpStr) );
	SetDlgItemText(hWndDlg, item, tmpStr);
	}


long GetDlgEditFormatedInteger(
	HWND										hWndDlg,
	int											item,
	LPSTR										format)
	{
	char										tmpStr[100];
	long 										num;
	long double							fl;

	GetDlgItemText(hWndDlg, item, tmpStr, sizeof(tmpStr) );
	fl = ParseFormatedNumber(tmpStr, format);
	fl += (fl > 0.0 ? 0.5: -0.5); //round

	while(fl > I_LARGEST_LONG  ||  fl < I_SMALLEST_LONG) fl /= 10;
	num = (long)fl;
	SetDlgEditFormatedInteger(hWndDlg, item, num, format);
	return num;
	}

void SetDlgEditFormatedInteger(
	HWND										hWndDlg,
	int											item,
	long 										num,
	LPSTR										format)
	{
	char										tmpStr[100];

	FormatFloatNumber(num, format, tmpStr, sizeof(tmpStr) );
	SetDlgItemText(hWndDlg, item, tmpStr);
	}

			
