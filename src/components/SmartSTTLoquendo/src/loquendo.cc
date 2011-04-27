// --------------------------------------------------------------------------
//
//  Copyright (C) 2009 Andreas Steck, Matthias Lutz
//
//        schlegel@hs-ulm.de
//        steck@hs-ulm.de
//
//        ZAFH Servicerobotik Ulm
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//  This file is part of the "SmartSoft SpeechOutput component".
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// --------------------------------------------------------------------------

//----------------------------------------------------------------------------
//
// CREDITS:
//
// The code for the communication with Loquendo was gracefully provided by the
// robocup team NimbRo of the University of Bonn in Germany.
// http://www.ais.uni-bonn.de/nimbro/
//
//----------------------------------------------------------------------------

#include "gen/SmartSTTLoquendo.hh"


/******************************************************************************/
/* Defines that must be changed by the user of this program                   */
/******************************************************************************/

/* Initialization file                                                        */
// set this in the ini file [loquendo] session
//#define LASRX_EX_SESSION_FILE "src/components/smartSpeechLoquendoInputServer/LoqASRexamples.session"

/* Audio mode                                                                 */
#define LASRX_EX_AUDIO_MODE LASRX_SAMPLES_AL

/* Source grammar and active rule (NULL = root rule)                          */
//#define LASRX_EX_GRAMMAR_SOURCE "src/components/smartSpeechLoquendoInputServer/grammers/butler.grxml"
// set this in the ini file [loquendo] grammar
//#define LASRX_EX_GRAMMAR_SOURCE "src/components/smartSpeechLoquendoInputServer/grammers/followMe.grxml"

#define LASRX_EX_RO_RULE NULL

/* Recognition object name                                                    */
#define LASRX_EX_RO_NAME "dynamic"

#include <dlfcn.h>
#include <semaphore.h>
#include <errno.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include "LoqASR.h"


#define LASRX_EX_DLL_LOAD(szDll)         dlopen(szDll, RTLD_LAZY | RTLD_GLOBAL)
#define LASRX_EX_DLL_FREE(hDll)          dlclose(hDll)
#define LASRX_EX_DLL_SYM(hDll, pszSym)   dlsym(hDll, pszSym)
typedef void* LASRX_EX_DLL_HND;
typedef sem_t LASRX_EX_EVENT;



// global variables
//std::string grammar_file;
//std::string session_file;

/******************************************************************************/
/* OS dependent functions                                                     */
/******************************************************************************/
static int __CreateEvent(LASRX_EX_EVENT *ev) {
        if (sem_init(ev, 0, 0) != 0) return LASRX_RETCODE_ERROR;
        return LASRX_RETCODE_OK;
}

static int __SetEvent(LASRX_EX_EVENT *ev) {
        if (sem_post(ev) != 0) return LASRX_RETCODE_ERROR;
        return LASRX_RETCODE_OK;
}

static int __WaitEvent(LASRX_EX_EVENT *ev) {
        int ret;
        do {
                ret = sem_wait(ev);
        } while (ret != 0 && errno == EINTR);
        if (ret != 0) return LASRX_RETCODE_ERROR;
        return LASRX_RETCODE_OK;
}

static int __RemoveEvent(LASRX_EX_EVENT *ev) {
        if (sem_destroy(ev) != 0) return LASRX_RETCODE_ERROR;
        return LASRX_RETCODE_OK;
}



/******************************************************************************/
/* Audio source DLLs definitions                                              */
/******************************************************************************/
typedef int (*LASRX_EX_AUDIO_NEW_HANDLE)(int nFormat, void **hAudio);
typedef int (*LASRX_EX_AUDIO_DELETE_HANDLE)(void *hAudio);
typedef int (*LASRX_EX_AUDIO_SET_DATA)(void *hAudio, char *szData);
typedef int (*LASRX_EX_AUDIO_REGISTER)(void *hAudio, void *hInstance);
typedef int (*LASRX_EX_AUDIO_UNREGISTER)(void *hAudio);
#define LASRX_EX_AUDIO_NEW_FUNCTION "NewHandle"
#define LASRX_EX_AUDIO_DELETE_FUNCTION "DeleteHandle"
#define LASRX_EX_AUDIO_SET_DATA_FUNCTION "SetData"
#define LASRX_EX_AUDIO_REGISTER_FUNCTION "Register"
#define LASRX_EX_AUDIO_UNREGISTER_FUNCTION "Unregister"
typedef struct {
    LASRX_EX_DLL_HND hAudioSourceDll;
    LASRX_EX_AUDIO_NEW_HANDLE pfNew;
    LASRX_EX_AUDIO_DELETE_HANDLE pfDelete;
    LASRX_EX_AUDIO_SET_DATA pfSetData;
    LASRX_EX_AUDIO_REGISTER pfRegister;
    LASRX_EX_AUDIO_UNREGISTER pfUnregister;
} LASRX_EX_DLL_POINTERS;

