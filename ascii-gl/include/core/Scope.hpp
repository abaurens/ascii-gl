#pragma once

#include <memory>

template<class T>
using Scope = std::unique_ptr<T>;