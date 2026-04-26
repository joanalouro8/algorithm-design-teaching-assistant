#pragma once

#include "parser.h"

struct Web;

bool sameInstruction(const RangePoint &a, const RangePoint &b);

bool has_interference(const Web &web1, const Web &web2);

bool is_same_web(const Web &web, const LiveRange &live_range);