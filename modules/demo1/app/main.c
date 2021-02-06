

#include "xx.h"

int main(void)
{
	int a = 255;

	print_debug("In %s file:\n", __FILE__);
	print_debug("%s:%dline %d\n", __func__, __LINE__, a);

	return 0;
}
