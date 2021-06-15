#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arg.h"

#define STRLEN    255
#define  DATE       __DATE__ 


int   iLineNo;
int   iFileCount   = 0;
int   iDlgCount    = 0;
int   iDialogID    = 1;
int   iDialogYSize = 0;
int   iXScale      = 100;
int   iYScale      = 100;
int   iDefYPos     = 40;
char  szInFile  [STRLEN];
char  szOutFile [STRLEN];
FILE  *fpIn, *fpOut;


ARGBLK args[] = {{ "xlate",      NULL, 0},
                 { "verbose",    NULL, 0},             
                 { "x",          "",   0},
                 { "y",          "",   0},
                 { "pos",        "",   0},
                 { "noinclude",  NULL, 0},
                 { "dlgval",     "",   0},
                 { "header",     NULL, 0},
                 { "class",      "",   0},
                 { "style",      "",   0},
                 { "include",    "",   0}
                };

typedef struct
   {  SHORT x;
      SHORT y;
      SHORT xs;
      SHORT ys;
   }  POS;

typedef struct
   {  char * win;
      char * os2;
   }  CLA;

//class xlations
//
//    WIN Class        OS/2 Class
//    -------------------------------------------
CLA   _cla[] = 
   {  {"button",       "WC_BUTTON"},
      {"scrollbar",    "WC_SCROLLBAR"},
      {"edit",         "WC_ENTRYFIELD"},
      {"static",       "WC_STATIC"},
      {"listbox",      "WC_LISTBOX"},
      {"combobox",     "WC_COMBOBOX"},
      {"dialog",       "dialog"},       /* not really a class */
      {NULL, NULL}
   };


typedef struct
   {  char * oldCla;  /* NULL = match any                        */
      char * winSty;  /* style to match, NULL = always add style */
      char * os2Cla;  /* !NULL = change class to this            */
      char * os2Sty;  /* new style, NULL = no equivalent         */
   }  STY;

