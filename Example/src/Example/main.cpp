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

	bismuth.DoString("Test t;");
	bismuth.JoinThread();

	bismuth::bis_class_template templ = bismuth::BismuthClassTemplate::CreateClassTemplate("Test", { {bismuth::ValueType::Int, bismuth::BismuthClassDomain::Public, "val" }}, {});

	bismuth.PushScope();

	bismuth.PushVariable("testClass", new bismuth::bis_class(bismuth::ValueType::Custom, new bismuth::BismuthClassInstance(templ) ));
	bismuth::IBismuthClassInstance::SetInstanceProperty(bismuth.var<bismuth::IBismuthClassInstance*>("testClass"), "val", 30);
	unsigned int _i = 0;
	bismuth::state::TraceObject to =  bismuth.TraceForObjects(bismuth.tokenize(":testClass:val"), _i);
	int* val = static_cast<int*>(to.ptr);
	std::cout << *val << std::endl;

	bismuth.PopScope();

	return 0;
}