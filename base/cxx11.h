#ifndef TOFT_BASE_CXX11_H
#define TOFT_BASE_CXX11_H
#pragma once

#include <features.h>

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus == 201103L
#define TOFT_CXX11_ENABLED 1
#endif

#endif // TOFT_BASE_CXX11_H
