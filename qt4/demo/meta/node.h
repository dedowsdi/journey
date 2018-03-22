#ifndef NODE_H
#define NODE_H
#include <QMetaType>

#include <iostream>

// Any class or struct that has a public default constructor, a public copy
// constructor, and a public destructor can be registered to QMetaType
class Node{

public:

  Node(){
    std::cout << "Node()" << std::endl;
  }

  Node(const Node& rhs){
    std::cout << "Node(const Node& rhs)" << std::endl;
  }

  ~Node(){
    std::cout << "~Node()" << std::endl;
  }

  void print() const{
    std::cout<< "node print" << std::endl;
  }
};

Q_DECLARE_METATYPE(Node);

#endif /* NODE_H */
