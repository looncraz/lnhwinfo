#include "Application.h"
#include "Utils.h"

int main(int argc, char **argv)
{
	if (HWUtils::Run(argc, argv))
		return (new HWApplication())->run(1, argv);

	fprintf(stderr, "Root password required!\n");
	return 1;
}
