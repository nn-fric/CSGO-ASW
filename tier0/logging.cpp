#include <tier0/logging.h>
#include <cstdarg>

static thread_local int g_nThreadLocalStateIndex = 0;

DEFINE_LOGGING_CHANNEL_NO_TAGS( LOG_GENERAL, "General" );
DEFINE_LOGGING_CHANNEL_NO_TAGS( LOG_ASSERT, "Assert" );

BEGIN_DEFINE_LOGGING_CHANNEL( LOG_CONSOLE, "Console", LCF_CONSOLE_ONLY, LS_MESSAGE );
ADD_LOGGING_CHANNEL_TAG( "Console" );
END_DEFINE_LOGGING_CHANNEL();

BEGIN_DEFINE_LOGGING_CHANNEL( LOG_DEVELOPER, "Developer", LCF_DEFAULT, LS_MESSAGE );
ADD_LOGGING_CHANNEL_TAG( "Developer" );
END_DEFINE_LOGGING_CHANNEL();

BEGIN_DEFINE_LOGGING_CHANNEL( LOG_DEVELOPER_VERBOSE, "DeveloperVerbose", LCF_DEFAULT, LS_MESSAGE );
ADD_LOGGING_CHANNEL_TAG( "DeveloperVerbose" );
END_DEFINE_LOGGING_CHANNEL();

BEGIN_DEFINE_LOGGING_CHANNEL( LOG_DEVELOPER_CONSOLE, "DeveloperConsole", LCF_CONSOLE_ONLY, LS_MESSAGE );
ADD_LOGGING_CHANNEL_TAG( "DeveloperVerbose" );
ADD_LOGGING_CHANNEL_TAG( "Console" );
END_DEFINE_LOGGING_CHANNEL();


CLoggingSystem::CLoggingSystem() :
	m_nChannelTagCount(0),
	m_ChannelTags{},
	m_nTagNamePoolIndex(0),
	m_TagNamePool(),
	m_nGlobalStateIndex(0),
	m_LoggingStates{}
{
	m_LoggingStates[0].m_nListenerCount = 1;
	m_LoggingStates[0].m_nPreviousStackEntry = 0;
	m_LoggingStates[0].m_pLoggingResponse = &m_DefaultLoggingResponse;
	m_LoggingStates[0].m_RegisteredListeners[0] = &m_DefaultLoggingListener;
}

CLoggingSystem::~CLoggingSystem()
{
}


LoggingChannelID_t CLoggingSystem::RegisterLoggingChannel( const char *pChannelName, RegisterTagsFunc registerTagsFunc, LoggingChannelFlags_t flags, LoggingSeverity_t minimumSeverity )
{
	if( !m_RegisteredChannels.empty() )
	{
		const LoggingChannelID_t index = FindChannel( pChannelName );

		if( index == INVALID_LOGGING_CHANNEL_ID )
			goto NEW_CHANNEL;

		if( registerTagsFunc )
			registerTagsFunc();

		if( m_RegisteredChannels[index].m_Flags == LCF_DEFAULT && m_RegisteredChannels[index].m_MinimumSeverity == LS_MESSAGE )
		{
			m_RegisteredChannels[index].m_Flags = flags;
			m_RegisteredChannels[index].m_MinimumSeverity = minimumSeverity;
		}

		return m_RegisteredChannels[index].m_ID;
	}
	else
	{
NEW_CHANNEL:
		LoggingChannel_t loggingChannel
		{
			.m_ID = GetChannelCount(),
			.m_Flags = flags,
			.m_MinimumSeverity = minimumSeverity
		};
		strlcpy( loggingChannel.m_Name, pChannelName, MAX_LOGGING_IDENTIFIER_LENGTH );

		if( registerTagsFunc )
			registerTagsFunc();

		m_RegisteredChannels.emplace_back( loggingChannel );
		return loggingChannel.m_ID;
	}
}

LoggingChannelID_t CLoggingSystem::FindChannel( const char *pChannelName ) const
{
	for( LoggingChannelID_t index = 0; index < GetChannelCount(); index++ )
	{
		if( !stricmp( m_RegisteredChannels[index].m_Name, pChannelName ) )
			return index;
	}

	return INVALID_LOGGING_CHANNEL_ID;
}


