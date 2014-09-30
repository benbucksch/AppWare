///////////////////////////////////////////////////////////////
//
//	(c) 1993 Novell, Inc.  All rights reserved.
//
//	THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//	TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//	COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//	CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//	TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//	NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//	AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//	LIABILITY.
//
///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//
// Object Group:	Essentials
// Object:		  		Calculation
//
// Filename:	"parser.cpp"
//
// Author:	Kevin Smart
//   Start:		July 13, 1992
//   Finish:	October 23, 1992
//
// Description:
//	Parsing routine for evaluating the expression
//
////////////////////////////////////////////////////////////////////////////

#include <a_alm.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <signal.h>
#include <o_number.h>

#include "calc_cls.h"
#include "calc_obj.h"
#include "strclass.h"
#include "calc.h"
#include "listdata.h"

////////////////////////////////////////////////////////////////////////////

extern GLOBAL G;

////////////////////////////////////////////////////////////////////////////

/* Constants rounded for 21 decimals. */
///#define	E		2.71828182845904523536
///#define PI	3.14159265358979323846

////////////////////////////////////////////////////////////////////////////

// functions seen only in this file
BOOL	eval				(STACK&, STACK&, STACK&, char*, BOOL doEval);
BOOL	isFunction	(const char*, functions&);
BOOL	isConstant	(const char*, long double&);
BOOL	isInput			(const char*, long double&, long double*);
BOOL	isObject		(const char*, long double&, AOBJECTID);
BOOL	isOperator	(const char*, operators&);

/////////////////////////Destructor for STACK class ////////////////////////

STACK::~STACK()

{
	while(top!=NULL) Pop();
}

//////////////////////Trap for Floating Point Exceptions////////////////////

#pragma	argsused
extern "C" {
	void  cdecl  FPEtrap(int sig)

	{
	  char*	Warning = "WARNING!!!\n\nAn illegal operation occurred.  The \
	error may have caused data to become corrupted.  It is HIGHLY RECOMMENDED \
	THAT YOU EXIT your program because the program's action may be \
	unpredictable.\n\n               Do you wish to EXIT?";

	  char*	Title 	= "Floating Point Exception";
	  int	MB	= MessageBox(GetActiveWindow(), Warning, Title,
				     MB_ICONSTOP | MB_YESNO);
	  if(MB==IDYES) _exit(1);

	  G.SetError("A floating point exception error occurred.");
		G.SetFPEerror(TRUE);
	}
}
/////////////////////////Trap for Math Library Errors///////////////////////

int _matherrl(_exceptionl* e)

{
  char* ErrorTypes[] = {"domain", "singularity", "overflow", "underflow",
												"total loss of precision",
												"partial loss of precision", "stack fault"};

	char	error[80];

  sprintf(error, "%s \"%s\" error occurred in the expression.",
		(e->type==3 || e->type==4) ? "An" : "A", ErrorTypes[e->type-1]);
  G.SetError(error);

	G.SetMATHerror(e->type);
  return 1;
}

/////////////////////////Customized String Tokenizer////////////////////////

int StrTok(const char* string, const char* token, char* output)

{
  int 	i, j;

  BOOL	match = FALSE;

  i = 0;
  while(i<lstrlen(string) && !match) {
    j = 0;
    while(j<lstrlen(token) && !match) {
      if(string[i]==token[j]) {
				match = TRUE;
				}
      j++;
      }
    if(!match) {
      output[i] = string[i];
      i++;
      }
    }

  output[i]=0;  // null terminate the string

  return i;  // return the number of characters before the first token
}

//////////////////////////Precedence Comparator/////////////////////////////

BOOL	HigherPrec(operators op1, operators op2)

// return TRUE if op1 has greater or equal precedence than op2
{
  switch(op1) {
    case _FACT:
				return TRUE;

    case NEGATE:
				if(op2!=_FACT) return TRUE;
        else return FALSE;

    case _POW:
				if(op2!=_FACT && op2!=NEGATE) return TRUE;
        else return FALSE;

    case _MULTIPLY:
    case _DIVIDE:
    case _DIV:
    case _MOD:
    case _FAN:
    case _FCD:
				if(op2!=_FACT && op2!=NEGATE && op2!=_POW) return TRUE;
				else return FALSE;

    case _SUBTRACT:
    case _ADD:
				if(op2==_ADD || op2==_SUBTRACT) return TRUE;
				else return FALSE;

    default:
				return FALSE;
    }  // end...switch(op1)
}

