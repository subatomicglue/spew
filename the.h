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

#ifndef THE_THE_THE_THE
#define THE_THE_THE_THE

/// If you'd like to change the namespace, #define SPEWNAMESPACE to your own name...
#ifndef SPEWNAMESPACE
   namespace spew          // default namespace to use is "spew".
#  define SPEWNAMESPACE spew
#else
   namespace SPEWNAMESPACE // otherwise, use user-supplied namespace name if provided
#endif
{


/// THE
/// clever little helper for accessing a singlton for any type...
/// usage
/// @code
///    the<int>() = 2; // access "the" one global int...
/// @endcode
template <typename T>
inline static T& the()
{
   static T instance;
   return instance;
}


} // spew namespace

#endif
