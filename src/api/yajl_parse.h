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
 * \file yajl_parse.h
 * Interface to YAJL's JSON stream parsing facilities.
 */

#include <yajl/yajl_common.h>

#ifndef __YAJL_PARSE_H__
#define __YAJL_PARSE_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
    /** Error codes returned from this interface. */
    typedef enum {
        /** No error was encountered. */
        yajl_status_ok,
        /** A client callback returned zero, stopping the parse. */
        yajl_status_client_canceled,
        /** An error occured during the parse.  Call yajl_get_error() for
         *  more information about the encountered error. */
        yajl_status_error
    } yajl_status;

    /** Return a human readable, english string for an error code. */
    YAJL_API const char * yajl_status_to_string(yajl_status code);

    /** An opaque handle to a parser. */
    typedef struct yajl_handle_t * yajl_handle;

    /** yajl is an event driven parser.  This means as json elements are
     *  parsed, you are called back to do something with the data.  The
     *  functions in this table indicate the various events for which
     *  you will be called back.  Each callback accepts a "context"
     *  pointer, this is a \c void \c * that is passed into the yajl_parse()
     *  function which the client code may use to pass around context.
     *
     *  All callbacks return an integer.  If non-zero, the parse will
     *  continue.  If zero, the parse will be canceled and
     *  \c yajl_status_client_canceled will be returned from the parse.
     *
     *  \attention
     *    A note about the handling of numbers:
     *
     *  \attention
     *    yajl will only convert numbers that can be represented in a
     *    double or a 64 bit (long long) int. All other numbers will be
     *    passed to the client in string form using the yajl_number()
     *    callback. Furthermore, if yajl_number() is not NULL, it will
     *    always be used to return numbers, that is yajl_integer() and
     *    yajl_double() will be ignored. If yajl_number() is NULL but one
     *    of yajl_integer() or yajl_double() are defined, parsing of a
     *    number larger than is representable in a double or 64 bit
     *    integer will result in a parse error.
     */
    typedef struct {
        int (* yajl_null)(void * ctx);
        int (* yajl_boolean)(void * ctx, int boolVal);
        int (* yajl_integer)(void * ctx, long long integerVal);
        int (* yajl_double)(void * ctx, double doubleVal);
        /** A callback which passes the string representation of the number
         *  back to the client.  Will be used for all numbers when present. */
        int (* yajl_number)(void * ctx, const char * numberVal,
                            size_t numberLen);

        /** Strings are returned as pointers into the JSON text when
         * possible. As a result they are _not_ zero-terminated. */
        int (* yajl_string)(void * ctx, const unsigned char * stringVal,
                            size_t stringLen);

        int (* yajl_start_map)(void * ctx);
        int (* yajl_map_key)(void * ctx, const unsigned char * key,
                             size_t stringLen);
        int (* yajl_end_map)(void * ctx);

        int (* yajl_start_array)(void * ctx);
        int (* yajl_end_array)(void * ctx);
    } yajl_callbacks;

    /** Allocate a parser handle.
     *  \param callbacks  A \c yajl_callbacks structure specifying the
     *                    functions to call when different JSON entities
     *                    are encountered in the input text. May be \c NULL,
     *                    which is only useful for validation.
     *  \param afs        Memory allocation functions, may be \c NULL to use the
     *                    C runtime library routines (malloc() and friends).
     *  \param ctx        A context pointer that will be passed to callbacks.
     */
    YAJL_API yajl_handle yajl_alloc(const yajl_callbacks * callbacks,
                                    yajl_alloc_funcs * afs,
                                    void * ctx);


    /** Configuration parameters for the parser, these should be passed to
     *  yajl_config() followed by any option specific argument(s). In general,
     *  all boolean configuration parameters default to *off*. */
    typedef enum {
        /**
         * Ignore javascript style comments present in JSON input. These are
         * not standard in JSON, although they are allowed in JSON5 input.
         *
         * yajl_config() argument type: int (boolean)
         *
         * Example: \code{.cpp}
         * yajl_config(h, yajl_allow_comments, 1); // turn comment support on
         * \endcode
         */
        yajl_allow_comments = 0x01,
        /**
         * When set the parser will verify that all strings in JSON input are
         * valid UTF8 and will emit a parse error if this is not so. When set,
         * this option makes parsing slightly more expensive (~7% depending
         * on the processor and compiler in use).
         *
         * yajl_config() argument type: int (boolean)
         *
         * Example: \code{.cpp}
         * yajl_config(h, yajl_dont_validate_strings, 1); // disable utf8 checking
         * \endcode
         */
        yajl_dont_validate_strings     = 0x02,
        /**
         * By default, upon calls to yajl_complete_parse(), yajl will ensure
         * the entire input text was consumed and will raise an error
         * otherwise. Turning this flag on cause yajl to disable the garbage
         * check. This can be useful when parsing JSON out of an input stream
         * that contains more than a single JSON document.
         *
         * yajl_config() argument type: int (boolean)
         *
         * Example: \code{.cpp}
         * yajl_config(h, yajl_allow_trailing_garbage, 1); // non-JSON follows
         * \endcode
         */
        yajl_allow_trailing_garbage = 0x04,
        /**
         * Allow multiple values to be parsed by a single handle. The entire
         * text must be valid JSON, and values can be seperated by any kind of
         * whitespace. This flag will change the behavior of the parser, and
         * cause it to continue parsing after a value is parsed, rather than
         * transitioning into a complete state. This option can be useful when
         * parsing multiple values from an input stream.
         *
         * yajl_config() argument type: int (boolean)
         *
         * Example: \code{.cpp}
         * yajl_config(h, yajl_allow_multiple_values, 1); // multi-doc stream
         * \endcode
         */
        yajl_allow_multiple_values = 0x08,
        /**
         * When yajl_complete_parse() is called the parser will check that the
         * top level value was completely consumed.  If called whilst in the
         * middle of parsing a value, yajl will enter an error state (premature
         * EOF). Setting this flag suppresses that check and the corresponding
         * error.
         *
         * yajl_config() argument type: int (boolean)
         *
         * Example: \code{.cpp}
         * yajl_config(h, yajl_allow_partial_values, 1); // might stop early
         * \endcode
         */
        yajl_allow_partial_values = 0x10,
        /**
         * The JSON5 standard allows additional formats for numbers, strings
         * and object keys which are not permitted by the JSON standard.
         * Setting this flag tells yajl to accept JSON5 standard input.
         * This flag also enables \c yajl_allow_comments since comments are
         * part of the JSON5 standard.
         *
         * yajl_config() argument type: int (boolean)
         *
         * Example: \code{.cpp}
         * yajl_config(h, yajl_allow_json5, 1); // We accept JSON5!
         * \endcode
         */
        yajl_allow_json5 = 0x20,
    } yajl_option;

    /** Set parser options associated with a parser handle. See the
     *  \ref yajl_option documentation for details of the available options
     *  and their arguments.
     *  \returns Zero in case of error, non-zero otherwise.
     */
    YAJL_API int yajl_config(yajl_handle hand, yajl_option opt, ...);

    /** Free a parser handle. */
    YAJL_API void yajl_free(yajl_handle hand);

    /** Parse some json!
     *  \param hand A handle to the json parser allocated with yajl_alloc().
     *  \param jsonText A pointer to the UTF8 json text to be parsed.
     *  \param jsonTextLength The length, in bytes, of input text.
     */
    YAJL_API yajl_status yajl_parse(yajl_handle hand,
                                    const unsigned char * jsonText,
                                    size_t jsonTextLength);

    /** Parse any remaining buffered json.
     *
     *  Since yajl is a stream-based parser, without an explicit end of
     *  input, yajl sometimes can't decide if content at the end of the
     *  stream is valid or not. For example, if "1" has been fed in,
     *  yajl can't know whether another digit is next or some character
     *  that would terminate the integer token.
     *
     *  \param hand a handle to the json parser allocated with yajl_alloc().
     */
    YAJL_API yajl_status yajl_complete_parse(yajl_handle hand);

    /** Get an error string describing the state of the parse.
     *
     *  If verbose is non-zero, the message will include the JSON text where
     *  the error occured, along with an arrow pointing to the specific char.
     *
     *  \returns A dynamically allocated string will be returned which should
     *  be freed with yajl_free_error().
     */
    YAJL_API unsigned char * yajl_get_error(yajl_handle hand, int verbose,
                                            const unsigned char * jsonText,
                                            size_t jsonTextLength);

    /** Get the amount of data consumed from the last chunk passed to yajl.
     *
     * In the case of a successful parse this can help you understand if
     * the entire buffer was consumed (which will allow you to handle
     * "junk at end of input").
     *
     * In the event an error is encountered during parsing, this function
     * affords the client a way to get the offset into the most recent
     * chunk where the error occured.  0 will be returned if no error
     * was encountered.
     */
    YAJL_API size_t yajl_get_bytes_consumed(yajl_handle hand);

    /** Free an error returned from yajl_get_error(). */
    YAJL_API void yajl_free_error(yajl_handle hand, unsigned char * str);

#ifdef __cplusplus
}
#endif

#endif
