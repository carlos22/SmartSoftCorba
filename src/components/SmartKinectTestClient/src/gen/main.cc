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

#include <iostream>
#include "SmartKinectTestClient.hh"

SmartKinectTestClient SmartKinectTestClient::_smartKinectTestClient;

int main(int argc, char *argv[])
{
	std::cout << "main...\n";
	SmartKinectTestClient::instance()->init(argc, argv);
	SmartKinectTestClient::instance()->run();
}

