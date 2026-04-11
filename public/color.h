#pragma once

//-----------------------------------------------------------------------------
// Purpose: Basic handler for a rgb set of colors
//			This class is fully inline
//-----------------------------------------------------------------------------
class Color
{
public:
	// constructors
	Color()
	{
		*reinterpret_cast<int *>( this ) = 0;
	}
	Color( const int _r, const int _g, const int _b )
	{
		SetColor( _r, _g, _b );
	}
	Color( const int _r, const int _g, const int _b, const int _a )
	{
		SetColor( _r, _g, _b, _a );
	}

	// set the color
	// r - red component (0-255)
	// g - green component (0-255)
	// b - blue component (0-255)
	// a - alpha component, controls transparency (0 - transparent, 255 - opaque);
	void SetColor( const int _r, const int _g, const int _b, const int _a = 0 )
	{
		m_color[0] = static_cast<unsigned char>( _r );
		m_color[1] = static_cast<unsigned char>( _g );
		m_color[2] = static_cast<unsigned char>( _b );
		m_color[3] = static_cast<unsigned char>( _a );
	}

	void GetColor( int &_r, int &_g, int &_b, int &_a ) const
	{
		_r = m_color[0];
		_g = m_color[1];
		_b = m_color[2];
		_a = m_color[3];
	}

	void SetRawColor( const int color32 )
	{
		*reinterpret_cast<int *>( this ) = color32;
	}

	int GetRawColor() const
	{
		return *reinterpret_cast<const int *>( this );
	}

	int r() const	{ return m_color[0]; }
	int g() const	{ return m_color[1]; }
	int b() const	{ return m_color[2]; }
	int a() const	{ return m_color[3]; }

	unsigned char &operator[]( const int index )
	{
		return m_color[index];
	}

	const unsigned char &operator[]( const int index ) const
	{
		return m_color[index];
	}

	bool operator ==( const Color &rhs ) const
	{
		return *reinterpret_cast<const int *>( this ) == *reinterpret_cast<const int *>( &rhs );
	}

	bool operator !=( const Color &rhs ) const
	{
		return !( operator == ( rhs ) );
	}

	Color &operator =( const Color &rhs )
	{
		SetRawColor( rhs.GetRawColor() );
		return *this;
	}

private:
	unsigned char m_color[4];
};