/******************************************************************************/
/* Definition related to the audio source                                     */
/******************************************************************************/
//#define LASRX_EX_AUDIO_SOURCE_LIBRARY "libLoqASRAlsa.so"
//New Alsa Loq data source
#define LASRX_EX_AUDIO_SOURCE_LIBRARY "libLoqASRAudioALSA.so"

/******************************************************************************/
/* Error reporting                                                            */
/******************************************************************************/
//static int __ReportError(char *szFunction, char *szFormat, ...) {
static int __ReportError(char *szFunction, char *szFormat, ...) {
    va_list pArgs;
    va_start(pArgs, szFormat);
    fprintf(stderr, "\nFunction %s - ", szFunction);
    vfprintf(stderr, szFormat, pArgs);
    va_end(pArgs);
    return 0;
}

static int __ReportLasrxError(char *szFunction, void *hHandle, char *szLasrxFunction, int nRetCode) {
    char *szErrorMessage;
    char *szApiBuffer;
    lasrxGetErrorMessage(hHandle, &szErrorMessage);
    lasrxLogGetApiBuffer(hHandle, &szApiBuffer);
    __ReportError(szFunction, "ERROR: Loquendo ASR function \"%s\" failed with code %d:\n === Error message ===\n%s\n === Log Buffer ===\n%s\n", szLasrxFunction, nRetCode, szErrorMessage == NULL ? "(null)" : szErrorMessage, szApiBuffer == NULL ? "(null)" : szApiBuffer);
    lasrxFree(szErrorMessage);
    lasrxFree(szApiBuffer);
    return 0;
}


