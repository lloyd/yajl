/*
 * Copyright 2007, Lloyd Hilaiel.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 * 
 *  3. Neither the name of Lloyd Hilaiel nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */ 

#include "yajl_lex.h"
#include "yajl_buf.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#ifdef YAJL_LEXER_DEBUG
static const char *
tokToStr(yajl_tok tok) 
{
    switch (tok) {
        case yajl_tok_bool: return "bool";
        case yajl_tok_colon: return "colon";
        case yajl_tok_comma: return "comma";
        case yajl_tok_eof: return "eof";
        case yajl_tok_error: return "error";
        case yajl_tok_left_brace: return "brace";
        case yajl_tok_left_bracket: return "bracket";
        case yajl_tok_null: return "null";
        case yajl_tok_integer: return "integer";
        case yajl_tok_double: return "double";
        case yajl_tok_right_brace: return "brace";
        case yajl_tok_right_bracket: return "bracket";
        case yajl_tok_string: return "string";
        case yajl_tok_string_with_escapes: return "string_with_escapes";
    }
    return "unknown";
}
#endif

/* Impact of the stream parsing feature on the lexer:
 *
 * YAJL support stream parsing.  That is, the ability to parse the first
 * bits of a chunk of JSON before the last bits are available (still on
 * the network or disk).  This makes the lexer more complex.  The
 * responsibility of the lexer is to handle transparently the case where
 * a chunk boundary falls in the middle of a token.  The way this is
 * accomplished is via a buffer and a character reading abstraction. 
 *
 * Overview of implementation
 *
 * When we lex to end of input string before end of token is hit, we
 * copy all of the input text composing the token into our lexBuf.
 * 
 * Every time we read a character, we do so through the readChar function.
 * readChar's responsibility is to handle pulling all chars from the buffer
 * before pulling chars from input text
 */

struct yajl_lexer_t {
    /* the overal line and char offset into the data */
    unsigned int lineOff;
    unsigned int charOff;

    /* error */
    yajl_lex_error error;

    /* a input buffer to handle the case where a token is spread over
     * multiple chunks */ 
    yajl_buf buf;

    /* in the case where we have data in the lexBuf, bufOff holds
     * the current offset into the lexBuf. */
    unsigned int bufOff;

    /* are we using the lex buf? */
    unsigned int bufInUse;
};

static unsigned char
readChar(yajl_lexer lxr, const unsigned char * txt, unsigned int *off) 
{
    if (lxr->bufInUse && yajl_buf_len(lxr->buf) &&
        lxr->bufOff < yajl_buf_len(lxr->buf))
    {
        return *((unsigned char *) yajl_buf_data(lxr->buf) + (lxr->bufOff)++);
    }
    return txt[(*off)++];
}

static void
unreadChar(yajl_lexer lxr, unsigned int *off) 
{
    if (*off > 0) (*off)--;
    else (lxr->bufOff)--;
}

yajl_lexer
yajl_lex_alloc(void)
{
    yajl_lexer lxr = (yajl_lexer) calloc(1, sizeof(struct yajl_lexer_t));
    lxr->buf = yajl_buf_alloc();
    return lxr;
}

void
yajl_lex_free(yajl_lexer lxr)
{
    yajl_buf_free(lxr->buf);
    free(lxr);
    return;
}

/* a lookup table which lets us quickly determine three things:
 * VEC - valid escaped conrol char
 * IJC - invalid json char
 * VHC - valid hex char
 * note.  the solidus '/' may be escaped or not.
 * note.  the
 */
