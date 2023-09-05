//
//  base64 encoding and decoding with C++.
//  Version: 2.rc.09 (release candidate)
//

#ifndef __T3D_BASE64_H__
#define __T3D_BASE64_H__


#include "T3DUtilsPrerequisites.h"

#if __cplusplus >= 201703L
#include <string_view>
#endif  // __cplusplus >= 201703L

namespace Tiny3D
{
    T3D_UTILS_API String base64_encode     (String const& s, bool url = false);
    T3D_UTILS_API String base64_encode_pem (String const& s);
    T3D_UTILS_API String base64_encode_mime(String const& s);

    T3D_UTILS_API String base64_decode(String const& s, bool remove_linebreaks = false);
    T3D_UTILS_API String base64_encode(unsigned char const*, size_t len, bool url = false);
    
#if __cplusplus >= 201703L
    //
    // Interface with std::string_view rather than const std::string&
    // Requires C++17
    // Provided by Yannic Bonenberger (https://github.com/Yannic)
    //
    T3D_UTILS_API String base64_encode     (std::string_view s, bool url = false);
    T3D_UTILS_API String base64_encode_pem (std::string_view s);
    T3D_UTILS_API String base64_encode_mime(std::string_view s);

    T3D_UTILS_API String base64_decode(std::string_view s, bool remove_linebreaks = false);
#endif  // __cplusplus >= 201703L
}


#endif  /*__T3D_BASE64_H__*/
