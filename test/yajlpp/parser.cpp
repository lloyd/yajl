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

#include <yajl/yajlpp.h>

#include <iostream>
#include <sstream>
#include <string>
using namespace std;

class myparser : public yajlpp::parser {

    private:
        stringstream buf;

    public:
        myparser() {}

        string buffer() const {
            return buf.str();
        }

        int cb_null() {
            buf << " null";
            return 1;
        }

        int cb_boolean(bool val) {
            buf << " bool: " << val;
            return 1;
        }

        int cb_integer(long long val) {
            buf << " integer: " << val;
            return 1;
        }

        int cb_double(double val) {
            buf << " double: " << val;
            return 1;
        }

        int cb_number(std::string val) {
            buf << " number: " << val;
            return 1;
        }

        int cb_string(std::string val) {
            buf << " string: " << val;
            return 1;
        }

        int cb_map_start() {
            buf << " {";
            return 1;
        }

        int cb_map_key(std::string val) {
            buf << " key: " << val;
            return 1;
        }

        int cb_map_end() {
            buf << " }";
            return 1;
        }

        int cb_array_start() {
            buf << " [";
            return 1;
        }

        int cb_array_end() {
            buf << " ]";
            return 1;
        }
};

int main()
{
    static std::string input = R"json({"obj":{"first" : 12.34, "second" : ["a_1", "a_2", null, 4536]}})json";
    static std::string expected = " { key: obj { key: first number: 12.34 key: second [ string: a_1 string: a_2 null number: 4536 ] } }";
    myparser p;
    p.parse(input);

    if (p.buffer() != expected) {
        return 0;
    }

    if (p.bytes_consumed() != input.size()) {
        return 0;
    }

    return 1;
}
