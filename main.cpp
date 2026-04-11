#include <SDL3/SDL_main.h>
#include <tier0/platform.h>
#include <tier0/icommandline.h>
#include <tier1/strtools.h>


char g_szBasedir[MAX_PATH];

int main( int argc, char* argv[] )
{
	CommandLine()->CreateCmdLine( argc, argv );

	const char *pszBasedir;
	CommandLine()->CheckParm( "-basedir", &pszBasedir ); // I think Valve messed it up, fixed
	strlcpy( g_szBasedir, pszBasedir, MAX_PATH );
	V_FixSlashes( g_szBasedir );

	if( CommandLine()->CheckParm( "-tslist" ) )
	{
		// TODO: implete me
	}

	if( CommandLine()->CheckParm( "-buildcubemaps") )
	{
		CommandLine()->AppendParm( "-nosound", nullptr );
		CommandLine()->AppendParm( "-noasync", nullptr );
	}

	// TODO: wtf
	CommandLine()->RemoveParm( "-w" );
	CommandLine()->RemoveParm( "-h" );
	CommandLine()->RemoveParm( "-width" );
	CommandLine()->RemoveParm( "-height" );
	CommandLine()->RemoveParm( "-sw" );
	CommandLine()->RemoveParm( "-startwindowed" );
	CommandLine()->RemoveParm( "-windowed" );
	CommandLine()->RemoveParm( "-window" );
	CommandLine()->RemoveParm( "-full" );
	CommandLine()->RemoveParm( "-fullscreen" );
	CommandLine()->RemoveParm( "-autoconfig" );
	CommandLine()->RemoveParm( "-mat_hdr_level" );

	return 0;
}