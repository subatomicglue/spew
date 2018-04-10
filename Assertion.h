/*
   spew - semi-powerful trace logger, debug output, stderr/stdout
   Copyright (c) 2006 kevin meinert all rights reserved

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301  USA
*/

#ifndef SPEWASSERT_INCLUDED
#define SPEWASSERT_INCLUDED

#include <iostream>
#include <vector>
#include <stdio.h>
#include <assert.h>

/* @file Assert.h
 * Example Usage:
 * @code
 * ASSERT( false, "my message" ); // run time error
 * STATICASSERT( false ); //< compile time error
 * @endcode
 */



class AssertBase
{
public:
	struct AssertionHandler
	{
		virtual bool call( const char* str, int line, const char* file ) = 0;
	};
	std::vector<AssertionHandler*> mHandlers;
	/// return true if break
	bool callAssert( const char* str, int line, const char* file )
	{
		if (0 == mHandlers.size())
		{
			std::cout << "assertion: " 
				<< "   '" << str << "'\n" 
				<< "   in '" << file << "' line " << line << "\n"
				<< "   (b) break, (c) continue: " << std::flush;
			char g = getchar();
			return g == 'b';
		}
		for (unsigned int x = 0; x < mHandlers.size(); ++x)
		{
			if (mHandlers[x]->call( str, line, file ))
			{
				return true;
			}
		}
		return false;
	}
	void reg( AssertionHandler& h );
	static AssertBase& instance() { static AssertBase b; return b; }
};

/// trigger a compiler breakpoint
#if defined( WIN32 ) && defined(_MSC_VER) && (_MSC_VER >= 1020)
	#define TRIGGERBREAK() __asm int 3
#else //< fallback if no compiler specific break instruction available.
	#define TRIGGERBREAK() assert( false )
#endif
		
#ifdef _DEBUG
	#define ASSERT( test,message ) \
	if (!test) \
	{ \
		/* see if we should break (true) or continue (false) */ \
		if (AssertBase::instance().callAssert( message, __LINE__, __FILE__ )) \
		{ \
			TRIGGERBREAK(); \
		} \
	}
#else
	#define ASSERT( test, message )
#endif

/// TODO: upgrade to boost's static assert for message output...
template <bool> struct STATIC_ASSERTION_FAILURE;
template <> struct STATIC_ASSERTION_FAILURE<true>{};
#define STATICASSERT( test, str )  sizeof( STATIC_ASSERTION_FAILURE<test> )



#endif // ASSERT_INCLUDED
