/*
 * Copyright (c) 2004-2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Erik Hallnor
 *          Steve Reinhardt
 *          Lisa Hsu
 *          Kevin Lim
 */

/**
 * @file
 * Cache template instantiations.
 */

#include "mem/config/cache.hh"

#if defined(USE_CACHE_LRU)
#include "mem/cache/tags/lru.hh"
#endif

#if defined(USE_CACHE_DCLRU)
#include "mem/cache/tags/dclru.hh"
#endif

#if defined(USE_CACHE_WPLRU)
#include "mem/cache/tags/wplru.hh"
#endif

#if defined(USE_CACHE_SPLRU)
#include "mem/cache/tags/splru.hh"
#endif

#if defined(USE_CACHE_FALRU)
#include "mem/cache/tags/fa_lru.hh"
#endif

#if defined(USE_CACHE_IIC)
#include "mem/cache/tags/iic.hh"
#endif

#include "mem/cache/cache_impl.hh"

// Template Instantiations
#ifndef DOXYGEN_SHOULD_SKIP_THIS


#if defined(USE_CACHE_FALRU)
template class Cache<FALRU>;
template class SplitMSHRCache<FALRU>;
template class SplitRPortCache<FALRU>;
template class DirtyCache<FALRU>;
#endif

#if defined(USE_CACHE_IIC)
template class Cache<IIC>;
template class SplitMSHRCache<IIC>;
template class SplitRPortCache<IIC>;
template class DirtyCache<IIC>;
#endif

#if defined(USE_CACHE_LRU)
template class Cache<LRU>;
template class SplitMSHRCache<LRU>;
template class SplitRPortCache<LRU>;
template class DirtyCache<LRU>;
#endif

#if defined(USE_CACHE_WPLRU)
template class Cache<WPLRU>;
template class SplitMSHRCache<WPLRU>;
template class SplitRPortCache<WPLRU>;
template class DirtyCache<WPLRU>;
#endif

#if defined(USE_CACHE_SPLRU)
template class Cache<SPLRU>;
template class SplitMSHRCache<SPLRU>;
template class SplitRPortCache<SPLRU>;
template class DirtyCache<SPLRU>;
#endif

#if defined(USE_CACHE_DCLRU)
template class Cache<DCLRU>;
template class SplitMSHRCache<DCLRU>;
template class SplitRPortCache<DCLRU>;
template class DirtyCache<DCLRU>;
#endif

#endif //DOXYGEN_SHOULD_SKIP_THIS
