#!/bin/bash

BASE_NAME="";
CURR_DIR=$PWD;

# the script expects exactly one parameter, representing a component-name
if [[ $# -ne 1 ]]
then
	echo "Usage:"
	echo "#./generate_ros_dependencies.sh <ComponentName>"
	echo
	exit 1;
else
	BASE_NAME=$1;
fi

# 1) search for ROS package 
BASE_DIR=`rospack find $BASE_NAME`;

if [ "$BASE_DIR" == "" ]
then
	# package not found -> exit 1
	echo "Check ROS_PACKAGE_PATH!";
	exit 1;
fi

# 2) generate FULL dependency list for the ROS package
DEP_LIST=`rospack depends $BASE_NAME`

# 3) create all include-path compiler flags
echo "# generated compiler include-paths for ROS dependencies of $BASE_NAME component" > $BASE_DIR/src/ros.mk;
# echo "ROS_CXX_FLAGS= -DBT_USE_DOUBLE_PRECISION -DBT_EULER_DEFAULT_ZYX -W -Wall -Wno-unused-parameter -fno-strict-aliasing \\" >> $BASE_DIR/src/ros.mk;
echo "ROS_CXX_FLAGS= -DBT_USE_DOUBLE_PRECISION -DBT_EULER_DEFAULT_ZYX -fno-strict-aliasing \\" >> $BASE_DIR/src/ros.mk;

for DEP in $DEP_LIST 
do
	DEP_DIR=`rospack find $DEP`;
	cd $DEP_DIR;

	# 3.1) search for include paths
	INC_DIRS=`find -type d -name include`;
	for INC_DIR in $INC_DIRS
	do
		INC_PATH=$DEP_DIR;
		INC_PATH+=`echo $INC_DIR | sed "s/\.//g"`;
		echo "-I$INC_PATH \\" >> $BASE_DIR/src/ros.mk;
	done

	# 3.2) if no include path available, search fo src path (directly in the package directory)
	if [ "$INC_DIRS" = "" ]
	then
		SRC_DIR=`find -maxdepth 1 -type d -name src`;
		if [ "$SRC_DIR" != "" ]
		then
			echo "-I$DEP_DIR/src \\" >> $BASE_DIR/src/ros.mk;
		else
			# if package neither consists of an include nor of a src directory, use the package directory as include path
			echo "-I$DEP_DIR \\" >> $BASE_DIR/src/ros.mk;
		fi
	fi
	cd $CURR_DIR;
done


echo "" >> $BASE_DIR/src/ros.mk;
echo "# generated linker flags (libraries directories, libraries and rpath directories)" >> $BASE_DIR/src/ros.mk;
echo "ROS_LD_FLAGS= \\" >> $BASE_DIR/src/ros.mk;

RPATH="-Wl,-rpath,"
LIBRARIES=""

for DEP in $DEP_LIST 
do
	DEP_DIR=`rospack find $DEP`;
	cd $DEP_DIR;
	INC_DIR=`find -maxdepth 1 -type d -name lib`;
	if [ "$INC_DIR" != "" ]
	then
		echo "-L$DEP_DIR/lib \\" >> $BASE_DIR/src/ros.mk;
		RPATH+="$DEP_DIR/lib:";
		cd lib;
		LIBS=`ls lib*.so | sed "s/\.so//g"`
		for LIB in $LIBS
		do
			LIBRARIES+="-l${LIB:3} ";
		done
		cd ..
	fi
	cd $CURR_DIR;
done

echo "$LIBRARIES \\" >> $BASE_DIR/src/ros.mk;
echo "$RPATH" >> $BASE_DIR/src/ros.mk;

echo "File $BASE_DIR/src/ros.mk generated!"
echo "1) include ros.mk in your makefile in $BASE_DIR/src/Makefile"
echo "2) use ROS_CXX_FLAGS for compilation"
echo "3) use ROS_LD_FLAGS for linking"

