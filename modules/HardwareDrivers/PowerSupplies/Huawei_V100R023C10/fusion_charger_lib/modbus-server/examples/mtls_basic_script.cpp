// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

static void client();
static void server();

#define ASS_NOT_NULL(exp)                                                                                              \
    {                                                                                                                  \
        if (!exp) {                                                                                                    \
            printf(#exp " is NULL!\n");                                                                                \
            exit(EXIT_FAILURE);                                                                                        \
        }                                                                                                              \
    }

#define ASS_POS(exp)                                                                                                   \
    {                                                                                                                  \
        if (exp <= 0) {                                                                                                \
            printf(#exp " is not positive\n");                                                                         \
            exit(EXIT_FAILURE);                                                                                        \
        }                                                                                                              \
    }

#define ASS_LIN_SOCK(exp)                                                                                              \
    if (exp < 0) {                                                                                                     \
        printf(#exp "Returned something negative\n");                                                                  \
        exit(EXIT_FAILURE);                                                                                            \
    }

#define ASS_SSL(ssl, exp)                                                                                              \
    {                                                                                                                  \
        int ret = exp;                                                                                                 \
        if (ret <= 0) {                                                                                                \
            printf(#exp " returned an error (%d): %s\n", SSL_get_error(ssl, ret),                                      \
                   ERR_error_string(SSL_get_error(ssl, ret), NULL));                                                   \
            SSL_shutdown(ssl);                                                                                         \
            SSL_free(ssl);                                                                                             \
            exit(EXIT_FAILURE);                                                                                        \
        }                                                                                                              \
    }

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s [client|server]\n", argv[0]);
        return 1;
    }

    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();

    switch (argv[1][0]) {
    case 'c':
        client();
        break;
    case 's':
        server();
        break;
    }
    return 0;
}

#define CA_BASE_NAME     "../testdata/ca"
#define CLIENT_BASE_NAME "../testdata/client"
#define SERVER_BASE_NAME "../testdata/server"
#define KEYF             ".key.pem"
#define CERTF            ".crt.pem"

static void set_ca(SSL_CTX* ctx) {
    ASS_POS(SSL_CTX_use_certificate_chain_file(ctx, CA_BASE_NAME CERTF));
    ASS_POS(SSL_CTX_load_verify_locations(ctx, CA_BASE_NAME CERTF, NULL));
}

static void keylog(const SSL* ssl, const char* line) {
    char* file_name = getenv("SSLKEYLOGFILE");
    if (file_name == NULL) {
        return;
    }

    int file = open(file_name, O_WRONLY | O_APPEND);
    if (file < 0) {
        printf("Could not open keylog %d\n", errno);
        return;
    }

    int err = write(file, line, strlen(line));
    if (err < 0) {
        printf("Could not write keylog %d", errno);
        close(file);
        return;
    }

    write(file, "\n", 1);

    close(file);
}

static void server() {
    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
    ASS_NOT_NULL(ctx);

    SSL_CTX_set_keylog_callback(ctx, keylog);

    set_ca(ctx);
    ASS_POS(SSL_CTX_use_certificate_file(ctx, SERVER_BASE_NAME CERTF, SSL_FILETYPE_PEM));
    ASS_POS(SSL_CTX_use_PrivateKey_file(ctx, SERVER_BASE_NAME KEYF, SSL_FILETYPE_PEM));

    int ret = SSL_CTX_check_private_key(ctx);
    if (ret != 1) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT | SSL_VERIFY_CLIENT_ONCE, NULL);
    SSL_CTX_set_verify_depth(ctx, 10);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    ASS_LIN_SOCK(sock);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8008);

    ASS_LIN_SOCK(bind(sock, (struct sockaddr*)&addr, sizeof(addr)));
    ASS_LIN_SOCK(listen(sock, 5));
    printf("Waiting for client\n");
    int client_sock = accept(sock, 0, 0);
    ASS_LIN_SOCK(client_sock);
    close(sock); // server sock not needed anymore yay

    printf("TCP Connected\n");

    auto ssl = SSL_new(ctx);
    ASS_NOT_NULL(ssl);

    ASS_POS(SSL_set_fd(ssl, client_sock));

    ASS_SSL(ssl, SSL_accept(ssl));
    printf("TLS Connected\n");

    if (SSL_get_verify_result(ssl) != X509_V_OK) {
        printf("Error: SSL verification failed.\n");
        SSL_free(ssl);
        close(sock);
        return;
    }

    printf("SSL connection using %s \n", SSL_get_cipher(ssl));

    auto client_cert = SSL_get_peer_certificate(ssl);
    if (client_cert == NULL) {
        printf("Error: Could not get client's certificate.\n");
    } else {
        printf("Successfully loaded client's certificate\n");
    }
    X509_free(client_cert);

    printf("reading 13 bytes...\n");

    char buf[13];
    int r = SSL_read(ssl, buf, sizeof(buf));
    if (r) {
        printf("%s", buf);
    } else {
        printf("Could not read lol");
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);

    close(client_sock);
}

static void client() {
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    ASS_NOT_NULL(ctx);

    set_ca(ctx);
    ASS_POS(SSL_CTX_use_certificate_file(ctx, CLIENT_BASE_NAME CERTF, SSL_FILETYPE_PEM));
    ASS_POS(SSL_CTX_use_PrivateKey_file(ctx, CLIENT_BASE_NAME KEYF, SSL_FILETYPE_PEM));

    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "Private key invalid\n");
        exit(EXIT_FAILURE);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8008);

    printf("Connecting...\n");
    ASS_LIN_SOCK(connect(sock, (struct sockaddr*)&addr, sizeof(addr)));
    printf("TCP Connected\n");

    SSL* ssl = SSL_new(ctx);
    ASS_NOT_NULL(ssl);

    ASS_POS(SSL_set_fd(ssl, sock));

    ASS_SSL(ssl, SSL_connect(ssl));
    printf("TLS Connected\n");

    if (SSL_get_verify_result(ssl) != X509_V_OK) {
        printf("Error: SSL verification failed.\n");
        SSL_free(ssl);
        close(sock);
        return;
    }

    auto client_cert = SSL_get_peer_certificate(ssl);
    if (client_cert == NULL) {
        printf("Error: Could not get a certificate from server.\n");
    } else {
        printf("Successfully loaded server certificate\n");
    }
    X509_free(client_cert);

    const char* reply = "Client I am\n";
    SSL_write(ssl, reply, strlen(reply) + 1);

    sleep(1);

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
}
