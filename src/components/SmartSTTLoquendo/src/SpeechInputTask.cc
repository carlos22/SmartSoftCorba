//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.0
// The SmartSoft Toolchain has been developed by:
//
// ZAFH Servicerobotic Ulm
// Christian Schlegel (schlegel@hs-ulm.de)
// University of Applied Sciences
// Prittwitzstr. 10
// 89075 Ulm (Germany)
//
// Information about the SmartSoft MDSD Toolchain is available at:
// smart-robotics.sourceforge.net
//
// This file is generated once. Modify this file to your needs.
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------

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

#include "SpeechInputTask.hh"
#include "gen/SmartSTTLoquendo.hh"
#include "loquendo.cc"
#include <iostream>

SpeechInputTask::SpeechInputTask()
{
	std::cout << "constructor SpeechInputTask\n";
}

int SpeechInputTask::svc()
{

    char szInputBuffer[128];
    /* DLL pointers                                                           */
    LASRX_EX_DLL_POINTERS tDll;
    /* Audio channel                                                          */
    void *hAudio;
    /* Loquendo ASR handles                                                   */
    lasrxHandleType hSession, hInstance;
    lasrxResultType nRetCode = LASRX_RETCODE_OK;
    lasrxStringType szVersionString = NULL;
    lasrxIntType nVersionInt;
    lasrxStringType szSystemInfo;
    lasrxStringType szId;
    /* Creates a session                                                      */
    if ((nRetCode = lasrxNewSession(&COMP->ini.loquendo.session[0], NULL, &hSession)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("main", NULL, "lasrxNewSession", nRetCode);
        return -1;
    }
    fprintf(stdout, "Created session %08X\n", hSession);
    /* Set audio mode                                                         */
    if ((nRetCode = lasrxSetAudioMode(hSession, LASRX_EX_AUDIO_MODE)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("main", hSession, "lasrxSetAudioMode", nRetCode);
        lasrxDeleteSession(hSession);
        return -1;
    }
    /* Get system information                                                 */
    if ((nRetCode = lasrxGetVersion(&szVersionString, &nVersionInt)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("main", hSession, "lasrxGetVersion", nRetCode);
        lasrxDeleteSession(hSession);
        return -1;
    }
    fprintf(stdout, "Version: %s (0x%08X)\n", szVersionString, nVersionInt);
    lasrxFree(szVersionString);
    if ((nRetCode = lasrxQuery(hSession, LASRX_FORMAT_TEXT, &szSystemInfo)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("main", hSession, "lasrxQuery", nRetCode);
        lasrxDeleteSession(hSession);
        return -1;
    }
    fprintf(stdout, "System Info: %s\n", szSystemInfo);
    lasrxFree(szSystemInfo);
    if ((nRetCode = lasrxNewInstance(hSession, NULL, LASRX_FALSE, &hInstance)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("main", hSession, "lasrxNewInstance", nRetCode);
        lasrxDeleteSession(hSession);
        return -1;
    }

    COMP->hInstance_global = hInstance;
    if ((nRetCode = lasrxGetInstanceId(hInstance, &szId)) != LASRX_RETCODE_OK) {
        __ReportLasrxError("main", hInstance, "lasrxGetInstanceId", nRetCode);
        lasrxDeleteInstance(hInstance);
        lasrxDeleteSession(hSession);
        return -1;
    }
    fprintf(stdout, "\nCreated instance %08X: %s\n", hInstance, szId);
    lasrxFree(szId);
    if ((nRetCode = lasrxSetCallbackGetEvent(hInstance, __GetEvent)) != LASRX_RETCODE_OK) {
		__ReportLasrxError("main", hInstance, "lasrxSetCallbackGetEvent", nRetCode);
		lasrxDeleteInstance(hInstance);
		lasrxDeleteSession(hSession);
		return -1;
	    }
	    if ((nRetCode = lasrxSetAudioGetEvent(hInstance, __GetAudioEvent)) != LASRX_RETCODE_OK) {
		__ReportLasrxError("main", hInstance, "lasrxSetAudioGetEvent", nRetCode);
		lasrxDeleteInstance(hInstance);
		lasrxDeleteSession(hSession);
		return -1;
	    }
	    if ((nRetCode = __BuildRO(hInstance, &COMP->ini.loquendo.grammar[0], LASRX_EX_RO_NAME)) != 0) {
		__ReportError("main", "ERROR building recognition object\n");
		lasrxDeleteInstance(hInstance);
		lasrxDeleteSession(hSession);
		return -1;
	    }
	    if ((nRetCode = __LoadAudioSource(hInstance, LASRX_EX_AUDIO_SOURCE_LIBRARY, &tDll, &hAudio)) != 0) {
		__ReportError("main", "ERROR setting audio source\n");
		lasrxDeleteInstance(hInstance);
		lasrxDeleteSession(hSession);
		return -1;
	    }

	    ////////////////////////

	    COMP->cvRecog.blockingUser(false);

            ///////////////////////
	    do
            {

	     //
	     // just if state is active
	     CHS::StatusCode          status;

	     COMP->cvRecog.wait();
	     COMP->recLock.acquire();

             if ((nRetCode = __Recog(hInstance, LASRX_EX_RO_NAME, LASRX_EX_RO_RULE, NULL, &tDll, hAudio)) != 0)
             {
               __ReportError("main", "ERROR in recognition\n");
             }

             COMP->recLock.release();
            } while (1);


    if ((nRetCode = __DeleteRO(hInstance, LASRX_EX_RO_NAME)) != 0) {
        __ReportError("main", "ERROR deleting recognition object\n");
        lasrxDeleteInstance(hInstance);
        lasrxDeleteSession(hSession);
        return -1;
    }
    lasrxDeleteInstance(hInstance);
    lasrxDeleteSession(hSession);

    return 0;


}