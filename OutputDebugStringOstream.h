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

#ifndef OUTPUTDEBUGSTRING_OUTPUTSTREAM
#define OUTPUTDEBUGSTRING_OUTPUTSTREAM

#ifdef WIN32
#  include <Windows.h> //< for OutputDebugString
#endif
#include "OstreamTemplate.h" //< for ostream generator

/// If you'd like to change the namespace, #define SPEWNAMESPACE to your own name...
#ifndef SPEWNAMESPACE
   namespace spew          // default namespace to use is "spew".
#  define SPEWNAMESPACE spew
#else
   namespace SPEWNAMESPACE // otherwise, use user-supplied namespace name if provided
#endif
{


/// plug-in for an OstreamTemplate template
/// defines a policy for how to output debugger trace window text...
struct DebuggerTraceWindowOutput
{ 
   inline void printf( const char* text ) 
   {
#if defined( WIN32 ) && defined(_MSC_VER) && (_MSC_VER >= 1020)// VC 4.2
         ::OutputDebugStringA( text );
#endif
   }
   inline void printf( const wchar_t* text ) 
   {
#if defined( WIN32 ) && defined(_MSC_VER) && (_MSC_VER >= 1020)// VC 4.2
         ::OutputDebugStringW( text );
#endif
   } 
};

/// Debugger Trace window ostream
typedef OstreamTemplate<char, DebuggerTraceWindowOutput>     DebuggerTraceWindowOstream;

/// Debugger Trace window ostream (unicode)
typedef OstreamTemplate<wchar_t, DebuggerTraceWindowOutput> wDebuggerTraceWindowOstream;


} // spew namespace

#endif
