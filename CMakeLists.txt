# include libraries
include_directories (${aia_SOURCE_DIR}/3rdparty/aiacommon)
include_directories (${aia_SOURCE_DIR}/3rdparty/ucascommon)

# find sources
file(GLOB src *.h *.hpp *.cpp)

# fix va_start error in VS 14+
add_definitions(-D_CRT_NO_VA_START_VALIDATION)

# create executable from sources
add_executable(myproject ${src})

# link the executable to other modules / libraries 
target_link_libraries(myproject aiacommon ucascommon ${OpenCV_LIBS})
