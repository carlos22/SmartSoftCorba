//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain Version 0.10.3
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

#ifndef _COMMSPEECHOBJECTS_ENUM_HH_SPEECHINPUTTAGTYPE
#define _COMMSPEECHOBJECTS_ENUM_HH_SPEECHINPUTTAGTYPE

#include <CommSpeechObjects/gen/enumSpeechInputTagTypeC.hh>
#include <string>
#include <ostream>

namespace CommSpeechObjects
{

class SpeechInputTagType
{
public:
	enum literal
	{

		SPEECH_SET_GRAMMAR = CommSpeechObjectsIDL::LITERAL_SPEECH_SET_GRAMMAR
	};

	SpeechInputTagType()
	{
	}

	SpeechInputTagType(literal t)
	{
		value = t;
	}

	operator literal()
	{
		return value;
	}

	SpeechInputTagType(CommSpeechObjectsIDL::ENUM_SpeechInputTagType t)
	{
		value = (literal) t;
	}

	bool operator ==(SpeechInputTagType& t)
	{
		return this->value == t.value;
	}

	CommSpeechObjectsIDL::ENUM_SpeechInputTagType get_value_idl()
	{
		return (CommSpeechObjectsIDL::ENUM_SpeechInputTagType) value;
	}

	literal get_value() const
	{
		return value;
	}

	std::string get_string() const
	{
		switch (value)
		{

		case SPEECH_SET_GRAMMAR:
			return "SpeechInputTagType::SPEECH_SET_GRAMMAR";
			break;

		default:
			return "UNKNOWN!";
			break;
		}
	}

private:
	literal value;
};

inline std::ostream &operator<<(std::ostream &os, const SpeechInputTagType &e)
{
	os << e.get_string();
	return os;
}

}
#endif
