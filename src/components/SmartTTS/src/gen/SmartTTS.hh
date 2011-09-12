//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.4
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
// Please do not modify this file. It will be re-generated
// running the code generator.
//--------------------------------------------------------------------------

#ifndef _SMARTTTS_HH
#define _SMARTTTS_HH

#include <iostream>
#include "smartSoft.hh"
#include "../SmartTTSCore.hh"
// include communication objects
#include <CommBasicObjects/commPropertySet.hh>
#include <CommSpeechObjects/commSpeechOutputMessage.hh>
// include tasks	

#include "../SpeechTask.hh"
// include handler
#include "../CompHandler.hh"
#include "../SpeechQueryHandler.hh"
#include "../SpeechSendHandler.hh"

#define COMP SmartTTS::instance()

class SmartTTS: public SmartTTSCore
{
private:
	static SmartTTS _smartTTS;

	// constructor
	SmartTTS();

	// copy-constructor
	SmartTTS(const SmartTTS& cc);

	// destructor
	~SmartTTS()
	{
	}
	;

	// load parameter from ini file
	void loadParameter(int argc, char *argv[]);

	// instantiate handler
	CompHandler compHandler;
	SpeechQueryHandler speechQueryHandler;
	SpeechSendHandler speechSendHandler;

	// ThreadQueueHandler 


public:
	// component
	CHS::SmartComponent *component;

	// create mutex


	// create condition mutex


	// instantiate tasks

	SpeechTask speechTask;

	// ports
	CHS::QueryServer<CommSpeechObjects::CommSpeechOutputMessage,
			CommBasicObjects::CommPropertySet> *queryServer;
	CHS::SendServer<CommSpeechObjects::CommSpeechOutputMessage> *sendServer;

	void init(int argc, char *argv[]);
	void run();

	// return singleton instance
	static SmartTTS* instance()
	{
		return (SmartTTS*) &_smartTTS;
	}

	// ini parameter
	struct ini_ini
	{

		// component struct
		struct ini_component
		{
			// the name of the component
			std::string name;
		} component;

		struct ini_queryServer
		{
			std::string serviceName;
		} queryServer;

		struct ini_sendServer
		{
			std::string serviceName;
		} sendServer;

		struct ini_text2speech
		{

			/**
			 * name of executable to be used for speech output
			 * (message text is piped to stdin of that process)
			 * e.g.:
			 * /bin/cat
			 * /usr/share/mbrola/say
			 * /usr/bin/festival --tts
			 * (echo won't work)
			 */
			std::string player;
		} text2speech;
	} ini;

};
#endif