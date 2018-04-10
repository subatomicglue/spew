
#include <stdio.h>
#include "Output.h"
#include "Once.h"
#include <assert.h>

void hit_a_key()
{
   std::cout << "\npress a key..." << std::endl;
   getchar();
}

int main( int argc, char *argv[] )
{
   spew::parseCommandLine( argc, argv );

   spew::Trace( spew::GFX,1 ) << "Trace] filter:gfx level:1 is on" << std::endl;
   spew::Trace( spew::GFX,2 ) << "Trace] filter:gfx level:2 is on" << std::endl;
   spew::Trace( spew::GFX,3 ) << "Trace] filter:gfx level:3 is on" << std::endl;
   spew::Trace( spew::GFX,4 ) << "Trace] filter:gfx level:4 is on" << std::endl;
   spew::Trace( spew::GFX,5 ) << "Trace] filter:gfx level:5 is on" << std::endl;
   spew::Trace( spew::SCRIPT,1 ) << "Trace] filter:SCRIPT level:1 is on" << std::endl;
   spew::Trace( spew::SCRIPT,2 ) << "Trace] filter:SCRIPT level:2 is on" << std::endl;
   spew::Trace( spew::SCRIPT,3 ) << "Trace] filter:SCRIPT level:3 is on" << std::endl;
   spew::Trace( spew::SCRIPT,4 ) << "Trace] filter:SCRIPT level:4 is on" << std::endl;
   spew::Trace( spew::SCRIPT,5 ) << "Trace] filter:SCRIPT level:5 is on" << std::endl;
   spew::Trace( spew::IO, 1, "Trace] filter:IO level:1 is on\n" );
   spew::Trace( spew::IO, 2, "Trace] filter:IO level:2 is on\n" );
   spew::Trace( spew::IO, 3, "Trace] filter:IO level:3 is on\n" );
   spew::Trace( spew::IO, 4, "Trace] filter:IO level:4 is on\n" );
   spew::Trace( spew::IO, 5, "Trace] filter:IO level:5 is on\n" );

   for (int x = 0; x < 10; ++x)
   {
      SPEW_ONCE( spew::StdOut( "this only outputs once\n" ), __LINE__, int );
   }

   // do unit tests...
   spew::OutputUnitTest::test();

   hit_a_key();
	return 0;
}