void CLoggingSystem::SetChannelSpewLevel( LoggingChannelID_t channelID, LoggingSeverity_t minimumSeverity )
{
	m_RegisteredChannels[channelID].m_MinimumSeverity = minimumSeverity;
}

void CLoggingSystem::SetChannelSpewLevelByName( const char *pName, LoggingSeverity_t minimumSeverity )
{
	for( LoggingChannelID_t index = 0; index < GetChannelCount(); index++ )
	{
		if( !stricmp( m_RegisteredChannels[index].m_Name, pName ) )
			m_RegisteredChannels[index].m_MinimumSeverity = minimumSeverity;
	}
}

void CLoggingSystem::SetChannelSpewLevelByTag( const char *pTag, LoggingSeverity_t minimumSeverity )
{
	for( LoggingChannelID_t index = 0; index < GetChannelCount(); index++ )
	{
		if( HasTag( index, pTag ) )
			m_RegisteredChannels[index].m_MinimumSeverity = minimumSeverity;
	}
}


void CLoggingSystem::AddTagToCurrentChannel( const char *pTagName )
{
	const LoggingChannelID_t index = GetChannelCount() - 1;

	if( m_RegisteredChannels[index].HasTag( pTagName ) )
		return;

	// Add to head
	LoggingTag_t *pTag = AllocTag( pTagName );
	pTag->m_pNextTag = m_RegisteredChannels[index].m_pFirstTag;
	m_RegisteredChannels[index].m_pFirstTag = pTag;
}


void CLoggingSystem::PushLoggingState( const bool bThreadLocal, const bool bClearState )
{
	m_StateMutex.lock();

	const int currentStateIndex = bThreadLocal ? g_nThreadLocalStateIndex : m_nGlobalStateIndex;
	const int unusedStateIndex = FindUnusedStateIndex();
	Assert( unusedStateIndex != -1 ); // should check whether is out of size

	if( bClearState )
	{
		m_LoggingStates[unusedStateIndex].m_nListenerCount = 0;
		m_LoggingStates[unusedStateIndex].m_pLoggingResponse = &m_DefaultLoggingResponse;
	}
	else
		memcpy( &m_LoggingStates[unusedStateIndex], &m_LoggingStates[currentStateIndex], sizeof( LoggingState_t ) );

	m_LoggingStates[unusedStateIndex].m_nPreviousStackEntry = currentStateIndex;

	if( bThreadLocal )
		g_nThreadLocalStateIndex = unusedStateIndex;
	else
		m_nGlobalStateIndex = unusedStateIndex;

	m_StateMutex.unlock();
}

void CLoggingSystem::PopLoggingState( bool bThreadLocal )
{
	m_StateMutex.lock();

	if( bThreadLocal )
		g_nThreadLocalStateIndex = m_LoggingStates[g_nThreadLocalStateIndex].m_nPreviousStackEntry;
	else
		m_nGlobalStateIndex = m_LoggingStates[m_nGlobalStateIndex].m_nPreviousStackEntry;

	m_StateMutex.unlock();
}


void CLoggingSystem::RegisterLoggingListener( ILoggingListener *pListener )
{
	m_StateMutex.lock();

	LoggingState_t *pLoggingState = GetCurrentState();

	pLoggingState->m_RegisteredListeners[pLoggingState->m_nListenerCount] = pListener;
	pLoggingState->m_nListenerCount++;

	m_StateMutex.unlock();
}

bool CLoggingSystem::IsListenerRegistered( ILoggingListener *pListener )
{
	m_StateMutex.lock();

	LoggingState_t *pLoggingState = GetCurrentState();

	for( int i = 0; i < pLoggingState->m_nListenerCount; i++ )
	{
		if( pLoggingState->m_RegisteredListeners[i] == pListener )
		{
			m_StateMutex.unlock();
			return true;
		}
	}

	m_StateMutex.unlock();
	return false;
}

void CLoggingSystem::ResetCurrentLoggingState()
{
	m_StateMutex.lock();

	LoggingState_t *pLoggingState = GetCurrentState();

	pLoggingState->m_nListenerCount = 0;
	pLoggingState->m_pLoggingResponse = &m_DefaultLoggingResponse;

	m_StateMutex.unlock();
}

