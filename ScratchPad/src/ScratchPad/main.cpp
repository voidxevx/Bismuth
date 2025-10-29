
#include <bismuth.h>
#include <iostream>

int main()
{
	std::shared_ptr<bis::ClassTemplate> templ = std::make_shared<bis::ClassTemplate>(bis::ClassTemplate({ {bis::GetHintType<int>(), bis::e_IdentityHasher("val"), bis::PropertyAccess::Domain::Public}, {bis::GetHintType<std::string>(), bis::e_IdentityHasher("priVal"), bis::PropertyAccess::Domain::Private} }, nullptr));
	std::unique_ptr<bis::state> bismuth = std::make_unique<bis::state>();

	bis::token* trace = bismuth->tokenize("testClass:val = 12");

	bismuth->PushScope();

	bismuth->PushTokens(trace->Next);

	bismuth->PushVariable(bis::e_IdentityHasher("testClass"), new bis::bisClass(templ), true);

	const unsigned int varLoc = bismuth->TraceIdentifier();

	bismuth->PopScope();


	return 0;
}