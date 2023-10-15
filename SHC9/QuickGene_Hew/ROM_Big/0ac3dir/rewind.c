/********************************************************************************/
/* Copyright (C) 1992 (2006 - 2011) Renesas Electronics Corporation and         */
/* Renesas Solutions Corp. All Rights Reserved.                                 */
/********************************************************************************/
/***********************************************************************/
/* SPEC;                                                               */
/*  NAME = rewind : point file pointer to top of file;                 */
/*                                                                     */
/*  FUNC = this module do the following functions                      */
/*         (1) file stream check;                                      */
/*         (2) active character write on output mode buffer;           */
/*         (3) buffer status reset;                                    */
/*         (4) ungetc status flag reset;                               */
/*         (5) file status flag reset;                                 */
/*                                                                     */
/*  CLAS = UNIT;                                                       */
/*                                                                     */
/* END;                                                                */
/***********************************************************************/

#define  _IOPROT  1
                                                         /* D S01006SP */
#include "lstdio.h"
#include "h2lfun.h"
#include "erewind.h"

/* ���G���g�����g�ǉ� A  SH-V.9.04.01 BSH009    */
#ifdef _REENTRANT
#include "sem.h"
#include <errno.h>
#endif /* _REENTRANT    */

/* ������ANSI�d�l�ɕύX C SH-V.9.04.01 BSH009   */
void rewind(FILE *stream)
{
    int write_rc;

    /* ���G���g�����g�ǉ� A SH-V.9.04.01 BSH009 */
#ifdef _REENTRANT
    int iob_no;
    int sem_no;
#endif /* _REENTRANT    */

    if (stream==0){
        _errno=PTRERR;
        return;
    }

    /* �����m�ۏ��� */  /* A SH-V.9.04.01 BSH009    */
#ifdef _REENTRANT
    for (iob_no = 0; iob_no < _nfiles && (&_iob[iob_no]) != stream; iob_no++);
    sem_no = _IOB_SEM + iob_no;
    if ( !wait_sem(sem_no) ) {
        /* �����m�ێ��s */
        _errno = EIOBRESM;
        return;
    }
#endif /* _REENTRANT    */

    if (flgck1(stream,(_IORW|_IOREAD|_IOWRITE))==0){
        _errno = NOTOPN;
        /* ����������� */  /* A SH-V.9.04.01 BSH009    */
#ifdef _REENTRANT
        if ( !signal_sem(sem_no) ) {
            /* ����������s */
            _errno = EIOBFRSM;
        }
#endif /* _REENTRANT    */
        return;
    }
    if (flgck1(stream,_IOWRITE)!=0){
        if (stream->_bufbase<stream->_bufptr){
            write_rc = write((int)stream->_iofd,
                             (char*)stream->_bufbase,
                             (unsigned)
                             (stream->_bufptr-stream->_bufbase));
        }
        stream->_bufcnt = stream->_buflen;
        stream->_bufptr = stream->_bufbase;
    }
    if (flgck1(stream,_IOREAD)!=0){
        stream->_bufcnt = 0;
        stream->_bufptr = stream->_bufbase+stream->_buflen;
    }
    lseek((int)stream->_iofd,0L,0);
    stream->_ioflag1 &= ~(_IOEOF|_IOERR);
    stream->_ioflag2 &= ~_UNGTC;

    /* ����������� */  /* A SH-V.9.04.01 BSH009    */
#ifdef _REENTRANT
    if ( !signal_sem(sem_no) ) {
        /* ����������s */
        _errno = EIOBFRSM;
    }
#endif /* _REENTRANT    */

}
