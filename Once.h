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

#ifndef SPEW_ONCE_INCLUDED
#define SPEW_ONCE_INCLUDED
#include <map>

/// Execute code once...
///    this calls 'call' only once from the line of code where called.
///    pass unique info for 'key':
///     - such as line number (__LINE__)
///     - or custom error message details (see code example)
/// key needs to have a compare method defined:
///    bool operator<( MyKeyType )
///
/// Example Usage:
/// @code:
///    struct errorinfo
///    {
///       inline errorinfo( int id ) : x( id ) {}
///       inline bool operator<( const errorinfo& i ) { return mId < i.mId; }
///       int mId;
///    };
///    SPEW_ONCE( spew::Log( "the %dth item is having a problem\n", x ), errorinfo( x ), errorinfo );
///    SPEW_ONCE( spew::Log( "error on line %d\n", __LINE__ ), __LINE__, int );
/// @endcode
#define SPEW_ONCE( call, key, keytype ) \
{\
    static std::map<keytype, bool> mOnceOnly;\
    if (0 == mOnceOnly.count( key ))\
    {\
        mOnceOnly[key] = true;\
        call;\
    }\
}


#endif // SPEW_ONCE_INCLUDED
