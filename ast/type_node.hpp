#pragma once
#include "node.hpp"
#include "expr_node.hpp"
#include <vector>
#include <memory>
#include <iostream>

class TypeNode : public Node {
public:
    std::string typeName;
    bool isReference;
    bool isPointer;
    bool isArray;
    ExpressionNode arraySize; //0 for non-array types
};