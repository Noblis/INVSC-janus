/*******************************************************************************
 * Copyright (c) 2013 Noblis, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 ******************************************************************************/

#ifndef JANUS_H
#define JANUS_H

#if defined JANUS_LIBRARY
#  if defined _WIN32 || defined __CYGWIN__
#    define JANUS_EXPORT __declspec(dllexport)
#  else
#    define JANUS_EXPORT __attribute__((visibility("default")))
#  endif
#else
#  if defined _WIN32 || defined __CYGWIN__
#    define JANUS_EXPORT __declspec(dllimport)
#  else
#    define JANUS_EXPORT
#  endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

JANUS_EXPORT void janus_initialize();

#ifdef __cplusplus
}
#endif

#endif // JANUS_H