////////////////////////////Expression Parser///////////////////////////////

BOOL	evaluate(const char* string, long double& result, char* errString,
							 AOBJECTID theObject, long double* theValues, BOOL doEval)

{
  char*		token;
	char*		token1 = "()!^*/%+- \r\n\t";
	char*		token2 = "+-0123456789.()beghjklpqrstuwxyzBEGHJKLPQRSTUWXYZ_ \r\n\t";
	char*		token3 = "+-0123456789.()abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_ \r\n\t";
	char*		token4 = "()!^*/%+-abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_ \r\n\t";

  STACK					func_stk(FUNCTION);
  STACK					optr_stk(OPERATOR);
  STACK					opnd_stk(NUMBER);

  stack_type		last_type	= OPERATOR;

  unsigned int	StrLen		= lstrlen(string);

  int						i, j;
  char*					output		= new char[StrLen+1];

  VAL						value;
  ELEMENT*			top;

  BOOL					first		= TRUE;
  BOOL					error		= FALSE;
  BOOL					skip;

  // make sure parenthesis match
  for (j=0, i=0; i<StrLen; i++) {
    if(string[i]=='(') {
      j++;
      }  // end...if(string[i]=='(')
    else if(string[i]==')') {
      j--;
      if(j<0) {
				break;
				}  // end...if(j<0)
      }  // end...else if(string[i]==')')
    }  // end...for(j=0, i=0; i<StrLen; i++)
  if(j!=0) {
    error = TRUE;
    lstrcpy(errString, "Mismatched parenthesis error.");
    delete[] output;
    return error;
    }

  // Set up operator stack for parsing (use implied parenthesis)
  value.optr	= BEGIN_PAREN;
  optr_stk.Push(value);
  // assume an answer of zero if there is no expression
  value.numb	= 0.0;
  opnd_stk.Push(value);

  // Parse and calculate the expression
  i=0;
  while (i<=StrLen && !error) {
    // eliminate white space characters
    if(isspace(string[i])) {
      i++;
      continue;
      }      

    skip = FALSE;
    // determine which token to use
    switch(last_type) {
      case FUNCTION:
      case OPERATOR:
			  	// next is an operand or function
        	token = token4;
					if(isdigit(string[i]) || string[i]=='.') {
						j = StrTok(&string[i], token, output);
						if(j!=0 && (string[i+j]=='e' || string[i+j]=='E')) {
	       			skip = TRUE;
	       			if(isdigit(string[i+j+1]) || string[i+j+1]=='-' || string[i+j+1]=='+') {
								output[j]=string[i+j];
		 						output[j+1]=string[i+j+1];
								j = j + 2 + StrTok(&string[i+j+2], token, &output[j+2]);
								}
							}
						}
          else {
	     			token = token1;
            }
					break;

      case NUMBER:
          if(isalpha(string[i])) {
	     			token = token2;  // next is an operator or an error
            j=0;
						}
			  	else {
	     			j = StrTok(&string[i], token3, output);
	     			skip = TRUE;
	     			if(j>1) {
	       			output[1]=0;
	       			j=1;
	       			}
            }
          break;
      }

    // find the location from the ith position to the first token
    if(!skip) {
      j = StrTok(&string[i], token, output);
      }

    // evaluate as necessary
    if(j==0) {
      // if j==0 then the next character is '(', ')', '+', '-', or an error
      switch(last_type) {
				case FUNCTION:
	     			switch(string[i]) {
	       			case '(':
             			value.optr = BEGIN_FUNC;
		    					optr_stk.Push(value);
		    					j = 1;
                  last_type = OPERATOR;
		    					break;

	       			case ')': // syntax error
	       			case '+': // syntax error 
	       			case '-': // syntax error
	       			default : // syntax error
		    					error = TRUE;
		    					lstrcpy(errString, "Syntax error in expression.");
		    					break;
	       			}  // end...switch(string[i])
	     			break;

				case OPERATOR:
						switch(string[i]) {
	      			case '(':
        					value.optr = BEGIN_PAREN;
		    					optr_stk.Push(value);
		    					j++;
		    					break;

	       			case '+': // does not change anything, so drop it!
		    					j++;
		    					break;

	       			case '-':
		    					value.optr = NEGATE;
		    					optr_stk.Push(value);
                  j++;
		    					break;

	       			case  0 :
		    					if(first) {
		      					j++;
		      					break;
							      }

							case ')': // syntax error
	       			default : // syntax error
		    					error = TRUE;
	     	    			lstrcpy(errString, "Syntax error in expression.");
		    					break;
	       			}  // end...switch(string[i])
	     			break;

				case NUMBER:
	    			switch(string[i]) {
       				case  0 : // end of string--same as ')'
	       			case ')': // evaluate stack until "BEGIN_PAREN" or "BEGIN_FUNC"              	    
	    						error = TRUE;
			    				lstrcpy(errString, "Syntax error in expression.");
			    				while((top=optr_stk.GetTop())!=NULL) {
		  	  				  if(top->value.optr==BEGIN_PAREN || top->value.optr==BEGIN_FUNC) {
											// make the final evaluation and quit
											error = !eval(opnd_stk, func_stk, optr_stk, errString, doEval);
											break;
											}  // end...if(top->value.optr...
										if(!eval(opnd_stk, func_stk, optr_stk, errString, doEval)) {
											break;
											}  // end if(!eval(opnd_stk, func_stk, optr_stk))
		      					}  // end...while(optr_stk.GetTop()!=NULL)
			    				j = 1;
			    				break;

	  	     		case '+':
		  	  				value.optr	= _ADD;
		    					// evaluate as necessary
		    					while((top=optr_stk.GetTop())!=NULL && !error && HigherPrec(top->value.optr, value.optr)) {
		      					error = !eval(opnd_stk, func_stk, optr_stk, errString, doEval);
		      					}  // end...while(top!=NULL && ...
		    					optr_stk.Push(value);
	                j = 1;
  	              last_type	= OPERATOR;
			    				break;
      	
	      	 		case '-': 
		    					value.optr	= _SUBTRACT;
		    					// evaluate as necessary
		    					while((top=optr_stk.GetTop())!=NULL && !error && HigherPrec(top->value.optr, value.optr)) {
		      					error = !eval(opnd_stk, func_stk, optr_stk, errString, doEval);
		      					}  // end...while(top!=NULL && ...
			    				optr_stk.Push(value);
  	              j = 1;
    	            last_type	= OPERATOR;
		  	  				break;

	      	 		case '(': // syntax error
	       			default : // syntax error
		    					error = TRUE;
	            		lstrcpy(errString, "Syntax error in expression.");
		    					break;
		       		}  // end...switch(string[i])
		     		break;

      	default : break;
				}  // end...switch(last_type)
      }  // end...if(j==0)
    else {  // j!=0
      switch(last_type) {
				case FUNCTION:
	    			error = TRUE;
	     			lstrcpy(errString, "A '(' must follow a function name.");
	     			break;

				case OPERATOR:
	     			if(isFunction(output, value.func)) {
              func_stk.Push(value);
	       			last_type = FUNCTION;
	       			}  // end...if(isFunction(output, value.func))
	     			else if(isConstant(output, value.numb)) {
              opnd_stk.Push(value);
	       			last_type = NUMBER;
	       			}  // end...else if(isConstant(output, value.numb))
	     			else if(isObject(output, value.numb, theObject)) {
	       			opnd_stk.Push(value);
	       			last_type = NUMBER;
	       			}  // end...else if(isObject(output, value.numb))
	     			else if(isInput(output, value.numb, theValues)) {
	      			opnd_stk.Push(value);
	       			last_type = NUMBER;
              }  // end...else if(isInput(output, value.numb, theValues)
	     			else {  // unrecognizable string
	       			error = TRUE;
	       			if(lstrlen(output)>20) {
		 						output[20] = output[21] = output[22] = '.';
								output[23] = '\0';
		 						}
	       			sprintf(errString, "Unable to recognize the string \"%s\" in the expression.", output);
	       			}  // end...if...else if...else if...else
	     			break;

				case NUMBER:
	    			if(isOperator(output, value.optr)) {
	      			if(value.optr==_FACT) {
		 						optr_stk.Push(value);
		 						error = !eval(opnd_stk, func_stk, optr_stk, errString, doEval);
		 						last_type = NUMBER;
		 						}  // end...if(value.optr==_FACT)
              else {  // value.optr!=_FACT
		 						// evaluate as necessary
	         			while((top=optr_stk.GetTop())!=NULL && !error && HigherPrec(top->value.optr, value.optr)) {
                  error = !eval(opnd_stk, func_stk, optr_stk, errString, doEval);
		   						}  // end...while(top!=NULL && ...
		 						optr_stk.Push(value);
		 						last_type = OPERATOR;
		 						}  // end...if(value.optr==_FACT)...else
	       			}  // end...if(isOperator(output))
	     			else {  // output is not an operator
	       			error = TRUE;
	       			lstrcpy(errString, "An operator must follow a variable or function.");
	       			}  // end...if(isOperator(output))...else
	     			break;

				default :
	     			error = TRUE;
	     			lstrcpy(errString, "Syntax error in expression.");
	     			break;
        }  // end...switch(last_type)
      }  // end...if(j==0)...else
    i += j;
    first = FALSE;
    }  // end...while(i<=StrLen)

  if(!error) {
    if((top=opnd_stk.GetTop())!=NULL) {
      value	= opnd_stk.Pop();
      result	= value.numb;
      }
    else {
      error = TRUE;
      lstrcpy(errString, "Syntax error in expression.");
      }
    }  // end...if(!error) 

  delete[] output;

  return error;
}

