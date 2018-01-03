/*
 * The pattern for saving memory (basically) by sharing intrinsic properties of
 * objects.  Imagine a huge number of similar objects which all have most of
 * their properties the same. It is natural to move these properties out of
 * these objects to some external data structure and provide each object with
 * the link to that data structure.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#define NUMBER_OF_SAME_TYPE_CHARS 3;

class FlyweightCharacter {
  unsigned short fontSizeIndex;  // index instead of actual font size
  unsigned short fontNameIndex;  // index instead of font name
  unsigned positionInStream;     // extrinsic property

public:
  FlyweightCharacter(unsigned short fontSizeIndex, unsigned short fontNameIndex,
    unsigned short positionInStream)
      : fontSizeIndex(fontSizeIndex),
        fontNameIndex(fontNameIndex),
        positionInStream(positionInStream) {}
  void print();
};

/*
 * Factory is a class holding the properties which are shared by many objects.
 * So instead of keeping these properties in those objects we keep them
 * externally, making objects flyweight. See more details in the comments of
 * main function.
 */
class Factory {
  Factory() {}

public:
  // lets imagine that sizes may be of floating point type
  static std::vector<float> fontSizes;
  static std::vector<std::string> fontNames;  // font name may be of variable
                                              // length (lets take 6 bytes is
                                              // average)

  static void setFontsAndNames() {
    fontSizes[0] = 1.0;
    fontSizes[1] = 1.5;
    fontSizes[2] = 2.0;

    fontNames[0] = "first_font";
    fontNames[1] = "second_font";
    fontNames[2] = "third_font";
  }
  static FlyweightCharacter* createFlyweightCharacter(
    unsigned short fontSizeIndex, unsigned short fontNameIndex,
    unsigned short positionInStream) {
    return new FlyweightCharacter(
      fontSizeIndex, fontNameIndex, positionInStream);
  }
};

std::vector<float> Factory::fontSizes(3);
std::vector<std::string> Factory::fontNames(3);

void FlyweightCharacter::print() {
  std::cout << "Font Size: " << Factory::fontSizes[fontSizeIndex]
            << ", font Name: " << Factory::fontNames[fontNameIndex]
            << ", character stream position: " << positionInStream << std::endl;
}

int main(int argc, char** argv) {
  std::vector<std::unique_ptr<FlyweightCharacter>> chars;

  Factory::setFontsAndNames();
  unsigned short limit = NUMBER_OF_SAME_TYPE_CHARS;

  for (unsigned short i = 0; i < limit; i++) {
    chars.push_back(std::unique_ptr<FlyweightCharacter>(
      Factory::createFlyweightCharacter(0, 0, i)));
    chars.push_back(std::unique_ptr<FlyweightCharacter>(
      Factory::createFlyweightCharacter(1, 1, i + 1 * limit)));
    chars.push_back(std::unique_ptr<FlyweightCharacter>(
      Factory::createFlyweightCharacter(2, 2, i + 2 * limit)));
  }

  for (unsigned short i = 0; i < chars.size(); i++) {
    chars[i]->print();
  }

  std::cin.get();
  return 0;
}
