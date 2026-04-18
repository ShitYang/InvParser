# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

project(wsf_parser)

include(GenerateExportHeader)

FILE(GLOB HDRS source/*.hpp source/grammar_parse/*.h ${PROJECT_BINARY_DIR}/*.h ${PROJECT_BINARY_DIR}/*.hpp)
FILE(GLOB SRCS source/*.cpp source/grammar_parse/*.cpp)

add_library(${PROJECT_NAME} SHARED ${HDRS} ${SRCS})

generate_export_header(${PROJECT_NAME} BASE_NAME wsf_parser)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source/grammar_parse ${CMAKE_CURRENT_SOURCE_DIR}/source ${PROJECT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC util wsf_util)

# add_subdirectory(test)
