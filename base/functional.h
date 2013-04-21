#ifndef TOFT_BASE_FUNCTION_H
#define TOFT_BASE_FUNCTION_H
#pragma once

#include "toft/base/cxx11.h"

#ifdef TOFT_CXX11_ENABLED

#include <functional>

#else

#include <tr1/functional>

namespace std {
using tr1::function;
using tr1::bind;
using tr1::bad_function_call;
namespace placeholders = tr1::placeholders;
}

#endif // TOFT_CXX11_ENABLED

#endif // TOFT_BASE_FUNCTION_H
