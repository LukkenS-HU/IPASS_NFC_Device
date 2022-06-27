add_library(PNHelpers INTERFACE)

target_include_directories(PNHelpers INTERFACE include)
target_link_libraries(PNHelpers INTERFACE PN532)
