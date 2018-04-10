# spew is a full featured output system for c++ programmers.

## features:

 * built-in outputs ready to go: Log, Trace, StdErr, StdOut
 * category filters and levels for any output type
 * filter configuration using included command line parsing utility
 * can attach custom ostreams to any output
 * cout (ostream) and printf syntax styles both supported
 * Trace compiles away to nothing in release builds
 * Trace outputs to the MSVC++ debugger output window
 * Log outputs to the file log.txt
 * easy to add in:  in-game consoles, unix, printf, other debuggers, etc... 
 * assert with assertion handler add in support.


## examples:

as seen in example 1 below, spew supports ostream ideas as well as printf ideas.
filters are optional, and may be specified to move output to other channels as a way to control excess text spewage.

example 1. using spew. Trace will output to the msvc++ debugger output window, or stdout in unix.
```
   spew::Trace << "Trace] default filter, default level" << std::endl;
   spew::Trace( spew::IO ) << "Trace] IO filter, default level" << std::endl;
   spew::Trace( spew::SCRIPT, 4 ) << "Trace] SCRIPT filter, level 4" << std::endl;
   spew::Log( "Log] default '%s', default '%s'\n", "filter", "level" );
   spew::StdOut( spew::GFX, "StdOut] GFX filter, default '%s'\n", "level" );
   spew::StdErr( spew::SOUND, 3, "StdErr] SOUND filter, '%s' '%d'\n", "level", 3 );
```

example 2. spew can self-configure using command line flags, or manually using function calls
```
   spew::parseCommandLine( argc, argv ); // put this first thing in main(..)

   spew::Log.SetFilter( spew::GFX ); // can configure from c++ side also...
   spew::Log.SetLevel( 3 ); // can configure from c++ side also...

   ::: syntax to pass command line args to spew
   > myapp.exe -TraceOnGfx -TraceLevel4 -StdErrOff -TraceOn
```


## license

```
/*
   spew - full featured trace logger, debug output, stderr/stdout
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
```
