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
/** Multi media audio source handling routines.                              **/
/**                                                                          **/
/******************************************************************************/
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <alsa/asoundlib.h>

#include "LoqASR.h" 
#include "LoqASRAudioALSA.h"

#define ASRX_MAX_STRING_LENGTH 1024
#define ASRX_MAX_BUFFER_SIZE 8192

typedef struct _tag_audio_alsa_handle {
	snd_pcm_t *pcm_handle;
	snd_async_handler_t *async_handler;
	snd_pcm_format_t format;
	int fragsize;
	int sizeshift;
	int inited;
    char buffer[ASRX_MAX_BUFFER_SIZE];

	lasrxHandleType inst;
	char *data;
    char error_message[ASRX_MAX_STRING_LENGTH];
} _LASRX_AUDIO_ALSA_HANDLE;


#if defined(ASRX_ALSA_CALLBACK)
static void audio_alsa_callback(snd_async_handler_t *handler) {
_LASRX_AUDIO_ALSA_HANDLE *lochnd = (_LASRX_AUDIO_ALSA_HANDLE*) snd_async_handler_get_callback_private(handler);

	int avail = snd_pcm_avail_update(lochnd->pcm_handle);
	if (avail == -EPIPE) {
		snd_pcm_prepare(lochnd->pcm_handle);
		avail = snd_pcm_avail_update(lochnd->pcm_handle);
	}

	if(avail>0) {
		int bytes = avail<<lochnd->sizeshift;
		if(bytes > ASRX_MAX_BUFFER_SIZE) { 
			bytes = ASRX_MAX_BUFFER_SIZE;
			avail = bytes>>lochnd->sizeshift;
		}
	
		int result = snd_pcm_readi(lochnd->pcm_handle, lochnd->buffer, avail);

		/* NOTE: result is in samples, but lasrxAudioStore() deals with bytes (channels*bytes_per_sample) */
		if(result>0)
			lasrxAudioStore(lochnd->inst, lochnd->buffer, result<<lochnd->sizeshift, LASRX_STOREMODE_DONTWAIT);
		else
			fprintf(stderr, "*** audio_alsa_callback: error during read (errno=%d message=%s)\n", result, snd_strerror(result));
	}
}
#endif

