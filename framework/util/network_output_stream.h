/*
 * Copyright © 2021 Igalia S.L.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef GFXRECON_UTIL_NETWORK_OUTPUT_STREAM_H
#define GFXRECON_UTIL_NETWORK_OUTPUT_STREAM_H

#include "util/defines.h"
#include "util/output_stream.h"
#include "util/platform.h"

#include <cstdio>
#include <string>
#include <mutex>

#define ASIO_DISABLE_THREADS
#include "asio.hpp"

GFXRECON_BEGIN_NAMESPACE(gfxrecon)
GFXRECON_BEGIN_NAMESPACE(util)

class NetworkOutputStream : public OutputStream
{
  public:
    NetworkOutputStream(const std::string& address);

    virtual ~NetworkOutputStream() override;

    virtual bool IsValid() override { return socket.get() != nullptr; }

    virtual size_t Write(const void* data, size_t len) override;

    virtual void Flush() override {  }

  private:
    std::unique_ptr<asio::io_context> context;
    std::unique_ptr<asio::ip::tcp::socket> socket;

    std::mutex write_mutex;
};

GFXRECON_END_NAMESPACE(util)
GFXRECON_END_NAMESPACE(gfxrecon)

#endif // GFXRECON_UTIL_NETWORK_OUTPUT_STREAM_H
