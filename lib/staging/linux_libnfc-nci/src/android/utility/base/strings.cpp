/******************************************************************************
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*  Copyright 2018 NXP
*
******************************************************************************/
#include <iostream>
#include <android-base/strings.h>
//#include "metricslogger/metrics_logger.h"
#include<cstring>
std::vector<std::string> android::base::Split(const std::string& s, const std::string& delimiters) {
	std::vector<std::string>result;
	std::string str(s);
	std::string  token(delimiters);
	while (str.size()) {
		size_t index = str.find(token);
		if (index != std::string::npos) {
			result.push_back(str.substr(0, index));
			str = str.substr(index + token.size());
			if (str.size() == 0)result.push_back(str);
		}
		else {
			result.push_back(str);
			str = "";
		}
	}
	return result;
}
#if 1
//Trim leading and traillin whitespaces and trailing carriage return
std::string android::base::Trim(const std::string& str)
{
	std::string s(str);
	size_t p = s.find_first_not_of(" \t");
	s.erase(0, p);

	p = s.find_last_not_of(" \t");
	if (std::string::npos != p)
		s.erase(p + 1);
	//trim carriage return i.e \r
	p = s.find_last_not_of(" \r");
	if (std::string::npos != p)
		s.erase(p + 1);
	else
		s.erase(p + 1);
	std::string result(s);
	return result;
}
#endif

//To remove dependency on metricslogger metricslogger.cpp
namespace android {
    namespace metricslogger {
        void LogCounter(const std::string& name, int32_t val) {
            std::cout << "ERROR: " << name << std::endl;
        }
    }  // namespace metricslogger
}  // namespace android

/*****************************************************************************************
 * Function     : strlcpy
 * Arguements   : destination - Pointer to location where a string has to be copied
 *                Source - Pointer to string location which has to be copied
 *                size   - Size of the destination buffer
 * Description  : In computer programming, the strlcpy function is intended to replace
 *              the function strcpy (which copies a string to a destination buffer)
 *              with a secure version that cannot overflow the destination buffer.
 *                This is not C standard library functions, but is available in the
 *              libraries on several Unix operating systems, including BSD, Mac OS X,
 *              Solaris, Android and IRIX, with notable exception of glibc on Linux,
 *                The strlcpy() function copies up to size - 1 characters from the
 *              NUL-terminated string src to dst, NUL-terminating the result.
 * Returns      : The total length of the string it tried to create.
 * Link   : https://en.wikibooks.org/wiki/C_Programming/C_Reference/nonstandard/strlcpy
 ****************************************************************************************/
size_t strlcpy(char *destination, const char *source, size_t size) {
    if (size > 0) {
      if (size > strlen(source))
          size = strlen(source); /* Copy only valid string literals */
      else /* Avoid possible buffer overflow */
          size -= 1; /* Destination size - 1 */
      for (auto i = 0; i < size; i++)
          *destination++ = *source++;
      *destination = '\0';
    }
   return size;
}
/*****************************************************************************************
* Function     : strlcat
* Arguements   : destination - Pointer to string which has to be concatinated
*                Source - Pointer to source string
*                size   - Size of the destination string
* Description  : Concatenate characters from src to dest and nul-terminate the resulting
*              string. As much of src is copied into dest as there is space for.
*                For more information please refer below link
*                http://www.delorie.com/djgpp/doc/libc/libc_762.html
* Returns      : The length of the string that strlcat tried to create is returned, whether
*              or not strlcat could store it in dest. If all of src was concatenated to dst,
*              the return value will be less than size.
*                If dest is not nul-terminated, then strlcat will consider dest to be size
*              in length and return size plus the length of src.
****************************************************************************************/
size_t  strlcat(char *destination, const char *source, size_t size)
{
    size_t  len;
    size_t  slen;

    len = 0;
    slen = strlen(source);
    while (*destination && size > 0)
    { /* walk through the dest string till NUL char */
        len++;
        destination++;
        size--;
    }
    while (*source && size-- > 1)
        *destination++ = *source++; // Logic of dennis ritchie
    if (size == 1 || *source == 0)
        *destination = '\0';
    return (slen + len);
}
