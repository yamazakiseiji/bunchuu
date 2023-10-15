/********************************************************************************/
/* Copyright (C) 1992 (2009 - 2011) Renesas Electronics Corporation and         */
/* Renesas Solutions Corp. All Rights Reserved.                                 */
/********************************************************************************/
/***********************************************************************/
/* SPEC;                                                               */
/* NAME = _fmtout : write data with format;                            */
/*        __fmtout :  if _NO_FLOAT defined, not support float          */
/*                                                                     */
/* FUNC = this module do the following functions;                      */
/* (1) convert integer to string                               */
/* (2) convert double to string                                */
/* */
/* CLAS = UNIT;                                                       */
/* */
/* END;                                                                */
/***********************************************************************/

                                                         /* D S01006SP */
#ifdef _FPS
#include <umachine.h>                                    /* A S50005SP */
#endif
#include "lstdio.h"
#define  _IOPROT 1
#include "h2lfun.h"
#include "lstdarg.h"
#include "lstdlib.h"
#include "lctype.h"
#include "lstring.h"
#include "llimits.h"

#include "e_fmtou.h"
#include "e_fmtotb.h"                                 /* A V8.0 LI-064 */
#ifdef _DSPC
#include "e_fmtfix.h"
#endif /*  _DSPC */

/* ferror 関数をサブ関数化 C SH-V.9.04.01 BSH009    */
#ifdef _REENTRANT
int _SUBFUNC(ferror)(FILE *);
#endif /* _REENTRANT    */

#define fmtflgon(status) (p_par2->_fmtflg |=  (status))  /* C S40011SP */
#define fmtflgof(status) (p_par2->_fmtflg &= ~(status))  /* C S40011SP */
#define fmtflgck(status) (p_par2->_fmtflg &   (status))  /* C S40011SP */
#ifdef _DSPC
#define fmtflgcheck(status) ((p_par2->_fmtflg & (status)) == status)
#endif /* _DSPC */

#define FORMERR  1		/* format error flag */
#define LEFTADJ  2		/* conversion spec flag, '-' */
#define ONLYONE  4		/* conversion spec flag, '#' */
#define SIGNPLS  8		/* conversion spec flag, '+' */
#define SIGNSPC 16		/* conversion spec flag, ' ' */
/* not pad char. */
#define GCONVRT 32		/* g or G flag       */



/***********************************************************************/
/* */
/* common area :                                                       */
/* */
/***********************************************************************/
                                                      /* D V8.0 LI-064 */

/*static long _outkind=0L;       *                        V.2.0A1 C T20003SN */
/*static char _fmtflg=(char)0;   * format analize flag            ^          */
/*static char *_str=NULL;        *                                |          */
/*static long _outch=0L;         * output character count         |          */
                                /*                                |          */
/*static char _padchar=(char)0;  * pad character                  |          */
/*static long _width=0L;         * field width                    |          */
/*static long _prec=0L;          *                                |          */
/*static char *_prec_addr0=NULL; * prec '0' insert address        |          */
/*static char *_prec_addr1=NULL; * prec '0' insert address        |          */
/*static char *_prec_addr2=NULL; * prec '0' insert address        |          */
/*static long _prec_ins0=0L;     * prec '0' char count            |          */
/*static long _prec_ins1=0L;     * prec '0' char count            |          */
/*static long _prec_ins2=0L;     * prec '0' char count            |          */
/*static long _with_ins=0L;      * remain width char count        v          */
/*static long _parasize=0L;      * type of parameter      V.2.0A1 C T20003SN */
/*static int (*_sofun)(int, FILE *); /* A Svv00 SP */

/*#if (defined _SH5M) || (defined _SH5C)						/* A S70004 P */ /* D V80#1.05 */
static void		_lltostr(long long int,char *,int, _FMTOTBL2 *);/* A S70004 P */
/*#endif 														/* A S70004 P */ /* D V80#1.05 */
static void     _ltostr(long, char *, int, _FMTOTBL2 *);       /* C S40011SP */
#ifndef _NO_FLOAT                                        /* C S70001SP */
#ifdef _FPS
extern void     _xtostr(char **, char *, int, _FMTOTBL2 *);    /* C S50017SP */
#else
static void     _xtostr(LDBL, char *, int, _FMTOTBL2 *);
                                   /* ^^^ C S01001SP  C S40001SP  C S40011SP */
static void     _round(char *, long);
#endif
#endif                                                       /* A S70001SP */
static void     _putstr(char *, long, _FMTOTBL1 *);            /* C S40011SP */
static void     _putnull(_FMTOTBL1 *);                         /* C S40011SP */
static int      _puterr(_FMTOTBL1 *);                          /* C S40011SP */
#ifdef _DSPC
static void     _fixedtostr(int*, char*, int, _FMTOTBL2*, _FIX_2TO10_DATA*);
#endif /* _DSPC */


