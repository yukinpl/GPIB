#include "pch.h"
#include "GPIB.h"


GPIB::GPIB() : m_defaultRM( 0 ) , m_session( 0 )
{
	
}


GPIB::~GPIB()
{
	if( 0 != m_session )
	{
		viClear( m_session ) ;
		viClose( m_session ) ;

		m_session = 0 ;
	}

	if( 0 != m_defaultRM )
	{
		viClear( m_defaultRM ) ;
		viClose( m_defaultRM ) ;

		m_defaultRM = 0 ;
	}
}


void GPIB::Create( char const * resourceName , HWND const & _hwnd , int32_t const & _uniqueId )
{
	resource = resourceName ;
	hwnd = _hwnd ;
	uniqueId = _uniqueId ;
}


bool GPIB::Connect()
{

	if( 0 != viOpenDefaultRM( &m_defaultRM ) )
	{
		return false ;
	}

	if( 0 != viOpen( m_defaultRM , resource , VI_NULL , VI_NULL , &m_session ) )
	{
		return false ;
	}

	if( 0 != viPrintf( m_session , "*RST\n" ) ) // initalize device
	{
		return false ;
	}

	if( 0 != viPrintf( m_session , "*IDN?\n" ) )
	{
		return false ;
	}

	while( 0 != viScanf( m_session , "%t" , m_buf ) )
	{
		Wait( 100 ) ;
	}

	if( 0 < strlen( m_buf ) )
	{
		return true ;
	}

	return false ;
}


void GPIB::Disconnect()
{
	if( 0 != m_session )
	{
		viClear( m_session ) ;
		viClose( m_session ) ;

		m_session = 0 ;
	}

	if( 0 != m_defaultRM )
	{
		viClear( m_defaultRM ) ;
		viClose( m_defaultRM ) ;

		m_defaultRM = 0 ;
	}
}


char const * GPIB::Send( char * pstr , bool hasRetVal ) const
{
	CString cmd( ( LPSTR ) pstr ) ;
	if( 0 == m_defaultRM || 0 == m_session )
	{
		return nullptr ;
	}

	if( 0 != viPrintf( m_session , "%s\n" , pstr ) )
	{
		return nullptr ;
	}

	if( hasRetVal )
	{
		while( 0 != viScanf( m_session , "%t" , m_buf ) )
		{
			Wait( 1000 ) ;
		}

		return m_buf ;
	}

	return nullptr ;
}


bool GPIB::Send( uint8_t const * pstr , int32_t const & length , bool hasRetVal )
{
	char sendBuf[ 512 ] ;
	memcpy_s( sendBuf , 512 , pstr , length ) ;

	if( 512 <= length )
	{
		sendBuf[ 511 ] = '\0' ;
	}
	else
	{
		sendBuf[ length ] = '\0' ;
	}

	char const * ptr = Send( sendBuf , hasRetVal ) ;

	if( true == hasRetVal )
	{
		size_t len = strlen( ptr ) ;
		if( 0 < len )
		{
			SendMessage( hwnd , WM_GPIB_RECEIVED , len , ( LPARAM ) this ) ;
			return true ;
		}
		else
		{
			return false ;
		}
	}
	
	return true ;
}


void GPIB::Wait( DWORD dwMillisecond ) const
{
	MSG msg ;
	ULONGLONG dwStart ;
	dwStart = GetTickCount64() ;

	while( GetTickCount64() - dwStart < dwMillisecond )
	{
		while( PeekMessage( & msg , NULL , 0 , 0 , PM_REMOVE ) )
		{
			TranslateMessage( & msg ) ;
			DispatchMessage( & msg ) ;
		}
	}
}



int32_t const & GPIB::GetUniqueId() const
{
	return uniqueId ;
}