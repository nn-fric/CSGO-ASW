cmake_minimum_required( VERSION 4.1 )
project( CSGO-ASW )

set( CMAKE_CXX_STANDARD 20 )
set( SDL_STATIC ON )
set( SDL_SHARED OFF )

add_subdirectory( SDL )

add_executable( csgo
		# launcher
		main.cpp

		# tier0
		tier0/commandline.cpp

		# tier1
		tier1/strtools.cpp
)

target_include_directories( csgo PRIVATE ./public )
target_link_libraries( csgo PRIVATE SDL3::SDL3-static )