/***********************************************************************/
/* */
/* main routine :                                                      */
/* */
/***********************************************************************/
int _fmtout(outkind, ofun, str, format, arg) /* C Svv00 SP */
    int             outkind;
    int (*ofun)(int, FILE *); /* A Svv00 SP */
    FILE           *str;
    const char     *format;
    char           *arg;
{
    _FMTOTBL1       _par1;                               /* A S40011SP */
    _FMTOTBL1      *p_par1 = &_par1;                     /* A S40011SP */
    _FMTOTBL2       _par2;                               /* A S40011SP */
    _FMTOTBL2      *p_par2 = &_par2;                     /* A S40011SP */

/*#if (defined _SH5M) || (defined _SH5C)				 /* A S70004 P */ /* D V80#1.05 */
	long long int   livalue;							 /* A S70004 P */
/*#endif 												 /* A S70004 P */ /* D V80#1.05 */
    long int        ivalue;
#ifndef _NO_FLOAT                                       /* A S70001SP */
    LDBL            xvalue;                  /* C S01001SP  C S40001SP */
#endif                                                  /* A S70001SP */
 	long int        length, i, j;
    char            tbuf[128];
    char           *cp;
    char           *objp;
    char           *p;
    char            prec_char;
    long            w;
    int             dataBuffer[2];
    char            padchar;                             /* A V8.0 LI-059 */

    p_par1 -> _outkind    = 0L;                          /* A S40011SP */
    p_par1 -> _str        = NULL;                        /* A S40011SP */
    p_par1 -> _outch      = 0L;                          /* A S40011SP */
    p_par2 -> _fmtflg     = (char)0;                     /* A S40011SP */
    p_par2 -> _padchar    = (char)0;                     /* A S40011SP */
    p_par2 -> _dummy      = (short)0;                    /* A S40011SP */
    p_par2 -> _width      = 0L;                          /* A S40011SP */
    p_par2 -> _prec       = 0L;                          /* A S40011SP */
    p_par2 -> _prec_addr0 = NULL;                        /* A S40011SP */
    p_par2 -> _prec_addr1 = NULL;                        /* A S40011SP */
    p_par2 -> _prec_addr2 = NULL;                        /* A S40011SP */
    p_par2 -> _prec_ins0  = 0L;                          /* A S40011SP */
    p_par2 -> _prec_ins1  = 0L;                          /* A S40011SP */
    p_par2 -> _prec_ins2  = 0L;                          /* A S40011SP */
    p_par2 -> _with_ins   = 0L;                          /* A S40011SP */

    if (outkind == 0) {
	if (str == 0) {
	    _errno = PTRERR;
	    return (EOF);
	}
	if (flgck1(((FILE *) str), (_IORW | _IOREAD | _IOWRITE)) == 0) {
	    _errno = NOTOPN;
	    return (EOF);
	}
	if (flgck1(((FILE *) str), _IORW) == 0 &&
	    flgck1(((FILE *) str), _IOREAD) != 0) {
	    _errno = EBADF;
	    return (EOF);
	}
        p_par1->_sofun=ofun;               /* A Svv00 SP   C S40011SP */
    }
    p_par1 -> _outkind = outkind;                       /* C S40011SP */
    p_par1 -> _str = (char *) str;                      /* C S40011SP */
    p_par1 -> _outch = 0;                               /* C S40011SP */
    p_par2 -> _fmtflg = (char) 0;                       /* C S40011SP */
    prec_char = (char) '0';

    if (_puterr(p_par1) != 0)                           /* C S40011SP */
	return (EOF);
    for (; *format != '\0' && _puterr(p_par1) == 0 && fmtflgck(FORMERR) == 0;){
                                                    /* ^^^ C S40011SP */
	if (*format == '%') {
	    format++;
	    p_par2 -> _fmtflg = (char) 0;/* all flag reset C S40011SP */
	    p_par2 -> _prec_addr0 = tbuf;               /* C S40011SP */
	    p_par2 -> _prec_addr1 = tbuf;               /* C S40011SP */
	    p_par2 -> _prec_addr2 = 0;                  /* C S40011SP */
	    p_par2 -> _prec_ins0  = 0;                  /* C S40011SP */
	    p_par2 -> _prec_ins1  = 0;                  /* C S40011SP */
	    p_par2 -> _prec_ins2  = 0;                  /* C S40011SP */
	    p_par2 -> _with_ins   = 0;                  /* C S40011SP */
	    for (; *format == '-' || *format == '+' ||
		 *format == ' ' || *format == '#';) {
		switch (*format) {
		case '-':
		    fmtflgon(LEFTADJ);
		    break;
		case '+':
		    fmtflgon(SIGNPLS);
		    break;
		case ' ':
		    if (fmtflgck(SIGNPLS) == 0)
			fmtflgon(SIGNSPC);
		    break;
		case '#':
		    fmtflgon(ONLYONE);
		    break;
		}
		format++;
	    }

	    p_par2 -> _padchar = (char) ' ';                   /* C S40011SP */
	    if (*format == '0') {
		p_par2 -> _padchar = (char) '0';               /* C S40011SP */
		format++;
	    }
	    p_par2 -> _width = 0;                              /* C S40011SP */
	    if (*format == '*') {
		p_par2 -> _width = va_arg(arg, int);           /* C S40011SP */
		if (p_par2 ->_width < 0) {                     /* C S40011SP */
		    fmtflgon(LEFTADJ);
		    p_par2 -> _width = -(p_par2 -> _width);    /* C S40011SP */
		}
		if (p_par2 -> _width > 512) {                  /* C S40011SP */
		    fmtflgon(FORMERR);
		    _errno = ECSPEC;
		}
		format++;
	    }
	    if (isdigit(*format) != 0) {
		p_par2 -> _width = 0;                          /* C S40011SP */
		for (; isdigit(*format) != 0;) {
		    w = (*format) - '0';
		    if (((512 - w) / 10) >=p_par2 -> _width)   /* C S40011SP */
			p_par2->_width = p_par2->_width*10+w;  /* C S40011SP */
		    else {
			fmtflgon(FORMERR);
			_errno = ECSPEC;
		    }
		    format++;
		}
	    }
	    p_par2 -> _prec = -1;                              /* C S40011SP */
	    if (*format == '.') {
		format++;
		if (*format == '*') {
		    p_par2 -> _prec = va_arg(arg, int);        /* C S40011SP */
		    if (p_par2 -> _prec < 0)                   /* C S40011SP */
			p_par2 -> _prec = -1;                  /* C S40011SP */
		    if (p_par2 -> _prec > 512) {               /* C S40011SP */
			fmtflgon(FORMERR);
			_errno = ECSPEC;
		    }
		    format++;
		}
		if (isdigit(*format) != 0) {
		    p_par2 -> _prec = 0;                       /* C S40011SP */
		    for (; isdigit(*format) != 0;) {
			w = (*format) - '0';
			if (((512 - w)/10) >= p_par2 -> _prec) /* C S40011SP */
			    p_par2->_prec = p_par2->_prec*10+w;/* C S40011SP */
			else {
			    fmtflgon(FORMERR);
			    _errno = ECSPEC;
			}
			format++;
		    }
		}
	    }
	    p_par2 -> _parasize = ' ';                         /* C S40011SP */
	    if (*format == 'h' || *format == 'l' || *format == 'L') {
		p_par2 -> _parasize = *format;                 /* C S40011SP */
		format++;
	    }
/*#if (defined _SH5M) || (defined _SH5C)				 /* A S70004 P   */ /* D V80#1.05 */
		p_par2 -> _parasize1 = ' ';						 /* A S70004 P   */
		if (*(format-1) == 'l'){						 /* A S70004 P   */
			if (*(format) == 'l'){						 /* A S70004 P   */
			  p_par2 -> _parasize1 = *format;			 /* A S70004 P   */
				format++;							     /* A S70004 P   */
			}											 /* A S70004 P   */
		}												 /* A S70004 P   */
	    switch (*format) {								 /* A S70004 P   */
	    case 'd':										 /* A S70004 P   */
	    case 'i':										 /* A S70004 P   */
	    case 'o':										 /* A S70004 P   */
	    case 'u':										 /* A S70004 P   */
	    case 'x':										 /* A S70004 P   */
	    case 'X':										 /* A S70004 P   */
	    if (p_par2 -> _parasize == 'l'){				 /* A S70004 P   */
	      if (p_par2 -> _parasize1 == 'l') 				 /* A S70004 P   */
		    livalue = va_arg(arg,long long int);		 /* A S70004 P   */
		  else  if (*format == 'u' || *format == 'X' ||	 /* A S70004 P   */
			*format == 'x' || *format == 'o')  			 /* A S70004 P   */
			livalue = (unsigned long int)				 /* A S70004 P   */
			    va_arg(arg, long int);			    	 /* A S70004 P   */
			else 										 /* A S70004 P   */
		    livalue = (long )va_arg(arg, long int);      /* A S70004 P   */
		   											     /* A S70004 P   */
		}												 /* A S70004 P   */
		else if (p_par2 -> _parasize == 'h'){			 /* A S70004 P   */
		    if (*format == 'u' || *format == 'X' ||		 /* A S70004 P   */
			*format == 'x' || *format == 'o')			 /* A S70004 P   */
			livalue = (unsigned short int)				 /* A S70004 P   */
			    va_arg(arg, int);						 /* A S70004 P   */
			else										 /* A S70004 P   */
		    livalue = (short int) va_arg(arg, int);		 /* A S70004 P   */
		}												 /* A S70004 P   */
		else if (*format == 'u' || *format == 'X' || 	 /* A S70004 P   */
			*format == 'x' || *format == 'o')			 /* A S70004 P   */
			livalue = (unsigned int)					 /* A S70004 P   */
			    va_arg(arg, int);						 /* A S70004 P   */
			else										 /* A S70004 P   */
		    livalue = (int) va_arg(arg, int);			 /* A S70004 P   */
														 /* A S70004 P   */
		/* 該当形式では精度が指定されていると"0"フラグは無効	A SH-B094060	*/
		if (p_par2 -> _prec == -1) {
			/* 精度指定無しの処理	*/
		    p_par2 -> _prec = 1; 						 /* A S70004 P   */
		} else {
			/* 精度指定有りの処理	*/
	    	if (p_par2 -> _padchar == '0') {
				p_par2 -> _padchar = (char) ' ';
			}
		}
		_lltostr(livalue, tbuf, (int) *format, p_par2);  /* A S70004 P   */
		cp = tbuf;										 /* A S70004 P   */
		length = strlen(cp);							 /* A S70004 P   */
		break;											 /* A S70004 P   */
														 /* A S70004 P   */
/* D V80#1.05 */
/* ^ #else												 /* A S70004 P   */
/* | 	    switch (*format) {
/* | 	    case 'd':
/* | 	    case 'i':
/* | 	    case 'o':
/* | 	    case 'u':
/* | 	    case 'x':
/* | 	    case 'X':
/* | 		if (p_par2 -> _parasize == 'l')                /* C S40011SP */
/* | 		    ivalue = va_arg(arg, long int);
/* | 		else if (p_par2 -> _parasize == 'h')           /* C S40011SP */
/* | 		    if (*format == 'u' || *format == 'X' ||
/* | 			*format == 'x' || *format == 'o')
/* | 			ivalue = (unsigned short int)
/* | 			    va_arg(arg, int);
/* | 		    else
/* | 			ivalue = (short int) va_arg(arg, int);
/* | 		else if (*format == 'u' || *format == 'X' ||
/* | 			 *format == 'x' || *format == 'o')
/* | 		    ivalue = (unsigned int) va_arg(arg, int);
/* | 		else
/* | 		    ivalue = va_arg(arg, int);
/* | 
/* | 		if (p_par2 -> _prec == -1)                     /* C S40011SP */
/* | 		    p_par2 -> _prec = 1;                       /* C S40011SP */
/* | 		_ltostr(ivalue, tbuf, (int) *format, p_par2);  /* C S40011SP */
/* | 		cp = tbuf;
/* | 		length = strlen(cp);
/* | 		break;
/* v #endif 													/* A S70004 P */
#ifndef _NO_FLOAT                                       /* ACS70001SP */
	    case 'g':
	    case 'G':
            if (p_par2->_prec == 0) { p_par2->_prec=1; }        /* A S93B199P */
	    case 'f':
	    case 'e':
	    case 'E':
#ifdef _FPS
		_xtostr(&arg, tbuf, (int) *format, p_par2);    /* C S50017SP */
                ivalue = get_fpscr();                          /* A S50005SP */
                ivalue &= 0xfff7ffff;              /* PR bit OFF  A S50005SP */
                set_fpscr(ivalue);                             /* A S50005SP */
	        cp = tbuf;
		length = strlen(cp);
		break;

#else
                if (p_par2 -> _parasize == 'L')    /* C S01001SP  C S40001SP */
                                                           /* ^^^ C S40011SP */
		    xvalue = va_arg(arg, LDBL);                /* C S40001SP */
                else                                           /* C S40001SP */
		    xvalue = (LDBL) va_arg(arg, double);       /* C S40001SP */
		if (p_par2 -> _prec == -1)                     /* C S40011SP */
		    p_par2 -> _prec = 6;                       /* C S40011SP */
		_xtostr(xvalue, tbuf, (int) *format, p_par2);
                                     /* ^^^ C S01001SP C S40001SP C S40011SP */
	        cp = tbuf;
		length = strlen(cp);
		break;
#endif
#endif                                     /* A S70001SP */

#ifdef _DSPC
        case 'r':                                       /* __fixed */
            if (p_par2->_prec == -1) {
                p_par2->_prec = 6;
            }
            dataBuffer[0] = 0;
            dataBuffer[1] = va_arg(arg, int);
            if (p_par2->_parasize == 'l') {
                _fixedtostr(dataBuffer, tbuf, (int)*format, p_par2,
                            &long_fix_2to10_data);
            }
            else {
                _fixedtostr(dataBuffer, tbuf, (int)*format, p_par2,
                            &fix_2to10_data);
            }
            cp = tbuf;
            length = strlen(cp);
            break;
        case 'a':                                       /* __acuum */
            if (p_par2->_prec == -1) {
                p_par2->_prec = 6;
            }
            if (p_par2->_parasize == 'l') {
                /* dataBuffer[0]=HIGH、dataBuffer[1]=LOW */
#ifdef _LIT
                dataBuffer[1] = va_arg(arg, int);
                dataBuffer[0] = va_arg(arg, int);
#else
                dataBuffer[0] = va_arg(arg, int);
                dataBuffer[1] = va_arg(arg, int);
#endif
                if (dataBuffer[0] & 0x00000080) {
                    dataBuffer[0] |= 0xffffff00;    /* extend sign */
                }
                _fixedtostr(dataBuffer, tbuf, (int)*format, p_par2,
                            &long_fix_2to10_data);
            }
            else {
                dataBuffer[1] = va_arg(arg, int);
                if (dataBuffer[1] & 0x00800000) {
                    dataBuffer[1] |= 0xff000000;    /* extend sign */
                    dataBuffer[0] = 0xffffffff;
                } else {
                    dataBuffer[0] = 0;
                }
                _fixedtostr(dataBuffer, tbuf, (int)*format, p_par2,
                            &fix_2to10_data);
            }
            cp = tbuf;
            length = strlen(cp);
            break;
#endif /* _DSPC */

	    case 'c':
		tbuf[0] = (char) va_arg(arg, int);
		cp = tbuf;
		length = 1;
		p_par2 -> _prec_addr1 = tbuf;                  /* C S40011SP */
		p_par2 -> _prec_ins1 = 0;                      /* C S40011SP */
		p_par2 -> _with_ins = p_par2->_width - length; /* C S40011SP */
		break;

	    case 's':
		cp = va_arg(arg, char *);
		length = strlen(cp);
		if (p_par2 -> _prec != -1 && p_par2 -> _prec < length)
                                                           /* ^^^ C S40011SP */
		    length = p_par2 -> _prec;                  /* C S40011SP */
		p_par2 -> _prec_addr1 = tbuf;                  /* C S40011SP */
		p_par2 -> _prec_ins1 = 0;                      /* C S40011SP */
		p_par2 -> _with_ins = p_par2->_width - length; /* C S40011SP */
		break;

#ifdef _DSPC
	    case 'P':
#endif
	    case 'p':
		ivalue = (int) va_arg(arg, char *);
		if (p_par2->_prec == -1)                       /* C S40011SP */
		    p_par2->_prec = 1;                         /* C S40011SP */
		_ltostr(ivalue, tbuf, 'x', p_par2);            /* C S40011SP */
		cp = tbuf;
		length = strlen(cp);
		break;

	    case 'n':
		objp = va_arg(arg, char *);
		*((int *) objp) = p_par1->_outch;              /* C S40011SP */
		break;

	    case '%':
		tbuf[0] = (char) '%';
		cp = tbuf;
		length = 1;
		p_par2->_prec_addr1 = tbuf;                    /* C S40011SP */
		p_par2->_prec_ins1 = 0;                        /* C S40011SP */
		p_par2->_with_ins = p_par2->_width - length;   /* C S40011SP */
		break;
	    default:
		_errno = ECSPEC;
		fmtflgon(FORMERR);

	    }
	    if (*format != 'n' && fmtflgck(FORMERR) == 0) {
		if (fmtflgck(LEFTADJ) == 0 && p_par2->_with_ins > 0) {
							   /* ^^^ C S40011SP */
                                                              /* A S90R453 P */
            if ((fmtflgck(ONLYONE) != 0) && (length > 2) && (*cp == '0') &&
                ((*format == 'x') || (*format == 'X')) &&
                (p_par2->_padchar == '0')) {
                if ((*(cp + 1) == 'x') || (*(cp + 1) == 'X')) {
                    _putstr(cp, 2, p_par1);
                    length -= 2;
                    cp += 2;
                }
            }                                                 /* A S90R453 P */

		    for (; p_par2->_with_ins > 0;) {           /* C S40011SP */
			_putstr(&p_par2->_padchar, 1L, p_par1);/* C S40011SP */
			p_par2->_with_ins--;                   /* C S40011SP */
		    }
		}
		if (p_par2->_prec_ins0 == 0 && p_par2->_prec_ins1 == 0 &&
		    p_par2->_prec_ins2 == 0) {          /* << ^^^ C S40011SP */
		    _putstr(cp, length, p_par1);               /* C S40011SP */
		} else {
		    i = p_par2 -> _prec_addr0 - cp;            /* C S40011SP */
		    j = i;
		    if (i > 0) {
			_putstr(cp, i, p_par1);                /* C S40011SP */
		    }
		    for (; p_par2->_prec_ins0 > 0; p_par2->_prec_ins0--) {
                                                           /* ^^^ C S40011SP */
			_putstr(&prec_char, 1L, p_par1);       /* C S40011SP */
		    }
		    i = p_par2->_prec_addr1-p_par2->_prec_addr0;/*C S40011SP */
		    j = j + i;
		    if (i > 0) {
			_putstr(p_par2->_prec_addr0,i,p_par1); /* C S40011SP */
		    }
		    for (; p_par2 -> _prec_ins1 > 0; p_par2 -> _prec_ins1--) {
                                                           /* ^^^ C S40011SP */
			_putstr(&prec_char, 1L, p_par1);       /* C S40011SP */
		    }
		    _putstr(p_par2 -> _prec_addr1, length-j, p_par1);
                                                           /* ^^^ C S40011SP */
		    for (; p_par2 -> _prec_ins2 > 0; p_par2 -> _prec_ins2--) {
                                                           /* ^^^ C S40011SP */
			_putstr(&prec_char, 1L, p_par1);       /* C S40011SP */
		    }
		}
		if (fmtflgck(LEFTADJ) != 0 && p_par2 -> _with_ins > 0) {
                                                           /* ^^^ C S40011SP */
		    if (p_par2->_padchar=='0')                            /* A V8.0 LI-059 */
		        padchar = ' ';                                    /* A V8.0 LI-059 */
		    else                                                  /* A V8.0 LI-059 */
		        padchar = p_par2->_padchar;                       /* A V8.0 LI-059 */
		    for (; p_par2 -> _with_ins > 0;) {         /* C S40011SP */
			_putstr(&padchar,1L,p_par1);           /* C S40011SP,C V8.0 LI-059 */
			p_par2 -> _with_ins--;                 /* C S40011SP */
		    }
		}
	    }
	    format++;
	} else {
	    p = format;
	    for (; *p != '\0' && *p != '%';)
		p++;
	    _putstr(format, (long) (p - format), p_par1);      /* C S40011SP */
	    format = p;
	}
    }
    _putnull(p_par1);                                          /* C S40011SP */
    return ((int)p_par1->_outch);                              /* C S40011SP */
}
/*#if (defined _SH5M) || (defined _SH5C)				 /* A S70004 P */ /* D V80#1.05 */
/***********************************************************A S70004 P */
/* 															A S70004 P */
/* _lltostr subroutine :                                    A S70004 P */
/* 															A S70004 P */
/***********************************************************A S70004 P */
static void												 /* A S70004 P */
_lltostr(x, buf, spec, p_par2)                           /* A S70004 P */
    long long int   x;		/* convert value */			 /* A S70004 P */
    char           *buf;	/* character set buffer */
    int             spec;	/* conversion spec. d/i/o/u/x/X  */
    _FMTOTBL2      *p_par2;
{
    unsigned long long int wx;							 /* A S70004 P */
    char            wbuf[24];	/* work area for edit */ /* C V80#1.05 */
    char           *buf_top;	/* top address of 'buf' */
    long            base;
    long            i;
    long            char_len;	/* all char length */
    long            wbuf_len;	/* numeric char len, after convert */

    buf_top = buf;
    *buf_top = (char) 0;
    switch (spec) {
    case 'd':
    case 'i':
    case 'u':
	base = 10;
	break;

    case 'o':
	base = 8;
	break;

    case 'x':
    case 'X':
	base = 16;
	break;

    default:
	break;
    }

    if (spec == 'd' || spec == 'i') {
	if (x >= 0)
	    wx = x;
	else
	    wx = -x;
    } else
	wx = (unsigned long long int) x;					/* C S70004 P */

    i = 0;
    char_len = 0;

    if (wx == 0 && p_par2->_prec != 0) {/* P-5C-375  C S40011SP */
	    wbuf[0] = (char) '0';	/* P-5C-375 */
	    i = 1;			/* P-5C-375 */
    }
    /* P-5C-375 */
    else {
	for (; wx > 0; i++) {
	    wbuf[i] = (char) (wx % base);
	    if (wbuf[i] <= 9)
		wbuf[i] += '0';
	    else if (spec == 'x')
		wbuf[i] += ('a' - 10);
	    else
		wbuf[i] += ('A' - 10);
	    wx = wx / base;
	}
    }

    if (fmtflgck(ONLYONE) != 0) {
	switch (spec) {
	case 'o':
	    if (x != 0) {
		wbuf[i++] = (char) '0';
	    }
	    break;
	case 'x':
	case 'X':
	    if (x != 0) {	/* P-5C-375 */
		*buf++ = (char) '0';
		*buf++ = (char) spec;	/* 'x' or 'X' */
		char_len = 2;
	    }
	    break;
	default:
	    break;
	}
    }
	/* char_lenに数値の文字数を含めないため削除	D SH-B094060	*/
    wbuf_len = i;

    if ((spec == 'd' || spec == 'i') && (p_par2->_prec != 0 || x != 0)) {
	if (x >= 0) {                 /* ^^^^^^ C S40011SP */
	    if (fmtflgck(SIGNPLS) != 0) {
		char_len++;
		*buf++ = (char) '+';
	    } else {
		if (fmtflgck(SIGNSPC) != 0) {
		    char_len++;
		    *buf++ = (char) ' ';
		}
	    }
	} else {
	    char_len++;
	    *buf++ = (char) '-';
	}
    }
    if (*buf_top == '+' || *buf_top == '-' || *buf_top == ' ') {
	p_par2->_prec_addr1 = buf_top + 1;                     /* C S40011SP */
    } else {
	if (fmtflgck(ONLYONE) != 0) {
	    switch (spec) {
	    case 'X':
	    case 'x':
		p_par2->_prec_addr1 = buf_top + 2;             /* C S40011SP */
		break;
	    case 'o':
		p_par2->_prec_addr1 = buf_top + 1;             /* C S40011SP */
		break;
	    default:
		break;
	    }
	} else {
	    p_par2->_prec_addr1 = buf_top;                     /* C S40011SP */
	}
    }

	/* 数値の文字数のみで精度との判定を実行するように変更	C SH-B094060	*/
	if (p_par2->_prec > wbuf_len) {                            /* C S40011SP */
		i = p_par2->_prec;                                     /* C S40011SP */
		p_par2->_prec_ins1 = p_par2->_prec - wbuf_len;         /* C S40011SP */
	} else {
		i = wbuf_len;
		p_par2->_prec_ins1 = 0;                                /* C S40011SP */
	}

	/* 精度判定後に符号と16進接頭語の文字数を加算する	A SH-B094060	*/
	i += char_len;

    if ((*buf_top == '+' || *buf_top == '-' || *buf_top == ' ') &&
	p_par2->_width > i && p_par2->_padchar == '0' &&       /* C S40011SP, C V8.0 LI-066 */
	fmtflgck(LEFTADJ) == 0) {                                          /* A V8.0 LI-066 */
	p_par2->_prec_ins1 += (p_par2->_width - i);            /* C S40011SP */
	p_par2->_with_ins = 0;                                 /* C S40011SP */
    } else {
	if (p_par2->_width > i) {                              /* C S40011SP */
	    p_par2->_with_ins = p_par2->_width - i;            /* C S40011SP */
	} else {
	    p_par2->_with_ins = 0;                             /* C S40011SP */
	}
    }

    for (i = wbuf_len - 1; i >= 0;)
	*buf++ = wbuf[i--];

    *buf++ = (char) '\0';

}
/*#endif												 /* A S70004 P   */ /* D V80#1.05 */

