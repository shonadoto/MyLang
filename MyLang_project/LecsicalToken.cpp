#include "LecsicalToken.h"

void LecsicalToken::tokenDebug() {
    std::cout << name_ << " ";
	switch (type_)
	{
	case LecsicalEnum::NONE:
		std::cout << "NONE";
		break;
	case LecsicalEnum::RESERVED:
		std::cout << "RESERVED";
		break;
	case LecsicalEnum::NAME:
		std::cout << "NAME";
		break;
	case LecsicalEnum::OPERATION:
		std::cout << "OPERATION";
		break;
	case LecsicalEnum::COLON:
		std::cout << "COLON";
		break;
	case LecsicalEnum::SEMI_COLON:
		std::cout << "SEMI_COLON";
		break;
	case LecsicalEnum::COMMA:
		std::cout << "COMMA";
		break;
	case LecsicalEnum::SQUARE_BRACE:
		std::cout << "SQUARE_BRACE";
		break;
	case LecsicalEnum::CURLY_BRACE:
		std::cout << "CURLY_BRACE";
		break;
	case LecsicalEnum::ROUND_BRACE:
		std::cout << "ROUND_BRACE";
		break;
	case LecsicalEnum::SPACE:
		std::cout << "SPACE";
		break;
	case LecsicalEnum::CONST_BOOL:
		std::cout << "CONST_BOOL";
		break;
	case LecsicalEnum::CONST_CHAR:
		std::cout << "CONST_CHAR";
		break;
	case LecsicalEnum::CONST_INT:
		std::cout << "CONST_INT";
		break;
	case LecsicalEnum::CONST_FLOAT:
		std::cout << "CONST_FLOAT";
		break;
	case LecsicalEnum::CONST_STRING:
		std::cout << "CONST_STRING";
		break;
	default:
		break;
	}
	std::cout << "\n";
}

std::string LecsicalToken::name() { return name_; }
int LecsicalToken::lineNumber() { return line_number_; }
int LecsicalToken::charNumber() { return char_number_; }
LecsicalEnum LecsicalToken::type() { return type_; }
std::string LecsicalToken::file_name() { return file_name_; }
void LecsicalToken::setName(std::string name) { name_ = name; }