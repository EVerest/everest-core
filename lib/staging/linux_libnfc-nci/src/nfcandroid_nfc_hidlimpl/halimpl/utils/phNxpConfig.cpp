/******************************************************************************
 *
 *  Copyright (C) 2011-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  The original Work has been changed by NXP Semiconductors.
 *
 *  Copyright (C) 2013-2014 NXP Semiconductors
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
 ******************************************************************************/

#include <phNxpConfig.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <list>
#include <sys/stat.h>
#include <string.h>

#include <phNxpLog.h>

#if GENERIC_TARGET
const char alternative_config_path[] = "/data/nfc/";
#else
const char alternative_config_path[] = CONFIG_PATH;
#endif

const char transport_config_path[] = "/etc/";

//#define config_name             "libnfc-nxp.conf"
//#define config_base       "libnfc-nxp-"
#define config_base       "libnfc-nxp"
#define config_ext        ".conf"
//#define config_init       "init"
#define config_init       ""
#define config_pn547      "pn547"
#define config_pn548      "pn548"
#define config_pn557      "pn557"
#define IsStringValue       0x80000000

const char config_timestamp_path[] = "/var/tmp/libnfc-nxp-stamp-h1";

using namespace::std;

class CNxpNfcParam : public string
{
public:
    CNxpNfcParam();
    CNxpNfcParam(const char* name, const string& value);
    CNxpNfcParam(const char* name, unsigned long value);
    virtual ~CNxpNfcParam();
    unsigned long numValue() const {return m_numValue;}
    const char*   str_value() const {return m_str_value.c_str();}
    size_t        str_len() const   {return m_str_value.length();}
private:
    string          m_str_value;
    unsigned long   m_numValue;
};

class CNxpNfcConfig : public vector<const CNxpNfcParam*>
{
public:
    enum{
        NXP_CFG_INIT  = NXP_CONFIG_TYPE_INIT,
		NXP_CFG_PN547 = NXP_CONFIG_TYPE_PN547,
		NXP_CFG_PN548 = NXP_CONFIG_TYPE_PN548,
		NXP_CFG_PN557 = NXP_CONFIG_TYPE_PN557,
		NXP_CFG_UNKNOWN
    };
    virtual ~CNxpNfcConfig();
    static CNxpNfcConfig& GetInstance(unsigned long type = NXP_CFG_INIT);
    friend void readOptionalConfig(const char* optional);
    int updateTimestamp();
    int checkTimestamp();

    bool    getValue(const char* name, char* pValue, size_t len) const;
    bool    getValue(const char* name, unsigned long& rValue) const;
    bool    getValue(const char* name, unsigned short & rValue) const;
    bool    getValue(const char* name, char* pValue, long len,long* readlen) const;
    const CNxpNfcParam*    find(const char* p_name) const;
    void    clean();
private:
    CNxpNfcConfig();
    bool    readConfig(const char* name, bool bResetContent);
    void    moveFromList();
    void    moveToList();
    void    add(const CNxpNfcParam* pParam);
    list<const CNxpNfcParam*> m_list;
    bool    mValidFile;
    unsigned long m_timeStamp;

    unsigned long   state;

    inline bool Is(unsigned long f) {return (state & f) == f;}
    inline void Set(unsigned long f) {state |= f;}
    inline void Reset(unsigned long f) {state &= ~f;}
};