//style xlations
//
//    OS/2 Class  WIN Style               New OS/2 Class     OS/2 Style
//    -----------------------------------------------------------------------
STY   _sty[] =

    /*---------------- Button Class---------------------*/
   {  {"WC_BUTTON", "BS_DEFPUSHBUTTON",       NULL,         "BS_DEFAULT"},
      {"WC_BUTTON", "BS_LEFTTEXT",            NULL,         NULL},
      {"WC_BUTTON", "BS_GROUPBOX",            "WC_STATIC",  "SS_GROUPBOX"},
      {"WC_BUTTON", "BS_OWNERDRAW",           NULL,         NULL},
      {"WC_BUTTON", NULL,                     NULL,         "WS_VISIBLE"},

    /*---------------- Combobox Class-------------------*/
      {"WC_COMBOBOX", "CBS_OWNERDRAWFIXED",   NULL, NULL},
      {"WC_COMBOBOX", "CBS_OWNERDRAWVARIABLE",NULL, NULL},
      {"WC_COMBOBOX", "CBS_AUTOHSCROLL",      NULL, NULL},
      {"WC_COMBOBOX", "CBS_SORT",             NULL, NULL},
      {"WC_COMBOBOX", "CBS_HASSTRINGS",       NULL, NULL},
      {"WC_COMBOBOX", "CBS_OEMCONVERT",       NULL, NULL},
      {"WC_COMBOBOX", "WS_VSCROLL",           NULL, ""},
      {"WC_COMBOBOX", NULL,                   NULL, "WS_VISIBLE"},

    /*---------------- Edit Class-----------------------*/
      {"WC_ENTRYFIELD", "ES_LOWERCASE",       NULL,    NULL},
      {"WC_ENTRYFIELD", "ES_UPPERCASE",       NULL,    NULL},
      {"WC_ENTRYFIELD", "ES_PASSWORD",        NULL,    "ES_UNREADABLE"},
      {"WC_ENTRYFIELD", "ES_MULTILINE",       "WC_MLE",""},
      {"WC_ENTRYFIELD", "ES_AUTOVSCROLL",     NULL,    ""},
      {"WC_ENTRYFIELD", "ES_AUTOHSCROLL",     NULL,    "ES_AUTOSCROLL"},
      {"WC_ENTRYFIELD", "ES_NOHIDESEL",       NULL,    NULL},
      {"WC_ENTRYFIELD", "ES_OEMCONVERT",      NULL,    NULL},
      {"WC_ENTRYFIELD", "WS_BORDER",          NULL,    "ES_MARGIN"},
      {"WC_ENTRYFIELD", NULL,                 NULL,    "WS_VISIBLE"},

    /*---------------Multi Edit Class--------------------*/
      {"WC_MLE", "ES_LOWERCASE",              NULL,    NULL},
      {"WC_MLE", "ES_UPPERCASE",              NULL,    NULL},
      {"WC_MLE", "ES_PASSWORD",               NULL,    NULL},
      {"WC_MLE", "ES_MULTILINE",              NULL,    ""},
      {"WC_MLE", "ES_BORDER",                 NULL,    "MLS_BORDER"},
      {"WC_MLE", "WS_BORDER",                 NULL,    "MLS_BORDER"},
      {"WC_MLE", "ES_AUTOVSCROLL",            NULL,    "MLS_VSCROLL"},
      {"WC_MLE", "ES_AUTOHSCROLL",            NULL,    "MLS_HSCROLL"},
      {"WC_MLE", "ES_NOHIDESEL",              NULL,    NULL},
      {"WC_MLE", "ES_OEMCONVERT",             NULL,    NULL},
      {"WC_MLE", "ES_LEFT",                   NULL,    ""},
      {"WC_MLE", "ES_RIGHT",                  NULL,    ""},
      {"WC_MLE", "ES_CENTER",                 NULL,    ""},
                    
      {"WC_MLE", NULL,                        NULL,    "WS_VISIBLE"},

    /*---------------- Lostbox Class--------------------*/
      {"WC_LISTBOX", "LBS_STANDARD",          NULL, NULL},
      {"WC_LISTBOX", "LBS_EXTENDEDSEL",       NULL, "LS_MULTIPLESEL"},
      {"WC_LISTBOX", "LBS_HASSTRINGS",        NULL, NULL},
      {"WC_LISTBOX", "LBS_NOTIFY",            NULL, ""},
      {"WC_LISTBOX", "LBS_MULTIPLESEL",       NULL, "LS_MULTIPLESEL"},
      {"WC_LISTBOX", "LBS_MULTICOLUMN",       NULL, "LS_HORIZSCROLL"},
      {"WC_LISTBOX", "LBS_NOINTEGRALHEIGHT",  NULL, "LS_NOADJUSTPOS"},
      {"WC_LISTBOX", "LBS_SORT",              NULL, NULL},
      {"WC_LISTBOX", "LBS_NOREDRAW",          NULL, NULL},
      {"WC_LISTBOX", "LBS_OWNERDRAWFIXED",    NULL, "LS_OWNERDRAW"},
      {"WC_LISTBOX", "LBS_OWNERDRAWVARIABLE", NULL, "LS_OWNERDRAW"},
      {"WC_LISTBOX", "LBS_USETABSTOPS",       NULL, NULL},
      {"WC_LISTBOX", "LBS_WANTKEYBOARDINPUT", NULL, NULL},
      {"WC_LISTBOX", "WS_VSCROLL",            NULL, ""},
      {"WC_LISTBOX", NULL,                    NULL, "WS_VISIBLE"},

    /*----------------Win Scrollbar Class------------------*/
      {"WC_SCROLLBAR", "SBS_RIGHTALIGN",      NULL, NULL},
      {"WC_SCROLLBAR", "SBS_LEFTALIGN",       NULL, NULL},
      {"WC_SCROLLBAR", "SBS_TOPALIGN",        NULL, NULL},
      {"WC_SCROLLBAR", "SBS_BOTTOMALIGN",     NULL, NULL},
      {"WC_SCROLLBAR", "SBS_SIZEBOX",         NULL, NULL},
      {"WC_SCROLLBAR", NULL,                  NULL, "WS_VISIBLE"},

    /*----------------Win Static Class---------------------*/
      {"WC_BUTTON", "BS_GROUPBOX",            "WC_STATIC",  "SS_GROUPBOX"},
      {"WC_STATIC", "BS_GROUPBOX",            NULL, ""},
      {"WC_STATIC", "SS_LEFT",                NULL, "SS_TEXT | DT_LEFT"},
      {"WC_STATIC", "SS_CENTER",              NULL, "SS_TEXT | DT_CENTER"},
      {"WC_STATIC", "SS_RIGHT",               NULL, "SS_TEXT | DT_RIGHT"},
      {"WC_STATIC", "SS_BITMAP",              NULL, "SS_ICON"},
      {"WC_STATIC", "SS_LEFTNOWORDWRAP",      NULL, NULL},
      {"WC_STATIC", "SS_SIMPLE",              NULL, "SS_TEXT | DT_LEFT"},
      {"WC_STATIC", "SS_NOPREFIX",            NULL, NULL},
      {"WC_STATIC", "SS_BLACKRECT",           NULL, "SS_FGNDRECT"},
      {"WC_STATIC", "SS_GRAYRECT",            NULL, "SS_HALFTONERECT"},
      {"WC_STATIC", "SS_WHITERECT",           NULL, "SS_BKGNDRECT"},
      {"WC_STATIC", "SS_BLACKFRAME",          NULL, "SS_FGNDFRAME"},
      {"WC_STATIC", "SS_GRAYFRAME",           NULL, "SS_HALFTONEFRAME"},
      {"WC_STATIC", "SS_WHITEFRAME",          NULL, "SS_BKGNDFRAME"},
      {"WC_STATIC", "SS_USERITEM",            NULL, NULL},
      {"WC_STATIC", NULL,                     NULL, "WS_VISIBLE | DT_MNEMONIC"},

    /*--------------Win Dialog Pseudo Class----------------*/
    /*---Used to convert styles of the dialog box itself---*/
      {"dialog", "WS_DLGFRAME",            NULL, "FS_NOBYTEALIGN | FS_DLGBORDER"},
      {"dialog", "WS_POPUP",               NULL, "WS_CLIPSIBLINGS | WS_SAVEBITS"},

    /*----------------All Button Classes-------------------*/
      {NULL, "WS_CHILD",                   NULL, ""},
      {NULL, "WS_BORDER",                  NULL, ""},
      {NULL, NULL, NULL, NULL}                        /* terminator */
   };