/******************************************************************************/
/* Load and connect to audio source                                           */
/******************************************************************************/
static int __LoadAudioSource(lasrxHandleType hInstance, char *szAudioSource, LASRX_EX_DLL_POINTERS *pDll, void **phAudio) {
    lasrxResultType nRetCode = LASRX_RETCODE_OK;
    lasrxHandleType hSession;
    lasrxIntType nAudioMode;
    if ((pDll->hAudioSourceDll = LASRX_EX_DLL_LOAD(szAudioSource)) == NULL) {
        __ReportError("__LoadAudioSource", "Error loading library \"%s\"\n", szAudioSource);
        return -1;
    }
    if ((pDll->pfNew = (LASRX_EX_AUDIO_NEW_HANDLE)LASRX_EX_DLL_SYM(pDll->hAudioSourceDll, LASRX_EX_AUDIO_NEW_FUNCTION)) == NULL) {
        __ReportError("__LoadAudioSource", "Error getting proc address \"%s\"\n", LASRX_EX_AUDIO_NEW_FUNCTION);
        LASRX_EX_DLL_FREE(pDll->hAudioSourceDll);
        return -1;
    }
    if ((pDll->pfDelete = (LASRX_EX_AUDIO_DELETE_HANDLE)LASRX_EX_DLL_SYM(pDll->hAudioSourceDll, LASRX_EX_AUDIO_DELETE_FUNCTION)) == NULL) {
        __ReportError("__LoadAudioSource", "Error getting proc address \"%s\"\n", LASRX_EX_AUDIO_DELETE_FUNCTION);
        LASRX_EX_DLL_FREE(pDll->hAudioSourceDll);
        return -1;
    }
    if ((pDll->pfSetData = (LASRX_EX_AUDIO_SET_DATA)LASRX_EX_DLL_SYM(pDll->hAudioSourceDll, LASRX_EX_AUDIO_SET_DATA_FUNCTION)) == NULL) {
        __ReportError("__LoadAudioSource", "Error getting proc address \"%s\"\n", LASRX_EX_AUDIO_SET_DATA_FUNCTION);
        LASRX_EX_DLL_FREE(pDll->hAudioSourceDll);
        return -1;
    }
    if ((pDll->pfRegister = (LASRX_EX_AUDIO_REGISTER)LASRX_EX_DLL_SYM(pDll->hAudioSourceDll, LASRX_EX_AUDIO_REGISTER_FUNCTION)) == NULL) {
        __ReportError("__LoadAudioSource", "Error getting proc address \"%s\"\n", LASRX_EX_AUDIO_REGISTER_FUNCTION);
        LASRX_EX_DLL_FREE(pDll->hAudioSourceDll);
        return -1;
    }
    if ((pDll->pfUnregister = (LASRX_EX_AUDIO_UNREGISTER)LASRX_EX_DLL_SYM(pDll->hAudioSourceDll, LASRX_EX_AUDIO_UNREGISTER_FUNCTION)) == NULL) {
        __ReportError("__LoadAudioSource", "Error getting proc address \"%s\"\n", LASRX_EX_AUDIO_UNREGISTER_FUNCTION);
        LASRX_EX_DLL_FREE(pDll->hAudioSourceDll);
        return -1;
    }
    if ((nRetCode = lasrxGetSessionHandle(hInstance, &hSession)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__LoadAudioSource", hInstance, "lasrxGetSessionHandle", nRetCode);
        LASRX_EX_DLL_FREE(pDll->hAudioSourceDll);
        return -1;
    }
    if ((nRetCode = lasrxGetAudioMode(hSession, &nAudioMode)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__LoadAudioSource", hInstance, "lasrxGetAudioMode", nRetCode);
        LASRX_EX_DLL_FREE(pDll->hAudioSourceDll);
        return -1;
    }
    if ((*(pDll->pfNew))(nAudioMode, phAudio) != 0) {
        __ReportError("__LoadAudioSource", "ERROR: function \"%s\" failed\n", LASRX_EX_AUDIO_NEW_FUNCTION);
        LASRX_EX_DLL_FREE(pDll->hAudioSourceDll);
        return -1;
    }
    if ((*(pDll->pfRegister))(*phAudio, hInstance) != 0) {
        __ReportError("__LoadAudioSource", "ERROR: function \"%s\" failed\n", LASRX_EX_AUDIO_REGISTER_FUNCTION);
        LASRX_EX_DLL_FREE(pDll->hAudioSourceDll);
        return -1;
    }
    return 0;
}

/******************************************************************************/
/* Loquendo ASR get events callback function                                  */
/******************************************************************************/
static lasrxResultType LASRX_STDCALL __GetEvent(lasrxHandleType hInstance, lasrxPointerType pUser, lasrxIntType nEvent, lasrxIntType nReason, lasrxStringType szId, lasrxPointerType pEventData, lasrxIntType nEventDataSize) {
    lasrxStringType szInstanceId;
    lasrxGetInstanceId(hInstance, &szInstanceId);
    fprintf(stdout, "\nGot event from instance %s, operation id: %s\n", szInstanceId == NULL ? "(null)" : szInstanceId, szId == NULL ? "(null)" : szId);
    switch (nEvent) {
    case LASRX_EVENT_START_VOICE_DETECTED:
        fprintf(stdout, "  LASRX_EVENT_START_VOICE_DETECTED: reason %d\n", nReason);
        break;
    case LASRX_EVENT_END_VOICE_DETECTED:
        fprintf(stdout, "  LASRX_EVENT_END_VOICE_DETECTED: reason %d\n", nReason);
        break;
    case LASRX_EVENT_PROMPT_STOP:
        fprintf(stdout, "  LASRX_EVENT_PROMPT_STOP: reason %d\n", nReason);
        break;
    case LASRX_EVENT_RESTART_FOR_OOV:
        fprintf(stdout, "  LASRX_EVENT_RESTART_FOR_OOV: reason %d\n", nReason);
        break;
    case LASRX_EVENT_RESTART_FOR_BGN:
        fprintf(stdout, "  LASRX_EVENT_RESTART_FOR_BGN: reason %d\n", nReason);
        break;
    case LASRX_EVENT_RESTART_FOR_EPD_ERROR:
        fprintf(stdout, "  LASRX_EVENT_RESTART_FOR_EPD_ERROR: reason %d\n", nReason);
        break;
    case LASRX_EVENT_SWITCHED_MODE:
        fprintf(stdout, "  LASRX_EVENT_SWITCHED_MODE: reason %d\n", nReason);
        break;
    case LASRX_EVENT_END_RECOG:
        fprintf(stdout, "  LASRX_EVENT_END_RECOG: reason %d\n", nReason);
        break;
    case LASRX_EVENT_END_TRECOG:
        fprintf(stdout, "  LASRX_EVENT_END_TRECOG: reason %d\n", nReason);
        break;
    case LASRX_EVENT_END_GENERATE_RO:
        fprintf(stdout, "  LASRX_EVENT_END_GENERATE_RO: reason %d\n", nReason);
        break;
    case LASRX_EVENT_END_VERIFY:
        fprintf(stdout, "  LASRX_EVENT_END_VERIFY: reason %d\n", nReason);
        break;
    case LASRX_EVENT_END_MVERIFY:
        fprintf(stdout, "  LASRX_EVENT_END_MVERIFY: reason %d\n", nReason);
        break;
    case LASRX_EVENT_END_ENROLL:
        fprintf(stdout, "  LASRX_EVENT_END_ENROLL: reason %d\n", nReason);
        break;
    case LASRX_EVENT_END_SEGMENT:
        fprintf(stdout, "  LASRX_EVENT_END_SEGMENT: reason %d\n", nReason);
        break;
    case LASRX_EVENT_END_RECORD:
        fprintf(stdout, "  LASRX_EVENT_END_RECORD: reason %d\n", nReason);
        break;
    default:
        fprintf(stdout, "  UNKNOWN (%d): reason %d\n", nEvent, nReason);
        break;
    }
    if (nReason != LASRX_RETCODE_OK) __ReportLasrxError("__GetEvent", hInstance, szId, nReason);
    return LASRX_RETCODE_OK;
}


/******************************************************************************/
/* Loquendo ASR get audio events callback function                            */
/******************************************************************************/
static lasrxResultType LASRX_STDCALL __GetAudioEvent(lasrxHandleType hInstance, lasrxPointerType pUser, lasrxIntType nEvent) {
    lasrxStringType szInstanceId;
    lasrxGetInstanceId(hInstance, &szInstanceId);
    //fprintf(stdout, "\nGot audio event from instance %s\n", szInstanceId == NULL ? "(null)" : szInstanceId);
    switch (nEvent) {
    case LASRX_EVENT_AUDIO_STOPPED:
        //fprintf(stdout, "  LASRX_EVENT_AUDIO_STOPPED\n");
        break;
    case LASRX_EVENT_AUDIO_WAITING_SAMPLES:
        //fprintf(stdout, "  LASRX_EVENT_AUDIO_WAITING_SAMPLES\n");
        break;
    case LASRX_EVENT_AUDIO_RUNNING:
        //fprintf(stdout, "  LASRX_EVENT_AUDIO_RUNNING\n");
        break;
    case LASRX_EVENT_AUDIO_WAITING_STOPPED:
        //fprintf(stdout, "  LASRX_EVENT_AUDIO_WAITING_STOPPED\n");
        break;
    default:
        //fprintf(stdout, "  UNKNOWN (%d)\n", nEvent);
        break;
    }
    return LASRX_RETCODE_OK;
}

/******************************************************************************/
/* Reports acoustic results                                                   */
/******************************************************************************/
static int __PrintSemanticTree(lasrxHandleType hInstance, lasrxHandleType hElement, int nIndent) {
    lasrxResultType nRetCode = LASRX_RETCODE_OK;
    lasrxIntType nType, nSons;
    lasrxFloatType fConfidence;
    lasrxEncodedStringType eszName = NULL, eszValue = NULL;
    lasrxHandleType hSon;
    if ((nRetCode = lasrxNLPGetElementDetails(hInstance, hElement, &nType, &fConfidence, &eszName)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__PrintSemanticTree", hInstance, "lasrxNLPGetElementDetails", nRetCode);
        return -1;
    }
    else if ((nRetCode = lasrxNLPGetNumberOfSons(hInstance, hElement, &nSons)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__PrintSemanticTree", hInstance, "lasrxNLPGetNumberOfSons", nRetCode);
        return -1;
    }
    else if ((nRetCode = lasrxNLPHandleToBuffer(hInstance, hElement, LASRX_FORMAT_TEXT, &eszValue)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__PrintSemanticTree", hInstance, "lasrxNLPHandleToBuffer", nRetCode);
        return -1;
    }
    else {
        int i;
        for (i = 0; i < nIndent; i++) fprintf(stdout, "  ");
        switch (nType) {
        case LASRX_NLP_TYPE_VOID:
            fprintf(stdout, " NLP: Name %s, Type VOID, Confidence %f, Sons %d, Value %s\n", eszName == NULL ? "(null)" : eszName, fConfidence, nSons, eszValue == NULL ? "(null)" : eszValue);
            break;
        case LASRX_NLP_TYPE_BOOLEAN:
            fprintf(stdout, " NLP: Name %s, Type BOOLEAN, Confidence %f, Sons %d, Value %s\n", eszName == NULL ? "(null)" : eszName, fConfidence, nSons, eszValue == NULL ? "(null)" : eszValue);
            break;
        case LASRX_NLP_TYPE_INT:
            fprintf(stdout, " NLP: Name %s, Type INTEGER, Confidence %f, Sons %d, Value %s\n", eszName == NULL ? "(null)" : eszName, fConfidence, nSons, eszValue == NULL ? "(null)" : eszValue);
            break;
        case LASRX_NLP_TYPE_STRING:
            fprintf(stdout, " NLP: Name %s, Type STRING, Confidence %f, Sons %d, Value %s\n", eszName == NULL ? "(null)" : eszName, fConfidence, nSons, eszValue == NULL ? "(null)" : eszValue);
            break;
        case LASRX_NLP_TYPE_FLOAT:
            fprintf(stdout, " NLP: Name %s, Type FLOAT, Confidence %f, Sons %d, Value %s\n", eszName == NULL ? "(null)" : eszName, fConfidence, nSons, eszValue == NULL ? "(null)" : eszValue);
            break;
        case LASRX_NLP_TYPE_STRUCT:
            fprintf(stdout, "+NLP: Name %s, Type STRUCT, Confidence %f, Sons %d, Value %s\n", eszName == NULL ? "(null)" : eszName, fConfidence, nSons, eszValue == NULL ? "(null)" : eszValue);
            break;
        case LASRX_NLP_TYPE_ARRAY:
            fprintf(stdout, " NLP: Name %s, Type ARRAY, Confidence %f, Sons %d, Value %s\n", eszName == NULL ? "(null)" : eszName, fConfidence, nSons, eszValue == NULL ? "(null)" : eszValue);
            break;
        default:
            fprintf(stdout, " NLP: Name %s, Type UNKNOWN, Confidence %f, Sons %d, Value %s\n", eszName == NULL ? "(null)" : eszName, fConfidence, nSons, eszValue == NULL ? "(null)" : eszValue);
            break;
        }

        for (i = 0; i < nSons; i++) {
            if ((nRetCode = lasrxNLPGetSon(hInstance, hElement, i, &hSon)) != LASRX_RETCODE_OK) {
                __ReportLasrxError("__PrintSemanticTree", hInstance, "lasrxNLPGetSon", nRetCode);
                return -1;
            }
            else {
                __PrintSemanticTree(hInstance, hSon, nIndent+1);
            }
        }

    }
    return 0;
}

static int __GetSemanticLisp(lasrxHandleType hInstance, lasrxHandleType hElement, int nIndent, std::string &str)
{
    lasrxResultType nRetCode = LASRX_RETCODE_OK;
    lasrxIntType nType, nSons;
    lasrxFloatType fConfidence;
    lasrxEncodedStringType eszName = NULL, eszValue = NULL;
    lasrxHandleType hSon;
    if ((nRetCode = lasrxNLPGetElementDetails(hInstance, hElement, &nType, &fConfidence, &eszName)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetSemanticLisp", hInstance, "lasrxNLPGetElementDetails", nRetCode);
        return -1;
    }
    else if ((nRetCode = lasrxNLPGetNumberOfSons(hInstance, hElement, &nSons)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetSemanticLisp", hInstance, "lasrxNLPGetNumberOfSons", nRetCode);
        return -1;
    }
    else if ((nRetCode = lasrxNLPHandleToBuffer(hInstance, hElement, LASRX_FORMAT_TEXT, &eszValue)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetSemanticLisp", hInstance, "lasrxNLPHandleToBuffer", nRetCode);
        return -1;
    }
    else {
        int i;

        if( nSons == 0 )
        {
          //fprintf(stdout, "(%s %s", eszName == NULL ? "null" : eszName, eszValue == NULL ? "error" : eszValue);
          std::ostringstream os;
          os << "(" << (char *)eszName << " " << (char*)eszValue;
          std::string buffer(os.str());
          str += buffer;
        }
        else
        {
          if(eszName != NULL)
          {
            //fprintf(stdout, "(%s(", eszName);
            std::ostringstream os;
            os << "(" <<(char*) eszName << "(";
            std::string buffer(os.str());
            str += buffer;
          }
        }

        //std::cout << "number sons: " << nSons << std::endl;
        for (i = 0; i < nSons; i++) {
            if ((nRetCode = lasrxNLPGetSon(hInstance, hElement, i, &hSon)) != LASRX_RETCODE_OK) {
                //std::cout << "no son\n";
                __ReportLasrxError("__GetSemanticLisp", hInstance, "lasrxNLPGetSon", nRetCode);
                return -1;
            }
            else {
                __GetSemanticLisp(hInstance, hSon, nIndent+1, str);
            }
        }

        if( nSons == 0 )
        {
          //fprintf(stdout, ")");
	  str += ")";
        }
        else
        {
          if(eszName != NULL)
          {
            //fprintf(stdout, "))");
    	    str += "))";
          }
        }


    }
    return 0;
}


static int __GetSemanticTopic(lasrxHandleType hInstance, lasrxHandleType hElement, int nIndent, std::string &str)
{
    lasrxResultType nRetCode = LASRX_RETCODE_OK;
    lasrxIntType nType, nSons;
    lasrxFloatType fConfidence;
    lasrxEncodedStringType eszName = NULL, eszValue = NULL;
    lasrxHandleType hSon;
    if ((nRetCode = lasrxNLPGetElementDetails(hInstance, hElement, &nType, &fConfidence, &eszName)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetSemanticLisp", hInstance, "lasrxNLPGetElementDetails", nRetCode);
        return -1;
    }
    else if ((nRetCode = lasrxNLPGetNumberOfSons(hInstance, hElement, &nSons)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetSemanticLisp", hInstance, "lasrxNLPGetNumberOfSons", nRetCode);
        return -1;
    }
    else if ((nRetCode = lasrxNLPHandleToBuffer(hInstance, hElement, LASRX_FORMAT_TEXT, &eszValue)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetSemanticLisp", hInstance, "lasrxNLPHandleToBuffer", nRetCode);
        return -1;
    }
///////////////////////
    if ((nRetCode = lasrxNLPGetSon(hInstance, hElement, 0, &hSon)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetSemanticLisp", hInstance, "lasrxNLPGetSon", nRetCode);
        return -1;
    }
/////////////////////

    if ((nRetCode = lasrxNLPGetElementDetails(hInstance, hSon, &nType, &fConfidence, &eszName)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetSemanticLisp", hInstance, "lasrxNLPGetElementDetails", nRetCode);
        return -1;
    }
    else {
      str = (char*)eszName;
    }
    return 0;
}





static int __GetRecognitionResults(lasrxHandleType hInstance, int nMaxHypos) {
    lasrxResultType nRetCode = LASRX_RETCODE_OK;
    lasrxIntType nNumHypos, nInterpretations;
    int i;
    lasrxEncodedStringType eszRORule = NULL;
    lasrxStringType szROName;
    lasrxIntType nCPU, nStart, nEnd, nRejection;
    lasrxFloatType fSNR;
    /*fprintf(stdout, "\nAcoustic results:\n");
    if ((nRetCode = lasrxRRGetRONameAndRule(hInstance, &szROName, &eszRORule)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetRecognitionResults", hInstance, "lasrxRRGetRONameAndRule", nRetCode);
        return -1;
    }
    fprintf(stdout, "ROName.RORule:   %s.%s\n", szROName, eszRORule);
    lasrxFree(szROName);
    lasrxFree(eszRORule);
    if ((nRetCode = lasrxRRGetCPUTime(hInstance, &nCPU)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetRecognitionResults", hInstance, "lasrxRRGetRONameAndRule", nRetCode);
        return -1;
    }
    fprintf(stdout, "CPU time:        %d\n", nCPU);
    if ((nRetCode = lasrxRRGetSpeechLimits(hInstance, LASRX_UNITS_FRAMES, &nStart, &nEnd)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetRecognitionResults", hInstance, "lasrxRRGetSpeechLimits", nRetCode);
        return -1;
    }
    fprintf(stdout, "Start-End frame: %d-%d\n", nStart, nEnd);
    if ((nRetCode = lasrxRRGetRejectionAdvice(hInstance, &nRejection)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetRecognitionResults", hInstance, "lasrxRRGetRejectionAdvice", nRetCode);
        return -1;
    }
    switch (nRejection) {
    case LASRX_FALSE:
        fprintf(stdout, "Rejection flag:  LASRX_FALSE\n");
        break;
    case LASRX_REJECTION_NOMATCH:
        fprintf(stdout, "Rejection flag:  LASRX_REJECTION_NOMATCH\n");
        break;
    default:
        fprintf(stdout, "Rejection flag:  UNKNOWN (%d)\n", nRejection);
        break;
    }
    if ((nRetCode = lasrxRRGetSignalToNoiseRatio(hInstance, &fSNR)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetRecognitionResults", hInstance, "lasrxRRGetSignalToNoiseRatio", nRetCode);
        return -1;
    }
    fprintf(stdout, "SNR:             %f\n", fSNR);*/


    lasrxFloatType fConfidence, fLikely;
    lasrxEncodedStringType eszString;
    lasrxIntType nWords;
    i=0;
    std::string semantic_str = "";
    std::string topic = "";
    lasrxEncodedStringType eszValue = NULL, eszName = NULL;
    lasrxIntType nType, nSons;
    lasrxHandleType hElement = NULL;
    std::string recog_text;


    if ((nRetCode = lasrxRRGetHypothesisConfidence(hInstance, i, &fConfidence)) != LASRX_RETCODE_OK) {
       __ReportLasrxError("__GetRecognitionResults", hInstance, "lasrxRRGetHypothesisConfidence", nRetCode);
       return -1;
    }
    //fprintf(stdout, " [%02d] Confidence:      %f\n", i, fConfidence);

    if ((nRetCode = lasrxRRGetHypothesisAcousticScore(hInstance, i, &fLikely)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetRecognitionResults", hInstance, "lasrxRRGetHypothesisAcousticScore", nRetCode);
        return -1;
    }
    //fprintf(stdout, " [%02d] Likelyhood:      %f\n", i, fLikely);


    if ((nRetCode = lasrxRRGetHypothesisString(hInstance, i, &eszString)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetRecognitionResults", hInstance, "lasrxRRGetHypothesisString", nRetCode);
        return -1;
    }
    //fprintf(stdout, " [%02d] String:          %s\n", i, eszString);

    recog_text = (char*)eszString;


    if ((nRetCode = lasrxNLPComputeInterpretations(hInstance, i, &nInterpretations)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetRecognitionResults", hInstance, "lasrxNLPComputeInterpretations", nRetCode);
        return -1;
    }

    //__PrintSemanticTree(hInstance, NULL, 0);


    __GetSemanticLisp(hInstance, NULL, 0, semantic_str);

    ///////////////////////////////////////
    // remove " from regcog string
    //////////////////////////////////////
    std::string tmp;
    for(unsigned int i=0;i<semantic_str.size();i++)
    {
       if(semantic_str[i]!='"')
       {
         tmp+=semantic_str[i];
       }
    }
    semantic_str = tmp;


    __GetSemanticTopic(hInstance, NULL, 0, topic);

    std::cout << "semantic:   " << semantic_str << std::endl;
    std::cout << "confidence: " << fConfidence << std::endl;
    std::cout << "text:       " << recog_text << std::endl;
    std::cout << "topic:      " << topic << std::endl;

    CommSpeechObjects::SpeechInputEventState speechInputEventState;

    speechInputEventState.set_semantic(semantic_str);
    speechInputEventState.set_confidence(fConfidence);
    speechInputEventState.set_text(recog_text);
    speechInputEventState.set_topic(topic);

    std::cout << "before put(speechInputEventState)\n";
    COMP->eventServer->put(speechInputEventState);
    std::cout << "after put(speechInputEventState)\n";

    if ((nRetCode = lasrxNLPReleaseInterpretation(hInstance)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__GetRecognitionResults", hInstance, "lasrxNLPReleaseInterpretation", nRetCode);
        return -1;
    }
    std::cout << "after lasrxNLPReleaseInterpretation\n";
    lasrxFree(eszString);
    std::cout << "after lasrxFree\n";
    return 0;
}



/******************************************************************************/
/* Builds grammar                                                             */
/******************************************************************************/
static int __BuildRO(lasrxHandleType hInstance, char *szSource, char *szName) {
    lasrxResultType nRetCode = LASRX_RETCODE_OK;
    lasrxStringType szRPName = LASRX_RPNAME_DEFAULT;
    lasrxStringType szOpId;
    if ((nRetCode = lasrxOTFSetGrammarROGenerationDirectives(hInstance, szSource, NULL, LASRX_RPNAME_DEFAULT, NULL, 0, NULL, LASRX_FALSE, szName, NULL)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__BuildRO", hInstance, "lasrxOTFSetGrammarROGenerationDirectives", nRetCode);
        return -1;
    }
    if ((nRetCode = lasrxOTFGenerateRO(hInstance, LASRX_RUNMODE_BLOCKING, &szOpId)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__BuildRO", hInstance, "lasrxOTFGenerateRO", nRetCode);
        return -1;
    }
    fprintf(stdout, "\nGenerate started: ID %s, retcode %d\n", szOpId, nRetCode);
    lasrxFree(szOpId);
    return nRetCode;
}

/******************************************************************************/
/* Deletes grammar                                                            */
/******************************************************************************/
static int __DeleteRO(lasrxHandleType hInstance, char *szName) {
    lasrxResultType nRetCode = LASRX_RETCODE_OK;
    char szCompleteName[256];
    sprintf(szCompleteName, "%s/%s", LASRX_RPNAME_DEFAULT, szName);
    if ((nRetCode = lasrxOTFDeleteRO(hInstance, szCompleteName)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__DeleteRO", hInstance, "lasrxOTFDeleteRO", nRetCode);
    }
    return nRetCode;
}

/******************************************************************************/
/* Perform recognition                                                        */
/******************************************************************************/
static int __Recog(void *hInstance, char *szName, void *eszRule, char *szDumpFile, LASRX_EX_DLL_POINTERS *pDll, void *hAudio) {
    lasrxResultType nRetCode = LASRX_RETCODE_OK;
    lasrxEncodedStringType eszRORule = NULL;
    lasrxStringType szOpId;
    char szCompleteName[256];
    sprintf(szCompleteName, "%s/%s", LASRX_RPNAME_DEFAULT, szName);
    if ((nRetCode = lasrxClearROs(hInstance)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__Recog", hInstance, "lasrxClearROs", nRetCode);
        return -1;
    }
    if ((nRetCode = lasrxAddRO(hInstance, szCompleteName, eszRule)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__Recog", hInstance, "lasrxAddRO", nRetCode);
        return -1;
    }
    if ((nRetCode = lasrxSetAudioDumpFileName(hInstance, szDumpFile)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__Recog", hInstance, "lasrxSetAudioDumpFileName", nRetCode);
        return -1;
    }

    std::cout << "SVC: vor state->acquire('active')\n";
    COMP->stateServer->acquire("active");

    usleep(100000);

    std::cout << "SVC: trying : state->release('active')\n";
    COMP->stateServer->release("active");
    std::cout << "SVC: state->release('active')\n";


    if ((nRetCode = lasrxRecog(hInstance, LASRX_RUNMODE_BLOCKING, &szOpId)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("__Recog", hInstance, "lasrxRecog", nRetCode);
        return -1;
    }
    fprintf(stdout, "\nRecognition started: ID %s, retcode %d\n", szOpId, nRetCode);
    __GetRecognitionResults(hInstance, 10);
    lasrxFree(szOpId);
    return nRetCode;
}

