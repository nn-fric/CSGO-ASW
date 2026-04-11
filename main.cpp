#include <SDL3/SDL_main.h>
#include <SDL3/SDL_messagebox.h>
#include <tier0/platform.h>
#include <tier0/icommandline.h>
#include <tier0/logging.h>
#include <tier1/strtools.h>

char g_szBasedir[MAX_PATH] = {};

DEFINE_LOGGING_CHANNEL_NO_TAGS( LOG_EngineInitialization, "EngineInitialization" );

class CLauncherLoggingListener : public ILoggingListener
{
public:
	void Log( const LoggingContext_t *pContext, const char *pMessage ) override
	{
		if( pContext->m_Flags == LCF_CONSOLE_ONLY )
			return;

		if( pContext->m_Severity == LS_ASSERT )
			SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_WARNING, "Assert", pMessage, nullptr );
		else if( pContext->m_Severity == LS_ERROR )
			SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Error", pMessage, nullptr );
		else if( pContext->m_ChannelID == LOG_EngineInitialization )
			SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_WARNING, "Warning", pMessage, nullptr );
	}
};

CLauncherLoggingListener g_LauncherLoggingListener;


int main( int argc, char* argv[] )
{
	GetGlobalLoggingSystem()->RegisterLoggingListener( &g_LauncherLoggingListener );
	CommandLine()->CreateCmdLine( argc, argv );

	const char *pszBasedir;
	CommandLine()->CheckParm( "-basedir", &pszBasedir ); // I think Valve messed it up, fixed
	strlcpy( g_szBasedir, pszBasedir, MAX_PATH );
	V_FixSlashes( g_szBasedir );

	if( CommandLine()->CheckParm( "-tslist" ) )
	{
		// TODO: implete me
		Log_Msg( LOG_DEVELOPER, "Running TSList tests\n" );
		//RunTSListTests( 10000, 1 );
		Log_Msg( LOG_DEVELOPER, "Running TSQueue tests\n" );
		//RunTSQueueTests( 10000, 1 );
		Log_Msg( LOG_DEVELOPER, "Running Thread Pool tests\n" );
		//RunThreadPoolTests();
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