CLA   *cla;
STY   *sty;

void Msg (void)
   {
   printf ("CvtDlg   Windows to OS/2 Dialog Conversion Util.      (C)ITI  v1.00 %s\n\n", DATE);
   printf ("USAGE: CVTDLG [options] InFiles\n\n");
   printf ("WHERE: InFiles ... Windows DLG file list, wildcards ok\n");
   printf ("       [options] . 0 or more of:\n\n");
   printf ("   -xlate ........ Show Translation Table\n");
   printf ("   -verbose ...... Verbose Messages\n");
   printf ("   -noinclude .... Don't put ""#include <os2.h>"" in OutputFile\n");
   printf ("   -header ....... assume WIN dlg name #defined to OS/2 ID in header\n");
   printf ("   -x # .......... specify new horizontal scale (treated as percentage)\n");
   printf ("   -y # .......... specify new vertical scale (treated as percentage)\n");
   printf ("   -pos # ........ specify new starting y pos (default = 40)\n");
   printf ("   -dlgval # ..... use # as 1st dialog ID (overridden by -header)\n");
   printf ("   -include FName. add #include \"FName\" to OutputFile\n");
   printf ("   -class FName .. add to or override Default class translations\n");
   printf ("   -style FName .. add to or override style translations\n\n");
   printf ("     If the input file has an extension of DLG, it's output file will have the\n");
   printf ("   extension of OS2. Otherwise, the output file will have the extension of DLG.\n");
   printf ("     The file format of 'FName' for the -class and -style options is the same\n");
   printf ("   as that produced by the -xlate option. blank lines and lines starting with a\n");
   printf ("   semicolon ';' are ignored.  New entrys take precedence over old ones.\n");
   exit   (0);
   }


