#include <Bismuth.h>
#include <iostream>

int main()
{
	bismuth::state bismuth = bismuth::state();

	bismuth.PushFunction("print", new bismuth::BismuthFunction_Native([](bismuth::state* _state) {
		const std::string val = _state->popFromReturn<std::string>();
		std::cout << val;
	}));
	bismuth.PushFunction("println", new bismuth::BismuthFunction_Native([](bismuth::state* _state) {
		const std::string val = _state->popFromReturn<std::string>();
		std::cout << val << "\n";
	}));

	const std::string src = R"(
		println("Hello,%world");
	)";


	bismuth.DoString(src);
	bismuth.JoinThread();

	return 0;
}