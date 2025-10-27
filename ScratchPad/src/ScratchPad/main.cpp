
#include <bismuth.h>
#include <iostream>

int main()
{
	std::shared_ptr<bis::ClassTemplate> templ = std::make_shared<bis::ClassTemplate>(bis::ClassTemplate({ {bis::GetHintType<int>(), bis::e_IdentityHasher("val"), bis::PropertyAccess::Domain::Public} }, nullptr));
	std::unique_ptr<bis::state> bismuth = std::make_unique<bis::state>();

	bismuth->PushScope();
	bismuth->PushVariable(bis::e_IdentityHasher("test"), new bis::bisInt(30));
	bismuth->PushVariable(bis::e_IdentityHasher("another"), new bis::bisBool(false));
	bismuth->PushVariable(bis::e_IdentityHasher("again"), new bis::bisString("Hello, world"));

	bismuth->PushScope();

	bismuth->PushVariable(bis::e_IdentityHasher("inside"), new bis::bisByte('h'));


	int val = bismuth->GetVariable<int>(bis::e_IdentityHasher("test")).value();
	std::cout << val << std::endl;


	bismuth->PopScope();
	bismuth->PopScope();

	return 0;
}