void Error (char *pszStr1, char *pszStr2)
   {
   printf ("Error (%s:%d): %s %s",
            szInFile, iLineNo, pszStr1, pszStr2);
   fclose (fpIn);
   fclose (fpOut);
   exit (0);
   }


/* Returns first non WhiteSpace Char, That char is still in stream
 * This procedure skips comments
 */
int SkipWhite (FILE *fp)
   {
   int      c, cold, cnew;
   fpos_t   pos;

   
   while (1)
      {
      while ((c = getc (fp)) == ' ' || c == '\n' || c == '\t')
         iLineNo += (c == '\n');
      /* skip comments */
      if (c == '/')
         {
         fgetpos (fp, &pos);
         if (cold = (getc (fp)) == '*')  /* where in a comment */
            {
            while (1)
               {
               if ((cnew = getc (fp)) == '/' && cold == '*')
                  break;
               cold = cnew;
               iLineNo += (cnew == '\n');
               }
            }
         else
            {
            fsetpos (fp, &pos);
            break;
            }
         }
      else
         {
         break;
         }
      }
   return (c == EOF ? c : ungetc (c, fp));
   }



/* Reads a string. Delimeted by a whitespace or , unless first nonwhite is a "
 * in which case spaces are ok.
 * dies on EOF
 * terminator is returned and still in stream
 */
int GetStr (FILE *fp, char *psz)
   {
   int   c;

   SkipWhite (fp);
   if ((c = getc (fp)) == EOF)
      Error ("Unexpected EOF While Starting To Read a String", "");

   *(psz++) = (char) c;
   if (c == '"')
      {
      do
         *(psz++) = (char)(c = getc (fp));
      while (c != '"' && c!= EOF);
      *psz = '\0';
      }
   else if (c == ',')
      {
      *psz = '\0';
      }
   else
      {
      while ((c = getc (fp)) != ' ' && c!= '\n' &&
              c!= EOF && c!= ',' && c != '|' && c != '\t')
         *(psz++) = (char) c;
      ungetc (c, fp);
      *psz = '\0';
      }
   return c;
   }








/* assumes no whitespace to worry about
 */
void StripQuotes (char *psz)
   {
   if (psz[strlen (psz) -1] != '"')
      return;
   psz[strlen (psz) -1] = '\0';
   if (*psz != '"')
      return;
   strcpy (psz, psz+1);
   }


/* Reads a val and returns it in i
 */
void GetVal (FILE *fp, int *i)
   {
   char  szTmp[STRLEN];

   GetStr (fp, szTmp);
   *i = atoi (szTmp);
   }


/* reads str from fp, compares to psz,
 * returns TRUE if SAME!
 */
BOOL Taste (FILE *fp, char *psz)
   {
   char  szTmp [STRLEN];

   GetStr (fp, szTmp);
   return !stricmp (psz, szTmp);
   }


/* reads str from fp, compares to psz,
 * dies if different
 */
void Eat (FILE *fp, char *psz)
   {
   char  szTmp [STRLEN];

   GetStr (fp, szTmp);
   if (!stricmp (psz, szTmp))
      return;
   printf ("CvtDlg Error (%s:%d): Unmatching Tokens Expected:(%s), Got:(%s)\n",
               szInFile, iLineNo, psz, szTmp);
   exit (1);
   }


/* reads a pos
 */
void GetPos (FILE *fp, POS *ppos)
   {
   GetVal (fp, &(ppos->x));
   Eat    (fp, ",");
   GetVal (fp, &(ppos->y));
   Eat    (fp, ",");
   GetVal (fp, &(ppos->xs));
   Eat    (fp, ",");
   GetVal (fp, &(ppos->ys));
   }


/* skip to eol
 */
void SkipLine (FILE *fp)
   {
   int   c;

   while ((c = getc (fp)) != '\n' && c != EOF)
      ;
   iLineNo++;
   }



void Scale (int *i, int iScale)
   {
   *i  = (SHORT) (((LONG) *i  * (LONG) iScale) / 100L);
   }

