/*-
 * Copyright (C) 2014 Pietro Cerutti <gahr@gahr.ch>
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
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef YAJLPP_H
#define YAJLPP_H

#include <cmath>
#include <cstring>
#include <exception>
#include <string>

#include <yajl/yajl_gen.h>
#include <yajl/yajl_parse.h>

//! YAJL C++ APIs.
namespace yajlpp {

    /**
     * \name Forward declarations.
     *
     * These are friend functions used internally by the yajlpp classes.
     */
    ///@{
    void generator_print_cb(void * ctx, const char * str, size_t len);
    int cb_null_dispatch(void *);
    int cb_bool_dispatch(void *, int);
    int cb_integer_dispatch(void *, long long);
    int cb_double_dispatch(void *, double);
    int cb_number_dispatch(void *, const char *, size_t);
    int cb_string_dispatch(void *, const unsigned char *, size_t);
    int cb_map_start_dispatch(void *);
    int cb_map_key_dispatch(void *, const unsigned char *, size_t);
    int cb_map_end_dispatch(void *);
    int cb_array_start_dispatch(void *);
    int cb_array_end_dispatch(void *);
    ///@}


    /**
     * \brief yajl_gen C++ APIs.
     *
     * \class yajlpp::generator yajlpp.h
     *
     * Generator synopsis:
     *
     *     // Create a generator instance
     *     yajlpp::generator gen;
     *
     *     // Set various configuration options
     *     gen.beautify(true);
     *     gen.indentString("\t");
     *     gen.validateUtf8(false);
     *     gen.escapeSolidus(true);
     *
     *     // Put various objects to the generator
     *     gen.map_open();
     *         gen << "title" << "London Calling"
     *             << "year" << 1979
     *             << "tracks";
     *             gen.array_open();
     *                 gen.map_open();
     *                     gen << "title" << "London Calling"
     *                         << "length" << 3.18;
     *                 gen.map_close();
     *                 gen.map_open();
     *                     gen << "title" << "Brand New Cadillac"
     *                         << "length" << 2.08;
     *                 gen.map.close();
     *             gen.array_close();
     *     gen.map_close();
     *
     *     // Get the resulting JSON
     *     std::string json { gen.result() };
     *
     *     // Check the current state
     *     if (!gen.good()) {
     *         switch (gen.status()) {
     *             case yajl_gen_keys_must_be_strings:
     *                 ...
     *                 break;
     *             case yajl_max_depth_exceeded:
     *                 ...
     *                 break;
     *             ...
     *         }
     *     }
     *
     *     // Reset / clear the generator
     *     gen.clear();
     *     gen.reset();
     */
    class generator {

        public:

            /*!
             * Construct a new yajlpp::generator instance. This constructor
             * might fail and throw std::bad_alloc if the allocation of the
             * underlying yajl_gen object fails.
             */
            generator()
                : m_status { yajl_gen_status_ok },
                  m_gen { yajl_gen_alloc(nullptr) }
            {
                if (m_gen == nullptr) throw std::bad_alloc();
                yajl_gen_config(m_gen, yajl_gen_print_callback, generator_print_cb, this);
            }

            /*!
             * Move constructor.
             */
            generator(generator&& other)
                : m_json { other.m_json },
                  m_status { other.m_status },
                  m_gen { other.m_gen }
            {
                other.m_json.clear();
                other.m_status = yajl_gen_status_ok;
                other.m_gen = nullptr;
            }

            /*!
             * Move assignment.
             */
            generator& operator=(generator&& other) {
                yajl_gen_free(m_gen);

                m_json = other.m_json;
                m_status = other.m_status;
                m_gen = other.m_gen;

                other.m_json.clear();
                other.m_status = yajl_gen_status_ok;
                other.m_gen = nullptr;

                return *this;
            }

            /*!
             * This object is not copy constructible.
             */
            generator(const generator& other) = delete;

            /*!
             * This object is not copy assignable.
             */
            generator& operator=(const generator& other) = delete;

            /*!
             * Destroy a yajlpp::generator instance. The underlying yajl_gen
             * object is free'd.
             */
            ~generator() { if (m_gen != nullptr) yajl_gen_free(m_gen); }


            /*!
             * \name State management.
             */
            ///@{

            /*!
             * Check whether the state of the underlying \ref yajl_gen object
             * is ok.
             */
            inline
            bool good() const {
                return m_status == yajl_gen_status_ok;
            }

            /*!
             * Return the underlying \ref yajl_gen_status code.
             */
            inline
            yajl_gen_status stauts() const {
                return m_status;
            }

            /*!
             * Return the generated JSON string.
             */
            inline
            const std::string& result() const {
                return m_json;
            }

            /*!
             * Clear the generated JSON string.
             */
            inline
            void clear() {
                m_json.clear();
            }

            /*!
             * Clear the underlying generator state. The output buffer is *not*
             * cleared. To do so, use \ref clear. If sep is supplied, it
             * is inserted in the output buffer, as specified in \ref
             * yajl_gen_reset.
             */
            inline
            void reset(std::string sep = std::string()) {
                yajl_gen_reset(m_gen, sep.empty() ? nullptr : sep.c_str());
            }

            ///@}


            /*!
             * \name Configuration options.
             */
            ///@{

            /*!
             * Turn on / off the generation of beautified (indented) output.
             * See \ref yajl_gen_beautify.
             */
            inline
            bool beautify(bool beauty) {
                return !yajl_gen_config(m_gen, yajl_gen_beautify, (beauty ? 1 : 0));
            }

            /*!
             * Specify the string to be used for indenting. By default, four
             * spaces are used. See \ref yajl_gen_indent_string.
             */
            inline
            bool indent_string(const std::string& indent) {
                return !yajl_gen_config(m_gen, yajl_gen_indent_string, indent.c_str());
            }

            /*!
             * Turn on / off validation of UTF-8 strings. See \ref
             * yajl_gen_validate_utf8.
             */
            inline
            bool validate_utf8(bool validate) {
                return !yajl_gen_config(m_gen, yajl_gen_validate_utf8, (validate ? 1 : 0));
            }

            /*!
             * Turn on / off escaping of slash characters. See \ref
             * yajl_gen_escape_solidus.
             */
            inline
            bool escape_solidus(bool escape) {
                return !yajl_gen_config(m_gen, yajl_gen_escape_solidus, (escape ? 1 : 0));
            }

            ///@}


            /*!
             * \name Special JSON values.
             */
            ///@{

            /*!
             * Generate a 'null' token. See \ref yajl_gen_null.
             */
            inline
            void null() {
                m_status = yajl_gen_null(m_gen);
            }

            /*!
             * Generate a '{' token. See \ref yajl_gen_map_open.
             */
            inline
            void map_open() {
                m_status = yajl_gen_map_open(m_gen);
            }

            /*!
             * Generate a '}' token. See \ref yajl_gen_map_close.
             */
            inline
            void map_close() {
                m_status = yajl_gen_map_close(m_gen);
            }

            /*!
             * Generate a '[' token. See \ref yajl_gen_array_open.
             */
            inline
            void array_open() {
                m_status = yajl_gen_array_open(m_gen);
            }

            /*!
             * Generate a ']' token. See \ref yajl_gen_array_close.
             */
            inline
            void array_close() {
                m_status = yajl_gen_array_close(m_gen);
            }

            ///@}


            /*!
             * \name Insertion of values into the JSON stream.
             */
            ///@{

            /*!
             * Insert an unsigned short in the output stream. See \ref
             * yajl_gen_integer.
             */
            inline
            generator& operator<<(unsigned short i) {
                m_status = yajl_gen_integer(m_gen, i);
                return *this;
            }

            /*!
             * Insert a short in the output stream. See \ref yajl_gen_integer.
             */
            inline
            generator& operator<<(short i) {
                m_status = yajl_gen_integer(m_gen, i);
                return *this;
            }

            /*!
             * Insert an unsigned int in the output stream. See \ref
             * yajl_gen_integer.
             */
            inline
            generator& operator<<(unsigned int i) {
                m_status = yajl_gen_integer(m_gen, i);
                return *this;
            }

            /*!
             * Insert an int in the output stream. See \ref yajl_gen_integer.
             */
            inline
            generator& operator<<(int i) {
                m_status = yajl_gen_integer(m_gen, i);
                return *this;
            }

            /*!
             * Insert an unsigned long the output stream. See \ref
             * yajl_gen_integer.
             */
            inline
            generator& operator<<(unsigned long i) {
                m_status = yajl_gen_integer(m_gen, i);
                return *this;
            }

            /*!
             * Insert a long the output stream. See \ref yajl_gen_integer.
             */
            inline
            generator& operator<<(long i) {
                m_status = yajl_gen_integer(m_gen, i);
                return *this;
            }

            /*!
             * Insert an unsigned long long the output stream. See \ref
             * yajl_gen_integer.
             */
            inline
            generator& operator<<(unsigned long long i) {
                m_status = yajl_gen_integer(m_gen, i);
                return *this;
            }

            /*!
             * Insert a long long the output stream. See \ref yajl_gen_integer.
             */
            inline
            generator& operator<<(long long i) {
                m_status = yajl_gen_integer(m_gen, i);
                return *this;
            }

            /*!
             * Insert a boolean into the output string. See \ref yajl_gen_bool.
             */
            inline
            generator& operator<<(bool b) {
                m_status = yajl_gen_bool(m_gen, b);
                return *this;
            }

            /*!
             * Insert a float into the output string. See \ref yajl_gen_double.
             */
            inline
            generator& operator<<(float f) {
                m_status = yajl_gen_double(m_gen, f);
                return *this;
            }

            /*!
             * Insert a double into the output string. See \ref yajl_gen_double.
             */
            inline
            generator& operator<<(double d) {
                m_status = yajl_gen_double(m_gen, d);
                return *this;
            }

            /*!
             * Insert a char pointer into the output string. See \ref
             * yajl_gen_string.
             */
            inline
            generator& operator<<(char * s) {
                m_status = yajl_gen_string(m_gen, reinterpret_cast<const unsigned char *>(s), strlen(s));
                return *this;
            }

            /*!
             * Insert a const char pointer into the output string. See \ref
             * yajl_gen_string.
             */
            inline
            generator& operator<<(const char * s) {
                m_status = yajl_gen_string(m_gen, reinterpret_cast<const unsigned char *>(s), strlen(s));
                return *this;
            }

            /*!
             * Insert a std::string into the output string. See \ref
             * yajl_gen_string.
             */
            inline
            generator& operator<<(const std::string& s) {
                m_status = yajl_gen_string(m_gen, reinterpret_cast<const unsigned char *>(s.c_str()), s.size());
                return *this;
            }

            ///@}


        private:
            /*!
             * Buffer with the generated JSON.
             */
            std::string m_json;

            /*!
             * Internal status. See \ref yajl_gen_status.
             */
            yajl_gen_status m_status;

            /*!
             * Internal generator. See \ref yajl_gen.
             */
            yajl_gen m_gen;


        friend
        /*!
         * Used internally as print callback to feed the m_json buffer.
         */
        inline
        void generator_print_cb(void * ctx, const char * str, size_t len) {
            generator * obj = static_cast<generator *>(ctx);
            obj->m_json.append(str, len);
        }
    };

    /**
     * \brief yajl_parse C++ APIs.
     *
     * \class yajlpp::parser yajlpp.h
     *
     * Parser synopsis:
     *
     *     // Define a class deriving publicly from yajlpp::parser and
     *     // implement the pure virtual callbacks declared in the base
     *     // class.
     *     struct myparser : public yajlpp::parser {
     *         myparser() {}
     *
     *         int cb_null() { ... }
     *         int cb_boolean() { ... }
     *         int cb_integer(long long i) { ... }
     *         .....
     *     };
     *
     *     // Create an instance of your parser class and invoke methods on it.
     *     myparser p;
     *     const char * input = "{ \"key\" : 12 }";
     *     yajl_status s = p.parse(input);
     *     if (s != yajl_status_ok) {
     *         cout << "Error after " << p.bytes_consumed() << " bytes: ";
     *         cout << p.get_error(input);
     *         ...
     *     } 
     */
    class parser {

        public:

            /*!
             * Construct a new yajlpp::parser. This constructor might fail and
             * throw std::bad_alloc if the allocation of the underlying
             * yajl_handle object fails.
             */
            parser()
                : m_handle { yajl_alloc(&m_callbacks, NULL, this) }
            {
                init_callbacks();
            }

            /*!
             * Move constructor.
             */
            parser(parser&& other)
                : m_handle { other.m_handle }
            {
                init_callbacks();
                yajl_free(other.m_handle);
                other.m_handle = nullptr;
            }

            /*!
             * Move assignment.
             */
            parser& operator=(parser&& other) {
                yajl_free(m_handle);

                m_handle = other.m_handle;
                init_callbacks();

                yajl_free(other.m_handle);
                other.m_handle = nullptr;

                return *this;
            }

            /*!
             * This object is not copy constructible.
             */
            parser(const parser& other) = delete;

            /*!
             * This object is not copy assignable.
             */
            parser& operator=(const parser& other) = delete;

            /*!
             * Destroy a yajlpp::parser instance. The underlaying yajl_handle
             * object is free'd.
             */
            virtual ~parser() {
                yajl_free(m_handle);
            }

            /*!
             * \name Configuration options.
             */
            ///@{

            /*!
             * Ignore JavaScript style comments. See \ref yajl_allow_comments.
             */
            inline
            bool allow_comments(bool allow) {
                return !yajl_config(m_handle, yajl_allow_comments, allow);
            }

            /*!
             * Make sure all strings are valid UTF8. This works the opposite
             * from plain \ref yajl_dont_validate_strings. Passing true to this
             * function turns validation on.
             */
            inline
            bool validate_utf8(bool validate) {
                return !yajl_config(m_handle, yajl_dont_validate_strings, !validate);
            }

            /*!
             * Allow trailing garbage. See \ref yajl_allow_trailing_garbage.
             */
            inline
            bool allow_trailing_garbage(bool allow) {
                return !yajl_config(m_handle, yajl_allow_trailing_garbage, allow);
            }

            /*!
             * Allow multiple values. See \ref yajl_allow_multiple_values.
             */
            inline
            bool allow_multiple_values(bool allow) {
                return !yajl_config(m_handle, yajl_allow_multiple_values, allow);
            }

            /*!
             * Allow partial values. See \ref yajl_allow_partial_values.
             */
            inline
            bool allow_partial_values(bool allow) {
                return !yajl_config(m_handle, yajl_allow_partial_values, allow);
            }


            ///@}

            /*!
             * Parse a string containing JSON text. See \ref yajl_parse.
             */
            inline
            yajl_status parse(std::string jsonText) {
                return yajl_parse(m_handle, reinterpret_cast<const unsigned char *>(jsonText.c_str()), jsonText.size());
            }

            /*!
             * Parse any remaining buffered JSON. See \ref yajl_complete_parse.
             */
            inline
            yajl_status complete_parse() {
                return yajl_complete_parse(m_handle);
            }

            /*!
             * Get the number of bytes consumed from the last chunk of input.
             * See \ref yajl_get_bytes_consumed.
             */
            inline
            size_t bytes_consumed() {
                return yajl_get_bytes_consumed(m_handle);
            }

            /*!
             * Return a string describing the state of the parser. See \ref
             * yajl_get_error.
             */
            inline
            std::string get_error(std::string jsonText, bool verbose = false) {
                unsigned char * buffer { yajl_get_error(m_handle, verbose, reinterpret_cast<const unsigned char *>(jsonText.c_str()), jsonText.size()) };
                std::string s { reinterpret_cast<const char *>(buffer) };
                yajl_free_error(m_handle, buffer);
                return s;
            }

            /*!
             * \name Callbacks
             *
             * These pure virtual callbacks must be implemented in derived
             * classed. If non-zero is returned, the parsing continues.
             * If zero is returned, the parsing is canceled and
             * yajl_status_client_canceled will be returned from the parse.
             */

            ///@{
                virtual int cb_null() =0;
                virtual int cb_boolean(bool) =0;
                virtual int cb_integer(long long) =0;
                virtual int cb_double(double) =0;
                virtual int cb_number(std::string) =0;
                virtual int cb_string(std::string) =0;
                virtual int cb_map_start() =0;
                virtual int cb_map_key(std::string) =0;
                virtual int cb_map_end() =0;
                virtual int cb_array_start() =0;
                virtual int cb_array_end() =0;
            ///@}

        private:

            /*!
             * Initialize the callbacks structure.
             */
            void init_callbacks() {
                m_callbacks = {
                    cb_null_dispatch,
                    cb_bool_dispatch,
                    cb_integer_dispatch,
                    cb_double_dispatch,
                    cb_number_dispatch,
                    cb_string_dispatch,
                    cb_map_start_dispatch, cb_map_key_dispatch,
                    cb_map_end_dispatch, cb_array_start_dispatch,
                    cb_array_end_dispatch
                };
            }

            /*!
             * \name Internal callbacks.
             */

            ///@{

                friend inline
                int cb_null_dispatch(void * ctx) {
                    return static_cast<parser *>(ctx)->cb_null();
                }

                friend inline
                int cb_bool_dispatch(void * ctx, int val) {
                    return static_cast<parser *>(ctx)->cb_boolean(val);
                }

                friend inline
                int cb_integer_dispatch(void * ctx, long long val) {
                    return static_cast<parser *>(ctx)->cb_integer(val);
                }

                friend inline
                int cb_double_dispatch(void * ctx, double val) {
                    return static_cast<parser *>(ctx)->cb_double(val);
                }

                friend inline
                int cb_number_dispatch(void * ctx, const char * val, size_t len) {
                    return static_cast<parser *>(ctx)->cb_number(std::string(val, len));
                }

                friend inline
                int cb_string_dispatch(void * ctx, const unsigned char * val, size_t len) {
                    return static_cast<parser *>(ctx)->cb_string(std::string(reinterpret_cast<const char *>(val), len));
                }

                friend inline
                int cb_map_start_dispatch(void * ctx) {
                    return static_cast<parser *>(ctx)->cb_map_start();
                }

                friend inline
                int cb_map_key_dispatch(void * ctx, const unsigned char * val, size_t len) {
                    return static_cast<parser *>(ctx)->cb_map_key(std::string(reinterpret_cast<const char *>(val), len));
                }

                friend inline
                int cb_map_end_dispatch(void * ctx) {
                    return static_cast<parser *>(ctx)->cb_map_end();
                }

                friend inline
                int cb_array_start_dispatch(void * ctx) {
                    return static_cast<parser *>(ctx)->cb_array_start();
                }

                friend inline
                int cb_array_end_dispatch(void * ctx) {
                    return static_cast<parser *>(ctx)->cb_array_end();
                }

            ///@}

            /*!
             * Callbacks structure. These are all virtual methods that must be
             * reimplemented in subclasses. See \ref yajl_callbacks.
             */
            yajl_callbacks m_callbacks;

            /*!
             * Internal handle. See \ref yajl_handle.
             */
            yajl_handle m_handle;
    };
}

#endif /* !YAJLPP_H */
