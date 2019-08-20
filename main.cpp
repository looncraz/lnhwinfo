#include "Application.h"
#include "Utils.h"

int main(int argc, char **argv)
{
	HWUtils::Run(argc, argv);
	return (new HWApplication())->run(1, argv);
}
