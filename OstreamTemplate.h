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

#ifndef OSTREAM_TEMPLATE
#define OSTREAM_TEMPLATE

#include <ostream>
#include <sstream>
#include <string>

/// If you'd like to change the namespace, #define SPEWNAMESPACE to your own name...
#ifndef SPEWNAMESPACE
   namespace spew          // default namespace to use is "spew".
#  define SPEWNAMESPACE spew
#else
   namespace SPEWNAMESPACE // otherwise, use user-supplied namespace name if provided
#endif
{


/// generic ostream template
/// simply supply an object that takes a char* (or wchar*) string.
///
/// usage:
/// @code
///     struct PrintfOutput { void printf( const char* str ) { ::printf( str ); } };
///     OstreamTemplate<char, PrintfOutput> printfstream;
///     printfstream << "hi y0 " << 1 << " " << 'c' << " vor boink\n" << std::endl;
/// @endcode
template<class CharT, typename Output, class TraitsT = std::char_traits<CharT> >
class OstreamTemplate : public std::basic_ostream<CharT, TraitsT>
{
public:
    OstreamTemplate() : std::basic_ostream<CharT, TraitsT>( mStringBuf = new StringbufTemplate<CharT, Output, TraitsT>() ), out( mStringBuf->out ) {}
    ~OstreamTemplate() { delete std::basic_ostream<CharT, TraitsT>::rdbuf(); }
    Output& out; //< outputter is accessable...
private:
    template <class CharT_, typename Output_, class TraitsT_ = std::char_traits<CharT> >
    class StringbufTemplate : public std::basic_stringbuf<CharT_, TraitsT_>
    {
    public:
        virtual ~StringbufTemplate() { sync(); }
        Output_ out;
    protected:
        int sync()
        {
            output_debug_string( std::basic_stringbuf<CharT_, TraitsT_>::str().c_str() );
            str( std::basic_string<CharT_>() );    // Clear the string buffer
            return 0;
        }
        void output_debug_string(const CharT_ *text) { out.printf( text ); }
    };
    StringbufTemplate<CharT, Output, TraitsT>* mStringBuf;
};


/// little test app you can use to make sure the any-stream works...
struct OstreamTemplateUnitTest
{
private:
    struct PrintfOutput { void printf( const char* str ) { ::printf( str ); } };
    OstreamTemplate<char, PrintfOutput>    printfstream;
public:
    void test()
    {
        printfstream << "hi y0 " << 1 << " " << 'c' << " vor boink\n" << std::endl;
    }
};

} //namespace spew

#endif
