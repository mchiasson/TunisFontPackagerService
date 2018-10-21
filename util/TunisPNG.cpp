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
#include "TunisPNG.h"

#include <cassert>
#include <png.h>
#include <Poco/LogStream.h>

TunisPNG::TunisPNG() :
    m_width(0),
    m_height(0),
    m_type(UNKNOWN)
{
}

TunisPNG::TunisPNG(size_t width, size_t height, ColorType type)
{
    m_width  = width;
    m_height = height;
    m_type   = type;

    switch(type)
    {
        case MONO:
            m_buffer.resize(width * height);
            break;
        case MONO_ALPHA:
            m_buffer.resize(width * height * 2);
            break;
        case RGB:
            m_buffer.resize(width * height * 3);
            break;
        case RGBA:
            m_buffer.resize(width * height * 4);
            break;
        default:
            m_buffer.resize(width * height * 4);
            break;
    }
}


TunisPNG::TunisPNG(const std::vector<uint8_t> &buffer, size_t width, size_t height, ColorType type)
{
    setBuffer(buffer, width, height, type);
}

TunisPNG::~TunisPNG()
{
    m_width = 0;
    m_height = 0;
    m_type = UNKNOWN;
}

void TunisPNG::setBuffer(const std::vector<uint8_t> &buffer, size_t width, size_t height, ColorType type)
{
    m_buffer = buffer;
    m_width  = width;
    m_height = height;
    m_type   = type;

    switch(type)
    {
    case MONO:
        assert(m_buffer.size() == (width * height));
        break;
    case MONO_ALPHA:
        assert(m_buffer.size() == (width * height * 2));
        break;
    case RGB:
        assert(m_buffer.size() == (width * height * 3));
        break;
    case RGBA:
        assert(m_buffer.size() == (width * height * 4));
        break;
    default:
        assert(m_buffer.size() == 0);
        break;
    }
}


bool TunisPNG::load(const std::string &file_name)
{
    Poco::LogStream log(Poco::Logger::get("util"));

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name.c_str(), "rb");
    if (!fp)
    {
        log.error() << "File " << file_name << " could not be opened for reading" << std::endl;
        return false;
    }

    unsigned char sig[8];
    fread(sig, 1, 8, fp);
    if (png_sig_cmp(sig, 0, 8))
    {
        log.error() << "File " << file_name << " is not recognized as a PNG file" << std::endl;
        fclose(fp);
        return false;
    }

    /* initialize stuff */
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png)
    {
        log.error() << "png_create_read_struct failed" << std::endl;
        fclose(fp);
        return false;
    }

    png_infop png_info = png_create_info_struct(png);
    if (!png_info)
    {
        log.error() << "png_create_info_struct failed" << std::endl;
        fclose(fp);
        return false;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        log.error() << "Error during init_io" << std::endl;
        fclose(fp);
        return false;
    }

    png_init_io(png, fp);
    png_set_sig_bytes(png, 8);

    png_read_info(png, png_info);

    m_width = png_get_image_width(png, png_info);
    m_height = png_get_image_height(png, png_info);
    png_byte png_color_type = png_get_color_type(png, png_info);
    png_byte bit_depth = png_get_bit_depth(png, png_info);

    if(bit_depth == 16)
    {
        png_set_strip_16(png); // change 16bits depth to 8bits depth
    }

    if(png_get_valid(png, png_info, PNG_INFO_tRNS))
    {
        png_set_tRNS_to_alpha(png); // no frigging idea what this tRNS is, but let's make it an alpha channel for now :-P
    }

    switch(png_color_type)
    {
    case PNG_COLOR_TYPE_PALETTE:
        png_set_palette_to_rgb(png);
        m_type = RGB;
        break;
    case PNG_COLOR_TYPE_GRAY:
        if (bit_depth < 8)
        {
            png_set_expand_gray_1_2_4_to_8(png); // change 1bit/2bits/4bits depth to 8bits depth
        }
        m_type = MONO;
        break;
    case PNG_COLOR_TYPE_GA:
        m_type = MONO_ALPHA;
        break;
    case PNG_COLOR_TYPE_RGB:
        m_type = RGB;
        break;
    case PNG_COLOR_TYPE_RGBA:
        m_type = RGBA;
        break;
    default:
        log.error() << "Unsupported PNG color type " << png_color_type << std::endl;
        fclose(fp);
        return false;
    }

    png_read_update_info(png, png_info);

    /* read file */
    if (setjmp(png_jmpbuf(png)))
    {
        log.error() << "Error during read_image" << std::endl;
        fclose(fp);
        return false;
    }

    switch(m_type)
    {
    case MONO:
        m_buffer.resize(m_width * m_height);
        break;
    case MONO_ALPHA:
        m_buffer.resize(m_width * m_height * 2);
        break;
    case RGB:
        m_buffer.resize(m_width * m_height * 3);
        break;
    case RGBA:
        m_buffer.resize(m_width * m_height * 4);
        break;
    }

    size_t stride = png_get_rowbytes(png, png_info);

    std::vector<uint8_t*> rowPointers;
    rowPointers.resize(m_height);
    for (size_t y = 0; y < m_height; ++y)
    {
        rowPointers[y] = &m_buffer[y*stride];
    }

    png_read_image(png, &rowPointers.front());


    fclose(fp);
    return true;
}

