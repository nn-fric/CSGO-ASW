#include <tier1/strtools.h>

void V_FixSlashes( char *pname, char separator )
{
	const int len = strlen( pname );
	const char newSeparator = separator == INCORRECT_PATH_SEPARATOR ? CORRECT_PATH_SEPARATOR : INCORRECT_PATH_SEPARATOR;

	for( int i = 0; i < len; i++ )
	{
		if( pname[i] == separator )
			pname[i] = newSeparator;
	}
}