#define VEC 1
#define IJC 2
#define VHC 4
const static char charLookupTable[256] =
{
/*00*/ IJC    , IJC    , IJC    , IJC    , IJC    , IJC    , IJC    , IJC    ,
/*08*/ IJC    , IJC    , IJC    , IJC    , IJC    , IJC    , IJC    , IJC    ,
/*10*/ IJC    , IJC    , IJC    , IJC    , IJC    , IJC    , IJC    , IJC    ,
/*18*/ IJC    , IJC    , IJC    , IJC    , IJC    , IJC    , IJC    , IJC    ,

/*20*/ 0      , 0      , VEC|IJC, 0      , 0      , 0      , 0      , 0      ,
/*28*/ 0      , 0      , 0      , 0      , 0      , 0      , 0      , VEC    ,
/*30*/ VHC    , VHC    , VHC    , VHC    , VHC    , VHC    , VHC    , VHC    ,
/*38*/ VHC    , VHC    , 0      , 0      , 0      , 0      , 0      , 0      ,

/*40*/ 0      , VHC    , VHC    , VHC    , VHC    , VHC    , VHC    , 0      ,
/*48*/ 0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      ,
/*50*/ 0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      ,
/*58*/ 0      , 0      , 0      , 0      , VEC|IJC, 0      , 0      , 0      ,

/*60*/ 0      , VHC    , VEC|VHC, VHC    , VHC    , VHC    , VEC|VHC, 0      ,
/*68*/ 0      , 0      , 0      , 0      , 0      , 0      , VEC    , 0      ,
/*70*/ 0      , 0      , VEC    , 0      , VEC    , 0      , 0      , 0      ,
/*78*/ 0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      ,

/* include these so we don't have to always check the range of the char */
       0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 
       0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 
       0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 
       0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 

       0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 
       0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 
       0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 
       0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 

       0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 
       0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 
       0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 
       0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 

       0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 
       0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 
       0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 
       0      , 0      , 0      , 0      , 0      , 0      , 0      , 0
};

/* lex a string.  input is the lexer, pointer to beginning of
 * json text, and start of string (offset).
 * a token is returned which has the following meanings:
 * yajl_tok_string: lex of string was successful.  offset points to
 *                  terminating '"'.
 * yajl_tok_eof: end of text was encountered before we could complete
 *               the lex.
 * yajl_tok_error: embedded in the string were unallowable chars.  offset
 *               points to the offending char
 */
#define STR_CHECK_EOF \
if (*offset >= jsonTextLen) { \
   tok = yajl_tok_eof; \
   break; \
}

static yajl_tok
yajl_lex_string(yajl_lexer lexer, const unsigned char * jsonText,
                unsigned int jsonTextLen, unsigned int * offset)
{
    yajl_tok tok = yajl_tok_error;
    int hasEscapes = 0;

    for (;;) {
		unsigned char curChar;

		STR_CHECK_EOF;

        curChar = readChar(lexer, jsonText, offset);

        /* quote terminates */
        if (curChar == '"') {
            tok = yajl_tok_string;
            break;
        }
        /* backslash escapes a set of control chars, */
        else if (curChar == '\\') {
            hasEscapes = 1;
            STR_CHECK_EOF;

            /* special case \u */
            curChar = readChar(lexer, jsonText, offset);
            if (curChar == 'u') {
                unsigned int i = 0;

                for (i=0;i<4;i++) {
                    STR_CHECK_EOF;                
                    curChar = readChar(lexer, jsonText, offset);                
                    if (!(charLookupTable[curChar] & VHC)) {
                        /* back up to offending char */
                        unreadChar(lexer, offset);
                        lexer->error = yajl_lex_string_invalid_hex_char;
                        goto finish_string_lex;
                    }
                }
            } else if (!(charLookupTable[curChar] & VEC)) {
                /* back up to offending char */
                unreadChar(lexer, offset);
                lexer->error = yajl_lex_string_invalid_escaped_char;
                goto finish_string_lex;                
            } 
        }
        /* check if it's a valid json char */
        else if (charLookupTable[curChar] & IJC) {
            /* back up to offending char */
            unreadChar(lexer, offset);
            lexer->error = yajl_lex_string_invalid_json_char;
            goto finish_string_lex;                
        }
        /* accept it, and move on */ 
    }
  finish_string_lex:
    /* tell our buddy, the parser, wether he needs to process this string
     * again */
    if (hasEscapes && tok == yajl_tok_string) {
        tok = yajl_tok_string_with_escapes;
    } 

    return tok;
}

#define NUM_CHECK_EOF if (*offset >= jsonTextLen) return yajl_tok_eof;