////////////////////////////////////////////////////////////////////////////

BOOL	isFunction	(const char* string, functions& func)

{
  char*	funcs[]	= {"ABS",  "ACOS", "ASIN",  "ATAN", "ATN",   "COS",  "SIN",
									 "TAN",  "EXP",  "LN",    "LOG",  "ROUND", "RND",  "INT",
								   "SQT",  "SQRT", "FLOOR", "CEIL", "COSH", "SINH",  "TANH",
								   "INV",  "FRAC"};

  #define num_funcs	(sizeof(funcs)/sizeof(funcs[0]))

  for(int i=0; i<num_funcs; i++) {
    if(!lstrcmpi(string, funcs[i])) break;
    }  // end...for(int i=0; i<num_funcs; i++)

  if(i<num_funcs) {
    switch(i) {
      case  0:  // "ABS"
	   			func = _ABS;
	   			break;

      case  1:  // "ACOS"
	   			func = _ACOS;
	   			break;

      case  2:  // "ASIN"
	   			func = _ASIN;
	   			break;

      case  3:  // "ATAN"
      case  4:  // "ATN"
	   			func = _ATAN;
	   			break;

      case  5:  // "COS"
	   			func = _COS;
	   			break;

      case  6:  // "SIN"
	   			func = _SIN;
	   			break;

      case  7:  // "TAN"
	   			func = _TAN;
	   			break;

      case  8:  // "EXP"
	   			func = _EXP;
          break;

      case  9:  // "LN"
	   			func = _LN;
	   			break;

      case 10:  // "LOG"
	   			func = _LOG;
	   			break;

     	case 11:  // "ROUND"
      case 12:  // "RND"
	   			func = _RND;
	   			break;

      case 13:  // "INT"
	   			func = _INT;
	   			break;

      case 14:  // "SQT"
      case 15:  // "SQRT"
	   			func = _SQRT;
	   			break;

      case 16:  // "FLOOR"
	   			func = _FLOOR;
	   			break;

      case 17:  // "CEIL"
	   			func = _CEIL;
	   			break;

      case 18:  // "COSH"
	   			func = _COSH;
	   			break;

      case 19:  // "SINH"
	   			func = _SINH;
	   			break;

      case 20:  // "TANH"
	   			func = _TANH;
	   			break;

      case 21:  // "INV"
	   			func = _INV;
	   			break;

      case 22:  // "FRAC"
	   			func = _FRAC;
          break;
           
      default:  // not implemented
          return FALSE;
      }  //  end...switch(i)

    return TRUE;
    }  // end...if(i<num_funcs)
  else {  // i>=num_funcs
    return FALSE;
    }  // end...if(i<num_funcs)...else
}

