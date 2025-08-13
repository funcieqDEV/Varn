#pragma once
#include "../expr_node.hpp"

class ConstInt : ExpressionNode {
public:
    int value_;
    ConstInt() = default;
    ConstInt(int value) : value_(value){}
};