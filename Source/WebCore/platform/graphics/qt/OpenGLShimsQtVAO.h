/*
 * Copyright (C) 2017 Konstantin Tokarev <annulen@yandex.ru>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <private/qopenglvertexarrayobject_p.h>

#ifndef VAO_FUNCTIONS
#error You must define VAO_FUNCTIONS macro before including this header
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
#define LOOKUP_VAO_FUNCTION(f, ...) VAO_FUNCTIONS->f(__VA_ARGS__)
#else
#define LOOKUP_VAO_FUNCTION(f, ...)
#endif

#define glGenVertexArrays(...)                    LOOKUP_VAO_FUNCTION(glGenVertexArrays, __VA_ARGS__)
#define glDeleteVertexArrays(...)                 LOOKUP_VAO_FUNCTION(glDeleteVertexArrays, __VA_ARGS__)
#define glIsVertexArray(...)                      LOOKUP_VAO_FUNCTION(glIsVertexArray, __VA_ARGS__)
#define glBindVertexArray(...)                    LOOKUP_VAO_FUNCTION(glBindVertexArray, __VA_ARGS__)
