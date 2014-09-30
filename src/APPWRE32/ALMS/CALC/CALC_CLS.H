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
//////////////////////////////////////////////////////////////*/

////////////////////////////////////////////////////////////////////////////
//
//
// Object Group:	Essentials
// Object:		  		Calculation
//
// Filename:	"calc_cls.hpp"
//
// Author:	Kevin Smart
//   Start:		July 13, 1992
//   Finish:	July xx, 1992
//
// Description:
//	Class definition for STACK used in parsing routine
//	to evaluate an expression
//
////////////////////////////////////////////////////////////////////////////

#ifndef __CALC_CLS_HPP
#define __CALC_CLS_HPP

/* Constants rounded for 21 decimals. */
#define	E		2.71828182845904523536
#define PI	M_PI    //3.14159265358979323846


////////////////////////////////////////////////////////////////////////////

//typedef void ( * FPE_TRAP)();
//
//extern "C" {
//  FPE_TRAP CALLBACK /*!!PORT!! was _far_ _pascal_*/ _FPInit();
//  void CALLBACK /*!!PORT!! was _far_ _pascal_*/ _FPTerm(FPE_TRAP);
//	}

enum operators {_FACT, NEGATE, _POW, _MULTIPLY, _DIVIDE, _DIV, _MOD,
								_FAN, _FCD, _ADD, _SUBTRACT, END_PAREN, BEGIN_PAREN,
								BEGIN_FUNC};

enum functions {_ABS,  _ACOS, _ASIN, _ATAN, _COS,  _SIN,   _TAN,  _EXP,
								_LN,   _LOG,  _RND,  _INT,  _SQRT, _FLOOR, _CEIL, _COSH,
								_SINH, _TANH, _INV,  _FRAC};

enum stack_type {NUMBER, OPERATOR, FUNCTION};

union VAL {
  long double	numb;
  operators		optr;
  functions		func;
  };

struct ELEMENT {
  VAL	      	value;
  ELEMENT*		prev;
  };

////////////////////////////////////////////////////////////////////////////

class STACK {
  private:
    stack_type	type;
    ELEMENT*		top;

  public:
		STACK(stack_type st)				{type = st; top = NULL;}
    ~STACK();

    void	Push(VAL val) {
      ELEMENT* ele	= new ELEMENT;
      ele->value	= val;
      ele->prev		= top;
      top		= ele;
      }

    VAL		Pop() {
      VAL val		= top->value;
      ELEMENT* tmp	= top->prev;
      delete top;
      top		= tmp;
      return val;
      }

    ELEMENT*		GetTop()				{return top;}
    stack_type	GetStackType()	{return type;}

  };

////////////////////////////////////////////////////////////////////////////

#endif	// #ifndef __CALC_CLS_HPP