/* converts a pos from os2 to win
 */
void CvtPos (POS *pos, int iYSize)
   {
   Scale (&(pos->x), iXScale);
   Scale (&(pos->y), iYScale);
   Scale (&(pos->xs),iXScale);
   Scale (&(pos->ys),iYScale);
   pos->y = iYSize - pos->y - pos->ys;
   }


/* converts win class to os2 class,
 * dies on error
 */
void CvtClass (char *psz)
   {
   int i = 0;

   while (cla[i].win != NULL)
      {
      if (!stricmp (psz, cla[i].win))
         {
         strcpy (psz, cla[i].os2);
         return;
         }
      i++;
      }
   Error ("No equivalent class found for win class:",psz);
   }







/* converts win style to os2 style
 * changes os2 class if necessary
 * handles all warnings
 * returns TRUE if class has changed
 */
BOOL CvtStyle (char *pszStyle, char *pszOs2Class)
   {
   int   i;
   BOOL  bReturn = FALSE;

   for (i =0; sty[i].winSty != NULL || sty[i].oldCla != NULL; i++)
      {
      /* find area with correct class */
      if (sty[i].oldCla != NULL && stricmp (sty[i].oldCla, pszOs2Class))
         continue;

      /* find matching style */
      if  (!(sty[i].winSty == NULL && *pszStyle == '\0') &&
          (stricmp (sty[i].winSty, pszStyle)))
         continue;

      /* change os/2 class if needed */
      if (sty[i].os2Cla != NULL)
         {
         strcpy (pszOs2Class, sty[i].os2Cla);
         bReturn = TRUE;
         }

      if (sty[i].os2Sty == NULL ||
          args[1].uiCount && sty[i].os2Sty[0] == '\0')   /* -v option */
         {
         printf ("Warning (%s:%d): No OS/2 Style for Win Style: %s, Class: %s\n",
                 szInFile, iLineNo, pszStyle,
                 (pszOs2Class == NULL ? "ANY" : pszOs2Class));
         *pszStyle = '\0';
         }
      else
         {
         strcpy (pszStyle, sty[i].os2Sty);
         }
      return bReturn;
      }
   /* no match, so no change win class or style */
   return bReturn;
   }







void GetStyles (FILE *fp, char *pszClass, char *pszStyles)
   {
   char  szTmp      [STRLEN];
   BOOL  bFirstStyle;
   int   c;
   fpos_t   fpos;
   int   iMaxBackup = 5;

   StripQuotes (pszClass);
   CvtClass (pszClass);
   *pszStyles = *szTmp = '\0';
   bFirstStyle = TRUE;

   fgetpos (fp, &fpos);
    do
      {
      GetStr (fp, szTmp);
      if (CvtStyle (szTmp, pszClass) && iMaxBackup--)
         {
         if (iMaxBackup < 4)
            printf ("Message (%s:%d): Class Change, Now: %s\n",
                       szInFile, iLineNo, pszClass);
         bFirstStyle = TRUE;
         *pszStyles  = '\0';
         fsetpos (fp, &fpos);
         continue;
         }

      if (szTmp[0] != '\0')
         {
         if (!bFirstStyle)
            strcat (pszStyles, " | ");
         strcat (pszStyles, szTmp);
         bFirstStyle = FALSE;
         }
      while ((c = getc (fp)) == ' ')
         ;
      if (c == ',' || c == '\n' || c == EOF)
         {
         ungetc (c, fp);
         break;
         }
      if (c != '|')
         {
         sprintf (szTmp, "(%c)", c);
         Error ("Unexpected token in style list", szTmp);
         }
      }
   while (TRUE);

   /* now see if there are any styles to always add */
   *szTmp = '\0';
   CvtStyle (szTmp, pszClass);
   if (*szTmp == '\0')
      return;
   strcat (pszStyles, (bFirstStyle ? "" : " | "));
   strcat (pszStyles, szTmp);
   }





