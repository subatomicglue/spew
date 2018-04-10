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

#ifndef OUTPUT_SYSTEM
#define OUTPUT_SYSTEM

#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdarg.h> // varargs
#include <assert.h>
#include <cstdio> // vsnprinf
#include "OstreamTemplate.h"
#  include "OutputDebugStringOstream.h" //< compiler trace window output
#include "the.h" //< singleton generator

#ifdef ERROR // wingdi.h has this :(
#undef ERROR
#endif


/// SPEW!
/// The 'spew' output system is contained inside the spew namespace.
/// If you'd like to change the namespace, just #define SPEWNAMESPACE to your own name...
/// Most users of spew will use the following features:
/// @see Log, Trace, StdErr, StdOut below for code examples and usage.
#ifndef SPEWNAMESPACE
namespace spew          // default namespace to use is "spew".
#define SPEWNAMESPACE spew
#else
namespace SPEWNAMESPACE // otherwise, use user-supplied namespace name if provided
#endif
{
/// list of filters available, 
/// use these enums when outputting, and when 
///   setting Output::SetFilter
/// modify this to add new ones.
/// --> hey!!! keep this in sync with gTagDescriptions below
enum Filter
{

   FILTERDEFAULT = 0xffffffff, //< FILTERALL
   FILTERALL = 0xffffffff,  //< always output
   FILTERNONE = 0x00000000, //< never output
   GFX      = 0x00000001,
   SCRIPT   = 0x00000002,
   SOUND    = 0x00000004,
   PHYSICS  = 0x00000008,
   IO       = 0x00000010,
   ANIM     = 0x00000020,
   LUA      = 0x00000040,
   ERROR    = 0x00000080,
};

/// use these enums when outputting
/// --> hey!!! keep this in sync with gTagDescriptions below
enum Level
{
   // use these in both SetLevel and when Outputing
   LEVEL1 = 0x00000001,
   LEVEL2 = 0x00000002,
   LEVEL3 = 0x00000004,
   LEVEL4 = 0x00000008,
   LEVEL5 = 0x00000010,

   // use these in SetLevel (not when outputing)
   LEVELNONE = 0x00000000, //< never output
   LEVEL1ANDLOWER = 0x00000001,
   LEVEL2ANDLOWER = 0x00000003,
   LEVEL3ANDLOWER = 0x00000007,
   LEVEL4ANDLOWER = 0x0000000f,
   LEVEL5ANDLOWER = 0x0000001f,
   LEVELALL = 0xffffffff,  //< output any level

   // just a constant, don't use this
   _LEVELHIGHEST = 5, 
   _LEVELDEFAULT = 0x00000001, //< L1 is default
};


// used for command line parsing (keep this in sync with the above)
// also, keep parseCommandLine(..) below in sync...
struct TagDescription
{
   const char* mName;
   int mTag;
};
const TagDescription gTagDescriptions[] = 
{
   { "GFX", GFX },
   { "SCRIPT", SCRIPT },
   { "SOUND", SOUND },
   { "PHYSICS", PHYSICS },
   { "IO", IO },
   { "ANIM", ANIM },
   { "LUA", LUA },
   { "ERROR", ERROR },

   { "Off", LEVELNONE },
   { "1", LEVEL1ANDLOWER },
   { "2", LEVEL2ANDLOWER },
   { "3", LEVEL3ANDLOWER },
   { "4", LEVEL4ANDLOWER },
   { "5", LEVEL5ANDLOWER },
   { "1only", LEVEL1 },
   { "2only", LEVEL2 },
   { "3only", LEVEL3 },
   { "4only", LEVEL4 },
   { "5only", LEVEL5 },
   { "Max", LEVELALL },

   { "", 0 } // keep this last...
};

/// converter between integers and LevelType
struct Level_
{
   inline Level_( const int number )
   {
      assert( 1 <= number && number <= _LEVELHIGHEST && "error: level exceeded" );
      const Level lt[] = { LEVEL1, LEVEL2, LEVEL3, LEVEL4, LEVEL5 };
      mType = lt[number-1];
   }
   inline Level_( const Level level ) { mType = level; }
   inline operator const Level() const { return mType; }
   Level mType;
};
/// converter between integers and LevelType
struct LevelSelect_
{
   inline LevelSelect_( const int number )
   {
      assert( 1 <= number && number <= _LEVELHIGHEST && "error: level exceeded" );
      const Level lt[] = { LEVEL1ANDLOWER, LEVEL2ANDLOWER, LEVEL3ANDLOWER, LEVEL4ANDLOWER, LEVEL5ANDLOWER };
      mType = lt[number-1];
   }
   inline LevelSelect_( const Level level ) { mType = level; }
   inline operator const Level() const { return mType; }
   Level mType;
};


/// output base type
/// all text output goes through this for logging, trace messages, command line output, etc...
/// see output types (under '@see' below) for examples of how to extend.
/// catagory filters are available.
///
/// for usage:
///  - ignore this class
///  - refer to StdOut, StdErr, Log, and Trace types below
///
/// examples:
/// @code
///    Log( "hi %s", steve );
///    Log( GFX, "hi %s", steve );
///    Log << "default category, default level" << std::endl;
///    Log( GFX, 0 ) << "render category, level 0!" << std::endl;
/// @endcode
/// @see Log, StdErr, StdOut, Trace
template <typename OUTPUTBASE_INIT, unsigned INCLUDE_IN_RELEASE_MODE = 0>
class OutputBase
{
   /// some compile-time constants to query if we're in debug mode...
   enum
   {
      MAX_BUF_SIZE = 256,
#ifdef _DEBUG
      IN_DEBUG_MODE = 1
#else
      IN_DEBUG_MODE = 0
#endif
   };

public:
   /// constructor
   OutputBase()
   {
      mStreamOut.out.mParent = this;
      mGlobalFilter = FILTERDEFAULT;
      mGlobalLevel = _LEVELDEFAULT;
      mOutputBaseInit.init( *this );
   }

   /// set the logging filter.
   /// all messages that aren't inluded in this 
   /// filter are filtered out before outputting...
   /// usage:  
   /// @code
   ///   #define StdOut OutputBase<StdOutInit>::instance()
   ///   StdOut.SetFilter( GFX );
   /// @endcode
   inline void SetFilter( int filter ) { mGlobalFilter = filter; }
   inline void AddFilter( int filter ) { mGlobalFilter |= filter; }
   inline void RemoveFilter( int filter ) { mGlobalFilter &= ~filter; }

   /// reset to default filter and default level...
   inline void SetDefaults() { mOutputBaseInit.reset( *this ); }

   /// set the logging level.
   /// messages not included in this level are filtered out before outputting...
   /// usage:
   /// @code
   ///   #define StdOut OutputBase<StdOutInit>::instance()
   ///   StdOut.SetLevel( LEVEL4ANDLOWER );
   /// @endcode
   inline void SetLevel( LevelSelect_ level ) { mGlobalLevel = level; }

   /// send formatted text to the output, similar to printf in the C stdlib
   /// uses default filter and level
   /// usage:  
   /// @code
   ///   #define StdOut OutputBase<StdOutInit>::instance()
   ///   StdOut( "hi %s", steve );
   /// @endcode
   inline void operator()( const char fmtstr[], ... )
   {
      if (INCLUDE_IN_RELEASE_MODE || IN_DEBUG_MODE)
      {
         va_list arg_ptr;
         va_start( arg_ptr, fmtstr );
         operator()( FILTERDEFAULT, fmtstr, arg_ptr );
         va_end( arg_ptr );
      }
   }

   /// send formatted text to the output, similar to printf in the C stdlib
   /// specify a filter, uses default level.
   /// usage:  
   /// @code
   ///   #define StdOut OutputBase<StdOutInit>::instance()
   ///   StdOut( GFX, "hi %s", steve );
   /// @endcode
   inline void operator()( Filter filter, const char fmtstr[], ... )
   {
      if (INCLUDE_IN_RELEASE_MODE || IN_DEBUG_MODE)
      {
         va_list arg_ptr;
         va_start( arg_ptr, fmtstr );
         operator()( filter, fmtstr, arg_ptr );
         va_end( arg_ptr );
      }
   }

   /// send formatted text to the output, similar to printf in the C stdlib
   /// specify a filter and level.
   /// usage:  
   /// @code
   ///   #define StdOut OutputBase<StdOutInit>::instance()
   ///   StdOut( GFX, 1, "hi %s", steve );
   /// @endcode
   inline void operator()( Filter filter, Level_ level, const char fmtstr[], ... )
   {
      if (INCLUDE_IN_RELEASE_MODE || IN_DEBUG_MODE)
      {
         va_list arg_ptr;
         va_start( arg_ptr, fmtstr );
         operator()( filter, level, fmtstr, arg_ptr );
         va_end( arg_ptr );
      }
   }

   /// vararg compatible implementation of print, most users wont need this.
   /// usage:  
   /// @code
   ///   #define StdOut OutputBase<StdOutInit>::instance()
   ///   void myprintf( const char* str, ... )
   ///   {
   ///      va_list arg_ptr;
   ///      va_start( arg_ptr, fmtstr );
   ///      StdOut( GFX, 2, arg_ptr );
   ///      va_end( arg_ptr );
   ///   }
   /// @endcode
   void operator()( Filter filter, Level_ level, const char fmtstr[], va_list& arg_ptr )
   {
      if (INCLUDE_IN_RELEASE_MODE || IN_DEBUG_MODE)
      {
         static char buf[MAX_BUF_SIZE] = "\0";
#ifdef WIN32
         _vsnprintf( buf, MAX_BUF_SIZE, fmtstr, arg_ptr );
#else
         vsprintf( buf, fmtstr, arg_ptr );
         assert( strlen( buf ) <= MAX_BUF_SIZE && "overflow" );
#endif

         // ensure nul termination
         buf[MAX_BUF_SIZE-1] = '\0';

         // output to every output stream if the filter matches.
         if (0 != (filter & mGlobalFilter) && 0 != (level.mType & mGlobalLevel))
            for (size_t x = 0; x < mOutStreams.size(); ++x)
               (*mOutStreams[x]) << buf << std::flush;
      }
   }

   /// vararg compatible implementation of print (+ level), most users wont need this.
   inline void operator()( Filter filter, const char fmtstr[], va_list& arg_ptr )
   {
      operator()( filter, _LEVELDEFAULT, fmtstr, arg_ptr );
   }

   /// ostream with filter and level specified
   /// usage:  
   /// @code
   ///   #define StdOut OutputBase<StdOutInit>::instance()
   ///   StdOut( GFX, 2 ) << "this is a test" << std::endl;
   /// @endcode
   inline std::ostream& operator()( Filter filter, Level_ level = _LEVELDEFAULT ) 
	{ 
		mStreamOut.out.mFilter = filter;
		mStreamOut.out.mLevel = level;
		return mStreamOut;
	}	
	
   /// ostream with no args (default filter and level)
   /// usage:  
   /// @code
   ///   #define StdOut OutputBase<StdOutInit>::instance()
   ///   StdOut << "this is a test" << std::endl;
   /// @endcode
   template <typename T>
   inline std::ostream& operator<<( T& blah )
   { 
      if (INCLUDE_IN_RELEASE_MODE || IN_DEBUG_MODE)
      {
         mStreamOut << blah;
      }
      return mStreamOut; 
   }

   /// get output object as an o-stream
   inline operator std::ostream&() { return mStreamOut; }

   /// holds the singleton object for each output type.
	static OutputBase& instance() { static OutputBase blah; return blah; } 

   /// list of output streams available.
   std::vector<std::ostream*> mOutStreams;
   //unsigned int mOutputFilter, mOutputLevel; 
   unsigned int mGlobalFilter, mGlobalLevel; //< global filter setting
   OUTPUTBASE_INIT mOutputBaseInit; //< used to initialize this outputter.

private:
   /// output functor for the OstreamTemplate (mStreamOut)...
	struct OutputAdaptor
	{
      OutputAdaptor() : mParent( NULL ), mFilter( FILTERDEFAULT ), mLevel( _LEVELDEFAULT ) {}
		inline void printf( const char* const str )
		{
         if (INCLUDE_IN_RELEASE_MODE || IN_DEBUG_MODE)
         {
	   		mParent->operator()( mFilter, mLevel, str );
         }
		}
      OutputBase* mParent;
      Filter mFilter;
      Level mLevel;
	};
   /// a stream for this output class...
	OstreamTemplate<char, OutputAdaptor> mStreamOut;
};


/////////////////////////////////////////////////////////////////////////
// --- Define some common output types ---
/////////////////////////////////////////////////////////////////////////

/// Log  (see OutputBase for docs)
/// use this silent logger for debug and trace output, logs messages to a file.
/// output is on by default, call Log::SetFilter to change the filter level...
struct InitLog
{
   void init( OutputBase<InitLog>& l )
   {
      reset( l );
      outstr.open( "log.txt" );
      l.mOutStreams.clear();
      //l.mOutStreams.push_back( &std::cout );
      //l.mOutStreams.push_back( &the<DebuggerTraceWindowOstream>() );
      l.mOutStreams.push_back( &outstr );
   }
   ~InitLog()
   {
      outstr.close();
   }
   inline void reset( OutputBase<InitLog>& l )
   {
      l.SetFilter( spew::FILTERALL );
      l.SetLevel( spew::LEVEL1ANDLOWER );
   }
   std::ofstream outstr;
};
//extern OutputBase<InitLog> Log;
#define Log OutputBase<SPEWNAMESPACE::InitLog>::instance()


/// Trace  (see OutputBase for docs)
/// use Trace for debug output.
/// output is off by default, call Trace::SetFilter to set your filter level...
struct InitTrace
{
   void init( OutputBase<InitTrace>& l )
   {
      reset( l );
      l.mOutStreams.push_back( &std::cout );
      l.mOutStreams.push_back( &the<DebuggerTraceWindowOstream>() );
   }
   inline void reset( OutputBase<InitTrace>& l )
   {
      l.SetFilter( spew::FILTERNONE );
      l.SetLevel( spew::LEVEL1ANDLOWER );
   }
};
#if defined(_DEBUG)
   // Trace exists only in debug mode
   //extern OutputBase<InitTrace> Trace;
   #define Trace OutputBase<SPEWNAMESPACE::InitTrace>::instance()
#else
   // Trace statements are compiled out in Release
   #define Trace FILTERNONE; if (0) SPEWNAMESPACE::StdErr
#endif

/// StdOut  (see OutputBase for docs)
/// for when you're writing a console app that 
/// needs to output on the command line in release 
/// modes for the user to see
/// NOTE: stdout on by default, set to FILTERNONE to hide disable output...
struct InitStdOut
{
   void init( OutputBase<InitStdOut, true>& l )
   {
      reset( l );
      l.mOutStreams.push_back( &std::cout );
   }
   inline void reset( OutputBase<InitStdOut, true>& l )
   {
      l.SetFilter( spew::FILTERALL ); // most people wouldn't use filters with stdout... :)
      l.SetLevel( spew::LEVEL1ANDLOWER );
   }
};
//extern OutputBase<InitStdOut, true> StdOut;
#define StdOut OutputBase<SPEWNAMESPACE::InitStdOut, true>::instance()

/// StdErr  (see OutputBase for docs)
/// for when you're writing a console app that 
/// needs to output on the command line in release 
/// modes for the user to see
/// NOTE: stderr off by default, set to FILTERALL to see stderr output...
struct InitStdErr
{
   void init( OutputBase<InitStdErr, true>& l )
   {
      reset( l );
      l.mOutStreams.push_back( &std::cerr );
   }
   inline void reset( OutputBase<InitStdErr, true>& l )
   {
      l.SetFilter( spew::FILTERNONE );
      l.SetLevel( spew::LEVEL1ANDLOWER );
   }
};
//extern OutputBase<InitStdErr, true> StdErr;
#define StdErr OutputBase<SPEWNAMESPACE::InitStdErr, true>::instance()


/// empty policy, use when making custom type where 
/// you want to add your own ostreams manually.
struct InitEmpty
{
   void init( OutputBase<InitEmpty, true>& l )
   {
      reset( l );
   }
   inline void reset( OutputBase<InitEmpty, true>& l )
   {
      l.SetFilter( spew::FILTERALL );
      l.SetLevel( spew::LEVEL1ANDLOWER );
   }
};


/// helper to set up the outputs via commandline
/// commad line syntax:
///   -[trace|log|stderr|stdout][on|off|level][filter|level]
/// examples:
///   myapp.exe -TraceOnGFX -LogLevel3 -StdErrNone
/// note: in release mode Trace will be compiled out, so cmd line args will have no effect.
inline static void parseCommandLine( int argc, char* argv[] )
{
   // add new output types here... (and below in the switch)
   const int num_types = 4;
   char* types[num_types] = { "Trace", "Log", "StdErr", "StdOut" };
   const int num_functions = 3;
   char* function[num_functions] = { "Off", "On", "Level" };

   // for each command line arg
   for (int x = 0; x < argc; ++x)
   {
      const int notfound = 123456;
      int pos = 1;
      int type = notfound;
      for (int typesIt = 0; typesIt < num_types; ++typesIt)
      {
         if (strnicmp( types[typesIt], &argv[x][pos], strlen( types[typesIt] ) ) == 0)
         {
            type = typesIt;
            pos += (int)strlen( types[typesIt] );
         }
      }
      int func = notfound;
      for (int functionIt = 0; functionIt < num_functions; ++functionIt)
      {
         if (strnicmp( function[functionIt], &argv[x][pos], strlen( function[functionIt] ) ) == 0)
         {
            func = functionIt;
            pos += (int)strlen( function[functionIt] );
         }
      }
      int tag = notfound;
      if (argv[x][pos] == '\0')
      {
         // support simple on/off case i.e. -TraceOff, -LogOn ("" for 3rd param)
         tag = FILTERALL; // add or remove "ALL"
      }
      else
      {
         int filterStringPairsIt = 0;
         while ('\0' != gTagDescriptions[filterStringPairsIt].mName[0])
         {
            if (strnicmp( gTagDescriptions[filterStringPairsIt].mName, &argv[x][pos], strlen( gTagDescriptions[filterStringPairsIt].mName ) ) == 0)
            {
               tag = gTagDescriptions[filterStringPairsIt].mTag;
               pos += (int)strlen( gTagDescriptions[filterStringPairsIt].mName );
            }
            ++filterStringPairsIt;
         }
      }

      // found one, set the flag...
      if (notfound != type && notfound != func && notfound != tag)
      {
         switch (type)
         {
         // add new output types here...
         case 0: 
            switch (func)
            {
            case 0: Trace.RemoveFilter( tag ); break;
            case 1: Trace.AddFilter( tag ); break;
            case 2: Trace.SetLevel( (Level)tag ); break;
            }
            break;
         case 1: 
            switch (func)
            {
            case 0: Log.RemoveFilter( tag ); break;
            case 1: Log.AddFilter( tag ); break;
            case 2: Log.SetLevel( (Level)tag ); break;
            }
            break;
         case 2:
            switch (func)
            {
            case 0: StdErr.RemoveFilter( tag ); break;
            case 1: StdErr.AddFilter( tag ); break;
            case 2: StdErr.SetLevel( (Level)tag ); break;
            }
            break;
         case 3:
            switch (func)
            {
            case 0: StdOut.RemoveFilter( tag ); break;
            case 1: StdOut.AddFilter( tag ); break;
            case 2: StdOut.SetLevel( (Level)tag ); break;
            }
            break;
         } // switch
      }
   } // for argv...
}

/// Unit test for Outputs.
struct OutputUnitTest
{
   static void test()
   {
      /// some common examples
      Log.SetDefaults();
      Log << "Log] default filter, default level" << std::endl;
      Log( GFX ) << "Log] GFX filter, default level" << std::endl;
      Log( SCRIPT, 4 ) << "Log] SCRIPT filter, level 4" << std::endl;
      Log( "Log] default '%s', default '%s'\n", "filter", "level" );
      Log( GFX, "Log] GFX filter, default '%s'\n", "level" );
      Log( SCRIPT, 3, "Log] SCRIPT filter, '%s' '%d'\n", "level", 3 );

      Trace.SetDefaults();
      Trace << "Trace] default filter, default level" << std::endl;
      Trace( GFX ) << "Trace] GFX filter, default level" << std::endl;
      Trace( SCRIPT, 4 ) << "Trace] SCRIPT filter, level 4" << std::endl;
      Trace( "Trace] default '%s', default '%s'\n", "filter", "level" );
      Trace( GFX, "Trace] GFX filter, default '%s'\n", "level" );
      Trace( SCRIPT, 3, "Trace] SCRIPT filter, '%s' '%d'\n", "level", 3 );

      StdErr.SetDefaults();
      StdErr << "StdErr] default filter, default level" << std::endl;
      StdErr( GFX ) << "StdErr] GFX filter, default level" << std::endl;
      StdErr( SCRIPT, 4 ) << "StdErr] SCRIPT filter, level 4" << std::endl;
      StdErr( "StdErr] default '%s', default '%s'\n", "filter", "level" );
      StdErr( GFX, "StdErr] GFX filter, default '%s'\n", "level" );
      StdErr( SCRIPT, 3, "StdErr] SCRIPT filter, '%s' '%d'\n", "level", 3 );

      StdOut.SetDefaults();
      StdOut << "StdOut] default filter, default level" << std::endl;
      StdOut( GFX ) << "StdOut] GFX filter, default level" << std::endl;
      StdOut( SCRIPT, 4 ) << "StdOut] SCRIPT filter, level 4" << std::endl;
      StdOut( "StdOut] default '%s', default '%s'\n", "filter", "level" );
      StdOut( GFX, "StdOut] GFX filter, default '%s'\n", "level" );
      StdOut( SCRIPT, 3, "StdOut] SCRIPT filter, '%s' '%d'\n", "level", 3 );

      OutputBase<InitEmpty, true> mycustomoutput;
      mycustomoutput.SetFilter( FILTERALL );
      mycustomoutput.SetLevel( LEVELALL );
      std::stringstream str;
      mycustomoutput.mOutStreams.push_back( &str );
      StdOut( "testing custom output [" );
      mycustomoutput << "bok" << std::flush;
      if (str.str() == "bok")
      {
         StdOut( "'bok' written successfully", str.str().c_str() );
      }
      StdOut( "]\n" );

      // test filters and levels.
      StdOut( "running filter tests on custom output... [" );
      mycustomoutput.mOutStreams.push_back( &std::cerr );
      mycustomoutput.SetFilter( IO );
      mycustomoutput( GFX, "F" );
      mycustomoutput.SetFilter( GFX );
      mycustomoutput( GFX, "." );
      mycustomoutput.SetFilter( GFX | IO | SCRIPT );
      mycustomoutput( GFX, "." );
      mycustomoutput.SetFilter( GFX | IO | SCRIPT );
      mycustomoutput( ANIM, "F" );
      mycustomoutput.SetFilter( FILTERALL );
      mycustomoutput( PHYSICS, "." );
      mycustomoutput.SetFilter( FILTERALL );
      mycustomoutput.SetLevel( LEVELALL );
      mycustomoutput( PHYSICS, LEVEL1, "." );
      mycustomoutput( PHYSICS, LEVEL2, "." );
      mycustomoutput( PHYSICS, LEVEL3, "." );
      mycustomoutput( PHYSICS, LEVEL4, "." );
      mycustomoutput( PHYSICS, LEVEL5, "." );
      mycustomoutput.SetLevel( LEVEL1ANDLOWER );
      mycustomoutput( PHYSICS, LEVEL1, "." );
      mycustomoutput( PHYSICS, LEVEL2, "F" );
      mycustomoutput( PHYSICS, LEVEL3, "F" );
      mycustomoutput( PHYSICS, LEVEL4, "F" );
      mycustomoutput( PHYSICS, LEVEL5, "F" );
      mycustomoutput.SetLevel( LEVEL2ANDLOWER );
      mycustomoutput( PHYSICS, LEVEL1, "." );
      mycustomoutput( PHYSICS, LEVEL2, "." );
      mycustomoutput( PHYSICS, LEVEL3, "F" );
      mycustomoutput( PHYSICS, LEVEL4, "F" );
      mycustomoutput( PHYSICS, LEVEL5, "F" );
      mycustomoutput.SetLevel( LEVEL5ANDLOWER );
      mycustomoutput( PHYSICS, LEVEL1, "." );
      mycustomoutput( PHYSICS, LEVEL2, "." );
      mycustomoutput( PHYSICS, LEVEL3, "." );
      mycustomoutput( PHYSICS, LEVEL4, "." );
      mycustomoutput( PHYSICS, LEVEL5, "." );
      mycustomoutput.SetFilter( PHYSICS );
      mycustomoutput.SetLevel( LEVEL3 );
      mycustomoutput( PHYSICS, LEVEL1, "F" );
      mycustomoutput( PHYSICS, LEVEL2, "F" );
      mycustomoutput( PHYSICS, LEVEL3, "." );
      mycustomoutput( PHYSICS, LEVEL4, "F" );
      mycustomoutput( PHYSICS, LEVEL5, "F" );
      StdOut( "]\n" );
   }
}; // Unit Test



}; // namespace spew

#endif
