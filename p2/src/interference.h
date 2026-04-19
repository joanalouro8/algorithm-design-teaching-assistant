#pragma once

#include "web.h"
#include "parser.h"

bool sameInstruction(const RangePoint &a, const RangePoint &b);

bool has_interference(const Web &web1, const Web &web2);