static yajl_tok
yajl_lex_number(yajl_lexer lexer, const unsigned char * jsonText,
                unsigned int jsonTextLen, unsigned int * offset)
{
    /** XXX: numbers are the only entities in json that we must lex
     *       _beyond_ in order to know that they are complete.  There
     *       is an ambiguous case for integers at EOF. */

    unsigned char c;

    yajl_tok tok = yajl_tok_integer;

    NUM_CHECK_EOF;    
    c = readChar(lexer, jsonText, offset);

    /* optional leading minus */
    if (c == '-') {
        NUM_CHECK_EOF;    
        c = readChar(lexer, jsonText, offset); 
    }

    /* a single zero, or a series of integers */
    if (c == '0') {
        NUM_CHECK_EOF;    
        c = readChar(lexer, jsonText, offset); 
    } else if (c >= '1' && c <= '9') {
        do {
            NUM_CHECK_EOF;    
            c = readChar(lexer, jsonText, offset); 
        } while (c >= '0' && c <= '9');
    } else {
        unreadChar(lexer, offset);
        lexer->error = yajl_lex_missing_integer_after_minus;
        return yajl_tok_error;
    }

    /* optional fraction (indicates this is floating point) */
    if (c == '.') {
        int numRd = 0;
        
        NUM_CHECK_EOF;
        c = readChar(lexer, jsonText, offset); 

        while (c >= '0' && c <= '9') {
            numRd++;
            NUM_CHECK_EOF;
            c = readChar(lexer, jsonText, offset); 
        } 

        if (!numRd) {
            unreadChar(lexer, offset);
            lexer->error = yajl_lex_missing_integer_after_decimal;
            return yajl_tok_error;
        }
        tok = yajl_tok_double;
    }

    /* optional exponent (indicates this is floating point) */
    if (c == 'e' || c == 'E') {
        NUM_CHECK_EOF;
        c = readChar(lexer, jsonText, offset); 

        /* optional sign */
        if (c == '+' || c == '-') {
            NUM_CHECK_EOF;
            c = readChar(lexer, jsonText, offset); 
        }

        if (c >= '1' && c <= '9') {
            do {
                NUM_CHECK_EOF;
                c = readChar(lexer, jsonText, offset); 
            } while (c >= '0' && c <= '9');
        } else {
            unreadChar(lexer, offset);
            lexer->error = yajl_lex_missing_integer_after_exponent;
            return yajl_tok_error;
        }
        tok = yajl_tok_double;
    }
    
    /* we always go "one too far" */
    unreadChar(lexer, offset);
    
    return tok;
}

yajl_tok
yajl_lex_lex(yajl_lexer lexer, const unsigned char * jsonText,
             unsigned int jsonTextLen, unsigned int * context,
             const unsigned char ** outBuf, unsigned int * outLen)
{
    yajl_tok tok = yajl_tok_error;
    unsigned char c;
    unsigned int startCtx = *context;

    *outBuf = NULL;
    *outLen = 0;

    for (;;) {
        assert(*context <= jsonTextLen);

        if (*context >= jsonTextLen) {
            tok = yajl_tok_eof;
            goto lexed;
        }

        c = readChar(lexer, jsonText, context);

        switch (c) {
            case '{':
                tok = yajl_tok_left_bracket;
                goto lexed;
            case '}':
                tok = yajl_tok_right_bracket;
                goto lexed;
            case '[':
                tok = yajl_tok_left_brace;
                goto lexed;
            case ']':
                tok = yajl_tok_right_brace;
                goto lexed;
            case ',':
                tok = yajl_tok_comma;
                goto lexed;
            case ':':
                tok = yajl_tok_colon;
                goto lexed;
            case '\t': case '\n': case '\v': case '\f': case '\r': case ' ':
                startCtx++;
                break;
            case 't': {
                const char * want = "rue";
                do {
                    if (*context >= jsonTextLen) {
                        tok = yajl_tok_eof;
                        goto lexed;
                    }
                    c = readChar(lexer, jsonText, context);
                    if (c != *want) {
                        unreadChar(lexer, context);
                        lexer->error = yajl_lex_invalid_string;
                        tok = yajl_tok_error;
                        goto lexed;
                    }
                } while (*(++want));
                tok = yajl_tok_bool;
                goto lexed;
            }
            case 'f': {
                const char * want = "alse";
                do {
                    if (*context >= jsonTextLen) {
                        tok = yajl_tok_eof;
                        goto lexed;
                    }
                    c = readChar(lexer, jsonText, context);
                    if (c != *want) {
                        unreadChar(lexer, context);
                        lexer->error = yajl_lex_invalid_string;
                        tok = yajl_tok_error;
                        goto lexed;
                    }
                } while (*(++want));
                tok = yajl_tok_bool;
                goto lexed;
            }
            case 'n': {
                const char * want = "ull";
                do {
                    if (*context >= jsonTextLen) {
                        tok = yajl_tok_eof;
                        goto lexed;
                    }
                    c = readChar(lexer, jsonText, context);
                    if (c != *want) {
                        unreadChar(lexer, context);
                        lexer->error = yajl_lex_invalid_string;
                        tok = yajl_tok_error;
                        goto lexed;
                    }
                } while (*(++want));
                tok = yajl_tok_null;
                goto lexed;
            }
            case '"': {
                tok = yajl_lex_string(lexer, (unsigned char *) jsonText,
                                      jsonTextLen, context);
                goto lexed;
            }
            case '-':
            case '0': case '1': case '2': case '3': case '4': 
            case '5': case '6': case '7': case '8': case '9': {
                /* integer parsing wants to start from the beginning */
                unreadChar(lexer, context);
                tok = yajl_lex_number(lexer, (unsigned char *) jsonText,
                                      jsonTextLen, context);
                goto lexed;
            }
            default:
                lexer->error = yajl_lex_invalid_char;
                tok = yajl_tok_error;
                goto lexed;
        }
    }


  lexed:
    /* need to append to buffer if the buffer is in use or
     * if it's an EOF token */
    if (tok == yajl_tok_eof || lexer->bufInUse) {
        if (!lexer->bufInUse) yajl_buf_clear(lexer->buf);
        lexer->bufInUse = 1;
        yajl_buf_append(lexer->buf, jsonText + startCtx, *context - startCtx);
        lexer->bufOff = 0;
        
        if (tok != yajl_tok_eof) {
            *outBuf = yajl_buf_data(lexer->buf);
            *outLen = yajl_buf_len(lexer->buf);
            lexer->bufInUse = 0;
        }
    } else if (tok != yajl_tok_error) {
        *outBuf = jsonText + startCtx;
        *outLen = *context - startCtx;
    }

    /* special case for strings. skip the quotes. */
    if (tok == yajl_tok_string || tok == yajl_tok_string_with_escapes)
    {
        assert(*outLen >= 2);
        (*outBuf)++;
        *outLen -= 2; 
    }


#ifdef YAJL_LEXER_DEBUG
    if (tok == yajl_tok_error) {
        printf("lexical error: %s\n",
               yajl_lex_error_to_string(yajl_lex_get_error(lexer)));
    } else if (tok == yajl_tok_eof) {
        printf("EOF hit\n");
    } else {
        printf("lexed %s: '", tokToStr(tok));
        fwrite(*outBuf, 1, *outLen, stdout);
        printf("'\n");
    }
#endif

    return tok;
}

