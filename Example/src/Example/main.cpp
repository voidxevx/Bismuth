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
		class Test
		{
			public int pubVal;
			protected int proVal;
			private int priVal;
		}	
	)";
	bismuth.Build(bismuth.tokenize(src));


	//bismuth.DoString("foo(100, 300);");
	//bismuth.JoinThread();

	/*struct testHandle
	{
		int myVal;

		testHandle(int v)
			: myVal(v)
		{}
	};

	testHandle* testH = new testHandle(12);

	bismuth::bis_class_template testClass_templ = bismuth::BismuthClassTemplate::CreateClassTemplate({ {"myVal", bismuth::ValueType::Int} });

	bismuth::bis_class testClass_inst = bismuth::bis_class(bismuth::ValueType::Custom, new bismuth::BismuthClassInstance{ testClass_templ });
	bismuth::bis_class testClass_handle = bismuth::bis_class(bismuth::ValueType::Custom, new bismuth::BismuthClassInstace_UserHandle{ { {"myVal", bismuth::BismuthUserHandle{bismuth::ValueType::Int, &testH->myVal }} } });

	bismuth::IBismuthClassInstance::SetInstanceProperty<int>(testClass_inst, "myVal", 30);
	bismuth::IBismuthClassInstance::SetInstanceProperty<int>(testClass_handle, "myVal", 30);*/

	return 0;
}