/*******************************************************************************
**
** Function:    isPrintable()
**
** Description: determine if 'c' is printable
**
** Returns:     1, if printable, otherwise 0
**
*******************************************************************************/
inline bool isPrintable(char c)
{
    return  (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
            c == '/' || c == '_' || c == '-' || c == '.';
}

/*******************************************************************************
**
** Function:    isDigit()
**
** Description: determine if 'c' is numeral digit
**
** Returns:     true, if numerical digit
**
*******************************************************************************/
inline bool isDigit(char c, int base)
{
    if ('0' <= c && c <= '9')
        return true;
    if (base == 16)
    {
        if (('A' <= c && c <= 'F') ||
            ('a' <= c && c <= 'f') )
            return true;
    }
    return false;
}

/*******************************************************************************
**
** Function:    getDigitValue()
**
** Description: return numerical value of a decimal or hex char
**
** Returns:     numerical value if decimal or hex char, otherwise 0
**
*******************************************************************************/
inline int getDigitValue(char c, int base)
{
    if ('0' <= c && c <= '9')
        return c - '0';
    if (base == 16)
    {
        if ('A' <= c && c <= 'F')
            return c - 'A' + 10;
        else if ('a' <= c && c <= 'f')
            return c - 'a' + 10;
    }
    return 0;
}

/*******************************************************************************
**
** Function:    CNxpNfcConfig::readConfig()
**
** Description: read Config settings and parse them into a linked list
**              move the element from linked list to a array at the end
**
** Returns:     1, if there are any config data, 0 otherwise
**
*******************************************************************************/
bool CNxpNfcConfig::readConfig(const char* name, bool bResetContent)
{
    enum {
        BEGIN_LINE = 1,
        TOKEN,
        STR_VALUE,
        NUM_VALUE,
        BEGIN_HEX,
        BEGIN_QUOTE,
        END_LINE
    };

    FILE*   fd;
    struct stat buf;
    string  token;
    string  strValue;
    unsigned long    numValue = 0;
    CNxpNfcParam* pParam = NULL;
    int     i = 0;
    int     base = 0;
    char    c;
    int     bflag = 0;
    state = BEGIN_LINE;
    /* open config file, read it into a buffer */
    //printf("\nCNxpNfcConfig::readConfig filename=[%s]\n",name);
    if ((fd = fopen(name, "rb")) == NULL)
    {
        //ALOGE("%s Cannot open config file %s\n", __func__, name);
        if (bResetContent)
        {
            //ALOGE("%s Using default value for all settings\n", __func__);
            mValidFile = false;
        }
        return false;
    }
    stat(name, &buf);
    m_timeStamp = (unsigned long)buf.st_mtime;

    mValidFile = true;
    if (size() > 0)
    {
        if (bResetContent)
        clean();
        else
            moveToList();
    }

    while (!feof(fd) && fread(&c, 1, 1, fd) == 1)
    {
        switch (state & 0xff)
        {
        case BEGIN_LINE:
            if (c == '#')
                state = END_LINE;
            else if (isPrintable(c))
            {
                i = 0;
                token.erase();
                strValue.erase();
                state = TOKEN;
                token.push_back(c);
            }
            break;
        case TOKEN:
            if (c == '=')
            {
                token.push_back('\0');
                state = BEGIN_QUOTE;
            }
            else if (isPrintable(c))
                token.push_back(c);
            else
                state = END_LINE;
            break;
        case BEGIN_QUOTE:
            if (c == '"')
            {
                state = STR_VALUE;
                base = 0;
            }
            else if (c == '0')
                state = BEGIN_HEX;
            else if (isDigit(c, 10))
            {
                state = NUM_VALUE;
                base = 10;
                numValue = getDigitValue(c, base);
                i = 0;
            }
            else if (c == '{')
            {
                state = NUM_VALUE;
                bflag = 1;
                base = 16;
                i = 0;
                Set(IsStringValue);
            }
            else
                state = END_LINE;
            break;
        case BEGIN_HEX:
            if (c == 'x' || c == 'X')
            {
                state = NUM_VALUE;
                base = 16;
                numValue = 0;
                i = 0;
                break;
            }
            else if (isDigit(c, 10))
            {
                state = NUM_VALUE;
                base = 10;
                numValue = getDigitValue(c, base);
                break;
            }
            else if (c != '\n' && c != '\r')
            {
                state = END_LINE;
                break;
            }
            // fall through to numValue to handle numValue

        case NUM_VALUE:
            if (isDigit(c, base))
            {
                numValue *= base;
                numValue += getDigitValue(c, base);
                ++i;
            }
            else if(bflag == 1 && (c == ' ' || c == '\r' || c=='\n' || c=='\t'))
            {
                break;
            }
            else if (base == 16 && (c== ','|| c == ':' || c == '-' || c == ' ' || c == '}'))
            {

                if( c=='}' )
                {
                    bflag = 0;
                }
                if (i > 0)
                {
                    int n = (i+1) / 2;
                    while (n-- > 0)
                    {
                        numValue = numValue >> (n * 8);
                        unsigned char c = (numValue)  & 0xFF;
                        strValue.push_back(c);
                    }
                }

                Set(IsStringValue);
                numValue = 0;
                i = 0;
            }
            else
            {
                if (c == '\n' || c == '\r')
                {
                    if(bflag == 0 )
                    {
                        state = BEGIN_LINE;
                    }
                }
                else
                {
                    if( bflag == 0)
                    {
                        state = END_LINE;
                    }
                }
                if (Is(IsStringValue) && base == 16 && i > 0)
                {
                    int n = (i+1) / 2;
                    while (n-- > 0)
                        strValue.push_back(((numValue >> (n * 8))  & 0xFF));
                }
                if (strValue.length() > 0)
                    pParam = new CNxpNfcParam(token.c_str(), strValue);
                else
                    pParam = new CNxpNfcParam(token.c_str(), numValue);
                add(pParam);
                strValue.erase();
                numValue = 0;
            }
            break;
        case STR_VALUE:
            if (c == '"')
            {
                strValue.push_back('\0');
                state = END_LINE;
                pParam = new CNxpNfcParam(token.c_str(), strValue);
                add(pParam);
            }
            else if (isPrintable(c))
                strValue.push_back(c);
            break;
        case END_LINE:
            if (c == '\n' || c == '\r')
                state = BEGIN_LINE;
            break;
        default:
            break;
        }
    }

    fclose(fd);

    moveFromList();
    return size() > 0;
}

/*******************************************************************************
**
** Function:    CNxpNfcConfig::CNxpNfcConfig()
**
** Description: class constructor
**
** Returns:     none
**
*******************************************************************************/
CNxpNfcConfig::CNxpNfcConfig() :
    mValidFile(true),
    m_timeStamp(0),
    state(0)
{
}

/*******************************************************************************
**
** Function:    CNxpNfcConfig::~CNxpNfcConfig()
**
** Description: class destructor
**
** Returns:     none
**
*******************************************************************************/
CNxpNfcConfig::~CNxpNfcConfig()
{
}

/*******************************************************************************
**
** Function:    CNxpNfcConfig::GetInstance()
**
** Description: get class singleton object
**
** Returns:     none
**
*******************************************************************************/
CNxpNfcConfig& CNxpNfcConfig::GetInstance(unsigned long cfgtype)
{
    static CNxpNfcConfig theInstance;
    static unsigned long current_cfg = NXP_CFG_UNKNOWN;
    static bool hw_cfg_loaded=false;
    bool load_cfg=false, reset_cfg = true;
    bool hw_cfg=false;
    string strPath;
    string cfg_name;

    current_cfg = cfgtype;

    cfg_name.assign(config_base);
    //printf("\nCNxpNfcConfig::GetInstance entry current_cfg=%d NXP_CFG_PN547=%d NXP_CFG_PN548=%d \n",current_cfg,NXP_CFG_PN547,NXP_CFG_PN548);
    switch(current_cfg)
    {
        case NXP_CFG_PN547:
        {
            //printf("\nCNxpNfcConfig::GetInstance PN547\n");
            cfg_name += config_pn547;
            hw_cfg = true;
            break;
        }
        case NXP_CFG_PN548:
        {
            //printf("\nCNxpNfcConfig::GetInstance PN548\n");
            cfg_name += config_pn548;
            hw_cfg = true;
            break;
        }
        case NXP_CFG_PN557:
        {
            //printf("\nCNxpNfcConfig::GetInstance PN557 for PN7160 \n");
            cfg_name += config_pn557;
            hw_cfg = true;
            break;
        }

        case NXP_CFG_INIT:
        default:
        {
            //printf("\nCNxpNfcConfig::GetInstance Default or  INIT\n");
            current_cfg = NXP_CFG_INIT;
            cfg_name += config_init;
            break;
        }
    }

    cfg_name += config_ext;

    if (theInstance.size() == 0 && theInstance.mValidFile)
    {
        load_cfg = true;
        hw_cfg_loaded = false;
        reset_cfg = true;
    }
    else if (true == hw_cfg_loaded)
    {
        ; /* Do Nothing */
    }
    else if (true == hw_cfg)
    {
        load_cfg = true;
        reset_cfg = false;
    }

    if (true == load_cfg)
    {
        int cfg_file_stat = -1;

        if (alternative_config_path[0] != '\0')
        {
            struct stat st;

            strPath.assign(alternative_config_path);
            strPath += cfg_name;
            cfg_file_stat = stat(strPath.c_str(), &st);
        }

        if( cfg_file_stat == -1 )
        {
            strPath.assign(transport_config_path);
            strPath += cfg_name;
        }

#if (NFC_CFG_DEBUG == 0x01)
            /* Since the config file is loaded before reading the configuration,
             * using the log macro may not print the below debug message, hence
             * fprintf used */
        fprintf(stderr, "%s: Reading Config File: %s...\n", __FUNCTION__, strPath.c_str());
#endif
        theInstance.readConfig(strPath.c_str(), reset_cfg);

        if ((!theInstance.empty())
               && (true == hw_cfg))
        {
            hw_cfg_loaded = true;
        }
    }

    return theInstance;
}

/*******************************************************************************
**
** Function:    CNxpNfcConfig::getValue()
**
** Description: get a string value of a setting
**
** Returns:     true if setting exists
**              false if setting does not exist
**
*******************************************************************************/
bool CNxpNfcConfig::getValue(const char* name, char* pValue, size_t len) const
{
    const CNxpNfcParam* pParam = find(name);
    if (pParam == NULL)
        return false;

    if (pParam->str_len() > 0)
    {
        memset(pValue, 0, len);
        memcpy(pValue, pParam->str_value(), pParam->str_len());
        return true;
    }
    return false;
}

bool CNxpNfcConfig::getValue(const char* name, char* pValue, long len,long* readlen) const
{
    const CNxpNfcParam* pParam = find(name);
    if (pParam == NULL)
        return false;

    if (pParam->str_len() > 0)
    {
        if(pParam->str_len() <= (unsigned long)len)
        {
            memset(pValue, 0, len);
            memcpy(pValue, pParam->str_value(), pParam->str_len());
            *readlen = pParam->str_len();
        }
        else
        {
            *readlen = -1;
        }

        return true;
    }
    return false;
}

/*******************************************************************************
**
** Function:    CNxpNfcConfig::getValue()
**
** Description: get a long numerical value of a setting
**
** Returns:     true if setting exists
**              false if setting does not exist
**
*******************************************************************************/
bool CNxpNfcConfig::getValue(const char* name, unsigned long& rValue) const
{
    const CNxpNfcParam* pParam = find(name);
    if (pParam == NULL)
        return false;

    if (pParam->str_len() == 0)
    {
        rValue = static_cast<unsigned long>(pParam->numValue());
        return true;
    }
    return false;
}

/*******************************************************************************
**
** Function:    CNxpNfcConfig::getValue()
**
** Description: get a short numerical value of a setting
**
** Returns:     true if setting exists
**              false if setting does not exist
**
*******************************************************************************/
bool CNxpNfcConfig::getValue(const char* name, unsigned short& rValue) const
{
    const CNxpNfcParam* pParam = find(name);
    if (pParam == NULL)
        return false;

    if (pParam->str_len() == 0)
    {
        rValue = static_cast<unsigned short>(pParam->numValue());
        return true;
    }
    return false;
}

/*******************************************************************************
**
** Function:    CNxpNfcConfig::find()
**
** Description: search if a setting exist in the setting array
**
** Returns:     pointer to the setting object
**
*******************************************************************************/
const CNxpNfcParam* CNxpNfcConfig::find(const char* p_name) const
{
    if (size() == 0)
        return NULL;

    for (const_iterator it = begin(), itEnd = end(); it != itEnd; ++it)
    {
        if (**it < p_name)
        {
            continue;
        }
        else if (**it == p_name)
        {
            if((*it)->str_len() > 0)
            {
                NXPLOG_EXTNS_D("%s found %s\n", __func__, p_name);
            }
            else
            {
                NXPLOG_EXTNS_D("%s found %s=(0x%lx)\n", __func__, p_name, (*it)->numValue());
            }
            return *it;
        }
        else
            break;
    }
    return NULL;
}

/*******************************************************************************
**
** Function:    CNfcConfig::clean()
**
** Description: reset the setting array
**
** Returns:     none
**
*******************************************************************************/
void CNxpNfcConfig::clean()
{
    if (size() == 0)
        return;

    for (iterator it = begin(), itEnd = end(); it != itEnd; ++it)
        delete *it;
    clear();
}

/*******************************************************************************
**
** Function:    CNfcConfig::Add()
**
** Description: add a setting object to the list
**
** Returns:     none
**
*******************************************************************************/
void CNxpNfcConfig::add(const CNxpNfcParam* pParam)
{
    if (m_list.size() == 0)
    {
        m_list.push_back(pParam);
        return;
    }
    for (list<const CNxpNfcParam*>::iterator it = m_list.begin(), itEnd = m_list.end(); it != itEnd; ++it)
    {
        if (**it < pParam->c_str())
            continue;
        m_list.insert(it, pParam);
        return;
    }
    m_list.push_back(pParam);
}

/*******************************************************************************
**
** Function:    CNxpNfcConfig::moveFromList()
**
** Description: move the setting object from list to array
**
** Returns:     none
**
*******************************************************************************/
void CNxpNfcConfig::moveFromList()
{
    if (m_list.size() == 0)
        return;

    for (list<const CNxpNfcParam*>::iterator it = m_list.begin(), itEnd = m_list.end(); it != itEnd; ++it)
        push_back(*it);
    m_list.clear();
}

/*******************************************************************************
**
** Function:    CNxpNfcConfig::moveToList()
**
** Description: move the setting object from array to list
**
** Returns:     none
**
*******************************************************************************/
void CNxpNfcConfig::moveToList()
{
    if (m_list.size() != 0)
        m_list.clear();

    for (iterator it = begin(), itEnd = end(); it != itEnd; ++it)
        m_list.push_back(*it);
    clear();
}

#if 0
/*******************************************************************************
**
** Function:    CNfcConfig::checkTimestamp()
**
** Description: check if config file has modified
**
** Returns:     0 if not modified, 1 otherwise.
**
*******************************************************************************/
int CNfcConfig::checkTimestamp()
{
    FILE*   fd;
    struct stat st;
    unsigned long value = 0;
    int ret = 0;

    if(stat(config_timestamp_path, &st) != 0)
    {
        ALOGD("%s file %s not exist, creat it.\n", __func__, config_timestamp_path);
        if ((fd = fopen(config_timestamp_path, "w+")) != NULL)
        {
            fwrite(&m_timeStamp, sizeof(unsigned long), 1, fd);
            fclose(fd);
        }
        return 1;
    }
    else
    {
        fd = fopen(config_timestamp_path, "r+");
        if(fd == NULL)
        {
            ALOGE("%s Cannot open file %s\n", __func__, config_timestamp_path);
            return 1;
        }

        fread(&value, sizeof(unsigned long), 1, fd);
        ret = (value != m_timeStamp);
        if(ret)
        {
            fseek(fd, 0, SEEK_SET);
            fwrite(&m_timeStamp, sizeof(unsigned long), 1, fd);
        }
        fclose(fd);
    }
    return ret;
}
#else
/*******************************************************************************
**
** Function:    CNxpNfcConfig::checkforTimestamp()
**
** Description: check if config file has modified
**
** Returns:     0 if not modified, 1 otherwise.
**
*******************************************************************************/
int CNxpNfcConfig::checkTimestamp()
{
    FILE*   fd;
    struct stat st;
    unsigned long value = 0;
    int ret = 0;

    if(stat(config_timestamp_path, &st) != 0)
    {
        return 1;
    }
    else
    {
        fd = fopen(config_timestamp_path, "r+");
        if(fd == NULL)
        {
            return 1;
        }

        fread(&value, sizeof(unsigned long), 1, fd);
        ret = (value != m_timeStamp);
        fclose(fd);
    }
    return ret;
}

#endif

/*******************************************************************************
**
** Function:    CNxpNfcConfig::updateTimestamp()
**
** Description: update if config file has modified
**
** Returns:     0 if not modified, 1 otherwise.
**
*******************************************************************************/
int CNxpNfcConfig::updateTimestamp()
{
    FILE*   fd;
    struct stat st;
    unsigned long value = 0;
    int ret = 0;

    if(stat(config_timestamp_path, &st) != 0)
    {
        if ((fd = fopen(config_timestamp_path, "w+")) != NULL)
        {
            fwrite(&m_timeStamp, sizeof(unsigned long), 1, fd);
            fclose(fd);
        }
        return 1;
    }
    else
    {
        fd = fopen(config_timestamp_path, "r+");
        if(fd == NULL)
        {
            return 1;
        }

        fread(&value, sizeof(unsigned long), 1, fd);
        ret = (value != m_timeStamp);
        if(ret)
        {
            fseek(fd, 0, SEEK_SET);
            fwrite(&m_timeStamp, sizeof(unsigned long), 1, fd);
        }
        fclose(fd);
    }
    return ret;
}

/*******************************************************************************
**
** Function:    CNxpNfcParam::CNxpNfcParam()
**
** Description: class constructor
**
** Returns:     none
**
*******************************************************************************/
CNxpNfcParam::CNxpNfcParam() :
    m_numValue(0)
{
}

/*******************************************************************************
**
** Function:    CNxpNfcParam::~CNxpNfcParam()
**
** Description: class destructor
**
** Returns:     none
**
*******************************************************************************/
CNxpNfcParam::~CNxpNfcParam()
{
}

/*******************************************************************************
**
** Function:    CNxpNfcParam::CNxpNfcParam()
**
** Description: class copy constructor
**
** Returns:     none
**
*******************************************************************************/
CNxpNfcParam::CNxpNfcParam(const char* name,  const string& value) :
    string(name),
    m_str_value(value),
    m_numValue(0)
{
}

/*******************************************************************************
**
** Function:    CNxpNfcParam::CNxpNfcParam()
**
** Description: class copy constructor
**
** Returns:     none
**
*******************************************************************************/
CNxpNfcParam::CNxpNfcParam(const char* name,  unsigned long value) :
    string(name),
    m_numValue(value)
{
}

/*******************************************************************************
**
** Function:    GetStrValue
**
** Description: API function for getting a string value of a setting
**
** Returns:     True if found, otherwise False.
**
*******************************************************************************/
extern "C" int GetNxpStrValue(const char* name, char* pValue, unsigned long len)
{
    CNxpNfcConfig& rConfig = CNxpNfcConfig::GetInstance();
    bool val_status = rConfig.getValue(name, pValue, len);
    NXPLOG_EXTNS_D("%s: NXP Config Parameter : %s\n", __FUNCTION__, name);
    return val_status;
}

/*******************************************************************************
**
** Function:    GetByteArrayValue()
**
** Description: Read byte array value from the config file.
**
** Parameters:
**              name    - name of the config param to read.
**              pValue  - pointer to input buffer.
**              bufflen - input buffer length.
**              len     - out parameter to return the number of bytes read from config file,
**                        return -1 in case bufflen is not enough.
**
** Returns:     TRUE[1] if config param name is found in the config file, else FALSE[0]
**
*******************************************************************************/
extern "C" int GetNxpByteArrayValue(const char* name, char* pValue,long bufflen, long *len)
{
    CNxpNfcConfig& rConfig = CNxpNfcConfig::GetInstance();
    bool val_status = rConfig.getValue(name, pValue, bufflen,len);
    NXPLOG_EXTNS_D("%s: NXP Config Parameter : %s\n", __FUNCTION__, name);
    return val_status;
}

/*******************************************************************************
**
** Function:    GetNumValue
**
** Description: API function for getting a numerical value of a setting
**
** Returns:     true, if successful
**
*******************************************************************************/
extern "C" int GetNxpNumValue(const char* name, void* pValue, unsigned long len)
{
    if (!pValue)
        return false;
    CNxpNfcConfig& rConfig = CNxpNfcConfig::GetInstance();
    const CNxpNfcParam* pParam = rConfig.find(name);

    if (pParam == NULL)
        return false;
    unsigned long v = pParam->numValue();
    if (v == 0 && pParam->str_len() > 0 && pParam->str_len() < 4)
    {
        const unsigned char* p = (const unsigned char*)pParam->str_value();
        for (unsigned int i = 0 ; i < pParam->str_len(); ++i)
        {
            v *= 256;
            v += *p++;
        }
    }

    NXPLOG_EXTNS_D("%s: NXP Config Parameter : %s=(0x%x)\n", __FUNCTION__, name, v);

    switch (len)
    {
    case sizeof(unsigned long):
        *(static_cast<unsigned long*>(pValue)) = (unsigned long)v;
        break;
    case sizeof(unsigned short):
        *(static_cast<unsigned short*>(pValue)) = (unsigned short)v;
        break;
    case sizeof(unsigned char):
        *(static_cast<unsigned char*> (pValue)) = (unsigned char)v;
        break;
    default:
        return false;
    }
    return true;
}

/*******************************************************************************
**
** Function:    resetConfig
**
** Description: reset settings array
**
** Returns:     none
**
*******************************************************************************/
extern "C" void resetNxpConfig()

{
    CNxpNfcConfig& rConfig = CNxpNfcConfig::GetInstance();

    rConfig.clean();
}

/*******************************************************************************
**
** Function:    readOptionalConfig()
**
** Description: read Config settings from an optional conf file
**
** Returns:     none
**
*******************************************************************************/
#if 0
void readOptionalConfig(const char* extra)
{
    string strPath;
    strPath.assign(transport_config_path);
    if (alternative_config_path[0] != '\0')
        strPath.assign(alternative_config_path);

    strPath += extra_config_base;
    strPath += extra;
    strPath += extra_config_ext;
    CNxpNfcConfig::GetInstance().readConfig(strPath.c_str(), false);
}
#endif

/*******************************************************************************
**
** Function:    isNxpConfigModified()
**
** Description: check if config file has modified
**
** Returns:     0 if not modified, 1 otherwise.
**
*******************************************************************************/
extern "C" int isNxpConfigModified()
{
    CNxpNfcConfig& rConfig = CNxpNfcConfig::GetInstance();
    return rConfig.checkTimestamp();
}

/*******************************************************************************
**
** Function:    updateNxpConfigTimestamp()
**
** Description: update if config file has modified
**
** Returns:     0 if not modified, 1 otherwise.
**
*******************************************************************************/
extern "C" int updateNxpConfigTimestamp()
{
    CNxpNfcConfig& rConfig = CNxpNfcConfig::GetInstance();
    return rConfig.updateTimestamp();
}

/*******************************************************************************
**
** Function:    isNxpConfigValid()
**
** Description: check if config file exists
**
** Returns:     1 if exists, 0 otherwise.
**
*******************************************************************************/
extern "C" int isNxpConfigValid(unsigned long type)
{
    CNxpNfcConfig& rConfig = CNxpNfcConfig::GetInstance(type);
    return (rConfig.size() != 0);
}
