﻿/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#ifndef __T3D_APP_KEYBOARD_H__
#define __T3D_APP_KEYBOARD_H__


#include "T3DPlatformPrerequisites.h"


namespace Tiny3D
{
    /**
     * 以使用美国 keyboard 为准，也即 Usage page 0x07
     */
    enum ScanCode
    {
        APP_SCANCODE_UNKNOWN = 0,
        APP_SCANCODE_A = 4,
        APP_SCANCODE_B = 5,
        APP_SCANCODE_C = 6,
        APP_SCANCODE_D = 7,
        APP_SCANCODE_E = 8,
        APP_SCANCODE_F = 9,
        APP_SCANCODE_G = 10,
        APP_SCANCODE_H = 11,
        APP_SCANCODE_I = 12,
        APP_SCANCODE_J = 13,
        APP_SCANCODE_K = 14,
        APP_SCANCODE_L = 15,
        APP_SCANCODE_M = 16,
        APP_SCANCODE_N = 17,
        APP_SCANCODE_O = 18,
        APP_SCANCODE_P = 19,
        APP_SCANCODE_Q = 20,
        APP_SCANCODE_R = 21,
        APP_SCANCODE_S = 22,
        APP_SCANCODE_T = 23,
        APP_SCANCODE_U = 24,
        APP_SCANCODE_V = 25,
        APP_SCANCODE_W = 26,
        APP_SCANCODE_X = 27,
        APP_SCANCODE_Y = 28,
        APP_SCANCODE_Z = 29,

        APP_SCANCODE_1 = 30,
        APP_SCANCODE_2 = 31,
        APP_SCANCODE_3 = 32,
        APP_SCANCODE_4 = 33,
        APP_SCANCODE_5 = 34,
        APP_SCANCODE_6 = 35,
        APP_SCANCODE_7 = 36,
        APP_SCANCODE_8 = 37,
        APP_SCANCODE_9 = 38,
        APP_SCANCODE_0 = 39,

        APP_SCANCODE_RETURN = 40,
        APP_SCANCODE_ESCAPE = 41,
        APP_SCANCODE_BACKSPACE = 42,
        APP_SCANCODE_TAB = 43,
        APP_SCANCODE_SPACE = 44,

        APP_SCANCODE_MINUS = 45,
        APP_SCANCODE_EQUALS = 46,
        APP_SCANCODE_LEFTBRACKET = 47,
        APP_SCANCODE_RIGHTBRACKET = 48,
        APP_SCANCODE_BACKSLASH = 49, /**< Located at the lower left of the return
                                      *   key on ISO keyboards and at the right end
                                      *   of the QWERTY row on ANSI keyboards.
                                      *   Produces REVERSE SOLIDUS (backslash) and
                                      *   VERTICAL LINE in a US layout, REVERSE
                                      *   SOLIDUS and VERTICAL LINE in a UK Mac
                                      *   layout, NUMBER SIGN and TILDE in a UK
                                      *   Windows layout, DOLLAR SIGN and POUND SIGN
                                      *   in a Swiss German layout, NUMBER SIGN and
                                      *   APOSTROPHE in a German layout, GRAVE
                                      *   ACCENT and POUND SIGN in a French Mac
                                      *   layout, and ASTERISK and MICRO SIGN in a
                                      *   French Windows layout.
                                      */
        APP_SCANCODE_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                                      *   instead of 49 for the same key, but all
                                      *   OSes I've seen treat the two codes
                                      *   identically. So, as an implementor, unless
                                      *   your keyboard generates both of those
                                      *   codes and your OS treats them differently,
                                      *   you should generate SDL_SCANCODE_BACKSLASH
                                      *   instead of this code. As a user, you
                                      *   should not rely on this code because SDL
                                      *   will never generate it with most (all?)
                                      *   keyboards.
                                      */
        APP_SCANCODE_SEMICOLON = 51,
        APP_SCANCODE_APOSTROPHE = 52,
        APP_SCANCODE_GRAVE = 53, /**< Located in the top left corner (on both ANSI
                                  *   and ISO keyboards). Produces GRAVE ACCENT and
                                  *   TILDE in a US Windows layout and in US and UK
                                  *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                                  *   and NOT SIGN in a UK Windows layout, SECTION
                                  *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                                  *   layouts on ISO keyboards, SECTION SIGN and
                                  *   DEGREE SIGN in a Swiss German layout (Mac:
                                  *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                                  *   DEGREE SIGN in a German layout (Mac: only on
                                  *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                                  *   French Windows layout, COMMERCIAL AT and
                                  *   NUMBER SIGN in a French Mac layout on ISO
                                  *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                                  *   SIGN in a Swiss German, German, or French Mac
                                  *   layout on ANSI keyboards.
                                  */
        APP_SCANCODE_COMMA = 54,
        APP_SCANCODE_PERIOD = 55,
        APP_SCANCODE_SLASH = 56,

