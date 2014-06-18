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

//! YAJL C++ APIs.
namespace yajlpp {

    void generator_print_cb(void * ctx, const char * str, size_t len);

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
     *     gen << gen.map_open()
     *             << "title" << "London Calling"
     *             << "year" << 1979
     *             << "tracks" << gen.array_open()
     *                 << gen.map_open()
     *                     << "title" << "London Calling"
     *                     << "length" << 3.18
     *                 << gen.map_close()
     *                 << gen.map_open()
     *                     << "title" << "Brand New Cadillac"
     *                     << "length" << 2.08
     *                 << gen.map.close()
     *             << gen.array_close()
     *         << gen.map_close();
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
                yajl_gen_reset(m_gen, sep.empty() ? NULL : sep.c_str());
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
            bool indentString(const std::string& indent) {
                return !yajl_gen_config(m_gen, yajl_gen_indent_string, indent.c_str());
            }

            /*!
             * Turn on / off validation of UTF-8 strings. See \ref
             * yajl_gen_validate_utf8.
             */
            inline
            bool validateUtf8(bool validate) {
                return !yajl_gen_config(m_gen, yajl_gen_validate_utf8, (validate ? 1 : 0));
            }

            /*!
             * Turn on / off escaping of slash characters. See \ref
             * yajl_gen_escape_solidus.
             */
            inline
            bool escapeSolidus(bool escape) {
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
            generator& null() {
                m_status = yajl_gen_null(m_gen);
                return *this;
            }

            /*!
             * Generate a '{' token. See \ref yajl_gen_map_open.
             */
            inline
            generator& map_open() {
                m_status = yajl_gen_map_open(m_gen);
                return *this;
            }

            /*!
             * Generate a '}' token. See \ref yajl_gen_map_close.
             */
            inline
            generator& map_close() {
                m_status = yajl_gen_map_close(m_gen);
                return *this;
            }

            /*!
             * Generate a '[' token. See \ref yajl_gen_array_open.
             */
            inline
            generator& array_open() {
                m_status = yajl_gen_array_open(m_gen);
                return *this;
            }

            /*!
             * Generate a ']' token. See \ref yajl_gen_array_close.
             */
            inline
            generator& array_close() {
                m_status = yajl_gen_array_close(m_gen);
                return *this;
            }

            ///@}


            /*!
             * \name Insertion of values into the JSON stream.
             */
            ///@{

            /*!
             * This allows to chain calls when using special JSON values, e.g.,
             * map_open() or map_close(). Example:
             *
             *     gen << "empty object" << gen.map_open() << gen.map_close();
             */
            inline
            generator& operator<<(generator&) {
                return *this;
            }

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
}

#endif /* !YAJLPP_H */