////////////////////////////////////////////////////////////////////////////

BOOL	isConstant	(const char* string, long double& numb)

{
  char*	cnsts[]				= {"E",   "PI"};
  char*	valid_numbers	= "+-eE0123456789.";

  #define num_cnsts	(sizeof(cnsts)/sizeof(cnsts[0]))

  for(int i=0; i<num_cnsts; i++) {
    if(!lstrcmpi(string, cnsts[i])) break;
    }  // end...for(int i=0; i<num_cnsts; i++)

  if(i<num_cnsts) {
    switch(i) {
      case  0:  // "E"
	   			numb = E;
	   			break;

      case  1:  // "PI"
	   			numb = PI;
	   			break;

      default:  // not implemented
          return FALSE;
      }  // end...switch(i)

    return TRUE;  // it was a constant
    }  // end...if(i<num_cnsts)
  else {  // i>=num_cnsts
    BOOL valid = TRUE;
    int  j;
    i=0;

    while(i<lstrlen(string) && valid) {
      valid	= FALSE;
      j		= 0;

      while(j<lstrlen(valid_numbers) && !valid) {
				if(string[i]==valid_numbers[j]) {
	  			valid=TRUE;
	  			}
        j++;
				}  // end...while(j<sizeof(valid_numbers) && ...
      i++;
      }  // end...while(i<lstrlen(string) && vaild)

    if(valid) {
      char*	endptr;
      numb	= _strtold(string, &endptr);
      if(endptr[0]!=0) { // the token string was not a constant
				return FALSE;
				}  // end...if(endptr[0]!=0)
      else { // endptr[0]==0
				return TRUE;  // the string was a constant
        }  // end...if(endptr[0]!=0)...else
      }  // end...if(valid)
    else { // valid==FALSE
      return FALSE;
      }  // end...if(valid)...else
    }  // end...if(i<num_cnsts)...else
}