        APP_SCANCODE_CAPSLOCK = 57,

        APP_SCANCODE_F1 = 58,
        APP_SCANCODE_F2 = 59,
        APP_SCANCODE_F3 = 60,
        APP_SCANCODE_F4 = 61,
        APP_SCANCODE_F5 = 62,
        APP_SCANCODE_F6 = 63,
        APP_SCANCODE_F7 = 64,
        APP_SCANCODE_F8 = 65,
        APP_SCANCODE_F9 = 66,
        APP_SCANCODE_F10 = 67,
        APP_SCANCODE_F11 = 68,
        APP_SCANCODE_F12 = 69,

        APP_SCANCODE_PRINTSCREEN = 70,
        APP_SCANCODE_SCROLLLOCK = 71,
        APP_SCANCODE_PAUSE = 72,
        APP_SCANCODE_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                       does send code 73, not 117) */
        APP_SCANCODE_HOME = 74,
        APP_SCANCODE_PAGEUP = 75,
        APP_SCANCODE_DELETE = 76,
        APP_SCANCODE_END = 77,
        APP_SCANCODE_PAGEDOWN = 78,
        APP_SCANCODE_RIGHT = 79,
        APP_SCANCODE_LEFT = 80,
        APP_SCANCODE_DOWN = 81,
        APP_SCANCODE_UP = 82,

        APP_SCANCODE_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                                         */
        APP_SCANCODE_KP_DIVIDE = 84,
        APP_SCANCODE_KP_MULTIPLY = 85,
        APP_SCANCODE_KP_MINUS = 86,
        APP_SCANCODE_KP_PLUS = 87,
        APP_SCANCODE_KP_ENTER = 88,
        APP_SCANCODE_KP_1 = 89,
        APP_SCANCODE_KP_2 = 90,
        APP_SCANCODE_KP_3 = 91,
        APP_SCANCODE_KP_4 = 92,
        APP_SCANCODE_KP_5 = 93,
        APP_SCANCODE_KP_6 = 94,
        APP_SCANCODE_KP_7 = 95,
        APP_SCANCODE_KP_8 = 96,
        APP_SCANCODE_KP_9 = 97,
        APP_SCANCODE_KP_0 = 98,
        APP_SCANCODE_KP_PERIOD = 99,

        APP_SCANCODE_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                                            *   keyboards have over ANSI ones,
                                            *   located between left shift and Y.
                                            *   Produces GRAVE ACCENT and TILDE in a
                                            *   US or UK Mac layout, REVERSE SOLIDUS
                                            *   (backslash) and VERTICAL LINE in a
                                            *   US or UK Windows layout, and
                                            *   LESS-THAN SIGN and GREATER-THAN SIGN
                                            *   in a Swiss German, German, or French
                                            *   layout. */
        APP_SCANCODE_APPLICATION = 101, /**< windows contextual menu, compose */
        APP_SCANCODE_POWER = 102, /**< The USB document says this is a status flag,
                                   *   not a physical key - but some Mac keyboards
                                   *   do have a power key. */
        APP_SCANCODE_KP_EQUALS = 103,
        APP_SCANCODE_F13 = 104,
        APP_SCANCODE_F14 = 105,
        APP_SCANCODE_F15 = 106,
        APP_SCANCODE_F16 = 107,
        APP_SCANCODE_F17 = 108,
        APP_SCANCODE_F18 = 109,
        APP_SCANCODE_F19 = 110,
        APP_SCANCODE_F20 = 111,
        APP_SCANCODE_F21 = 112,
        APP_SCANCODE_F22 = 113,
        APP_SCANCODE_F23 = 114,
        APP_SCANCODE_F24 = 115,
        APP_SCANCODE_EXECUTE = 116,
        APP_SCANCODE_HELP = 117,
        APP_SCANCODE_MENU = 118,
        APP_SCANCODE_SELECT = 119,
        APP_SCANCODE_STOP = 120,
        APP_SCANCODE_AGAIN = 121,   /**< redo */
        APP_SCANCODE_UNDO = 122,
        APP_SCANCODE_CUT = 123,
        APP_SCANCODE_COPY = 124,
        APP_SCANCODE_PASTE = 125,
        APP_SCANCODE_FIND = 126,
        APP_SCANCODE_MUTE = 127,
        APP_SCANCODE_VOLUMEUP = 128,
        APP_SCANCODE_VOLUMEDOWN = 129,
    /* not sure whether there's a reason to enable these */
    /*     APP_SCANCODE_LOCKINGCAPSLOCK = 130,  */
    /*     APP_SCANCODE_LOCKINGNUMLOCK = 131, */
    /*     APP_SCANCODE_LOCKINGSCROLLLOCK = 132, */
        APP_SCANCODE_KP_COMMA = 133,
        APP_SCANCODE_KP_EQUALSAS400 = 134,

