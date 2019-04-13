// very basic parser, doesn't support float, power
#include <iostream>

std::string expressionString;
const char* expressionToParse;

char peek()
{
  return *expressionToParse;
}

char get()
{
  return *expressionToParse++;
}

int expression();

// 0-9
int number()
{
  int result = get() - '0';
  while (peek() >= '0' && peek() <= '9')
  {
    result = 10*result + get() - '0';
  }
  return result;
}

// number|(expression)|-factor
int factor()
{
  if (peek() >= '0' && peek() <= '9')
    return number();
  else if (peek() == '(')
  {
    get(); // '('
    int result = expression();
    get(); // ')'
    return result;
  }
  else if (peek() == '-')
  {
    get();
    return -factor();
  }
  throw std::runtime_error("factor error");
}

// factor[*/]factor[*/]factor...
int term()
{
  int result = factor();
  while (peek() == '*' || peek() == '/')
    if (get() == '*')
      result *= factor();
    else
      result /= factor();
  return result;
}

// term[+-]term[+-]term...
int expression()
{
  int result = term();
  while (peek() == '+' || peek() == '-')
    if (get() == '+')
      result += term();
    else
      result -= term();
  return result;
}

int main(int argc, char *argv[])
{
  while(std::cin >> expressionString)
  {
    if(expressionString.front() == 'q')
      break;

    expressionToParse = expressionString.c_str();
    std::cout << expression() << std::endl;
  }

  return 0;
}
