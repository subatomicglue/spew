#include "Assertion.h"

int main()
{
	//STATICASSERT( false, "a message for the static assert!" );
	ASSERT( false, "false was hit!" );
	ASSERT( true, "false was not hit!" );
	std::cout << "thanks for continuing..." << std::endl;
}