void CLoggingSystem::SetLoggingResponsePolicy( ILoggingResponsePolicy *pLoggingResponse )
{
	m_StateMutex.lock();

	LoggingState_t *pLoggingState = GetCurrentState();

	if( pLoggingResponse )
		pLoggingState->m_pLoggingResponse = pLoggingResponse;
	else
		pLoggingState->m_pLoggingResponse = &m_DefaultLoggingResponse;

	m_StateMutex.unlock();
}


LoggingResponse_t CLoggingSystem::LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, const char *pMessage )
{
	LoggingResponse_t loggingResponse = LR_ABORT;
	LoggingContext_t loggingContext = {
		.m_ChannelID = channelID,
		.m_Flags =  m_RegisteredChannels[channelID].m_Flags,
		.m_Severity = severity
	};

	if( channelID >= 0 && channelID < GetChannelCount() )
	{
		m_StateMutex.lock();

		LoggingState_t *pLoggingState = GetCurrentState();

		for( int i = 0; i < pLoggingState->m_nListenerCount; i++ )
		{
			pLoggingState->m_RegisteredListeners[i]->Log( &loggingContext, pMessage );
		}

		loggingResponse = pLoggingState->m_pLoggingResponse->OnLog( &loggingContext );

		m_StateMutex.unlock();

		if( loggingResponse == LR_ABORT )
		{
			LoggingSystem_Log( LOG_DEVELOPER_VERBOSE, LS_MESSAGE, "Exiting due to logging LR_ABORT request.\n" );
			SDL_Quit();
		}

		if( loggingResponse == LR_DEBUGGER && severity != LS_MESSAGE )
		{
			SDL_TriggerBreakpoint();
		}
	}

	return loggingResponse;
}


CLoggingSystem::LoggingState_t *CLoggingSystem::GetCurrentState()
{
	int index = g_nThreadLocalStateIndex;

	if( index == 0 )
		index = m_nGlobalStateIndex;

	return &m_LoggingStates[index];
}

const CLoggingSystem::LoggingState_t *CLoggingSystem::GetCurrentState() const
{
	int index = g_nThreadLocalStateIndex;

	if( index == 0 )
		index = m_nGlobalStateIndex;

	return &m_LoggingStates[index];
}


int CLoggingSystem::FindUnusedStateIndex() const
{
	for( int index = 0; index < MAX_LOGGING_STATE_COUNT; index++ )
	{
		if( m_LoggingStates[index].m_nListenerCount < 0 )
			return index;
	}

	return -1;
}


CLoggingSystem::LoggingTag_t *CLoggingSystem::AllocTag( const char *pTagName )
{
	// All in stack
	LoggingTag_t *pTag = &m_ChannelTags[m_nChannelTagCount++];

	pTag->m_pTagName = m_TagNamePool + m_nTagNamePoolIndex;
	pTag->m_pNextTag = nullptr;

	strlcpy( m_TagNamePool + m_nTagNamePoolIndex, pTagName, MAX_LOGGING_TAG_CHARACTER_COUNT - m_nTagNamePoolIndex );
	m_nTagNamePoolIndex += strlen( pTagName ) + 1;

	return pTag;
}


LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, const char *pMessageFormat, ... )
{
	char message[MAX_LOGGING_MESSAGE_LENGTH];

	va_list va;
	va_start( va, pMessageFormat );
	vsnprintf( message, MAX_LOGGING_MESSAGE_LENGTH, pMessageFormat, va );

	return GetGlobalLoggingSystem()->LogDirect( channelID, severity, message );
}

LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, const char *pMessage )
{
	return GetGlobalLoggingSystem()->LogDirect( channelID, severity, pMessage );
}

LoggingResponse_t LoggingSystem_LogAssert( const char *pMessageFormat, ... )
{
	char message[MAX_LOGGING_MESSAGE_LENGTH];

	va_list va;
	va_start( va, pMessageFormat );
	vsnprintf( message, MAX_LOGGING_MESSAGE_LENGTH, pMessageFormat, va );

	return GetGlobalLoggingSystem()->LogDirect( LOG_ASSERT, LS_ASSERT, message );
}
