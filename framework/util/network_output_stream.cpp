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

#include "util/network_output_stream.h"

#include "util/logging.h"
#include "util/platform.h"

#include <chrono>

using asio::ip::tcp;

GFXRECON_BEGIN_NAMESPACE(gfxrecon)
GFXRECON_BEGIN_NAMESPACE(util)

NetworkOutputStream::NetworkOutputStream(const std::string& address)
{
    context = std::make_unique<asio::io_context>();

    const auto separator = address.find_last_of(':');
    const auto host = address.substr(0, separator);
    const auto port = address.substr(separator + 1);

    tcp::resolver resolver(*context);
    tcp::resolver::query query(host, port);

    asio::error_code ec;

    do
    {
        auto endpoint_iterator = resolver.resolve(query, ec);

        if (ec)
        {
            GFXRECON_LOG_ERROR("Could not resolve to %s due to %s... waiting for 1s", address.c_str(), ec.message().c_str());
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        socket = std::make_unique<asio::ip::tcp::socket>(*context);
        asio::connect(*socket, endpoint_iterator, ec);

        if (ec)
        {
            GFXRECON_LOG_ERROR("Could not connect to %s due to %s... waiting for 1s", address.c_str(), ec.message().c_str());
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    } while (ec);
}

NetworkOutputStream::~NetworkOutputStream()
{
}

size_t NetworkOutputStream::Write(const void* data, size_t len)
{
    std::lock_guard<std::mutex> guard(write_mutex);

    asio::error_code ec;
    size_t size = asio::write(*socket, asio::buffer(data, len), asio::transfer_all(), ec);

    if (ec && ec != asio::error::broken_pipe)
    {
        GFXRECON_LOG_ERROR("Write failed due to %s", ec.message().c_str());
        exit(1);
    }

    if (ec)
        return 0;

    return size;
}

GFXRECON_END_NAMESPACE(util)
GFXRECON_END_NAMESPACE(gfxrecon)
