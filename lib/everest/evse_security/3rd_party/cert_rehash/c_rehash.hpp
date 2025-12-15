/* c_rehash.c - Create hash symlinks for certificates
 * C implementation based on the original Perl and shell versions
 *
 * Copyright (c) 2013-2014 Timo Teräs <timo.teras@iki.fi>
 * All rights reserved.
 *
 * This software is licensed under the MIT License.
 * Full license available at: http://opensource.org/licenses/MIT
 */

#include <array>
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

#include <everest/logging.hpp>

constexpr auto MAX_COLLISIONS = 256;

namespace evse_security {

struct entry_info {
    struct entry_info* next;
    char* filename;
    unsigned short old_id;
    unsigned char need_symlink;
    std::array<unsigned char, EVP_MAX_MD_SIZE> digest;
};

struct bucket_info {
    struct bucket_info* next;
    struct entry_info *first_entry, *last_entry;
    unsigned int hash;
    unsigned short type;
    unsigned short num_needed;
};

enum Type {
    TYPE_CERT = 0,
    TYPE_CRL
};

static const std::array<const char*, 2> symlink_extensions = {"", "r"};
static const std::array<const char*, 4> file_extensions = {"pem", "crt", "cer", "crl"};

static int evpmdsize;
static const EVP_MD* evpmd;

static std::array<struct bucket_info*, 257> hash_table;

static void bit_set(unsigned char* set, unsigned bit) {
    set[bit / 8] |= 1 << (bit % 8);
}

static int bit_isset(unsigned char* set, unsigned bit) {
    return set[bit / 8] & (1 << (bit % 8));
}

static void add_entry(int type, unsigned int hash, const char* filename, const unsigned char* digest, int need_symlink,
                      unsigned short old_id) {
    struct bucket_info* bi = nullptr;
    struct entry_info* ei = nullptr;
    struct entry_info* found = nullptr;
    const unsigned int ndx = (type + hash) % hash_table.size();

    for (bi = hash_table.at(ndx); bi != nullptr; bi = bi->next) {
        if (bi->type == type && bi->hash == hash) {
            break;
        }
    }
    if (bi == nullptr) {
        bi = static_cast<bucket_info*>((calloc(1, sizeof(*bi))));
        if (bi == nullptr) {
            return;
        }
        bi->next = hash_table.at(ndx);
        bi->type = type;
        bi->hash = hash;
        hash_table.at(ndx) = bi;
    }

    for (ei = bi->first_entry; ei != nullptr; ei = ei->next) {
        if ((digest != nullptr) && memcmp(digest, ei->digest.data(), evpmdsize) == 0) {
            EVLOG_warning << "Skipping duplicate certificate in file " << std::string(filename);
            return;
        }
        if (strcmp(filename, ei->filename) == 0) {
            found = ei;
            if (digest == nullptr) {
                break;
            }
        }
    }
    ei = found;
    if (ei == nullptr) {
        if (bi->num_needed >= MAX_COLLISIONS) {
            return;
        }
        ei = static_cast<entry_info*>(calloc(1, sizeof(*ei)));
        if (ei == nullptr) {
            return;
        }

        ei->old_id = ~0;
        ei->filename = strdup(filename);
        if (bi->last_entry != nullptr) {
            bi->last_entry->next = ei;
        }
        if (bi->first_entry == nullptr) {
            bi->first_entry = ei;
        }
        bi->last_entry = ei;
    }

    if (old_id < ei->old_id) {
        ei->old_id = old_id;
    }
    if ((need_symlink != 0) && (ei->need_symlink == 0U)) {
        ei->need_symlink = 1;
        bi->num_needed++;
        memcpy(ei->digest.data(), digest, evpmdsize);
    }
}

static int handle_symlink(const char* filename, const char* fullpath) {
    static std::array<const signed char, 55> xdigit = {
        0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15};
    std::array<char, NAME_MAX> linktarget;
    char* endptr = nullptr; // NOLINT(misc-const-correctness): would not work with call to strtoul
    unsigned int hash = 0;
    unsigned char ch = 0;
    int i = 0;
    int type = 0;
    int id = 0;
    ssize_t n = 0;

    for (i = 0; i < 8; i++) {
        ch = filename[i] - '0';
        if (ch >= xdigit.size() || xdigit.at(ch) < 0) {
            return -1;
        }
        hash <<= 4;
        hash += xdigit.at(ch);
    }
    if (filename[i++] != '.') {
        return -1;
    }
    for (type = symlink_extensions.size() - 1; type > 0; type--) {
        if (strcasecmp(symlink_extensions.at(type), &filename[i]) == 0) {
            break;
        }
    }
    i += strlen(symlink_extensions.at(type));

    id = strtoul(&filename[i], &endptr, 10);
    if (*endptr != 0) {
        return -1;
    }

    auto linktarget_size = linktarget.size() * sizeof(decltype(linktarget)::value_type);
    n = readlink(fullpath, linktarget.data(), linktarget_size);
    if (n >= linktarget_size || n < 0) {
        return -1;
    }
    linktarget.at(n) = 0;

    EVLOG_debug << "Found existing symlink " << std::string(filename) << " for " << hash << " (" << type
                << "), certname " << std::string(linktarget.data(), strlen(linktarget.data()));
    add_entry(type, hash, linktarget.data(), nullptr, 0, id);
    return 0;
}

static int handle_certificate(const char* filename, const char* fullpath) {
    STACK_OF(X509_INFO)* inf = nullptr;
    const X509_INFO* x = nullptr;
    BIO* b = nullptr;
    const char* ext = nullptr;
    std::array<unsigned char, EVP_MAX_MD_SIZE> digest;
    const X509_NAME* name = nullptr;
    int i = 0;
    int type = 0;
    const int ret = -1;

    ext = strrchr(filename, '.');
    if (ext == nullptr) {
        return 0;
    }
    for (i = 0; i < file_extensions.size(); i++) {
        if (strcasecmp(file_extensions.at(i), ext + 1) == 0) {
            break;
        }
    }
    if (i >= file_extensions.size()) {
        return -1;
    }

    b = BIO_new_file(fullpath, "r");
    if (b == nullptr) {
        return -1;
    }
    inf = PEM_X509_INFO_read_bio(b, nullptr, nullptr, nullptr);
    BIO_free(b);
    if (inf == nullptr) {
        return -1;
    }

    if (sk_X509_INFO_num(inf) == 1) {
        x = sk_X509_INFO_value(inf, 0);
        if (x->x509 != nullptr) {
            type = TYPE_CERT;
            name = X509_get_subject_name(x->x509);
            X509_digest(x->x509, evpmd, digest.data(), nullptr);
        } else if (x->crl != nullptr) {
            type = TYPE_CRL;
            name = X509_CRL_get_issuer(x->crl);
            X509_CRL_digest(x->crl, evpmd, digest.data(), nullptr);
        }
        if (name != nullptr) {
            add_entry(type, X509_NAME_hash(name), filename, digest.data(), 1, ~0);
        }
    } else {
        EVLOG_warning << std::string(filename) << " does not contain exactly one certificate or CRL: skipping";
    }

    sk_X509_INFO_pop_free(inf, X509_INFO_free);

    return ret;
}

static int hash_dir(const char* dirname) {
    struct bucket_info* bi = nullptr;
    struct bucket_info* nextbi = nullptr;
    struct entry_info* ei = nullptr;
    struct entry_info* nextei = nullptr;
    struct dirent* de = nullptr;
    struct stat st;
    std::array<unsigned char, MAX_COLLISIONS / 8> idmask;
    int i = 0;
    int n = 0;
    int nextid = 0;
    int buflen = 0;
    int ret = -1;
    const char* pathsep = nullptr;
    char* buf = nullptr;
    DIR* d = nullptr;

    evpmd = EVP_sha1();
    evpmdsize = EVP_MD_size(evpmd);

    if (access(dirname, R_OK | W_OK | X_OK) != 0) {
        EVLOG_error << "Access denied '" << std::string(dirname) << "'";
        return -1;
    }

    buflen = strlen(dirname);
    pathsep = ((buflen != 0) && dirname[buflen - 1] == '/') ? "" : "/";
    buflen += NAME_MAX + 2;
    buf = static_cast<char*>(malloc(buflen));
    if (buf == nullptr) {
        goto err;
    }

    EVLOG_debug << "Doing " << std::string(dirname);
    d = opendir(dirname);
    if (d == nullptr) {
        goto err;
    }

    while ((de = readdir(d)) != nullptr) {
        if (snprintf(buf, buflen, "%s%s%s", dirname, pathsep, de->d_name) >= buflen) {
            continue;
        }
        if (lstat(buf, &st) < 0) {
            continue;
        }
        if (S_ISLNK(st.st_mode) && handle_symlink(de->d_name, buf) == 0) {
            continue;
        }
        if (strcmp(buf, "/etc/ssl/certs/ca-certificates.crt") == 0) {
            /* Ignore the /etc/ssl/certs/ca-certificates.crt file */
            EVLOG_debug << "Skipping /etc/ssl/certs/ca-certificates.crt file";
            continue;
        }
        handle_certificate(de->d_name, buf);
    }
    closedir(d);

    for (i = 0; i < hash_table.size(); i++) {
        for (bi = hash_table.at(i); bi != nullptr; bi = nextbi) {
            nextbi = bi->next;
            EVLOG_debug << "Type " << bi->type << " hash " << bi->hash << " num entries " << bi->num_needed << ":";

            nextid = 0;
            memset(idmask.data(), 0, (bi->num_needed + 7) / 8);
            for (ei = bi->first_entry; ei != nullptr; ei = ei->next) {
                if (ei->old_id < bi->num_needed) {
                    bit_set(idmask.data(), ei->old_id);
                }
            }

            for (ei = bi->first_entry; ei != nullptr; ei = nextei) {
                nextei = ei->next;
                EVLOG_debug << "\t(old_id " << ei->old_id << ", need_symlink " << static_cast<int>(ei->need_symlink)
                            << ") Cert " << std::string(ei->filename, strlen(ei->filename)) << ":";

                if (ei->old_id < bi->num_needed) {
                    /* Link exists, and is used as-is */
                    if (snprintf(buf, buflen, "%08x.%s%d", bi->hash, symlink_extensions.at(bi->type), ei->old_id) < 0) {
                        return -1;
                    }
                    EVLOG_debug << "link " << std::string(ei->filename, strlen(ei->filename)) << " -> "
                                << std::string(buf, strlen(buf));
                } else if (ei->need_symlink != 0U) {
                    /* New link needed (it may replace something) */
                    while (bit_isset(idmask.data(), nextid) != 0) {
                        nextid++;
                    }

                    if (snprintf(buf, buflen, "%s%s%n%08x.%s%d", dirname, pathsep, &n, bi->hash,
                                 symlink_extensions.at(bi->type), nextid) < 0) {
                        return -1;
                    };
                    EVLOG_debug << "link " << std::string(ei->filename, strlen(ei->filename)) << " -> "
                                << std::string(buf + n, strlen(buf + n));
                    unlink(buf);
                    symlink(ei->filename, buf);
                } else {
                    /* Link to be deleted */
                    if (snprintf(buf, buflen, "%s%s%n%08x.%s%d", dirname, pathsep, &n, bi->hash,
                                 symlink_extensions.at(bi->type), ei->old_id) < 0) {
                        return -1;
                    };
                    EVLOG_debug << "unlink " << std::string(buf + n, strlen(buf + n));
                    unlink(buf);
                }
                free(ei->filename);
                free(ei);
            }
            free(bi);
        }
        hash_table.at(i) = nullptr;
    }

    ret = 0;
err:
    free(buf);
    return ret;
}

} // namespace evse_security