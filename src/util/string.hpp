//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2006 Twan van Laarhoven                           |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

#ifndef HEADER_UTIL_STRING
#define HEADER_UTIL_STRING

/** @file util/string.hpp
 *
 *  String and character utility functions and macros
 */

// ----------------------------------------------------------------------------- : Includes

#include "prec.hpp"
#include "for_each.hpp"
#include <ctype.h>
#include <boost/preprocessor/cat.hpp>

class wxTextOutputStream;

// ----------------------------------------------------------------------------- : String type

/// The string type used throughout MSE
typedef wxString String;

DECLARE_TYPEOF_NO_REV(String); // iterating over characters in a string

// ----------------------------------------------------------------------------- : Unicode

/// u if UNICODE is defined, a otherwise
#ifdef UNICODE
#	define IF_UNICODE(u,a) u
#else
#	define IF_UNICODE(u,a) a
#endif

#undef _
/// A string/character constant, correctly handled in unicode builds
#define _(S) IF_UNICODE(BOOST_PP_CAT(L,S), S)

/// The character type used
typedef IF_UNICODE(wchar_t, char) Char;
	
/// Decode a UTF8 string
/** In non-unicode builds the input is considered to be an incorrectly encoded utf8 string.
 *  In unicode builds it is a normal string, utf8 already decoded.
 *  Also removes a byte-order-mark from the start of the string if it is pressent
 */
String decodeUTF8BOM(const String& s);

/// UTF8 Byte order mark for writing at the start of files
/** In non-unicode builds it is UTF8 encoded \xFEFF
*  In unicode builds it is a normal \xFEFF
*/
const Char BYTE_ORDER_MARK[] = IF_UNICODE(L"\xFEFF", "\xEF\xBB\xBF");

/// Writes a string to an output stream, encoded as UTF8
void writeUTF8(wxTextOutputStream& stream, const String& str);

// ----------------------------------------------------------------------------- : Char functions

// Character set tests
inline bool isSpace(Char c) { return IF_UNICODE( iswspace(c) , isspace(c) ); }
inline bool isAlpha(Char c) { return IF_UNICODE( iswalpha(c) , isalpha(c) ); }
inline bool isDigit(Char c) { return IF_UNICODE( iswdigit(c) , isdigit(c) ); }
inline bool isAlnum(Char c) { return IF_UNICODE( iswalnum(c) , isalnum(c) ); }
inline bool isUpper(Char c) { return IF_UNICODE( iswupper(c) , isupper(c) ); }
inline bool isLower(Char c) { return IF_UNICODE( iswlower(c) , islower(c) ); }
// Character conversions
inline Char toUpper(Char c) { return IF_UNICODE( towupper(c) , toupper(c) ); }
inline Char toLower(Char c) { return IF_UNICODE( towlower(c) , tolower(c) ); }

// ----------------------------------------------------------------------------- : String utilities

/// Remove whitespace from both ends of a string
String trim(const String&);

/// Remove whitespace from the start of a string
String trimLeft(const String&);

// ----------------------------------------------------------------------------- : Words

/// Returns the last word in a string
String lastWord(const String&);

/// Remove the last word from a string, leaves whitespace before that word
String stripLastWord(const String&);

// ----------------------------------------------------------------------------- : Caseing

/// Make each word in a string start with an upper case character.
/// for use in menus
String capitalize(const String&);

/// Make the first word in a string start with an upper case character.
/// for use in dialogs
String capitalizeSentence(const String&);

/// Convert a field name to cannocial form: lower case and ' ' instead of '_'
/// non alphanumeric characters are ignored
/// "camalCase" is converted to words "camel case"
String cannocialNameForm(const String&);

// ----------------------------------------------------------------------------- : EOF
#endif