////////////////////////////////////////////////////////////////////////////

BOOL	isInput		(const char* string, long double& numb, long double* value)

{
  if(!value) {
    return FALSE;
    }

  char*	inpts[]		= {"X", "Y"};

  #define num_inpts	(sizeof(inpts)/sizeof(inpts[0]))

  for(int i=0; i<num_inpts; i++) {
    if(!lstrcmpi(string, inpts[i])) break;
    }  // end...for(int i=0; i<num_inpts; i++)

  if(i<num_inpts) {
    switch(i) {
      case  0:  // "X"
					numb = value[0];
	   			break;

      case  1:  // "Y"
	   			numb = value[1];
	   			break;

      default:  // only X & Y allowed
          return FALSE;
      }  // end...switch(i)

    return TRUE;  // it was an input
    }  // end...if(i<num_inpts)
  else {
    return FALSE;
    }
}

////////////////////////////////////////////////////////////////////////////

BOOL	isObject	(const char* string, long double& numb, AOBJECTID theObject)

{
  if(!theObject) {
    return FALSE;
    }

  BOOL					retVal = FALSE;

	CALCULATION*	calc = (CALCULATION*)AObjGetRuntimeInfo(theObject);

	AOBJECTID*	variableOBJIDs = calc->GetData0();

	CALCHEADER*	calcHeader = calc->GetData3();

	LISTDATA			ld;

	if (calc->GetVariableNames()!=NULL && variableOBJIDs!=NULL && calcHeader!=NULL) {
		for(short i=0; i<calcHeader->count && !retVal; i++) {
			ld.Init(variableOBJIDs[i], calc->Get(i));

			if(!lstrcmp(string, ld.Alias())) {
				// load numb with the value of the number
				ONmbrGetReal(ld.TheObject(), &numb);

				retVal = TRUE;
				}
			}
	}
  return retVal;
}

////////////////////////////////////////////////////////////////////////////

BOOL	isOperator	(const char* string, operators& optr)

