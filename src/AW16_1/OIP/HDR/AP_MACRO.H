/* ****************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * **************************************************************** */

#ifndef AP_MACRO_H
#define AP_MACRO_H

/* ****************************************************************
 * Macros used to define and implement a protocol.
 * **************************************************************** */

#ifdef __cplusplus

/* C++ macros map directly to C++ constructs. */

#define APROT_DECLAREPROTOCOL(p)        class p;

#define APROT_BEGINPROTOCOL_(p)         class p {
#define APROT_BEGINPROTOCOL(p,pp)       class p : public pp {
#define APROT_BEGINPROTOCOLIMP_         APROT_BEGINPROTOCOL_
#define APROT_BEGINPROTOCOLIMP          APROT_BEGINPROTOCOL

#define APROT_ENDPROTOCOL_(p)           }
#define APROT_ENDPROTOCOL(p,pp)         }
#define APROT_ENDPROTOCOLIMP_           APROT_ENDPROTOCOL_
#define APROT_ENDPROTOCOLIMP            APROT_ENDPROTOCOL

#define APROT_BEGINSERVICES_(p)         public:
#define APROT_BEGINSERVICES(p,pp)       public:
#define APROT_BEGINSERVICESIMP_         APROT_BEGINSERVICES_
#define APROT_BEGINSERVICESIMP          APROT_BEGINSERVICES

#define APROT_ENDSERVICES_(p)
#define APROT_ENDSERVICES(p,pp)
#define APROT_ENDERVICESIMP_            APROT_ENDSERVICES_
#define APROT_ENDSERVICESIMP            APROT_ENDSERVICES

#define APROT_SERVICE(r,n,p)            virtual r ALMAPI n p = 0;
#define APROT_SERVICEIMP(r,n,p)         r ALMAPI n p

#define APROT_THIS(p)
#define APROT_VOID(p)                   void

#define APROT_BEGINAPI                  extern "C" {
#define APROT_ENDAPI                    }


#define APROT_PUBLIC                    public:
#define APROT_PRIVATE                   private:
#define APROT_PROTECTED                 protected:


#else

/*
 * C macros will define a structure to hold pointers to method functions and
 * a structure to hold pointer to vtable and data members.  The name of the
 * vtable structure is <protocol-name>_vtable, and the name of the structure to
 * hold vtable pointer and data members is just the protocol name.  These
 * are typedef'ed for convenience.
*/


#define APROT_DECLAREPROTOCOL(p)        typedef struct p##_ p;

#define APROT_BEGINPROTOCOL_(p)         typedef struct p##_ p;
#define APROT_BEGINPROTOCOL(p,pp)       typedef struct p##_ p;
#define APROT_BEGINPROTOCOLIMP_         APROT_BEGINPROTOCOL_
#define APROT_BEGINPROTOCOLIMP          APROT_BEGINPROTOCOL

#define APROT_ENDPROTOCOL_(p)           } p
#define APROT_ENDPROTOCOL(p,pp)         } p
#define APROT_ENDPROTOCOLIMP_           APROT_ENDPROTOCOL_
#define APROT_ENDPROTOCOLIMP            APROT_ENDPROTOCOL

#define APROT_BEGINSERVICES_(p)         typedef struct p##__v {
#define APROT_BEGINSERVICES(p,pp)       typedef struct p##__v {     \
                                        pp##_vtable   m_p;
#define APROT_BEGINSERVICESIMP_         APROT_BEGINSERVICES_
#define APROT_BEGINSERVICESIMP          APROT_BEGINSERVICES


#define APROT_ENDSERVICES_(p)           } p##_vtable;               \
                                        typedef struct p##_ {       \
                                        p##_vtable FAR*   m_vptr;
#define APROT_ENDSERVICES(p,pp)         } p##_vtable;               \
                                        typedef struct p##_ {       \
                                        pp      m_p;
#define APROT_ENDSERVICESIMP_           APROT_ENDSERVICES_
#define APROT_ENDSERVICESIMP            APROT_ENDSERVICES


#define APROT_SERVICE(r,n,p)            r (ALMAPI *n) p;
#define APROT_SERVICEIMP(r,n,p)           

#define APROT_THIS(p)                   p FAR*,
#define APROT_VOID(p)                   p FAR*

#define APROT_BEGINAPI
#define APROT_ENDAPI                      

#define APROT_PUBLIC
#define APROT_PRIVATE
#define APROT_PROTECTED

#endif

#endif  /* !AP_MACRO_H */

/* ****************************************************************
 * Macros used during default implementation
 * **************************************************************** */

#undef APROT_CPP

#ifdef APROT_DEFAULTIMPLEMENTATION

#ifdef __cplusplus
#define APROT_CPP(x)                      ;
#else
#define APROT_CPP(x)
#endif  /* !__cplusplus */

#undef APROT_DEFAULTIMPLEMENTATION

#else

#ifdef __cplusplus
#define APROT_CPP(x)  x
#else
#define APROT_CPP(x)
#endif  /* !__cplusplus */

#endif  /* APROT_DEFAULTIMPLEMENTATION */
