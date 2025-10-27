
#include <bismuth.h>
#include <iostream>

int main()
{
	std::shared_ptr<bis::ClassTemplate> templ = std::make_shared<bis::ClassTemplate>(bis::ClassTemplate({ {bis::GetHintType<int>(), bis::e_IdentityHasher("val"), bis::PropertyAccess::Domain::Public} }, nullptr));
	std::unique_ptr<bis::state> bismuth = std::make_unique<bis::state>();

	bismuth->PushScope();
	bismuth->PushVariable(bis::e_IdentityHasher("test"), new bis::bisInt(30));
	int val = bismuth->GetVariable<int>(bis::e_IdentityHasher("test")).value();
	std::cout << val << std::endl;
	bismuth->PopScope();

	return 0;
}