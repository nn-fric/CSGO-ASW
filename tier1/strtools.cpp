#include <tier1/strtools.h>


void V_FixSlashes( char *pname, char separator )
{
	if( separator == CORRECT_PATH_SEPARATOR )
	{
		while( pname != nullptr )
		{
			if( *pname == INCORRECT_PATH_SEPARATOR )
				*pname = separator;

			pname++;
		}
	}
	else if( separator == INCORRECT_PATH_SEPARATOR )
	{
		while( pname != nullptr )
		{
			if( *pname == CORRECT_PATH_SEPARATOR )
				*pname = separator;

			pname++;
		}
	}
}