        APP_SCANCODE_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                                footnotes in USB doc */
        APP_SCANCODE_INTERNATIONAL2 = 136,
        APP_SCANCODE_INTERNATIONAL3 = 137, /**< Yen */
        APP_SCANCODE_INTERNATIONAL4 = 138,
        APP_SCANCODE_INTERNATIONAL5 = 139,
        APP_SCANCODE_INTERNATIONAL6 = 140,
        APP_SCANCODE_INTERNATIONAL7 = 141,
        APP_SCANCODE_INTERNATIONAL8 = 142,
        APP_SCANCODE_INTERNATIONAL9 = 143,
        APP_SCANCODE_LANG1 = 144, /**< Hangul/English toggle */
        APP_SCANCODE_LANG2 = 145, /**< Hanja conversion */
        APP_SCANCODE_LANG3 = 146, /**< Katakana */
        APP_SCANCODE_LANG4 = 147, /**< Hiragana */
        APP_SCANCODE_LANG5 = 148, /**< Zenkaku/Hankaku */
        APP_SCANCODE_LANG6 = 149, /**< reserved */
        APP_SCANCODE_LANG7 = 150, /**< reserved */
        APP_SCANCODE_LANG8 = 151, /**< reserved */
        APP_SCANCODE_LANG9 = 152, /**< reserved */

        APP_SCANCODE_ALTERASE = 153, /**< Erase-Eaze */
        APP_SCANCODE_SYSREQ = 154,
        APP_SCANCODE_CANCEL = 155,
        APP_SCANCODE_CLEAR = 156,
        APP_SCANCODE_PRIOR = 157,
        APP_SCANCODE_RETURN2 = 158,
        APP_SCANCODE_SEPARATOR = 159,
        APP_SCANCODE_OUT = 160,
        APP_SCANCODE_OPER = 161,
        APP_SCANCODE_CLEARAGAIN = 162,
        APP_SCANCODE_CRSEL = 163,
        APP_SCANCODE_EXSEL = 164,

