# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# ****************************************************************************
# Updated by Infoscitex, a DCS Company.
# ****************************************************************************
project(wsf_util)

file(GLOB SRCS *.?pp)

add_library(${PROJECT_NAME} SHARED ${SRCS})

target_link_libraries(${PROJECT_NAME} PUBLIC util ${SWDEV_THREAD_LIB} ${SWDEV_DL_LIB})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Set the subfolder that the target will appear under in an IDE's solution explorer.
# This isn't actually related to any file system folders.
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "core")
