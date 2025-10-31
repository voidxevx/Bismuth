
#include <bismuth.h>
#include <iostream>

int main()
{

	bis::state* bis = new bis::state();

	const std::string src = R"(
		mutable int myVal = 12;
		const int copy = myVal;
	)";

	bis::token* tokens = bis->tokenize(src);
	bis->PushTokens(tokens);

	bis->PushScope();
	bis->Evaluate();
	bis->PopScope();

	return 0;
}