void AddClass (char *szFile)
   {
   FILE  *fp;
   int   c, i, j, k;
   char  szWin [STRLEN];
   char  szos2 [STRLEN];


   args[8].uiCount = k = 0;
   /* get entry count */
   for (i = 0; cla[i].win != NULL; i++)
      ;

   if ((fp = fopen  (szFile, "r")) == NULL)
      Error ("Cannot open class file:", szFile);
   while (TRUE)
      {
      if (SkipWhite (fp) == EOF)
         break;
      GetStr (fp, szWin);

      if (*szWin == ';')
         {
         while ((c = getc (fp)) != '\n' && c != EOF)
            ;
         continue;
         }

      if (SkipWhite (fp) == EOF)
         break;
      GetStr (fp, szos2);

      cla = realloc (cla, sizeof (CLA) * (++i + 1));
      for (j = i; j > k; j--)
         cla[j] = cla[j-1];
      cla[k].win = strdup (szWin);
      cla[k].os2 = strdup (szos2);
      k++;
      }
   fclose (fp);
   }



void AddStyle (char *szFile)
   {
   FILE  *fp;
   int   c, i, j, k;
   char  szOldCla [STRLEN];
   char  szWinSty [STRLEN];
   char  szOs2Cla [STRLEN];
   char  szOs2Sty [STRLEN];


   args[9].uiCount = k = 0;
   /* get entry count */
   for (i = 0; sty[i].winSty != NULL || sty[i].oldCla != NULL; i++)
      ;

   if ((fp = fopen  (szFile, "r")) == NULL)
      Error ("Cannot open style file:", szFile);
   while (TRUE)
      {
      if (SkipWhite (fp) == EOF)
         break;
      GetStr (fp, szOldCla);

      if (*szOldCla == ';')
         {
         while ((c = getc (fp)) != '\n' && c != EOF)
            ;
         continue;
         }

      if (SkipWhite (fp) == EOF)
         break;
      GetStr (fp, szWinSty);

      if (SkipWhite (fp) == EOF)
         break;
      GetStr (fp, szOs2Cla);

      if (SkipWhite (fp) == EOF)
         break;
      GetStr (fp, szOs2Sty);

      sty = realloc (sty, sizeof (STY) * (++i + 1));
      for (j = i; j > k; j--)
         sty[j] = sty[j-1];
      sty[k].oldCla =  (*szOldCla == '*' ? NULL : strdup (szOldCla));
      sty[k].winSty =  (*szWinSty == '*' ? NULL : strdup (szWinSty));
      sty[k].os2Cla =  (*szOs2Cla == '*' ? NULL : strdup (szOs2Cla));
      if (!stricmp (szOs2Sty, "<warning>"))
         sty[k].os2Sty = NULL;
      else if (!stricmp (szOs2Sty, "<blank>"))
         sty[k].os2Sty = strdup ("");
      else
         sty[k].os2Sty = strdup (szOs2Sty);
      k++;
      }
   fclose (fp);
   }



/**********************************************************************/


/* xlates the tail end of a dlg desc
 */
BOOL XTail (FILE *fpIn, FILE *fpOut)
   {

//    Tail Eaten By XCtl in XBody
//   /*--- Read Windows Tail ---*/
//   Eat (fpIn, "END");

   /*--- Write OS/2 Tail ---*/
   fprintf (fpOut, "   END\nEND\n\n");
   return TRUE;
   }


/* xlates a control line
 * returns FALSE and eats tail once no more control lines 
 */
BOOL XCtl (FILE *fpIn, FILE *fpOut)
   {
   char   szCtlTxt  [STRLEN];
   char   szCtlVal  [STRLEN];
   char   szClass   [STRLEN];
   char   szStyles  [STRLEN];
   char  *psz;
   POS    pos;

   /*--- Read Windows Control Entry ---*/
   if (!Taste (fpIn, "CONTROL"))
      return FALSE;
   GetStr (fpIn, szCtlTxt);           Eat (fpIn, ",");
   /*--- convert accelerator token ---*/
   if ((psz = strchr (szCtlTxt, '&')) != NULL)
      *psz = '~';

   GetStr (fpIn, szCtlVal);           Eat (fpIn, ",");
   GetStr (fpIn, szClass);          Eat (fpIn, ",");
   GetStyles (fpIn, szClass, szStyles);
   if (*szStyles != '\0')
      Eat (fpIn, ",");
   GetPos (fpIn, &pos);
   CvtPos (&pos, iDialogYSize);
   SkipLine (fpIn);

   /*---Write OS/2 Control Entry ---*/
   fprintf (fpOut, "      CONTROL %s, %s, ", szCtlTxt, szCtlVal);
   fprintf (fpOut, "%d, %d, %d, %d, ", pos.x, pos.y, pos.xs, pos.ys);
   fprintf (fpOut, "%s,  %s\n", szClass, szStyles);
   return TRUE;
   }


