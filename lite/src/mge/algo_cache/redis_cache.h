/**
 * \file lite/src/mge/algo_cache/redis_cache.h
 *
 * This file is part of MegEngine, a deep learning framework developed by
 * Megvii.
 *
 * \copyright Copyright (c) 2020-2020 Megvii Inc. All rights reserved.
 */

#pragma once

#include "lite_build_config.h"

#if !defined(WIN32) && LITE_BUILD_WITH_MGE && LITE_WITH_CUDA
#include <cpp_redis/cpp_redis>
#include <string>
#include <vector>
#include "megbrain/utils/persistent_cache.h"

namespace lite {

//! TODO: fix one thread set cache when other threads is using old cache
class RedisCache final : public mgb::PersistentCache {
public:
    RedisCache(std::string redis_ip, size_t port, std::string password);

    bool is_valid() { return m_client.is_connected(); }
    ~RedisCache() {}
    void init(std::shared_ptr<mgb::PersistentCache> old) { m_old = old; }

    mgb::Maybe<Blob> get(const std::string& category, const Blob& key) override;

    void put(const std::string& category, const Blob& key,
             const Blob& value) override;

private:
    std::shared_ptr<mgb::PersistentCache> m_old;
    LITE_MUTEX m_mtx;
    cpp_redis::client m_client;
    const std::string m_ip;
    const size_t m_port;
    const std::string m_password;
};

}  // namespace lite
#endif
// vim: syntax=cpp.doxygen foldmethod=marker foldmarker=f{{{,f}}}