const char *
yajl_lex_error_to_string(yajl_lex_error error)
{
    switch (error) {
        case yajl_lex_e_ok:
            return "ok, no error";
        case yajl_lex_string_invalid_escaped_char:
            return "inside a string, '\\' occurs before a character "
                   "which it may not.";
        case yajl_lex_string_invalid_json_char:            
            return "invalid character inside string.";
        case yajl_lex_string_invalid_hex_char:
            return "invalid (non-hex) character occurs after '\\u' inside "
                   "string.";
        case yajl_lex_invalid_char:
            return "invalid char in json text.";
        case yajl_lex_invalid_string:
            return "invalid string in json text.";
        case yajl_lex_missing_integer_after_exponent:
            return "malformed number, a digit is required after the exponent.";
        case yajl_lex_missing_integer_after_decimal:
            return "malformed number, a digit is required after the "
                   "decimal point.";
        case yajl_lex_missing_integer_after_minus:
            return "malformed number, a digit is required after the "
                   "minus sign.";
    }
    return "unknown error code";
}


/** allows access to more specific information about the lexical
 *  error when yajl_lex_lex returns yajl_tok_error. */
yajl_lex_error
yajl_lex_get_error(yajl_lexer lexer)
{
    if (lexer == NULL) return (yajl_lex_error) -1;
    return lexer->error;
}

unsigned int yajl_lex_current_line(yajl_lexer lexer)
{
    return lexer->lineOff;
}

unsigned int yajl_lex_current_char(yajl_lexer lexer)
{
    return lexer->charOff;
}

yajl_tok yajl_lex_peek(yajl_lexer lexer, const unsigned char * jsonText,
                       unsigned int jsonTextLen, unsigned int offset)
{
    const unsigned char * outBuf;
    unsigned int outLen;
    unsigned int bufLen = yajl_buf_len(lexer->buf);
    unsigned int bufOff = lexer->bufOff;
    unsigned int bufInUse = lexer->bufInUse;
    yajl_tok tok;
    
    tok = yajl_lex_lex(lexer, jsonText, jsonTextLen, &offset,
                       &outBuf, &outLen);

    lexer->bufOff = bufOff;
    lexer->bufInUse = bufInUse;
    yajl_buf_truncate(lexer->buf, bufLen);
    
    return tok;
}
