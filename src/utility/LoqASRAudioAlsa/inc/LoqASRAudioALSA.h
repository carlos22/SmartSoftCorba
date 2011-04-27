/******************************************************************************/
/*                                                                            */
/*                     Copyright 2000-2007 LOQUENDO S.p.A.                    */ 
/*                                                                            */
/*                             All rights reserved.                           */ 
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/**                                                                          **/
/**                           F L E X U S   3 0 0 0                          **/
/**                                                                          **/
/** File audio source handling routines.                                     **/
/**                                                                          **/
/******************************************************************************/
/******************************************************************************/

/*************************************** function prototypes ****************************************/

#ifdef WIN32
#define AUDIO_STDCALL __stdcall
#else
#define AUDIO_STDCALL
#endif

int AUDIO_STDCALL NewHandle(int format, void **hnd);
int AUDIO_STDCALL DeleteHandle(void *hnd);
int AUDIO_STDCALL SetData(void *hnd, char *waveid);
int AUDIO_STDCALL Register(void *hnd, void *instance);
int AUDIO_STDCALL Unregister(void *hnd);
char * AUDIO_STDCALL GetData(void *hnd);
char * AUDIO_STDCALL GetErrorMessage(void *hnd);
