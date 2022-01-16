IF(COMMAND cmake_policy)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

include_directories(/usr/local/include/Wt)

cmake_minimum_required(VERSION 3.10)
#find_package(Qt5 COMPONENTS Sql Core Network REQUIRED)
#find_package ( Qt5 COMPONENTS Core REQUIRED)
find_package ( Qt5Core REQUIRED )
find_package ( Qt5Sql REQUIRED )
find_package ( Qt5Network REQUIRED )
find_package ( Qt5Widgets REQUIRED )
find_package ( Qt5Concurrent REQUIRED )

add_subdirectory(QcjData)
SET (WT_CONNECTOR "wthttp" CACHE STRING "Connector used (wthttp or wtfcgi)")
SET(NIXIESTAT_UI_SOURCE
    main.cpp
    DBManager.cpp
    MQTT.cpp
    LocationTab.cpp
    LogTopicHandler.cpp
    NixieStatApplication.cpp
    PingTopicHandler.cpp
    WtApplication.cpp
    ${CMAKE_BINARY_DIR}/moc_MQTT.cpp
    ${CMAKE_BINARY_DIR}/moc_DBManager.cpp
    ${CMAKE_BINARY_DIR}/moc_LogTopicHandler.cpp
    ${CMAKE_BINARY_DIR}/moc_WtApplication.cpp
    ${CMAKE_BINARY_DIR}/moc_PingTopicHandler.cpp
)
SET(NIXIESTAT_UI_TARGET nixiestat_ui.wt)
add_executable(${NIXIESTAT_UI_TARGET} ${NIXIESTAT_UI_SOURCE})
target_include_directories(${NIXIESTAT_UI_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE}/QcjData .)
target_compile_features(${NIXIESTAT_UI_TARGET} PUBLIC cxx_std_17)
target_link_libraries(${NIXIESTAT_UI_TARGET} ${WT_CONNECTOR} wt QcjData Qt5::Sql Qt5::Core Qt5::Network qmqtt)
#target_link_libraries(${NIXIESTAT_UI_TARGET} ${WT_CONNECTOR} wt Qt5::Sql Qt5::Core Qt5::Network qmqtt)

#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/DBManager.h        ${CMAKE_BINARY_DIR}/moc_DBManager.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/LogTopicHandler.h  ${CMAKE_BINARY_DIR}/moc_LogTopicHandler.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/MQTT.h             ${CMAKE_BINARY_DIR}/moc_MQTT.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/WtApplication.h  ${CMAKE_BINARY_DIR}/moc_WtApplication.cpp)
qt5_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/PingTopicHandler.h ${CMAKE_BINARY_DIR}/moc_PingTopicHandler.cpp)




