#pragma once

#include "afxdialogex.h"
#include <stdint.h>
#include "compile.option.gpib.h"
#include "visa.h"

#define WM_GPIB_RECEIVED ( WM_USER + 10 )

class GPIB
{

private :
	HWND hwnd ;

private : 
	int32_t uniqueId ;

private :
	ViSession m_defaultRM ;
	ViSession m_session ;

	char const * resource ;
	char m_buf[ 256 ] = { 0 , } ;

	void Wait( DWORD dwMillisecond ) const ;

public :
	void Create( char const * resourceName , HWND const & hwnd = 0 , int32_t const & uniqueId = 0 ) ;
	bool Connect() ;
	void Disconnect() ;

	char const * Send( char * pstr , bool hasRetVal = true ) const ;
	bool Send( uint8_t const * pstr , int32_t const & length , bool hasRetVal = true ) ;

	int32_t const & GetUniqueId() const ;

	 GPIB() ;
	~GPIB() ;
	
} ;
