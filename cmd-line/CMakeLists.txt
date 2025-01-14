PROJECT( aruspix )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

# Uncomment this to build for i386
#SET(CMAKE_OSX_ARCHITECTURES i386 CACHE STRING "change to needed architecture for a smaller library" FORCE)

SET( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}" )

SET( LINUX_LIBRARIES "-luuid")	  
FIND_PACKAGE(wxWidgets COMPONENTS base )

OPTION(imDir "/usr")
FIND_LIBRARY(IM_LIBRARY im "${imDir}/lib")
FIND_LIBRARY(IM_PROCESS_LIBRARY im_process "${imDir}/lib")
FIND_LIBRARY(IM_FFTW_LIBRARY im_fftw "${imDir}/lib")

OPTION(torchDir "/usr")
FIND_LIBRARY(TORCH_LIBRARY torch "${torchDir}/lib")

SET( aruspixSources ${aruspixSources}

	../src/cmdline/main.cpp

	../src/app/axapp.cpp
	../src/app/axfile.cpp
	../src/app/axprocess.cpp

	../src/im/imbrink3classes.cpp
	../src/im/imbrink.cpp
	../src/im/imext.cpp
	../src/im/imkmeans.cpp
	../src/im/imoperator.cpp
	../src/im/impage.cpp
	../src/im/imregister.cpp
	../src/im/imstaff.cpp
	../src/im/imstaffsegment.cpp
	
	../src/ml/mldecoder.cpp
	../src/ml/mldecoderpage.cpp
	../src/ml/mldecoderstaff.cpp

	../src/mus/musapp.cpp
	../src/mus/musbarline.cpp 
	../src/mus/musbboxdc.cpp 
	../src/mus/musbeam.cpp
	../src/mus/musclef.cpp 
	../src/mus/musdc.cpp 
	../src/mus/musdoc.cpp	
	../src/mus/musdurationinterface.cpp 
	../src/mus/musio.cpp 
	../src/mus/musiodarms.cpp
	../src/mus/musiomei.cpp
	../src/mus/musiomlf.cpp 
	../src/mus/musiomusxml.cpp
	../src/mus/musiopae.cpp 
	../src/mus/musiowwg.cpp
	../src/mus/muskeysig.cpp
	../src/mus/muslayer.cpp
	../src/mus/muslayerelement.cpp 
	../src/mus/musleipzigbbox.cpp
	../src/mus/musmensur.cpp
	../src/mus/musneume.cpp
	../src/mus/musneumesymbol.cpp 
	../src/mus/musnote.cpp 
	../src/mus/musmlfdic.cpp
	../src/mus/musobject.cpp 
	../src/mus/muspage.cpp 
	../src/mus/muspitchinterface.cpp 
	../src/mus/muspositioninterface.cpp 
	../src/mus/musrc.cpp 
	../src/mus/musrc_beam.cpp  
	../src/mus/musrc_bezier.cpp 
	../src/mus/musrc_element.cpp 
	../src/mus/musrc_graph.cpp 
	../src/mus/musrc_page.cpp
	../src/mus/musrc_neume.cpp
	../src/mus/musrc_tuplet.cpp
	../src/mus/musrest.cpp 
	../src/mus/musslur.cpp
	../src/mus/musstaff.cpp 
	../src/mus/mussvgdc.cpp 
	../src/mus/mussymbol.cpp 
	../src/mus/mussystem.cpp 
	../src/mus/mustie.cpp
	../src/mus/mustuplet.cpp
	../src/mus/muswwg.cpp

	../src/recognition/rec.cpp
	../src/recognition/recfile.cpp
	../src/recognition/recmodels.cpp

	../src/tinyxml/tinystr.cpp
	../src/tinyxml/tinyxml.cpp
	../src/tinyxml/tinyxmlerror.cpp
	../src/tinyxml/tinyxmlparser.cpp

)

INCLUDE_DIRECTORIES(
	../src
	../src/app
	../src/recognition
	../src/rtmidi
	../src/tinyxml
	../src/im
	../src/mus
	../src/ml
	${wxWidgets_INCLUDE_DIRS}
	${imDir}/include
	${torchDir}/include
	/usr/include
)

ADD_DEFINITIONS(
	-DAX_RECOGNITION
	-DAX_CMDLINE
	-D_FILE_OFFSET_BITS=64	
	-D_LARGE_FILES	
	-DwxDEBUG_LEVEL=0
	-pthread
	-Wno-write-strings
	-Wno-format-security
	-Wno-non-pod-varargs
	)

ADD_EXECUTABLE( aruspix-cmdline ${aruspixSources} )

TARGET_LINK_LIBRARIES( aruspix-cmdline
	-lz
	-lpthread
	${TORCH_LIBRARY}
	"${OSX_LIBRARIES}"
	"${LINUX_LIBRARIES}"
	${wxWidgets_LIBRARIES}
	${IM_LIBRARY}
	${IM_PROCESS_LIBRARY}
	${IM_FFTW_LIBRARY}
)

install (TARGETS aruspix-cmdline DESTINATION bin)
INSTALL(DIRECTORY data/svg/ DESTINATION share/aruspix/svg FILES_MATCHING PATTERN "*.xml")