/***********************************************************************/
/* */
/* _ltostr subroutine :                                                */
/* */
/***********************************************************************/
static void
_ltostr(x, buf, spec, p_par2)                            /* C S40011SP */
    long int        x;		/* convert value */
    char           *buf;	/* character set buffer */
    int             spec;	/* conversion spec. d/i/o/u/x/X */
    _FMTOTBL2      *p_par2;                              /* A S40011SP */
{
    unsigned long int wx;
    char            wbuf[12];	/* work area for edit */
    char           *buf_top;	/* top address of 'buf' */
    long            base;
    long            i;
    long            char_len;	/* all char length */
    long            wbuf_len;	/* numeric char len, after convert */

    buf_top = buf;
    *buf_top = (char) 0;
    switch (spec) {
    case 'd':
    case 'i':
    case 'u':
	base = 10;
	break;

    case 'o':
	base = 8;
	break;

    case 'x':
    case 'X':
	base = 16;
	break;

    default:
	break;
    }

    if (spec == 'd' || spec == 'i') {
	if (x >= 0)
	    wx = x;
	else
	    wx = -x;
    } else
	wx = (unsigned long int) x;

    i = 0;
    char_len = 0;

    if (wx == 0 && p_par2->_prec != 0) {/* P-5C-375  C S40011SP */
	    wbuf[0] = (char) '0';	/* P-5C-375 */
	    i = 1;			/* P-5C-375 */
    }
    /* P-5C-375 */
    else {
	for (; wx > 0; i++) {
	    wbuf[i] = (char) (wx % base);
	    if (wbuf[i] <= 9)
		wbuf[i] += '0';
	    else if (spec == 'x')
		wbuf[i] += ('a' - 10);
	    else
		wbuf[i] += ('A' - 10);
	    wx = wx / base;
	}
    }

    if (fmtflgck(ONLYONE) != 0) {
	switch (spec) {
	case 'o':
	    if (x != 0) {
		wbuf[i++] = (char) '0';
	    }
	    break;
	case 'x':
	case 'X':
	    if (x != 0) {	/* P-5C-375 */
		*buf++ = (char) '0';
		*buf++ = (char) spec;	/* 'x' or 'X' */
		char_len = 2;
	    }
	    break;
	default:
	    break;
	}
    }
    char_len = char_len + i;
    wbuf_len = i;

    if ((spec == 'd' || spec == 'i') && (p_par2->_prec != 0 || x != 0)) {
	if (x >= 0) {                 /* ^^^^^^ C S40011SP */
	    if (fmtflgck(SIGNPLS) != 0) {
		char_len++;
		*buf++ = (char) '+';
	    } else {
		if (fmtflgck(SIGNSPC) != 0) {
		    char_len++;
		    *buf++ = (char) ' ';
		}
	    }
	} else {
	    char_len++;
	    *buf++ = (char) '-';
	}
    }
    if (*buf_top == '+' || *buf_top == '-' || *buf_top == ' ') {
	p_par2->_prec_addr1 = buf_top + 1;                     /* C S40011SP */
    } else {
	if (fmtflgck(ONLYONE) != 0) {
	    switch (spec) {
	    case 'X':
	    case 'x':
		p_par2->_prec_addr1 = buf_top + 2;             /* C S40011SP */
		break;
	    case 'o':
		p_par2->_prec_addr1 = buf_top + 1;             /* C S40011SP */
		break;
	    default:
		break;
	    }
	} else {
	    p_par2->_prec_addr1 = buf_top;                     /* C S40011SP */
	}
    }

    if (p_par2->_prec > char_len) {                            /* C S40011SP */
	i = p_par2->_prec;                                     /* C S40011SP */
	p_par2->_prec_ins1 = p_par2->_prec - char_len;         /* C S40011SP */
    } else {
	i = char_len;
	p_par2->_prec_ins1 = 0;                                /* C S40011SP */
    }

    if ((*buf_top == '+' || *buf_top == '-' || *buf_top == ' ') &&
	p_par2->_width > i && p_par2->_padchar == '0' &&       /* C S40011SP, C V8.0 LI-066 */
	fmtflgck(LEFTADJ) == 0) {                                          /* A V8.0 LI-066 */
	p_par2->_prec_ins1 += (p_par2->_width - i);            /* C S40011SP */
	p_par2->_with_ins = 0;                                 /* C S40011SP */
    } else {
	if (p_par2->_width > i) {                              /* C S40011SP */
	    p_par2->_with_ins = p_par2->_width - i;            /* C S40011SP */
	} else {
	    p_par2->_with_ins = 0;                             /* C S40011SP */
	}
    }

    for (i = wbuf_len - 1; i >= 0;)
	*buf++ = wbuf[i--];

    *buf++ = (char) '\0';

}