{
  char*	optrs[]	= {"!", "^", "*", "/", "DIV", "%", "MOD",  "FAN",  "FCD",
								   "+", "-"};

  #define num_optrs	(sizeof(optrs)/sizeof(optrs[0]))

  for(int i=0; i<num_optrs; i++) {
    if(!lstrcmpi(string, optrs[i])) break;
    }  // end...for(int i=0; i<num_optrs; i++)

  if(i<num_optrs) {
    switch(i) {
      case  0:  // "!"
	   			optr = _FACT;
	   			break;

      case  1:  // "^"
	   			optr = _POW;
	   			break;

      case  2:  // "*"
	   			optr = _MULTIPLY;
	   			break;

      case  3:  // "/"
	   			optr = _DIVIDE;
	   			break;

      case  4:  // "DIV"
	   			optr = _DIV;
	   			break;

      case  5:  // "%"
      case  6:  // "MOD"
	   			optr = _MOD;
	   			break;

      case  7:  // "FAN"
	   			optr = _FAN;
	   			break;

      case  8:  // "FCD"
	   			optr = _FCD;
	   			break;

      case  9:  // "+"
	   			optr = _ADD;
	   			break;

      case 10:  // "-"
	   			optr = _SUBTRACT;
	   			break;

      default:  // not implemented
	   			return FALSE;
      }  //  end...switch(i)

    return TRUE;
    }  // end...if(i<num_optrs)
  else {  // i>=num_optrs
    return FALSE;
    }  // end...if(i<num_optrs)...else
}

////////////////////////////////////////////////////////////////////////////

BOOL eval(STACK& opnd_stk, STACK& func_stk, STACK& optr_stk, char* errString, BOOL doEval)

