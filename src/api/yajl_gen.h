/*
 * Copyright (c) 2007-2014, Lloyd Hilaiel <me@lloyd.io>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * \file yajl_gen.h
 * Interface to YAJL's JSON generation facilities.
 */

#include <yajl/yajl_common.h>

#ifndef __YAJL_GEN_H__
#define __YAJL_GEN_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
    /** Generator status codes. */
    typedef enum {
        /** No error. */
        yajl_gen_status_ok = 0,
        /** At a point where a map key is generated, a function other than
         *  yajl_gen_string() was called. */
        yajl_gen_keys_must_be_strings,
        /** YAJL's maximum generation depth was exceeded.  see
         *  \ref YAJL_MAX_DEPTH. */
        yajl_max_depth_exceeded,
        /** A generator function (yajl_gen_XXX()) was called while in an error
         *  state. */
        yajl_gen_in_error_state,
        /** A complete JSON document has been generated. */
        yajl_gen_generation_complete,
        /** yajl_gen_double() was passed an invalid floating point value
         *  (infinity or NaN). */
        yajl_gen_invalid_number,
        /** A print callback was passed in, so there is no internal
         * buffer to get from. */
        yajl_gen_no_buf,
        /** Returned from yajl_gen_string() when the \ref yajl_gen_validate_utf8
         *  option is enabled and invalid UTF8 was passed by client code.
         */
        yajl_gen_invalid_string
    } yajl_gen_status;

    /** An opaque handle to a generator */
    typedef struct yajl_gen_t * yajl_gen;

    /** A callback used for "printing" the results. */
    typedef void (*yajl_print_t)(void * ctx,
                                 const char * str,
                                 size_t len);

    /** Configuration parameters for the parser, these may be passed to
     *  yajl_gen_config() followed by option specific argument(s). In general,
     *  all boolean configuration parameters default to *off*. */
    typedef enum {
        /**
         * Generate indented (beautiful) output.
         *
         * yajl_gen_config() argument type: int (boolean)
         *
         * Example: \code{.cpp}
         * yajl_gen_config(g, yajl_gen_beautify, 1); // Human format please
         * \endcode
         */
        yajl_gen_beautify = 0x01,
        /**
         * Set the indent string which is used when \ref yajl_gen_beautify
         * is enabled, which may only contain whitespace characters such as
         * \c \\t or some number of spaces. The default is four spaces ' '.
         *
         * yajl_gen_config() argument type: const char *
         *
         * Example: \code{.cpp}
         * yajl_gen_config(g, yajl_gen_indent_string, "  "); // 2 spaces
         * \endcode
         */
        yajl_gen_indent_string = 0x02,
        /**
         * Set a function and context argument that should be used to
         * output the generated json. The function should conform to the
         * \ref yajl_print_t prototype while the context argument may be any
         * void * of your choosing.
         *
         * yajl_gen_config() arguments: \ref yajl_print_t, void *
         *
         * Example: \code{.cpp}
         * yajl_gen_config(g, yajl_gen_print_callback, myFunc, myVoidPtr);
         * \endcode
         */
        yajl_gen_print_callback = 0x04,
        /**
         * Normally the generator does not validate that strings you
         * pass to it via yajl_gen_string() are valid UTF8.  Enabling
         * this option will cause it to do so.
         *
         * yajl_gen_config() argument type: int (boolean)
         *
         * Example: \code{.cpp}
         * yajl_gen_config(g, yajl_gen_validate_utf8, 1); // Check UTF8
         * \endcode
         */
        yajl_gen_validate_utf8 = 0x08,
        /**
         * The forward solidus (slash or '/' in human) is not required to be
         * escaped in json text.  By default, YAJL will not escape it in the
         * iterest of saving bytes.  Setting this flag will cause YAJL to
         * always escape '/' in generated JSON strings.
         *
         * yajl_gen_config() argument type: int (boolean)
         */
        yajl_gen_escape_solidus = 0x10,
        /**
         * Special numbers such as NaN and Infinity cannot be represented in
         * the original JSON, but are permitted in JSON5. Setting this flag
         * allows YAJL to output the JSON5 representation of these special
         * numbers instead of returning with an error, and to emit map keys
         * that are valid javascript identifiers without quotes.
         *
         * yajl_gen_config() argument type: int (boolean)
         *
         * Example: \code{.cpp}
         * yajl_gen_config(g, yajl_gen_json5, 1); // Output JSON5
         * \endcode
         */
        yajl_gen_json5 = 0x20,
    } yajl_gen_option;

    /** Set generator options associated with a generator handle. See the
     *  \ref yajl_gen_option documentation for details of the available
     *  options and their arguments.
     *  \returns Zero in case of error, non-zero otherwise.
     */
    YAJL_API int yajl_gen_config(yajl_gen hand, yajl_gen_option opt, ...);

    /** Allocate a generator handle
     *  \param allocFuncs An optional pointer to a structure which allows the
     *                    client to provide memory allocation functions for
     *                    use by yajl. May be \c NULL to use the C runtime
     *                    library's malloc(), free() and realloc().
     *
     *  \returns An allocated handle on success, \c NULL on failure (bad params)
     */
    YAJL_API yajl_gen yajl_gen_alloc(const yajl_alloc_funcs * allocFuncs);

    /** Free a generator handle. */
    YAJL_API void yajl_gen_free(yajl_gen hand);

    YAJL_API yajl_gen_status yajl_gen_integer(yajl_gen hand, long long int number);
    /** Generate a floating point number.
     *  \param hand     The generator handle.
     *  \param number   The value to output. The values Infinity or NaN are
     *                  only accepted if the \ref yajl_gen_json5 option is set,
     *                  as these values have no legal representation in JSON;
     *                  the generator will return \ref yajl_gen_invalid_number
     *                  otherwise.
     */
    YAJL_API yajl_gen_status yajl_gen_double(yajl_gen hand, double number);
    YAJL_API yajl_gen_status yajl_gen_number(yajl_gen hand,
                                             const char * num,
                                             size_t len);
    YAJL_API yajl_gen_status yajl_gen_string(yajl_gen hand,
                                             const unsigned char * str,
                                             size_t len);
    YAJL_API yajl_gen_status yajl_gen_null(yajl_gen hand);
    YAJL_API yajl_gen_status yajl_gen_bool(yajl_gen hand, int boolean);
    YAJL_API yajl_gen_status yajl_gen_map_open(yajl_gen hand);
    YAJL_API yajl_gen_status yajl_gen_map_close(yajl_gen hand);
    YAJL_API yajl_gen_status yajl_gen_array_open(yajl_gen hand);
    YAJL_API yajl_gen_status yajl_gen_array_close(yajl_gen hand);

    /** Access the zero-terminated generator buffer. If incrementally
     *  outputing JSON, one should call yajl_gen_clear to clear the
     *  buffer.  This allows stream generation. */
    YAJL_API yajl_gen_status yajl_gen_get_buf(yajl_gen hand,
                                              const unsigned char ** buf,
                                              size_t * len);

    /** Clear yajl's output buffer, but maintain all internal generation
     *  state. This function will not reset the generator state, and is
     *  intended to enable incremental JSON outputing. */
    YAJL_API void yajl_gen_clear(yajl_gen hand);

    /** Reset the generator state. Allows a client to generate multiple
     *  JSON entities in a stream.
     *  \param hand The generator handle.
     *  \param sep This string will be inserted to separate the previously
     *             generated output from the following; passing \c NULL means
     *             *no separation* of entites (beware that generating
     *             multiple JSON numbers without a separator creates
     *             ambiguous output).
     *
     *  Note: This call does not clear yajl's output buffer, which must be
     *  accomplished explicitly by calling yajl_gen_clear(). */
    YAJL_API void yajl_gen_reset(yajl_gen hand, const char * sep);

#ifdef __cplusplus
}
#endif

#endif
