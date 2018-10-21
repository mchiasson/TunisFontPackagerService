/*
 * MIT License
 *
 * Copyright (c) 2017-2018 Mathieu-Andre Chiasson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef KTDPNG_H
#define KTDPNG_H

#include <string>
#include <vector>
#include <cassert>

class TunisPNG
{
public:

    enum ColorType
    {
        UNKNOWN,
        MONO,        // 1 byte per texel.
        MONO_ALPHA,  // 2 bytes per texel.
        RGB,         // 3 bytes per texel.
        RGBA         // 4 bytes per texel.
    };

    struct ColorMono      {uint8_t m;};
    struct ColorMonoAlpha {uint8_t m, a;};
    struct ColorRGB       {uint8_t r, g, b;};
    struct ColorRGBA      {uint8_t r, g, b, a;};

    TunisPNG();
    TunisPNG(size_t width, size_t height, ColorType type);
    TunisPNG(const std::vector<uint8_t> &buffer, size_t width, size_t height, ColorType type);
    ~TunisPNG();

    void setBuffer(const std::vector<uint8_t> &buffer, size_t width, size_t height, ColorType type);

    bool load(const std::string &fileName);
    bool store(const std::string &fileName);

    inline std::vector<uint8_t> &getBuffer() {return m_buffer;}
    inline ColorMono *getBufferAsMono() {assert(m_type == MONO); return reinterpret_cast<ColorMono *>(m_buffer.data());}
    inline ColorMonoAlpha *getBufferAsMonoAlpha() {assert(m_type == MONO_ALPHA); return reinterpret_cast<ColorMonoAlpha *>(m_buffer.data());}
    inline ColorRGB *getBufferAsRGB() {assert(m_type == RGB); return reinterpret_cast<ColorRGB *>(m_buffer.data());}
    inline ColorRGBA *getBufferAsRGBA() {assert(m_type == RGBA); return reinterpret_cast<ColorRGBA *>(m_buffer.data());}
    inline size_t getWidth() const  {return m_width;}
    inline size_t getHeight() const  {return m_height;}
    inline ColorType getType() const {return m_type;}

private:

    TunisPNG(const TunisPNG &other);
    TunisPNG &operator=(const TunisPNG &other);

    std::vector<uint8_t> m_buffer;
    size_t m_width;
    size_t m_height;
    ColorType m_type;
};

#endif // KTDPNG_H