        APP_SCANCODE_KP_00 = 176,
        APP_SCANCODE_KP_000 = 177,
        APP_SCANCODE_THOUSANDSSEPARATOR = 178,
        APP_SCANCODE_DECIMALSEPARATOR = 179,
        APP_SCANCODE_CURRENCYUNIT = 180,
        APP_SCANCODE_CURRENCYSUBUNIT = 181,
        APP_SCANCODE_KP_LEFTPAREN = 182,
        APP_SCANCODE_KP_RIGHTPAREN = 183,
        APP_SCANCODE_KP_LEFTBRACE = 184,
        APP_SCANCODE_KP_RIGHTBRACE = 185,
        APP_SCANCODE_KP_TAB = 186,
        APP_SCANCODE_KP_BACKSPACE = 187,
        APP_SCANCODE_KP_A = 188,
        APP_SCANCODE_KP_B = 189,
        APP_SCANCODE_KP_C = 190,
        APP_SCANCODE_KP_D = 191,
        APP_SCANCODE_KP_E = 192,
        APP_SCANCODE_KP_F = 193,
        APP_SCANCODE_KP_XOR = 194,
        APP_SCANCODE_KP_POWER = 195,
        APP_SCANCODE_KP_PERCENT = 196,
        APP_SCANCODE_KP_LESS = 197,
        APP_SCANCODE_KP_GREATER = 198,
        APP_SCANCODE_KP_AMPERSAND = 199,
        APP_SCANCODE_KP_DBLAMPERSAND = 200,
        APP_SCANCODE_KP_VERTICALBAR = 201,
        APP_SCANCODE_KP_DBLVERTICALBAR = 202,
        APP_SCANCODE_KP_COLON = 203,
        APP_SCANCODE_KP_HASH = 204,
        APP_SCANCODE_KP_SPACE = 205,
        APP_SCANCODE_KP_AT = 206,
        APP_SCANCODE_KP_EXCLAM = 207,
        APP_SCANCODE_KP_MEMSTORE = 208,
        APP_SCANCODE_KP_MEMRECALL = 209,
        APP_SCANCODE_KP_MEMCLEAR = 210,
        APP_SCANCODE_KP_MEMADD = 211,
        APP_SCANCODE_KP_MEMSUBTRACT = 212,
        APP_SCANCODE_KP_MEMMULTIPLY = 213,
        APP_SCANCODE_KP_MEMDIVIDE = 214,
        APP_SCANCODE_KP_PLUSMINUS = 215,
        APP_SCANCODE_KP_CLEAR = 216,
        APP_SCANCODE_KP_CLEARENTRY = 217,
        APP_SCANCODE_KP_BINARY = 218,
        APP_SCANCODE_KP_OCTAL = 219,
        APP_SCANCODE_KP_DECIMAL = 220,
        APP_SCANCODE_KP_HEXADECIMAL = 221,

        APP_SCANCODE_LCTRL = 224,
        APP_SCANCODE_LSHIFT = 225,
        APP_SCANCODE_LALT = 226, /**< alt, option */
        APP_SCANCODE_LGUI = 227, /**< windows, command (apple), meta */
        APP_SCANCODE_RCTRL = 228,
        APP_SCANCODE_RSHIFT = 229,
        APP_SCANCODE_RALT = 230, /**< alt gr, option */
        APP_SCANCODE_RGUI = 231, /**< windows, command (apple), meta */

        APP_SCANCODE_MODE = 257,    /**< I'm not sure if this is really not covered
                                     *   by any of the above, but since there's a
                                     *   special KMOD_MODE for it I'm adding it here
                                     */

        /* @} *//* Usage page 0x07 */

        /**
         *  \name Usage page 0x0C
         *
         *  These values are mapped from usage page 0x0C (USB consumer page).
         */
        /* @{ */

        APP_SCANCODE_AUDIONEXT = 258,
        APP_SCANCODE_AUDIOPREV = 259,
        APP_SCANCODE_AUDIOSTOP = 260,
        APP_SCANCODE_AUDIOPLAY = 261,
        APP_SCANCODE_AUDIOMUTE = 262,
        APP_SCANCODE_MEDIASELECT = 263,
        APP_SCANCODE_WWW = 264,
        APP_SCANCODE_MAIL = 265,
        APP_SCANCODE_CALCULATOR = 266,
        APP_SCANCODE_COMPUTER = 267,
        APP_SCANCODE_AC_SEARCH = 268,
        APP_SCANCODE_AC_HOME = 269,
        APP_SCANCODE_AC_BACK = 270,
        APP_SCANCODE_AC_FORWARD = 271,
        APP_SCANCODE_AC_STOP = 272,
        APP_SCANCODE_AC_REFRESH = 273,
        APP_SCANCODE_AC_BOOKMARKS = 274,

        /* @} *//* Usage page 0x0C */

        /**
         *  \name Walther keys
         *
         *  These are values that Christian Walther added (for mac keyboard?).
         */
        /* @{ */

        APP_SCANCODE_BRIGHTNESSDOWN = 275,
        APP_SCANCODE_BRIGHTNESSUP = 276,
        APP_SCANCODE_DISPLAYSWITCH = 277, /**< display mirroring/dual display
                                               switch, video mode switch */
        APP_SCANCODE_KBDILLUMTOGGLE = 278,
        APP_SCANCODE_KBDILLUMDOWN = 279,
        APP_SCANCODE_KBDILLUMUP = 280,
        APP_SCANCODE_EJECT = 281,
        APP_SCANCODE_SLEEP = 282,

        APP_SCANCODE_APP1 = 283,
        APP_SCANCODE_APP2 = 284,