/* xlates dlg body
 */
BOOL XBody (FILE *fpIn, FILE *fpOut)
   {
   while (XCtl (fpIn, fpOut))
      ;
   return TRUE;
   }


/* xlates dlg head
 */
BOOL XHead (FILE *fpIn, FILE *fpOut)
   {
   POS   pos;
   int   c;
   char  szDlgName [STRLEN];
   char  szStyle   [STRLEN];
   char  szCaption [STRLEN];
   char  szTmp     [STRLEN];


   /*--- Return FALSE if at end of Input File ---*/
   while ((c = SkipWhite (fpIn)) == '\n')
      ;
   if (c == EOF)
      return FALSE;

   /*--- Read Windows Header ---*/
   /*--- Skip any #includes ---*/
   while (TRUE)
      {
      GetStr (fpIn, szDlgName);
      if (stricmp (szDlgName, "#include"))
         break;
      fprintf (fpOut, "#include ");
      while ((c = getc (fpIn)) != '\n' && c != EOF)
         putc (c, fpOut);
      putc ('\n', fpOut);
      }
   Eat (fpIn, "DIALOG");
   Eat (fpIn, "LOADONCALL");
   Eat (fpIn, "MOVEABLE");
   Eat (fpIn, "DISCARDABLE");
   GetPos (fpIn, &pos);
   CvtPos (&pos, 100);
   iDialogYSize = pos.ys;
   pos.y = iDefYPos;
   if (Taste (fpIn, "CAPTION"))
      {
      GetStr (fpIn, szCaption);
      Eat (fpIn, "STYLE");
      }
   else
      {
      strcpy (szCaption, "\"\"");
      }
   GetStyles (fpIn, "dialog", szStyle);
   Eat (fpIn, "BEGIN");
   SkipLine (fpIn);


   /*---Write OS/2 Header ---*/
   fprintf (fpOut, "DLGTEMPLATE %s LOADONCALL MOVEABLE DISCARDABLE\n",
      (args[7].uiCount ? szDlgName : itoa(iDialogID, szTmp, 10)));
   fprintf (fpOut, "BEGIN\n");
   fprintf (fpOut, "   DIALOG %s, %s, %d, %d, %d, %d, %s\n",
            szCaption,
            (args[7].uiCount ? szDlgName : itoa(iDialogID, szTmp, 10)),
            pos.x, pos.y, pos.xs, pos.ys, szStyle);
   fprintf (fpOut, "   BEGIN\n");

   iDialogID++;
   return TRUE;
   }


/* Processes one dialog description 
 * returns false on eof
 */
BOOL ProcessDialog (FILE *fpIn, FILE *fpOut)
   {
   if (!XHead (fpIn, fpOut))     return FALSE;
   if (!XBody (fpIn, fpOut))     return FALSE;
   if (!XTail (fpIn, fpOut))     return FALSE;
   iDlgCount++;
   return TRUE;
   }


