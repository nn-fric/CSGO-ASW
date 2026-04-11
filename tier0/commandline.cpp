#include <vector>
#include <tier0/platform.h>
#include <tier0/icommandline.h>
#include <tier1/strtools.h>

class CCommandLine : public ICommandLine
{
public:
	CCommandLine();
	~CCommandLine();

	void					CreateCmdLine( int argc, char *argv[] ) override;

	const char				*CheckParm( const char *psz, const char **ppszValue ) const override;
	void					RemoveParm( const char *pszParm ) override;
	void					AppendParm(const char *pszParm, const char *pszValues) override;

	const char				*ParmValue( const char *psz, const char *pDefaultVal ) const override;
	int						ParmValue( const char *psz, int nDefaultVal ) const override;
	float					ParmValue( const char *psz, float flDefaultVal ) const override;

	int						ParmCount() const override;
	int						FindParm( const char *pszParm ) const override;
	const char*				GetParm( int nIndex ) const override;

	void					SetParm( int nIndex, const char *pNewParm ) override;

	static constexpr bool	IsParm( const char *pszParm );

private:
	std::vector<const char *> m_vecParms;
};


static CCommandLine g_CommandLine;

ICommandLine *CommandLine()
{
	return &g_CommandLine;
}


CCommandLine::CCommandLine()
{
	;
}

CCommandLine::~CCommandLine()
{
	;
}


void CCommandLine::CreateCmdLine( int argc, char *argv[] )
{
	m_vecParms.clear();
	m_vecParms.reserve( argc );

	for( int i = 0; i < argc; i++ )
		m_vecParms.emplace_back( argv[i] );
}


const char *CCommandLine::CheckParm( const char *pszParm, const char **ppszValue ) const
{
	int nIndex = FindParm( pszParm );

	if( ppszValue )
		*ppszValue = nullptr;

	if( nIndex != 0 )
	{
		if( nIndex + 1 != m_vecParms.size() )
			*ppszValue = m_vecParms[ nIndex + 1 ];

		return m_vecParms[ nIndex ];
	}

	return nullptr;
}

void CCommandLine::RemoveParm( const char *pszParm )
{
	int nIndex = FindParm( pszParm );

	if( nIndex != 0 )
	{
		m_vecParms.erase( m_vecParms.begin() + nIndex );

		if( nIndex != m_vecParms.size() )
		{
			if( !IsParm( m_vecParms[nIndex] ) )
				m_vecParms.erase( m_vecParms.begin() + nIndex );
		}
	}
}

void CCommandLine::AppendParm( const char *pszParm, const char *pszValues )
{
	m_vecParms.emplace_back( pszParm );

	if( pszValues )
		m_vecParms.emplace_back( pszValues );
}


const char *CCommandLine::ParmValue( const char *pszParm, const char *pDefaultVal ) const
{
	int nIndex = FindParm( pszParm );

	if( nIndex != 0 && ++nIndex != m_vecParms.size() )
	{
		if( !IsParm( m_vecParms[nIndex] ) )
			return m_vecParms[nIndex];
	}

	return pDefaultVal;
}

int CCommandLine::ParmValue( const char *pszParm, int nDefaultVal ) const
{
	int nIndex = FindParm( pszParm );

	if( nIndex != 0 && ++nIndex != m_vecParms.size() )
	{
		if( !IsParm( m_vecParms[nIndex] ) )
			return atoi( m_vecParms[nIndex] );
	}

	return nDefaultVal;
}

float CCommandLine::ParmValue( const char *pszParm, float flDefaultVal ) const
{
	int nIndex = FindParm( pszParm );

	if( nIndex != 0 && ++nIndex != m_vecParms.size() )
	{
		if( !IsParm( m_vecParms[nIndex] ) )
			return atof( m_vecParms[nIndex] );
	}

	return flDefaultVal;
}


int CCommandLine::ParmCount() const
{
	return m_vecParms.size();
}

int CCommandLine::FindParm( const char *pszParm ) const
{
	for( int i = 1; i < m_vecParms.size(); i++ )
	{
		if( !stricmp( m_vecParms[i], pszParm ) )
			return i;
	}

	return 0;
}

const char* CCommandLine::GetParm( int nIndex ) const
{
	return m_vecParms[ nIndex ];
}


void CCommandLine::SetParm( int nIndex, const char *pNewParm )
{
	if( nIndex > 0 && nIndex < m_vecParms.size() )
		m_vecParms[ nIndex ] = pNewParm;
}


constexpr bool CCommandLine::IsParm( const char *pszParm )
{
	return pszParm[0] == '-' || pszParm[0] == '+';
}