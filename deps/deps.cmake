# Set dependences paths
set(DEPS_PATHS
	)
include_directories(${DEPS_PATHS})

# Find other dependences
set(OpenCV_STATIC ON)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Add subdirectory
foreach(DEPS_PATH ${DEPS_PATHS})
	add_subdirectory(${DEPS_PATH})
endforeach()