        /* @} *//* Walther keys */

        /**
         *  \name Usage page 0x0C (additional media keys)
         *
         *  These values are mapped from usage page 0x0C (USB consumer page).
         */
        /* @{ */

        APP_SCANCODE_AUDIOREWIND = 285,
        APP_SCANCODE_AUDIOFASTFORWARD = 286,

        /* @} *//* Usage page 0x0C (additional media keys) */

        /* Add any other keys here. */

        APP_NUM_SCANCODES = 512 /**< not a key, just marks the number of scancodes
                                     for array bounds */
    };

    #define TKEY_SCANCODE_MASK (1<<30)
    #define T3D_SCANCODE_TO_KEYCODE(X)  ((X) | TKEY_SCANCODE_MASK)
    
    enum KeyCode
    {
        TKEY_UNKNOWN = 0,

        TKEY_RETURN = '\r',
        TKEY_ESCAPE = '\033',
        TKEY_BACKSPACE = '\b',
        TKEY_TAB = '\t',
        TKEY_SPACE = ' ',
        TKEY_EXCLAIM = '!',
        TKEY_QUOTEDBL = '"',
        TKEY_HASH = '#',
        TKEY_PERCENT = '%',
        TKEY_DOLLAR = '$',
        TKEY_AMPERSAND = '&',
        TKEY_QUOTE = '\'',
        TKEY_LEFTPAREN = '(',
        TKEY_RIGHTPAREN = ')',
        TKEY_ASTERISK = '*',
        TKEY_PLUS = '+',
        TKEY_COMMA = ',',
        TKEY_MINUS = '-',
        TKEY_PERIOD = '.',
        TKEY_SLASH = '/',
        TKEY_0 = '0',
        TKEY_1 = '1',
        TKEY_2 = '2',
        TKEY_3 = '3',
        TKEY_4 = '4',
        TKEY_5 = '5',
        TKEY_6 = '6',
        TKEY_7 = '7',
        TKEY_8 = '8',
        TKEY_9 = '9',
        TKEY_COLON = ':',
        TKEY_SEMICOLON = ';',
        TKEY_LESS = '<',
        TKEY_EQUALS = '=',
        TKEY_GREATER = '>',
        TKEY_QUESTION = '?',
        TKEY_AT = '@',
        /*
           Skip uppercase letters
         */
        TKEY_LEFTBRACKET = '[',
        TKEY_BACKSLASH = '\\',
        TKEY_RIGHTBRACKET = ']',
        TKEY_CARET = '^',
        TKEY_UNDERSCORE = '_',
        TKEY_BACKQUOTE = '`',
        TKEY_a = 'a',
        TKEY_b = 'b',
        TKEY_c = 'c',
        TKEY_d = 'd',
        TKEY_e = 'e',
        TKEY_f = 'f',
        TKEY_g = 'g',
        TKEY_h = 'h',
        TKEY_i = 'i',
        TKEY_j = 'j',
        TKEY_k = 'k',
        TKEY_l = 'l',
        TKEY_m = 'm',
        TKEY_n = 'n',
        TKEY_o = 'o',
        TKEY_p = 'p',
        TKEY_q = 'q',
        TKEY_r = 'r',
        TKEY_s = 's',
        TKEY_t = 't',
        TKEY_u = 'u',
        TKEY_v = 'v',
        TKEY_w = 'w',
        TKEY_x = 'x',
        TKEY_y = 'y',
        TKEY_z = 'z',

        TKEY_CAPSLOCK = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_CAPSLOCK),

        TKEY_F1 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F1),
        TKEY_F2 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F2),
        TKEY_F3 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F3),
        TKEY_F4 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F4),
        TKEY_F5 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F5),
        TKEY_F6 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F6),
        TKEY_F7 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F7),
        TKEY_F8 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F8),
        TKEY_F9 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F9),
        TKEY_F10 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F10),
        TKEY_F11 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F11),
        TKEY_F12 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F12),

        TKEY_PRINTSCREEN = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_PRINTSCREEN),
        TKEY_SCROLLLOCK = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_SCROLLLOCK),
        TKEY_PAUSE = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_PAUSE),
        TKEY_INSERT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_INSERT),
        TKEY_HOME = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_HOME),
        TKEY_PAGEUP = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_PAGEUP),
        TKEY_DELETE = '\177',
        TKEY_END = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_END),
        TKEY_PAGEDOWN = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_PAGEDOWN),
        TKEY_RIGHT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_RIGHT),
        TKEY_LEFT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_LEFT),
        TKEY_DOWN = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_DOWN),
        TKEY_UP = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_UP),

        TKEY_NUMLOCKCLEAR = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_NUMLOCKCLEAR),
        TKEY_KP_DIVIDE = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_DIVIDE),
        TKEY_KP_MULTIPLY = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_MULTIPLY),
        TKEY_KP_MINUS = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_MINUS),
        TKEY_KP_PLUS = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_PLUS),
        TKEY_KP_ENTER = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_ENTER),
        TKEY_KP_1 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_1),
        TKEY_KP_2 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_2),
        TKEY_KP_3 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_3),
        TKEY_KP_4 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_4),
        TKEY_KP_5 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_5),
        TKEY_KP_6 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_6),
        TKEY_KP_7 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_7),
        TKEY_KP_8 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_8),
        TKEY_KP_9 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_9),
        TKEY_KP_0 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_0),
        TKEY_KP_PERIOD = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_PERIOD),

        TKEY_APPLICATION = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_APPLICATION),
        TKEY_POWER = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_POWER),
        TKEY_KP_EQUALS = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_EQUALS),
        TKEY_F13 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F13),
        TKEY_F14 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F14),
        TKEY_F15 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F15),
        TKEY_F16 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F16),
        TKEY_F17 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F17),
        TKEY_F18 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F18),
        TKEY_F19 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F19),
        TKEY_F20 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F20),
        TKEY_F21 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F21),
        TKEY_F22 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F22),
        TKEY_F23 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F23),
        TKEY_F24 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_F24),
        TKEY_EXECUTE = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_EXECUTE),
        TKEY_HELP = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_HELP),
        TKEY_MENU = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_MENU),
        TKEY_SELECT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_SELECT),
        TKEY_STOP = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_STOP),
        TKEY_AGAIN = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_AGAIN),
        TKEY_UNDO = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_UNDO),
        TKEY_CUT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_CUT),
        TKEY_COPY = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_COPY),
        TKEY_PASTE = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_PASTE),
        TKEY_FIND = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_FIND),
        TKEY_MUTE = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_MUTE),
        TKEY_VOLUMEUP = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_VOLUMEUP),
        TKEY_VOLUMEDOWN = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_VOLUMEDOWN),
        TKEY_KP_COMMA = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_COMMA),
        TKEY_KP_EQUALSAS400 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_EQUALSAS400),

        TKEY_ALTERASE = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_ALTERASE),
        TKEY_SYSREQ = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_SYSREQ),
        TKEY_CANCEL = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_CANCEL),
        TKEY_CLEAR = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_CLEAR),
        TKEY_PRIOR = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_PRIOR),
        TKEY_RETURN2 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_RETURN2),
        TKEY_SEPARATOR = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_SEPARATOR),
        TKEY_OUT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_OUT),
        TKEY_OPER = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_OPER),
        TKEY_CLEARAGAIN = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_CLEARAGAIN),
        TKEY_CRSEL = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_CRSEL),
        TKEY_EXSEL = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_EXSEL),

        TKEY_KP_00 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_00),
        TKEY_KP_000 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_000),
        TKEY_THOUSANDSSEPARATOR = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_THOUSANDSSEPARATOR),
        TKEY_DECIMALSEPARATOR = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_DECIMALSEPARATOR),
        TKEY_CURRENCYUNIT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_CURRENCYUNIT),
        TKEY_CURRENCYSUBUNIT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_CURRENCYSUBUNIT),
        TKEY_KP_LEFTPAREN = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_LEFTPAREN),
        TKEY_KP_RIGHTPAREN = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_RIGHTPAREN),
        TKEY_KP_LEFTBRACE = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_LEFTBRACE),
        TKEY_KP_RIGHTBRACE = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_RIGHTBRACE),
        TKEY_KP_TAB = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_TAB),
        TKEY_KP_BACKSPACE = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_BACKSPACE),
        TKEY_KP_A = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_A),
        TKEY_KP_B = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_B),
        TKEY_KP_C = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_C),
        TKEY_KP_D = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_D),
        TKEY_KP_E = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_E),
        TKEY_KP_F = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_F),
        TKEY_KP_XOR = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_XOR),
        TKEY_KP_POWER = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_POWER),
        TKEY_KP_PERCENT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_PERCENT),
        TKEY_KP_LESS = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_LESS),
        TKEY_KP_GREATER = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_GREATER),
        TKEY_KP_AMPERSAND = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_AMPERSAND),
        TKEY_KP_DBLAMPERSAND = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_DBLAMPERSAND),
        TKEY_KP_VERTICALBAR = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_VERTICALBAR),
        TKEY_KP_DBLVERTICALBAR = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_DBLVERTICALBAR),
        TKEY_KP_COLON = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_COLON),
        TKEY_KP_HASH = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_HASH),
        TKEY_KP_SPACE = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_SPACE),
        TKEY_KP_AT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_AT),
        TKEY_KP_EXCLAM = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_EXCLAM),
        TKEY_KP_MEMSTORE = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_MEMSTORE),
        TKEY_KP_MEMRECALL = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_MEMRECALL),
        TKEY_KP_MEMCLEAR = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_MEMCLEAR),
        TKEY_KP_MEMADD = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_MEMADD),
        TKEY_KP_MEMSUBTRACT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_MEMSUBTRACT),
        TKEY_KP_MEMMULTIPLY = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_MEMMULTIPLY),
        TKEY_KP_MEMDIVIDE = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_MEMDIVIDE),
        TKEY_KP_PLUSMINUS = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_PLUSMINUS),
        TKEY_KP_CLEAR = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_CLEAR),
        TKEY_KP_CLEARENTRY = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_CLEARENTRY),
        TKEY_KP_BINARY = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_BINARY),
        TKEY_KP_OCTAL = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_OCTAL),
        TKEY_KP_DECIMAL = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_DECIMAL),
        TKEY_KP_HEXADECIMAL = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KP_HEXADECIMAL),

        TKEY_LCTRL = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_LCTRL),
        TKEY_LSHIFT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_LSHIFT),
        TKEY_LALT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_LALT),
        TKEY_LGUI = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_LGUI),
        TKEY_RCTRL = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_RCTRL),
        TKEY_RSHIFT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_RSHIFT),
        TKEY_RALT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_RALT),
        TKEY_RGUI = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_RGUI),

        TKEY_MODE = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_MODE),

        TKEY_AUDIONEXT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_AUDIONEXT),
        TKEY_AUDIOPREV = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_AUDIOPREV),
        TKEY_AUDIOSTOP = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_AUDIOSTOP),
        TKEY_AUDIOPLAY = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_AUDIOPLAY),
        TKEY_AUDIOMUTE = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_AUDIOMUTE),
        TKEY_MEDIASELECT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_MEDIASELECT),
        TKEY_WWW = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_WWW),
        TKEY_MAIL = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_MAIL),
        TKEY_CALCULATOR = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_CALCULATOR),
        TKEY_COMPUTER = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_COMPUTER),
        TKEY_AC_SEARCH = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_AC_SEARCH),
        TKEY_AC_HOME = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_AC_HOME),
        TKEY_AC_BACK = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_AC_BACK),
        TKEY_AC_FORWARD = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_AC_FORWARD),
        TKEY_AC_STOP = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_AC_STOP),
        TKEY_AC_REFRESH = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_AC_REFRESH),
        TKEY_AC_BOOKMARKS = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_AC_BOOKMARKS),

        TKEY_BRIGHTNESSDOWN = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_BRIGHTNESSDOWN),
        TKEY_BRIGHTNESSUP = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_BRIGHTNESSUP),
        TKEY_DISPLAYSWITCH = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_DISPLAYSWITCH),
        TKEY_KBDILLUMTOGGLE = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KBDILLUMTOGGLE),
        TKEY_KBDILLUMDOWN = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KBDILLUMDOWN),
        TKEY_KBDILLUMUP = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_KBDILLUMUP),
        TKEY_EJECT = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_EJECT),
        TKEY_SLEEP = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_SLEEP),
        TKEY_APP1 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_APP1),
        TKEY_APP2 = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_APP2),

        TKEY_AUDIOREWIND = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_AUDIOREWIND),
        TKEY_AUDIOFASTFORWARD = T3D_SCANCODE_TO_KEYCODE(APP_SCANCODE_AUDIOFASTFORWARD)
    };
}



#endif  /*__T3D_APP_KEYBOARD_H__*/