void ShowXlate (void)
   {
   int i;

   printf (";Default Class Translations:\n\n");
   printf (";WIN Class      OS/2 Class\n");
   printf (";------------------------------\n");
   for (i = 0; cla[i].win != NULL; i++)
      printf (" %-15s%-15s\n", cla[i].win, cla[i].os2);

   printf ("\n\n;Style Translations:\n\n");
   printf (";OS/2 Class      WIN Style          NEW OS/2 Class  OS/2 Style\n");
   printf (";-----------------------------------------------------------------\n");
   for (i = 0; sty[i].winSty != NULL || sty[i].oldCla != NULL; i++)
      printf (" %-16s%-23s%-12s%-20s\n",
               (sty[i].oldCla == NULL ? "*" : sty[i].oldCla),
               (sty[i].winSty == NULL ? "*" : sty[i].winSty),
               (sty[i].os2Cla == NULL ? "*" : sty[i].os2Cla),
               (sty[i].os2Sty == NULL ? "<warning>" :
                 (sty[i].os2Sty[0] == '\0' ? "<blank>" : sty[i].os2Sty)));
   exit (0);
   }


main (int argc, char *argv[])
   {
   int   uReturn = 0,
         uError, i;
   char  szTmp [STRLEN];
   char  *psz;

   for (i = 0; _cla[i].win != NULL; i++);
   cla = malloc (sizeof (CLA) * (i + 1));
   memcpy (cla, _cla, sizeof (CLA) * (i + 1));

   for (i = 0; _sty[i].winSty != NULL || _sty[i].oldCla != NULL; i++);
   sty = malloc (sizeof (STY) * (i + 1));
   memcpy (sty, _sty, sizeof (STY) * (i + 1));

   szInFile[0] = '\0';
   putchar ('\n');
   if (argc < 2)
      return Msg();

   do
      {
      uReturn += (USHORT) ProcessParams (argv+uReturn+1, args, 11, &uError);
      if (uError)
         Error ("error with parameter :", argv[ uError % 0x0100]);
//         {
//         printf ("Error processing param # %d\n\n", uError % 0x0100);
//         }

      if (args[8].uiCount)                    /*class param */
         AddClass (args[8].pszParam);
      if (args[9].uiCount)                    /*style param */
         AddStyle (args[9].pszParam);
      if (args[0].uiCount)
         ShowXlate ();

      if (argv[uReturn] == NULL)
         Error ("Input File Name required\n", "");

      strcpy (szInFile, argv[uReturn]);
      strcpy (szTmp, szInFile);
      if ((psz = strchr (szTmp, '.')) == NULL)
         strcpy (szOutFile, strcat (szTmp, ".DLG"));
      else if (stricmp (psz, ".DLG"))
         {
         *psz = '\0';
         strcpy (szOutFile, strcat (szTmp, ".DLG"));
         }
      else
         {
         *psz = '\0';
         strcpy (szOutFile, strcat (szTmp, ".OS2"));
         }

      if ((fpIn = fopen  (szInFile, "r")) == NULL)
         Error ("Cannot open input file:", szInFile);
      if ((fpOut = fopen (szOutFile, "w")) == NULL)
         Error ("Cannot open output file:", szOutFile);

      if (args[2].uiCount)                    /*x param */
         iXScale = atoi (args[2].pszParam);
      if (args[3].uiCount)                    /*y param */
         iYScale = atoi (args[3].pszParam);
      if (args[4].uiCount)                    /*pos param */
         iDefYPos = atoi (args[4].pszParam);
      if (args[6].uiCount)                    /*dlgval param */
         iDialogID = atoi (args[6].pszParam);

      iLineNo = 1;
      iFileCount++;
      printf ("\nProcessing WIN:%s -> OS2:%s -x %d -y %d -p %d %s\n",
               szInFile, szOutFile, iXScale, iYScale, iDefYPos,
               (args[1].uiCount ? "-v" : ""));

      if (!args[5].uiCount)
         fprintf (fpOut, "#include <os2.h>\n");
      if (args[10].uiCount)
         fprintf (fpOut, "#include \"%s\"\n", args[10].pszParam);
      while (ProcessDialog (fpIn, fpOut))
         ;
      fclose (fpIn);
      fclose (fpOut);
      }
   while (uReturn + 1 < argc);

   printf ("\n%d Dialog%s Processed in %d File%s\n",
            iDlgCount,  (iDlgCount  == 1 ? "" : "s"),
            iFileCount, (iFileCount == 1 ? "" : "s"));
   printf ("Done.\n");
   return 0;
   }











