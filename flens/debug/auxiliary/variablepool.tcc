/*
 *   Copyright (c) 2009, Michael Lehn
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1) Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2) Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3) Neither the name of the FLENS development group nor the names of
 *      its contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FLENS_DEBUG_AUXILIARY_VARIABLEPOOL_TCC
#define FLENS_DEBUG_AUXILIARY_VARIABLEPOOL_TCC 1

#include <cxxstd/iostream.h>
#include <cxxstd/string.h>
#include <cxxstd/sstream.h>
#include <flens/auxiliary/issame.h>
#include <flens/debug/auxiliary/basename.h>
#include <flens/debug/auxiliary/typeid.h>
#include <flens/debug/auxiliary/variablepool.h>

namespace flens { namespace verbose {

template <typename T>
std::string
VariablePool::name(const T &var)
{
    unsigned long address = reinterpret_cast<unsigned long>(&var);
    std::string   type = typeId(var);

    std::stringstream sstream;
    sstream << type << address;
    std::string key = sstream.str();
    sstream.str("");
    sstream.clear();

    if (typeCount_.count(type)==0) {
        typeCount_[type] = 0;
    }
    if (tmpTron && id_.count(key)==0) {
        isTmp_[key] = 1;
    }
    if (id_.count(key)==0) {
        id_[key] = ++typeCount_[type];
    }

    if (isTmp_.count(key)>0 && isTmp_[key]==1) {
        sstream << "tmp_";
    }

    sstream << basename(var) << id_[key];
    return sstream.str();
}

template <typename T>
std::string
VariablePool::name(const T &var) const
{
    unsigned long address = reinterpret_cast<unsigned long>(&var);
    std::string   type = typeId(var);

    std::stringstream sstream;
    sstream << type << address;
    std::string key = sstream.str();
    sstream.str("");
    sstream.clear();

    if (typeCount_.count(type)==0) {
        return "unknown";
    }
    if (id_.count(key)==0) {
        return "unkown";
    }

    sstream << basename(var) << id_.find(key)->second;
    return sstream.str();
}

template <typename T>
void
VariablePool::addTemporary(const T &var)
{
    unsigned long address = reinterpret_cast<unsigned long>(&var);
    std::string   type = typeId(var);

    std::stringstream sstream;
    sstream << type << address;
    std::string key = sstream.str();
    sstream.str("");
    sstream.clear();

    isTmp_[key] = 1;
}

template <typename T>
void
VariablePool::removeTemporary(const T &var)
{
    unsigned long address = reinterpret_cast<unsigned long>(&var);
    std::string   type = typeId(var);

    std::stringstream sstream;
    sstream << type << address;
    std::string key = sstream.str();
    sstream.str("");
    sstream.clear();

    isTmp_.erase(key);
    id_.erase(key);
}

} } // namespace verbose, namespace flens

#endif // FLENS_DEBUG_AUXILIARY_VARIABLEPOOL_TCC