static int AUDIO_STDCALL audio_alsa_start(lasrxHandleType inst, void *hnd) {
_LASRX_AUDIO_ALSA_HANDLE *lochnd = (_LASRX_AUDIO_ALSA_HANDLE*)hnd;
snd_pcm_hw_params_t *hwparams=NULL;
int ret=0, buffered_time=500000, SampleRate=0, RealSampleRate=0;
char *errstr = NULL;

	if (lochnd == NULL)
		return LASRX_RETCODE_AUDIO;

	if (lasrxGetSamplingFrequency(inst, &SampleRate) != LASRX_RETCODE_OK) {
		sprintf(lochnd->error_message, "Cannot obtain the Sampling Rate from ASR");
		return LASRX_RETCODE_AUDIO;
	}

	if ((SampleRate != 8000) && (SampleRate != 16000)) {
		sprintf(lochnd->error_message, "Invalid sample rate (%d)", SampleRate);
		return LASRX_RETCODE_AUDIO;
	}

#if defined(ASRX_ALSA_CALLBACK)
	if ((ret = snd_pcm_open (&lochnd->pcm_handle, lochnd->data?lochnd->data:"default", SND_PCM_STREAM_CAPTURE|SND_PCM_ASYNC, 0)) < 0)
		errstr = "Unable to open pcm device";
	
	if ((ret>=0) && ((ret = snd_async_add_pcm_handler (&lochnd->async_handler, lochnd->pcm_handle, audio_alsa_callback, hnd)) < 0))
		errstr = "Unable to open set pcm callback";
#else
	if ((ret = snd_pcm_open (&lochnd->pcm_handle, lochnd->data?lochnd->data:"default", SND_PCM_STREAM_CAPTURE, 0)) < 0)
		errstr = "Unable to open pcm device";
#endif

	if ((ret>=0) && ((ret = snd_pcm_hw_params_malloc(&hwparams)) < 0)) 
		errstr = "Cannot allocate hardware parameter structure";

	if ((ret>=0) && ((ret = snd_pcm_hw_params_any(lochnd->pcm_handle, hwparams)) < 0))
		errstr = "Failed to initialize hwparams";

	if ((ret>=0) && ((ret = snd_pcm_hw_params_set_access(lochnd->pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0))
		errstr = "Error setting access";

	RealSampleRate = SampleRate;
	if ((ret>=0) && ((ret = snd_pcm_hw_params_set_rate_near(lochnd->pcm_handle, hwparams, &RealSampleRate, 0)) < 0))
		errstr = "Error setting rate";
	
	if (RealSampleRate != SampleRate) {
		errstr = "Sampling rate is not supported";
		ret = -1;
	}

	if ((ret>=0) && ((ret = snd_pcm_hw_params_set_channels(lochnd->pcm_handle, hwparams, 1)) < 0))
		errstr = "Error setting channels";

	if (lochnd->format == LASRX_SAMPLES_UL) {
#ifdef DEBUG
		fprintf(stderr, "*** uLaw %d\n", SampleRate);
#endif
		if ((ret>=0) && ((ret = snd_pcm_hw_params_set_format(lochnd->pcm_handle, hwparams, SND_PCM_FORMAT_MU_LAW)) < 0))
			errstr = "Couldn't set sample format to SND_PCM_FORMAT_MU_LAW";
	}
	else if (lochnd->format == LASRX_SAMPLES_AL) {
#ifdef DEBUG
		fprintf(stderr, "*** aLaw %d\n", SampleRate);
#endif
		if ((ret>=0) && ((ret = snd_pcm_hw_params_set_format(lochnd->pcm_handle, hwparams, SND_PCM_FORMAT_A_LAW)) < 0))
			errstr = "Couldn't set sample format to SND_PCM_FORMAT_A_LAW";
	}
	else {
		lochnd->sizeshift = 1;
#ifdef DEBUG
		fprintf(stderr, "*** linear 16bit %d\n", SampleRate);
#endif
		if ((ret>=0) && ((ret = snd_pcm_hw_params_set_format(lochnd->pcm_handle, hwparams, SND_PCM_FORMAT_S16)) < 0))
			errstr = "Couldn't set sample format to SND_PCM_FORMAT_S16";
	}

    if ((ret>=0) && ((ret = snd_pcm_hw_params_set_buffer_time_near(lochnd->pcm_handle, hwparams, &buffered_time, 0)) < 0))
		errstr = "Error setting buffersize";
		
	if ((ret>=0) && ((ret = snd_pcm_hw_params(lochnd->pcm_handle, hwparams)) < 0))
		errstr = "Error setting HW params";

	if(hwparams)
		snd_pcm_hw_params_free(hwparams);

	if(ret<0) {
		sprintf(lochnd->error_message, "%s (error=%d message=%s)\n", errstr, ret, snd_strerror(ret));
		fprintf(stderr, "*** audio_alsa_start: %s", lochnd->error_message);
		return LASRX_RETCODE_AUDIO;
	}

	lochnd->fragsize = SampleRate/5;
	lochnd->error_message[0] = '\0';

	return LASRX_RETCODE_OK;
}

static int AUDIO_STDCALL audio_alsa_stop(lasrxHandleType inst, void *hnd) {
_LASRX_AUDIO_ALSA_HANDLE *lochnd = (_LASRX_AUDIO_ALSA_HANDLE*)hnd;

	if (lochnd == NULL)
		return LASRX_RETCODE_AUDIO;

	snd_pcm_drop(lochnd->pcm_handle);
	snd_pcm_close(lochnd->pcm_handle);
	
	lochnd->pcm_handle = 0;
	lochnd->error_message[0] = '\0';
	return LASRX_RETCODE_OK;
}

static int AUDIO_STDCALL audio_alsa_processing(lasrxHandleType inst, void *hnd) {
_LASRX_AUDIO_ALSA_HANDLE *lochnd = (_LASRX_AUDIO_ALSA_HANDLE*)hnd;
int result = 0;

#if !defined(ASRX_ALSA_CALLBACK)
#if !defined(ASRX_ALSA_LARGEIO)
	result = snd_pcm_readi(lochnd->pcm_handle, lochnd->buffer, lochnd->fragsize);
	
	if (result == -EPIPE) {
		snd_pcm_drop(lochnd->pcm_handle);
		snd_pcm_prepare(lochnd->pcm_handle);
		result = snd_pcm_readi(lochnd->pcm_handle, lochnd->buffer, lochnd->fragsize);
	}

#else
	int avail = snd_pcm_avail_update(lochnd->pcm_handle);
	if (avail == -EPIPE) {
		snd_pcm_prepare(lochnd->pcm_handle);
		avail = snd_pcm_avail_update(lochnd->pcm_handle);
	}

	if(avail>0) {
		int bytes = avail<<lochnd->sizeshift;
		if(bytes > ASRX_MAX_BUFFER_SIZE) { 
			bytes = ASRX_MAX_BUFFER_SIZE;
			avail = bytes>>lochnd->sizeshift;
		}
	
		result = snd_pcm_readi(lochnd->pcm_handle, lochnd->buffer, avail);
	}
#endif
	/* NOTE: fragsize is in samples, but lasrxAudioStore() deals with bytes (channels*bytes_per_sample) */
	if(result>0)
		lasrxAudioStore(lochnd->inst, lochnd->buffer, result<<lochnd->sizeshift, LASRX_STOREMODE_DONTWAIT);
	else
		fprintf(stderr, "*** audio_alsa_processing: error during read (errno=%d message=%s)\n", result, snd_strerror(result));
#endif

    return LASRX_RETCODE_OK;
}

int AUDIO_STDCALL NewHandle(int format, void **hnd) {
_LASRX_AUDIO_ALSA_HANDLE *lochnd;

	*hnd = NULL;

	if ((lochnd = calloc(1, sizeof(_LASRX_AUDIO_ALSA_HANDLE))) == NULL) {
		return LASRX_RETCODE_NO_MORE_MEMORY;
	}

	lochnd->format = format;
	*hnd = lochnd;
	return LASRX_RETCODE_OK;
}

int AUDIO_STDCALL DeleteHandle(void *hnd) {
_LASRX_AUDIO_ALSA_HANDLE *lochnd = (_LASRX_AUDIO_ALSA_HANDLE*)hnd;

	if(lochnd) {
		if(lochnd->pcm_handle)
			audio_alsa_stop(NULL, hnd);

		free(lochnd);
	}

	return LASRX_RETCODE_OK;
}

int AUDIO_STDCALL SetData(void *hnd, char *wavedev) {
_LASRX_AUDIO_ALSA_HANDLE *lochnd = (_LASRX_AUDIO_ALSA_HANDLE*)hnd;

	if (hnd == NULL)
		return LASRX_RETCODE_AUDIO;

	lochnd->data = wavedev;
	return LASRX_RETCODE_OK;
}

int AUDIO_STDCALL Register(void *hnd, void *instance) {
_LASRX_AUDIO_ALSA_HANDLE *lochnd = (_LASRX_AUDIO_ALSA_HANDLE*)hnd;
lasrxResultType ret = LASRX_RETCODE_OK;

	if (hnd == NULL)
		return LASRX_RETCODE_AUDIO;

	if ((ret = lasrxSetAudioStart(instance, audio_alsa_start)) != LASRX_RETCODE_OK) {
		return ret;
	}

	if ((ret = lasrxSetAudioStop(instance, audio_alsa_stop)) != LASRX_RETCODE_OK) {
		return ret;
	}

	if ((ret = lasrxSetAudioProcessing(instance, audio_alsa_processing)) != LASRX_RETCODE_OK) {
		return ret;
	}

	if ((ret = lasrxSetAudioDataPointer(instance, hnd)) != LASRX_RETCODE_OK) {
		return ret;
	}

	lochnd->inst = instance;
	return ret;
}

int AUDIO_STDCALL Unregister(void *hnd) {
_LASRX_AUDIO_ALSA_HANDLE *lochnd = (_LASRX_AUDIO_ALSA_HANDLE*)hnd;
lasrxResultType ret = LASRX_RETCODE_OK;

	if ((ret = lasrxSetAudioStart(lochnd->inst, NULL)) != LASRX_RETCODE_OK) {
		return ret;
	}

	if ((ret = lasrxSetAudioStop(lochnd->inst, NULL)) != LASRX_RETCODE_OK) {
		return ret;
	}

	if ((ret = lasrxSetAudioProcessing(lochnd->inst, NULL)) != LASRX_RETCODE_OK) {
		return ret;
	}

	if ((ret = lasrxSetAudioDataPointer(lochnd->inst, NULL)) != LASRX_RETCODE_OK) {
		return ret;
	}

	lochnd->inst = NULL;
	return ret;
}

char * AUDIO_STDCALL GetData(void *hnd) {
_LASRX_AUDIO_ALSA_HANDLE *lochnd = (_LASRX_AUDIO_ALSA_HANDLE*)hnd;

	if (hnd == NULL)
		return NULL;

	return lochnd->data;
}

char * AUDIO_STDCALL GetErrorMessage(void *hnd) {
_LASRX_AUDIO_ALSA_HANDLE *lochnd = (_LASRX_AUDIO_ALSA_HANDLE*)hnd;

	if (hnd == NULL)
		return "???";

	return lochnd->error_message;
}