#ifndef _NO_FLOAT                                       /* C S70001SP */
#ifndef _FPS                                                   /* A S50005SP */
/************************************************************* C S01001SP ^ */
/*                                                             C S01001SP | */
/* _xtostr subroutine :                                     C S40001SP */
/*                                                                     */
/***********************************************************************/
static void _xtostr(x,buf,spec,p_par2)       /* C S40001SP  C S40011SP */
LDBL  x;                                                 /* C S40001SP */
char *buf;
int  spec;
_FMTOTBL2 *p_par2;                                       /* C S40011SP */
{
    char wbuf[8];
    char sbuf[30],*sp,*buf_top;         /* decimal string */
    long exp;
    char sign;
    char w;
    long i,j,k,l,m;
    long slen;
    long slen_wrk;                                              /* A S93B199P */
    int  cnv_rc;

    // bufの先頭アドレスを保存
    buf_top = buf;
    // sbuf[0]:最上位桁の桁上がり用
    sbuf[0]=(char)'0';

    // 数値を文字列に変換してsbufに格納
    if (x!=0.0){                        /* if x is (double) 0.0 ? */
        cnv_rc=_xti(x,(INT)17,wbuf,&exp,&sign);  /* convert LDBL  C S40001SP */
        if (cnv_rc!=0){
            *buf=0;
            if (p_par2->_prec > p_par2->_width)                /* C S40011SP */
            p_par2->_with_ins = p_par2->_prec;                 /* C S40011SP */
            else
                p_par2->_with_ins = p_par2->_width;            /* C S40011SP */
            switch(cnv_rc){
                case  1: p_par2->_padchar='+';                 /* C S40011SP */
                         break;
                case -1: p_par2->_padchar='-';                 /* C S40011SP */
                         break;
                default: p_par2->_padchar='*';                 /* C S40011SP */
                         break;
            }
            return;
        }
        _its(wbuf,sbuf+1,(INT)17);
    }
    else{
        sign=(char)1;
        sbuf[1]=(char)0;
    }

    // 後続の不要な0を削除(この時点では前方の0)
    for (i=1; sbuf[i]=='0'; i++);
    if (i>1){                         /* after zero delete */
        k=strlen(sbuf+i);
        exp+=(i-1);
        for (j=1; j<=k; j++,i++){
            sbuf[j]=sbuf[i];
        }
        sbuf[j]=0;
    }

    slen = strlen(sbuf+1);

    // 文字列の前後入れ替え
    sp=sbuf+1;
    for (i=0;i<slen/2;i++){
        w = *(sp+i);
        *(sp+i)=*(sp+slen-1-i);
        *(sp+slen-1-i) = w;
    }

    // 数値0.0の設定
    if (slen==0){
        sbuf[1] = (char)'0';
        slen = 1;
        exp = 0;
    }

    // 'g','G'指定時の表示方法切り替え
    if (spec=='g' || spec=='G'){
        fmtflgon(GCONVRT);
        //　丸め処理と桁上がりした時の桁数補正                                
        if (p_par2->_prec < slen) { _round(sbuf, (long)(p_par2->_prec + 1)); }  /* A S93B199P */
        if (sbuf[0]=='0') { slen_wrk = slen; }                                  /* A S93B199P */
        else              { slen_wrk = slen + 1; }                              /* A S93B199P */
        // spec書き換え  指数が5以下、または精度以上で'e'変換  それ以外'f'変換
        if ((exp+slen_wrk-1)<=-5 || (exp+slen_wrk-1)>=p_par2->_prec)            /* C S93B199P */
          spec -= 2;
        else
          spec = (char)'f';
    }

    // bufに符号をセット
    if (sign==1){
        w = (char)fmtflgck(SIGNPLS | SIGNSPC);
        switch (w){
            case  SIGNPLS :
                      *buf++ = (char)'+';
                      k=1;                                  /* A V8.0 LI-056 */
                      break;
            case  SIGNSPC :
                      *buf++ = (char)' ';
                      k=1;                                  /* A V8.0 LI-056 */
                      break;
            default :
                      k=0;                                  /* A V8.0 LI-056 */
                      break;

        }
    }
    else {                                                  /* A V8.0 LI-056 */
      *buf++ = (char)'-';
      k=1;                                                  /* A V8.0 LI-056 */
    }                                                       /* A V8.0 LI-056 */

    // 'f'指定時の表示設定
    if (spec=='f'){
       // 整数部.0
       if (exp>=0){
           // 全てbufへコピー
           for (i=0;i<slen;i++){
               *buf++ = sbuf[i+1];
               // 'g'指定時の精度減算
               if (fmtflgck(GCONVRT)!=0) { p_par2->_prec--; }   /* A S93B199P */
           }

/*         k=0;                                             ** D V8.0 LI-056 */

           // 有効桁以外の整数部を後方へ0埋め
           p_par2->_prec_addr1=buf;                            /* C S40011SP */
           p_par2->_prec_ins1=exp;                             /* C S40011SP */
           // 'g'指定時の精度減算
           if (fmtflgck(GCONVRT)!=0) { p_par2->_prec -= exp; }  /* A S93B199P */

           // 小数点文字の挿入
           if ((fmtflgck(GCONVRT) == 0 && p_par2->_prec > 0) ||
					       /* ^^^ C X21010V   C S40011SP */
               (fmtflgck(ONLYONE)!=0 )){		 /* C X21010V */
               *buf++ = (char)'.';
               k++;
           }                        /* if after zero output ? */
           // 小数点文字以下の0挿入
           if (fmtflgck(GCONVRT)==0 ||  /* (NOT %g %G) OR (%#g,%#G) case */
              (fmtflgck(GCONVRT)!=0 && fmtflgck(ONLYONE)!=0)){
               p_par2->_prec_addr2=buf; /* after zero put process C S40011SP */
               p_par2->_prec_ins2 = p_par2->_prec;             /* C S40011SP */
               k += p_par2->_prec;                             /* C S40011SP */
           }

           k=k+exp+slen;
           // 整数部で先頭の0を埋める設定
           if ((*buf_top=='+' || *buf_top=='-' || *buf_top==' ') &&
                   p_par2->_width>k && p_par2->_padchar=='0' &&/* C S40011SP, C V8.0 LI-066 */
                   fmtflgck(LEFTADJ) == 0){                                /* A V8.0 LI-066 */
               p_par2->_prec_addr0 = buf_top + 1;              /* C S40011SP */
               p_par2->_prec_ins0  = p_par2->_width - k;       /* C S40011SP */
               p_par2->_with_ins = 0;                          /* C S40011SP */
           }
           else{
               if (p_par2->_width>k){                          /* C S40011SP */
                   p_par2->_with_ins = p_par2->_width - k;     /* C S40011SP */
               }
               else{
                   p_par2->_with_ins = 0;                      /* C S40011SP */
               }
           }

       }
       // 整数部.小数部
       else{                            /* exp<0 case */
           exp += slen;
           // 丸め処理 'g'指定時は実行しない
           if (fmtflgck(GCONVRT)==0){                           /* A S93B199P */
               j = exp+p_par2->_prec;                              /* C S40011SP */
               sp=sbuf+1;
               if ((j<slen) && (j>=0)){                      /* C S20020SP */
                   _round(sbuf,(long)j+1);
               }
           }                                                    /* A S93B199P */
           // 整数部有り
           if (exp>0){                  /* after normalize, exp>0 case */
               if (sbuf[0]=='0'){
                   i=1;
                   j=1;
               }
               else{  // 最上位桁が桁上がりした時の補正
                   i=0;
                   j=0;
                   exp++;
                   slen++;
               }
/*             k=0;                                         ** D V8.0 LI-056 */
               // bufに整数部を格納
               for (; exp>0; i++){
                   *buf++ = sbuf[i];
                   // 'g'指定時の精度減算
                   if (fmtflgck(GCONVRT)!=0) { p_par2->_prec--; }   /* A S93B199P */
                   exp--;
                   k++;
               }
               // bufに小数点を格納
               *buf++ = (char)'.';          /* C X21010V */
               k++;                         /* C X21010V */

               // bufに小数部を格納
               for (;(i-j)<slen && p_par2->_prec>0;){          /* C S40011SP */
                   *buf++ = sbuf[i++];
                   p_par2->_prec--;                            /* C S40011SP */
                   k++;
               }                   /* if after zero output ? */
               // 後続の0を精度の残数だけ埋める設定
               if (fmtflgck(GCONVRT)==0 ||
                  (fmtflgck(GCONVRT)!=0 && fmtflgck(ONLYONE)!=0)){
                   k+=p_par2->_prec;                           /* C S40011SP */
                   p_par2->_prec_addr1=buf;                    /* C S40011SP */
                   p_par2->_prec_ins1=p_par2->_prec;           /* C S40011SP */
               }
               else{  // 後続の0を削除
                   if (*(buf-1)=='0'){
                       for (; *(buf-1)=='0'; buf--,k--);
                   }
               }
               // 小数点の削除
               if (fmtflgck(ONLYONE)==0 && *(buf - 1) == '.' /* A X21010V */
                                    &&			     /* A X21010V */
                 (p_par2->_prec_ins1 == 0 || fmtflgck(GCONVRT) != 0)){
						     /* A X21010V C S40011SP */
                   buf--;			     /* A X21010V */
                   k = k - p_par2->_prec_ins1 - 1;   /* A X21010V C S40011SP */
		           p_par2->_prec_ins1  = 0;	     /* A X21010V C S40011SP */
		           p_par2->_prec_addr1 = buf_top;    /* A X21010V C S40011SP */
               }				     /* A X21010V */

               // 整数部で先頭の0を埋める設定
               if ((*buf_top=='+' || *buf_top=='-' || *buf_top==' ') &&
                   p_par2->_width>k && p_par2->_padchar=='0' &&/* C S40011SP, C V8.0 LI-066 */
                   fmtflgck(LEFTADJ)==0){                                  /* A V8.0 LI-066 */
                   p_par2->_prec_addr0 = buf_top + 1;          /* C S40011SP */
                   p_par2->_prec_ins0  = p_par2->_width - k;   /* C S40011SP */
                   p_par2->_with_ins = 0;                      /* C S40011SP */
               }
               else{
                   if (p_par2->_width>k){                      /* C S40011SP */
                       p_par2->_with_ins = p_par2->_width - k; /* C S40011SP */
                   }
                   else{
                       p_par2->_with_ins = 0;                  /* C S40011SP */
                   }
               }
               if (p_par2->_prec_addr1==buf_top)               /* C S40011SP */
                   p_par2->_prec_addr1 = p_par2->_prec_addr0;  /* C S40011SP */

           }
           // 整数部無し
           else{                        /* exp<=0  case */
               if (sbuf[0]=='0'){
                   j=i=1;
               }
               else{  // 最上位桁が桁上がりした時の補正
                   j=i=0;
                   exp++;
                   slen++;
               }

               // bufに整数部を格納
               if (exp>0){              /* P-5C-367 */
                   i=1;                 /* P-5C-367 */
                   *buf++=sbuf[0]; 
                   // 'g'指定時の精度減算
                   if (fmtflgck(GCONVRT)!=0) { p_par2->_prec--; }   /* A S93B199P */
               }                        /* P-5C-367 */
               else
                   *buf++=(char)'0';

               k++;                                         /* C V8.0 LI-056 */

               // bufに小数点を格納
               *buf++ = (char)'.';          /* C X21010V */
               k++;                         /* C X21010V */
               // 小数部で先頭の0を埋める設定(expは小数点以下の0の数を示す)
               if (exp<0){
                   p_par2->_prec_addr1=buf;                    /* C S40011SP */
                   // 'g'指定時は0をexpの数だけ挿入する
                   if (fmtflgck(GCONVRT)!=0 || p_par2->_prec>(-exp)){   /* C S93B199P */
                       p_par2->_prec_ins1=-exp;                /* C S40011SP */
                       k+=(-exp);
                   }
                   else{
                       p_par2->_prec_ins1=p_par2->_prec;       /* C S40011SP */
                       k+=p_par2->_prec;                       /* C S40011SP */
                   }
                   // 'g'指定時は0を精度に含めない
                   if (fmtflgck(GCONVRT)==0) {                 /* A S93B199P */ 
                       p_par2->_prec+=exp;                         /* C S40011SP */
                   }                                           /* A S93B199P */ 
               }

               // bufに小数部を格納
               for (; (i-j)<slen && p_par2->_prec>0; i++){                            /* C S40011SP */
                   *buf++ = sbuf[i];
                   p_par2->_prec--;                            /* C S40011SP */
                   k++;
               }                      /* if after zero output ? */
               // 後続の0を精度の残数だけ埋める設定
               if (fmtflgck(GCONVRT)==0 ||
                  (fmtflgck(GCONVRT)!=0 && fmtflgck(ONLYONE)!=0)){
                   if (p_par2->_prec > 0){                     /* C S40011SP */
                       p_par2->_prec_addr2 = buf;              /* C S40011SP */
                       p_par2->_prec_ins2 = p_par2->_prec;     /* C S40011SP */
                       k += p_par2->_prec;                     /* C S40011SP */
                   }
               }
               else{  // 後続の0を削除
                   if (*(buf-1)=='0'){
                       for (; *(buf-1)=='0'; buf--,k--);
                   }
               }
               // 小数点の削除
              if (fmtflgck(ONLYONE)==0 && *(buf - 1) == '.' /* A X21010V */
                                    &&			     /* A X21010V */
                 ((p_par2->_prec_ins1 == 0 && p_par2->_prec_ins2 == 0)||
                 fmtflgck(GCONVRT) != 0)){              /* ^^^ C S40011SP */
							     /* A X21010V */
                   buf--;				     /* A X21010V */
                   k = k - p_par2->_prec_ins1 - p_par2->_prec_ins2 - 1;
						     /* A X21010V C S40011SP */
		           p_par2->_prec_ins1 = p_par2->_prec_ins2 = 0;
						/* ^^^^ A X21010V C S40011SP */
		           p_par2->_prec_addr1 = buf_top;    /* A X21010V C S40011SP */
		           p_par2->_prec_addr2 = 0;	     /* A X21010V C S40011SP */
               }					     /* A X21010V */
               // 整数部で先頭の0を埋める設定
               if ((*buf_top=='+' || *buf_top=='-' || *buf_top==' ') &&
                    p_par2->_width>k && p_par2->_padchar=='0'&&/* C S40011SP, C V8.0 LI-066 */
                    fmtflgck(LEFTADJ)==0){                                 /* A V8.0 LI-066 */
                   p_par2->_prec_addr0 = buf_top + 1;          /* C S40011SP */
                   p_par2->_prec_ins0  = p_par2->_width - k;   /* C S40011SP */
                   p_par2->_with_ins = 0;                      /* C S40011SP */
               }
               else{
                   if (p_par2->_width>k){                      /* C S40011SP */
                       p_par2->_with_ins = p_par2->_width - k; /* C S40011SP */
                   }
                   else{
                       p_par2->_with_ins = 0;                  /* C S40011SP */
                   }
               }
               if (p_par2->_prec_addr1 == buf_top)             /* C S40011SP */
                   p_par2->_prec_addr1 = p_par2->_prec_addr0;  /* C S40011SP */

           }
       }

    }
    // 'e'指定時の表示設定
    else{
        // 丸め処理 'g'指定時は実行しない
        if (fmtflgck(GCONVRT)==0){                              /* A S93B199P */
            if ((p_par2->_prec + 1) < slen){                       /* C S40011SP */
                _round(sbuf, (long)(p_par2->_prec + 2));           /* C S40011SP */
            }
        }                                                       /* A S93B199P */
        if (sbuf[0]=='0'){
            i=1;
        }
        else{  // 最上位桁が桁上がりした時の補正
            i=0;
            exp--;
            slen++;
        }
        exp += (slen-i);
        // bufに整数部を格納
        *buf++ = sbuf[i];
        // 'g'指定時の精度減算
        if (fmtflgck(GCONVRT)!=0) { p_par2->_prec--; }          /* A S93B199P */
        k++;                                                /* C V8.0 LI-056 */
        // bufに小数点を格納
        *buf++ = (char)'.';          /* C X21010V */
        k++;                         /* C X21010V */
        // bufに小数部を格納
        for (j=i+1; j<=slen && p_par2->_prec > 0; j++){        /* C S40011SP */
            *buf++ = sbuf[j];
            p_par2->_prec--;                                   /* C S40011SP */
            k++;
        }
        // 後続の0を精度の残数だけ埋める設定
        if (fmtflgck(GCONVRT)==0 ||
           (fmtflgck(GCONVRT)!=0 && fmtflgck(ONLYONE)!=0)){
            p_par2->_prec_addr1=buf;                           /* C S40011SP */
            p_par2->_prec_ins1 =p_par2->_prec;                 /* C S40011SP */
            k+=p_par2->_prec;                                  /* C S40011SP */
        }
        else{  // 後続の0を削除
            if (*(buf-1)=='0'){
                for (; *(buf-1)=='0'; buf--,k--);
            }
        }
        // 小数点の削除
        if (fmtflgck(ONLYONE)==0 && *(buf - 1) == '.' /* A X21010V */
                                    &&			     /* A X21010V */
            (p_par2->_prec_ins1 == 0 || fmtflgck(GCONVRT) != 0)){
						/* ^^^ A X21010V  C S40011SP */
            buf--;				     /* A X21010V */
            k = k - p_par2->_prec_ins1 - 1; /* A X21010V   C S40011SP */
		    p_par2->_prec_addr1 = buf_top;  /* A X21010V   C S40011SP */
		    p_par2->_prec_ins1  = 0; 	   /* A X21010V   C S40011SP */
        }					     /* A X21010V */
        // bufに指数の底を格納
        if (spec=='e')
            *buf++ = (char)'e';
        else
           *buf++ = (char)'E';
        k++;

        // bufに指数の符号を格納
        if (exp>=0)
            *buf++ = (char)'+';
        else{
            *buf++ = (char)'-';
            exp = -exp;
        }
        k++;

        // bufに指数を格納
        if (exp>=100){
            buf += 2;
            k+=3;
        }
        else{
            *buf++ = (char)'0';
            *buf   = (char)'0';
            k+=2;
        }
        sp = buf+1;
        for (;exp>0;){
            *buf-- = (char)(exp%10+'0');
            exp = exp/10;
        }
        buf=sp;


        // 整数部で先頭の0を埋める設定
        if ((*buf_top=='+' || *buf_top=='-' || *buf_top==' ') &&
                p_par2->_width>k && p_par2->_padchar=='0' &&   /* C S40011SP, C V8.0 LI-066 */
                fmtflgck(LEFTADJ)==0){                                     /* A V8.0 LI-066 */
            p_par2->_prec_addr0 = buf_top + 1;                 /* C S40011SP */
            p_par2->_prec_ins0  = p_par2->_width - k;                            /* C S40011SP */
            p_par2->_with_ins = 0;                             /* C S40011SP */
        }
        else{
            if (p_par2->_width>k){                             /* C S40011SP */
                p_par2->_with_ins = p_par2->_width - k;        /* C S40011SP */
            }
            else{
                p_par2->_with_ins = 0;                         /* C S40011SP */
            }
        }
        if (p_par2->_prec_addr1==buf_top)                      /* C S40011SP */
            p_par2->_prec_addr1 = p_par2->_prec_addr0;                            /* C S40011SP */

    }

    *buf++ = (char)'\0';                                    /* C S01001SP | */
}                                                           /* C S01001SP v */


