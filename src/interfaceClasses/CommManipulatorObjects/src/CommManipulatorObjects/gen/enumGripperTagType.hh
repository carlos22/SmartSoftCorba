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

#ifndef _COMMMANIPULATOROBJECTS_ENUM_HH_GRIPPERTAGTYPE
#define _COMMMANIPULATOROBJECTS_ENUM_HH_GRIPPERTAGTYPE

#include <CommManipulatorObjects/gen/enumGripperTagTypeC.hh>
#include <string>
#include <ostream>

namespace CommManipulatorObjects
{

class GripperTagType
{
public:
	enum literal
	{

		GRIPPER_CLOSE = CommManipulatorObjectsIDL::LITERAL_GRIPPER_CLOSE,

		GRIPPER_OPEN = CommManipulatorObjectsIDL::LITERAL_GRIPPER_OPEN,

		SET_ACCELERATION = CommManipulatorObjectsIDL::LITERAL_SET_ACCELERATION,

		SET_CURRENT = CommManipulatorObjectsIDL::LITERAL_SET_CURRENT,

		SET_SPEED = CommManipulatorObjectsIDL::LITERAL_SET_SPEED
	};

	GripperTagType()
	{
	}

	GripperTagType(literal t)
	{
		value = t;
	}

	operator literal()
	{
		return value;
	}

	GripperTagType(CommManipulatorObjectsIDL::ENUM_GripperTagType t)
	{
		value = (literal) t;
	}

	bool operator ==(GripperTagType& t)
	{
		return this->value == t.value;
	}

	CommManipulatorObjectsIDL::ENUM_GripperTagType get_value_idl()
	{
		return (CommManipulatorObjectsIDL::ENUM_GripperTagType) value;
	}

	literal get_value() const
	{
		return value;
	}

	std::string get_string() const
	{
		switch (value)
		{

		case GRIPPER_CLOSE:
			return "GripperTagType::GRIPPER_CLOSE";
			break;

		case GRIPPER_OPEN:
			return "GripperTagType::GRIPPER_OPEN";
			break;

		case SET_ACCELERATION:
			return "GripperTagType::SET_ACCELERATION";
			break;

		case SET_CURRENT:
			return "GripperTagType::SET_CURRENT";
			break;

		case SET_SPEED:
			return "GripperTagType::SET_SPEED";
			break;

		default:
			return "UNKNOWN!";
			break;
		}
	}

private:
	literal value;
};

inline std::ostream &operator<<(std::ostream &os, const GripperTagType &e)
{
	os << e.get_string();
	return os;
}

}
#endif