bool TunisPNG::store(const std::string &file_name)
{
    Poco::LogStream log(Poco::Logger::get("util"));

    if (m_buffer.size() == 0)
    {
        log.error() << "Buffer is empty. Nothing to store!" << std::endl;
        return false;
    }

    /* create file */
    FILE *f = fopen(file_name.c_str(), "wb");
    if (!f)
    {
        log.error() << "File " << file_name << " could not be opened for writing" << std::endl;
        return false;
    }

    /* initialize stuff */
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png)
    {
        log.error() << "png_create_write_struct failed" << std::endl;
        fclose(f);
        return false;
    }

    png_infop png_info = png_create_info_struct(png);
    if (!png_info)
    {
        log.error() << "png_create_info_struct failed" << std::endl;
        fclose(f);
        return false;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        log.error() << "Error during init_io" << std::endl;
        fclose(f);
        return false;
    }

    png_init_io(png, f);

    /* write header */
    if (setjmp(png_jmpbuf(png)))
    {
        log.error() << "Error during writing header" << std::endl;
        fclose(f);
        return false;
    }

    uint8_t png_color_type;
    switch(m_type)
    {
    case MONO:       png_color_type = PNG_COLOR_TYPE_GRAY; break;
    case MONO_ALPHA: png_color_type = PNG_COLOR_TYPE_GA;   break;
    case RGB:        png_color_type = PNG_COLOR_TYPE_RGB;  break;
    case RGBA:       png_color_type = PNG_COLOR_TYPE_RGBA; break;
    }

    png_set_IHDR(png,
                 png_info,
                 m_width,
                 m_height,
                 8,
                 png_color_type,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);
    png_write_info(png, png_info);

    /* write bytes */
    if (setjmp(png_jmpbuf(png)))\
    {
        log.error() << "Error during writing bytes" << std::endl;
        fclose(f);
        return false;
    }

    size_t stride = png_get_rowbytes(png, png_info);

    std::vector<uint8_t*> rowPointers;
    rowPointers.resize(m_height);
    for (size_t y = 0; y < m_height; ++y)
    {
        rowPointers[y] = &m_buffer[y*stride];
    }

    png_write_image(png, &rowPointers.front());

    /* end write */
    if (setjmp(png_jmpbuf(png)))
    {
        log.error() << "Error during end of write" << std::endl;;
        fclose(f);
        return false;
    }

    png_write_end(png, NULL);

    fclose(f);
    return true;
}