void
_round(base, ptr)
    char           *base;
    long            ptr;
{

    if (*(base + ptr) > '4') {
	*(base + ptr) = (char) '0';
	ptr--;
	*(base + ptr) += 1;
	for (; ptr > 0; ptr--) {
	    if (*(base + ptr) > '9') {
		*(base + ptr) -= 10;
		*(base + ptr - 1) += 1;
	    }
	}
	if (*(base + 1) > '9') {
	    *(base + 1) -= 10;
	    *base += 1;
	}
    }
}
#endif                                                         /* A S50005SP */
#endif                                                     /* A S70001SP */

#ifdef _DSPC
/***********************************************************************/
/*                                                                     */
/* _fixedtostr subroutine :                                            */
/*                                                                     */
/***********************************************************************/
static void _fixedtostr(int* fix_data, char* buffer, int format,
                        _FMTOTBL2* p_par2, _FIX_2TO10_DATA* change_data)
{
    char fixedBuffer[40];             /* fixed decimal string buffer */
    char integerBuffer[10];           /* integer decimal string buffer */
    int cnt1, cnt2;
    char sign = '+';
    unsigned int width = change_data->width;
    unsigned int checkPoint = change_data->checkPoint;
    char* buffer_top = buffer;
    int stringLength;
    int bufferLength;
    int integerData = 0;
    int fixedData = 0;
    int workData;

    if (fix_data == 0) {            /* fixed data zero */
        fixedBuffer[0] = '0';
        fixedBuffer[1] = '\0';
    }
    else {
        fixedData = *(fix_data + 1);
        if (format == 'a') {
            if (p_par2->_parasize == 'l') {
                integerData = *(fix_data + 0);
                if (integerData < 0) {
                    fixedData = -fixedData;
                    integerData = -integerData;
                    if (fixedData != 0) {
                        integerData--;
                    }
                    sign = '-';
                }
                integerData <<= 1;
                if (fixedData & 0x80000000) {
                    integerData |= 1;
                    fixedData &= 0x7fffffff;
                }
            }
            else {
                if (fixedData < 0) {
                    fixedData = -fixedData;
                    sign = '-';
                }
                integerData = fixedData >> 15;
            }
        }
        else {
            if (fixedData == 0x80000000) { /* long fixed data minus min */
                sign = '-';
                fixedData = 0;
                integerData = 1;
            } else if (fixedData < 0) {  /* fixed data minus */
                sign = '-';
                fixedData = -fixedData;  /* change sign */
            }
        }

        /* change fixed type binary -> decimal before fixed point*/
        if (integerData) {
            integerBuffer[3] = '\0';
            workData = integerData;
            for (cnt1 = 2; cnt1 >= 0; cnt1--) {
                integerBuffer[cnt1] = (workData % 10) + '0';
                workData /= 10;
            }
        }
        if ((format == 'a') && (p_par2->_parasize != 'l')) {
            fixedData &= 0x00007fff;   /* acuum mask after fixed point */
            fixedData <<= 16;
        }

        for (cnt1 = 0; cnt1 < sizeof(fixedBuffer); cnt1++) {
            fixedBuffer[cnt1] = 0;
        }

        /* change fixed type binary -> decimal after fixed point*/
        /* fixed bits length's loop */
        for (cnt1 = 0; cnt1 < (width * width); cnt1 += width) {
            fixedData <<= 1;
            if (fixedData & checkPoint) {
                /* add one bit's decimal value */
                for (cnt2 = width - 1; cnt2 >= 0; cnt2--) {
                    fixedBuffer[cnt2] += change_data->data[cnt1 + cnt2];
                    if (fixedBuffer[cnt2] >= 10) {
                        fixedBuffer[cnt2] -= 10;
                        fixedBuffer[cnt2 - 1] ++;
                    }
                } /* for */
            } /* if */
        } /* for */
        for (cnt1 = width - 1; cnt1 >= 0; cnt1--) {
            fixedBuffer[cnt1] += '0';
        }
    } /* if */

    if (sign == '+') {
        switch (fmtflgck(SIGNPLS | SIGNSPC)) {
        case SIGNPLS:
            *buffer++ = '+';
            bufferLength = 1;                               /* A V8.0 LI-056 */
            break;
        case SIGNSPC:
            *buffer++ = ' ';
            bufferLength = 1;                               /* A V8.0 LI-056 */
            break;
        default:
            bufferLength = 0;                               /* A V8.0 LI-056 */
            break;
        }
    }
    else {
        *buffer++ = '-';
        bufferLength = 1;                                   /* A V8.0 LI-056 */
    }

/*  bufferLength = 0;                                       ** D V8.0 LI-056 */

    /* copy before fixed point '.' integer data */
    if (integerData) {
        for (cnt1 = 0; integerBuffer[cnt1]; cnt1++) {
            if (integerBuffer[cnt1] != '0') {
                break;
            }
        }
        if (integerBuffer[cnt1] != '\0') {
            while (integerBuffer[cnt1]) {
                *buffer++ = integerBuffer[cnt1];
                bufferLength++;
                cnt1++;
            }
        }
    }
    else {
        *buffer++ = '0';
        bufferLength++;
    }

    /* copy output buffer */
    if ((p_par2->_prec > 0) || fmtflgcheck(ONLYONE)) {
        *buffer++ = '.';                     /* use precision or use '#' */
        bufferLength++;                                     /* A V8.0 LI-056 */
    }

    stringLength = strlen(fixedBuffer);

    /* copy after fixed point '.' fixed data */
    for (cnt1 = 0; (cnt1 < stringLength) && (p_par2->_prec > 0); cnt1++) {
        *buffer++ = fixedBuffer[cnt1];
        p_par2->_prec--;
        bufferLength++;
    }

    /* set after padding point */
    bufferLength += p_par2->_prec;
    p_par2->_prec_addr1 = buffer;
    p_par2->_prec_ins1 = p_par2->_prec;

    /* nothing after fixed point */
    if (!fmtflgcheck(ONLYONE) && (*(buffer - 1) == '.')) {
        buffer--;
        bufferLength -= (p_par2->_prec_ins1 - 1);
        p_par2->_prec_addr1 = buffer_top;
        p_par2->_prec_ins1 = 0;
    }

    /*set before padding point */
    if (((*buffer_top == '+') || (*buffer_top == '-') || (*buffer_top == ' '))
            && (p_par2->_width > bufferLength) && (p_par2->_padchar == '0')     /* C V8.0 LI-066 */
            && (fmtflgck(LEFTADJ) == 0)) {                                      /* A V8.0 LI-066 */
        /* padding in between sign and data */
        p_par2->_prec_addr0 = buffer_top + 1;
        p_par2->_prec_ins0 = p_par2->_width - bufferLength - 1;
        p_par2->_with_ins = 0;
    }
    else {
        /* padding in top */
        if (p_par2->_width > bufferLength) {
            p_par2->_with_ins = p_par2->_width - bufferLength;  /* C V8.0 LI-056 */
        }
        else {
            p_par2->_with_ins = 0;
        }
    } /* if */
    if (p_par2->_prec_addr1 == buffer_top) {
        p_par2->_prec_addr1 = p_par2->_prec_addr0;
    }
    *buffer++ = '\0';
}
#endif /* _DSPC */

