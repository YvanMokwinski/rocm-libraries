/*! \file */
/* ************************************************************************
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights Reserved.
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ************************************************************************ */
#ifndef HIPDSS_EXPORT_H
#define HIPDSS_EXPORT_H

#ifdef HIPDSS_STATIC_DEFINE
#  define HIPDSS_EXPORT
#else
#  if defined(_WIN32) || defined(__CYGWIN__)
#    if defined(hipdss_EXPORTS) || defined(rocdss_EXPORTS)
#      define HIPDSS_EXPORT __declspec(dllexport)
#    else
#      define HIPDSS_EXPORT __declspec(dllimport)
#    endif
#  else
#    define HIPDSS_EXPORT __attribute__((visibility("default")))
#  endif
#endif

#ifndef HIPDSS_DEPRECATED
#  define HIPDSS_DEPRECATED __attribute__((__deprecated__))
#endif

#ifndef HIPDSS_DEPRECATED_MSG
#  define HIPDSS_DEPRECATED_MSG(msg) __attribute__((__deprecated__(msg)))
#endif

#if !defined(HIPDSSAPI)
#  if defined(_WIN32)
#    define HIPDSSAPI __stdcall
#  else
#    define HIPDSSAPI
#  endif
#endif

#endif /* HIPDSS_EXPORT_H */