{
  VAL					value;
  long double	x, y;
  operators		optr;
  functions		func;

  G.SetFPEerror(FALSE);
	G.SetMATHerror(FALSE);

//  FPE_TRAP	oldTrap = _FPInit();
  if((long)signal(SIGFPE, FPEtrap)==(long)SIG_ERR) { // set up the floating point exception trap
    if(MessageBox(NULL, "Unable to create floating point exception trap.", NULL, MB_OKCANCEL)==IDCANCEL)
      return FALSE;
    }

  value	= opnd_stk.Pop();
  y	= value.numb;
  value	= optr_stk.Pop();
	optr	= value.optr;

	if(!doEval)
  	value.numb = 0;

  switch(optr) {
    case NEGATE:
				if(doEval) value.numb = -y;
	 			break;

    case BEGIN_PAREN:
				if(doEval) value.numb = y;
	 			break;

    case BEGIN_FUNC:
	 			value	= func_stk.Pop();
	 			func	= value.func;
	 			switch(func) {
	  			case _ABS:
							if(doEval) value.numb = fabsl(y);
							break;

	   			case _ACOS:
							if(doEval) value.numb = acosl(y);
							break;

	   			case _COS:
							if(doEval) value.numb = cosl(y);
							break;

	   			case _ASIN:
							if(doEval) value.numb = asinl(y);
							break;

	   			case _SIN:
							if(doEval) value.numb = sinl(y);
							break;

	   			case _ATAN:
							if(doEval) value.numb = atanl(y);
							break;

	   			case _TAN:
							if(doEval) value.numb = tanl(y);
							break;

	   			case _EXP:
							if(doEval) value.numb = expl(y);
							break;

          case _LN:
							if(doEval) value.numb = logl(y);
							break;

	   			case _LOG:
							if(doEval) value.numb = log10l(y);
							break;

	   			case _RND:
							if(doEval) value.numb = floorl(y+0.5);
							break;

	   			case _INT:
							if(doEval) value.numb = y>0 ? floorl(y) : ceill(y);
							break;

	   			case _SQRT:
							if(doEval) value.numb = sqrtl(y);
							break;

	   			case _FLOOR:
							if(doEval) value.numb = floorl(y);
							break;

	   			case _CEIL:
							if(doEval) value.numb = ceill(y);
							break;

	   			case _COSH:
							if(doEval) value.numb = coshl(y);
							break;

	   			case _SINH:
							if(doEval) value.numb = sinhl(y);
							break;

	   			case _TANH:
							if(doEval) value.numb = tanhl(y);
							break;

					case _INV:
							if(doEval) {
  							if(y==0) {
	  	 	  				lstrcpy(errString, "Division by zero error.");
//									_FPTerm(oldTrap);
								  return FALSE;
          	 	  	}
								value.numb = 1/y;
                }
							break;

	   			case _FRAC:
							if(doEval) value.numb = y>0 ? y-floorl(y) : y-ceill(y);
              break;

	   			default :
//		   				_FPTerm(oldTrap);
							return FALSE;
	   			}
				break;

		case _FACT:
				if(doEval) {
		 			x		= floorl(y+0.5);  // round and do integer factorial
		 			if(x==0.0) {
	  	 			value.numb	= 1.0;
	   				}  // end...if(x==0.0)
	 				else if(x>1754.0 || x<0.0) {
	          if(x>1754.0) {
		     			lstrcpy(errString, "Overflow error in factorial.");
							}
	   				else {
	     				lstrcpy(errString, "Can't do factorials of negative numbers.");
							}
//	   				_FPTerm(oldTrap);
		   			return FALSE;
	  	 			}  // end...else if(x<0.0)
	 				else {  // 0<x<1754
	   				value.numb		= x;
          	x--;
		   			while(x>1.0) {
		     			value.numb	*= x;
	  	   			x--;
	    	 			}  // end...while(x>0)
						}  // end...if(x==0.0)...else if(x<0.0)...else
          }  // end..if(doEval)
	 			break;

    case _POW:
				value		= opnd_stk.Pop();
				x				= value.numb;
				if(doEval) value.numb	= powl(x, y);
	 			break;

    case _MULTIPLY:
	 			value		= opnd_stk.Pop();
	 			x				= value.numb;
				if(doEval) value.numb	= x*y;
	 			break;

    case _DIVIDE:
				if(doEval) {
					if(y==0) {
	  				lstrcpy(errString, "Division by zero error.");
//				  	_FPTerm(oldTrap);
	   				return FALSE;
						}
					}
	 			value		= opnd_stk.Pop();
	 			x				= value.numb;
				if(doEval) value.numb	= x/y;
	 			break;

    case _DIV:
				value		= opnd_stk.Pop();
				if(doEval) {
		 			x				= value.numb;
		 			if(y==0) {
	  	 			lstrcpy(errString, "Division by zero error.");
//	  	 			_FPTerm(oldTrap);
	   				return FALSE;
	   				}
		 			value.numb	= x/y - fmodl(x, y);
					modfl(x/y, &value.numb);
          }
	 			break;

    case _MOD:
				value		= opnd_stk.Pop();
				if(doEval) {
		 			x				= value.numb;
		 			if(y==0) {
	  	 			lstrcpy(errString, "Division by zero error.");
//	  	 			_FPTerm(oldTrap);
	   				return FALSE;
	   				}
					value.numb	= fmodl(x, y);
          }
	 			break;

    case _FAN:
				value		= opnd_stk.Pop();
				if(doEval) {
		 			x				= value.numb;
		 			if(x==0.0 || (x==-1.0 && y>=0.0)) {
	  	 			lstrcpy(errString, "Division by zero error.");
//						_FPTerm(oldTrap);
	   				return FALSE;
	   				}
					value.numb	= (1.0-powl((1.0+x), -y))/x;
          }
	 			break;

    case _FCD:
				value		= opnd_stk.Pop();
				if(doEval) {
		 			x				= value.numb;
		 			if(x==-1.0 && y<=0.0) {
	  	 			lstrcpy(errString, "Division by zero error.");
//	  	 			_FPTerm(oldTrap);
	   				return FALSE;
	   				}
					value.numb	= powl((1.0+x), y);
          }
	 			break;

    case _ADD:
        value		= opnd_stk.Pop();
	 			x				= value.numb;
				if(doEval) value.numb	= x+y;
	 			break;

    case _SUBTRACT:
	 			value		= opnd_stk.Pop();
	 			x				= value.numb;
				if(doEval) value.numb	= x-y;
	 			break;

    default :
//	 			_FPTerm(oldTrap);
	 			return FALSE;
    }

  opnd_stk.Push(value);
//  _FPTerm(oldTrap);

  if(G.MATHerror() || G.FPEerror()) {
		lstrcpy(errString, G.Error());
    return FALSE;
    }
  else return TRUE;
}

////////////////////////////////////////////////////////////////////////////
