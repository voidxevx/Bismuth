#include <Bismuth.h>
#include <iostream>

int main()
{
	bismuth::state bismuth = bismuth::state();

	bismuth.PushFunction("print", new bismuth::BismuthFunction_Native(bismuth::ValueType::Nil, {}, [](bismuth::state* _state) {
		const std::string val = _state->popFromReturn<std::string>();
		std::cout << val;
	}));
	bismuth.PushFunction("println", new bismuth::BismuthFunction_Native(bismuth::ValueType::Nil, {}, [](bismuth::state* _state) {
		const std::string val = _state->popFromReturn<std::string>();
		std::cout << val << "\n";
	}));
	bismuth.PushFunction("+", new bismuth::BismuthFunction_Native(bismuth::ValueType::Int, { {bismuth::ValueType::Int, "A", std::nullopt }, {bismuth::ValueType::Int, "B", std::nullopt} }, [](bismuth::state* _state) {
		const int A = _state->var<int>("A");
		const int B = _state->var<int>("B");
		_state->pushToReturn(new int(A + B), bismuth::ValueType::Int, true);
	}));

	const std::string src = R"(
		class Test extends Foo
		{
			public int pubVal;
			protected int proVal;
			private int priVal;

			public Test __init__(int val)
			{
				pubVal = val;
				proVal = 45;
				priVal = 200;
			}

			public int ADD()
			{
				return +(+(pubVal, proVal), priVal);
			}
		}
	)";
	bismuth.BuildString(src);
	bismuth.JoinFinishedBuild();

	bismuth.DoString("Test t = Test(200);");
	bismuth.JoinThread();

	return 0;
}