SERVER=0
GUI=0
BIG_XRAY=0
LASER=0
VACUUM=0

DEBUG=0
BUILDDIR="build"
CLEAN=0
REBUILD=0
CMAKE_PRE=""
CMAKE_POST=""

usage() { echo -e "
Usage: $0 [x] [v] [l] [s] [-c] [-b] [g] [e] [-j] <Number of threads>
-[no option]: only make
 -s: server only, otherwise client
 -x: xray box
 -v: vacuum box
 -l: laser box
 -c: Clean
 -b: Builds/Rebuilds CMake files normal mode
 -g: Build/Rebuilds only gui
 -e: Debug mode
 -j: Number of threads to compile through
 " ; exit 1; }

 while getopts ":sxvlbcj:ge" opt ; do
	case $opt in
	s) 
        SERVER=1
		REBUILD=1
		;;	
	x) 
        BIG_XRAY=1
		REBUILD=1
		;;
	v) 
        VACUUM=1
		REBUILD=1
		;;
	l) 
        LASER=1
		REBUILD=1
		;;
	b) 
		echo "Building of CMake files Required"
		REBUILD=1
		;;
	c) 
		echo "Clean Required"
		CLEAN=1
		;;
	j) 
		echo "Number of compiler threads: $OPTARG" 
		COMPILERTHREADS=$OPTARG
		;;
	g) 
		echo "Compiling Options: GUI" 
		GUI=1
		REBUILD=1
		;;  
	e)
		echo "Compiling Options: Debug" 
		DEBUG=1
		;;
  \?)
    echo "Invalid option: -$OPTARG" 
		usage
    exit 1
   	;;
  :)
    echo "Option -$OPTARG requires an argument."
		usage
    exit 1
    ;;	
	esac
done

#build dir doesnt exist
if [ ! -d "$BUILDDIR" ] ; then
	echo "No Build Directory. Building of Cmake files required"
	mkdir $BUILDDIR;
	REBUILD=1
else
	#rebuild not requested, but no makefile
	if [ $REBUILD -eq 0 ] && [ ! -f "$BUILDDIR/Makefile" ] ; then
		echo "No Makefile. Building of Cmake files required"
		REBUILD=1
	fi
fi

#Debug
if [ $DEBUG -eq 1 ]; then
	CMAKE_POST+=" -DCMAKE_BUILD_TYPE=Debug "
# CMAKE_POST+=" -DCMAKE_BUILD_TYPE=Debug -DSLS_USE_SANITIZER=ON "
	echo "Debug Option enabled"
fi 

# server only
if [ $SERVER -eq 1 ]; then
	CMAKE_POST+=" -DUSE_SERVER=ON "
	echo "Enabling Compile Option: Server Only"
fi

#big xraybox
if [ $BIG_XRAY -eq 1 ]; then
	CMAKE_POST+=" -DUSE_BIG_XRAY=ON "
	echo "Enabling Compile Option: Big Xray Box"
fi

#laser box
if [ $LASER -eq 1 ]; then
	CMAKE_POST+=" -DUSE_LASER=ON "
	echo "Enabling Compile Option: Laser Box"
fi

#vacuum box
if [ $VACUUM -eq 1 ]; then
	CMAKE_POST+=" -DUSE_VACUUM=ON "
	echo "Enabling Compile Option: Vacuum Box"
fi

#gui
if [ $GUI -eq 1 ]; then
	CMAKE_POST+=" -DUSE_GUI=ON "
	echo "Enabling Compile Option: GUI"
fi


#enter build dir
cd $BUILDDIR;
echo "in "$PWD


#cmake
if [ $REBUILD -eq 1 ]; then
	rm -f CMakeCache.txt
if [ $SERVER -eq 0 ]; then
	BUILDCOMMAND="$CMAKE_PRE cmake3 $CMAKE_POST .."
else
	BUILDCOMMAND="$CMAKE_PRE cmake $CMAKE_POST .."
fi
	echo $BUILDCOMMAND
	eval $BUILDCOMMAND
fi

#make clean
if [ $CLEAN -eq 1 ]; then
	make clean;
fi


#make
if [ $COMPILERTHREADS -gt 0 ]; then
	BUILDCOMMAND="make -j$COMPILERTHREADS"
	echo $BUILDCOMMAND
	eval $BUILDCOMMAND
else
	make
fi