/***********************************************************************/
/* */
/* _putstr subroutine :                                                */
/* */
/***********************************************************************/
static void
_putstr(buf, len, p_par1)                                /* C S40011SP */
    char           *buf;
    long            len;
    _FMTOTBL1      *p_par1;                              /* A S40011SP */
{
    int             i;

    if (p_par1->_outkind == 1) {                               /* C S40011SP */
	memcpy((void *)p_par1->_str, (const void *)buf, (size_t)len);
	p_par1->_str += len;                           /* <<< ^^^ C S40011SP */
	p_par1->_outch += len;                                 /* C S40011SP */
    } else {
	for (i = 0; i < len; i++) {
	    if ((*p_par1->_sofun)((int)*buf++, ((FILE *)(p_par1->_str)))==EOF)
					       /* ^^^ C Svv00 SP  C S40011SP */
		break;
	    else
		p_par1->_outch += 1;                           /* C S40011SP */
	}
    }
}



/***********************************************************************/
/* */
/* _putnull subroutine :                                               */
/* */
/***********************************************************************/
static void
_putnull(p_par1)                                         /* C S40011SP */
_FMTOTBL1 *p_par1;                                       /* C S40011SP */
{
    if (p_par1->_outkind == 1)                           /* C S40011SP */
	*p_par1->_str = (char) '\0';                     /* C S40011SP */
}



/***********************************************************************/
/* */
/* _puterr subroutine :                                                */
/* */
/***********************************************************************/
static
_puterr(p_par1)                                          /* C S40011SP */
_FMTOTBL1 *p_par1;                                       /* C S40011SP */
{
    if (p_par1->_outkind == 1)                           /* C S40011SP */
	return (0);
    else
    /* ferror 関数をサブ関数化 C SH-V.9.04.01 BSH009    */
	return (_SUBFUNC(ferror)(((FILE *)p_par1->_str)));    /* C S40011SP */
}
