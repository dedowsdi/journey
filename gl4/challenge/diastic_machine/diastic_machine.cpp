/*
 * use every character and it's index in seed to find word in sentence.
 */
#include <iostream>
#include <string_util.h>
#include <stream_util.h>

int main(int argc, char *argv[])
{

  if(argc < 3)
  {
    std::cout << "wrong command, it should be " << argv[0] << " seed filename" << std::endl;
  }

  std::string seed = argv[1];
  std::string filepath = argv[2];
  std::string file_content = stream_util::read_resource(filepath);
  auto words = string_util::split_regex(file_content, R"(\W+)");

  std::string::size_type next_index = 0;
  std::string result;
  for (int i = 0; i < seed.size(); ++i) 
  {
    for (int j = next_index; j < words.size(); ++j)
    {
      const std::string& word = words[j];
      if(word.size() > i && word.at(i) == seed.at(i))
      {
        if(!result.empty())
          result += " ";
        result += word;
        next_index = j + 1;
        break;
      }
    }
  }

  std::cout << result << std::endl;
  
  return 0;
}
