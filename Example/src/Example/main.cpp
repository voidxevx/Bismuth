#include <Bismuth.h>
#include <iostream>

int main()
{
	bismuth::state bismuth = bismuth::state();

	const std::string src = R"(
		int val1 = 300;
		int val2 = 20;
		val1 = val2;
	)";


	bismuth.DoString(src);

	std::cout << bismuth.GetVariable<int>("val1") << std::endl;

	return 0;
}