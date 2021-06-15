/* 
 * arg.h 
 * Craig Fitzgerald
 * This module helps parse command line parameters
 */

#define  NO_ERROR       0
#define  NO_EXTENSION   1
#define  UNKNOWN_CMD    2

#ifndef BOOL 
#define  BOOL           int 
#define  FALSE          0
#define  TRUE           1
#endif

/*
 * pszArgument = the string we are looking for
 * pszParam    = if NULL, the value in uiCount is incremented
 *               once each time pszArgument is encountered.
 *               if not NULL a value is expected after the param
 *               and is put in pszParam and uiCount is incremented
 * uiCount     = the number of times pszArgument is encountered
 *               in the command string
 *
 * if an argument is encountered on the command line without a 
 * - or / preceeding it, ProcessParams returns with its index
 */



typedef  struct 
   {
            char  *pszArgument;
            char  *pszParam;
   unsigned int   uiCount;
   } ARGBLK;

extern int ProcessParams (char         *argv [], 
                          ARGBLK       argrec [], 
                          unsigned int uiCount, 
                          unsigned int *uiError);

/* Returns # of args processed.  Compound args (I.E. -F 5000 ) count   *
 * as two. If an error occurs, the error code is stored in the high    *
 * byte of iError and the relative index of the offending arg is       *
 * stored in the low byte.  In case of multiple errors the last one    *
 * encountered is returned. In case of compound argument redifinitions *
 * the last definition is used.                                        */




