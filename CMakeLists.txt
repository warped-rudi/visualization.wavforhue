project(visualization.wavforhue)

cmake_minimum_required(VERSION 2.6)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
#find_package(platform REQUIRED)
find_package(p8-platform REQUIRED)
find_package(kodiplatform REQUIRED)
find_package(CURL REQUIRED)
#find_package(yajl REQUIRED) #why is it so damn difficult to include Kodi's libraries???

set(WAVFORHUE_SOURCES src/Main.cpp
                      src/FFT.cpp
                      src/WavforHue.cpp
                      src/WavforHue_Thread.cpp
                      src/jsoncpp.cpp
                      src/WavforHue_Kodi.cpp
                      src/WavforHue_Settings.hpp
                      src/Main.h
                      src/FFT.h
                      src/WavforHue.h
                      src/WavforHue_Thread.h
                      src/WavforHue_Kodi.h)

if(WIN32)
  set(SHADER_FILES src/DefaultVertexShader.hlsl
                   src/DefaultPixelShader.hlsl)
  set(SHADER_INCLUDES)
  foreach( SHADER_FILE ${SHADER_FILES} )
    get_filename_component( FileName ${SHADER_FILE} NAME_WE )
    if(${FileName} MATCHES "(.+Vertex.+)")
      set(ShaderType vs)
    elseif(${FileName} MATCHES "(.+Pixel.+)")
      set(ShaderType ps)
    else()
      message(FATAL_ERROR ${SHADER_FILE}": Unknown shader type.")
    endif()
    set( IncFile src/${FileName}.inc )
    list( APPEND SHADER_INCLUDES ${IncFile} )
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${IncFile}
      COMMAND fxc /nologo /T${ShaderType}_4_0_level_9_1 /Vn ${FileName}Code /Fh ${IncFile} ${SHADER_FILE}
      MAIN_DEPENDENCY ${SHADER_FILE}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      VERBATIM)
	message(STATUS "Running command: fxc /nologo /T${ShaderType}_4_0_level_9_1 /Vn ${FileName}Code /Fh ${IncFile} ${SHADER_FILE}")
	message(STATUS "Using shader file ${SHADER_FILE}")
	message(STATUS "Using working directory ${CMAKE_CURRENT_SOURCE_DIR}")
	message(STATUS "Outputting ${CMAKE_CURRENT_SOURCE_DIR}/${IncFile}")
  endforeach(SHADER_FILE)
  add_custom_target(generate ALL DEPENDS ${SHADER_INCLUDES})
else()
  find_package(OpenGL)
  if(OPENGL_FOUND)
    add_definitions(-DHAS_OPENGL)
    set(DEPLIBS ${OPENGL_LIBRARIES})
  else()
    find_package(OpenGLES2)
    if(OPENGLES2_FOUND)
      set(INCLUDES ${OPENGLES2_INCLUDE_DIR})
      add_definitions(-DHAS_GLES2)
      set(WAVFORHUE_SOURCES src/Main_gles.cpp
                            src/FFT.cpp
                            src/WavforHue.cpp
                            src/WavforHue_Thread.cpp
                            src/VisGUIShader.cpp
                            src/VisMatrixGLES.cpp
                            src/VisShader.cpp
                            src/jsoncpp.cpp
                            src/WavforHue_Kodi.cpp
                            src/WavforHue_Settings.hpp
                            src/Main_gles.h
                            src/FFT.h
                            src/WavforHue.h
                            src/WavforHue_Thread.h
                            src/VisGUIShader.h
                            src/VisMatrixGLES.h
                            src/VisShader.h
                            src/WavforHue_Kodi.h)
      set(DEPLIBS ${OPENGLES2_LIBRARIES})
    else()
      message(FATAL_ERROR "No rendering API found. Bailing")
    endif()
  endif()
endif()

list(APPEND DEPLIBS ${CURL_LIBRARIES} ${platform_LIBRARIES} ${kodiplatform_LIBRARIES} ${YAJL_LIBRARY})

if(WIN32)
  list(APPEND DEPLIBS ws2_32)
endif()

include_directories(${INCLUDES}
                    ${CURL_INCLUDE_DIR}
                    ${KODI_INCLUDE_DIR}
                    ${p8-platform_INCLUDE_DIRS}
                    ${kodiplatform_INCLUDE_DIRS})
#                    ${YAJL_INCLUDE_DIR}
#                    ${platform_INCLUDE_DIRS}           



build_addon(visualization.wavforhue WAVFORHUE DEPLIBS)

include(CPack)
