int main() {
    // Print current time
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <openssl/evp.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

// Cross-platform high-resolution time in nanoseconds
unsigned long long get_nanoseconds() {
#ifdef _WIN32
    FILETIME ft;
    GetSystemTimePreciseAsFileTime(&ft);

    ULARGE_INTEGER uli;
    uli.LowPart  = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;

    // FILETIME = 100ns intervals since Jan 1 1601
    return uli.QuadPart * 100;
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (unsigned long long)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
#endif
}

int main() {
    // Print current time (commented out in your version, but kept)
    time_t currentTime = time(NULL);
    //printf("Current time: %s", ctime(&currentTime));

    // Seed RNG
    srand((unsigned int)currentTime);

    // Get high-resolution time
    unsigned long long ns = get_nanoseconds();

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%llu", ns);

    // --- SHA256 using EVP functions (Modern OpenSSL) ---
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        perror("EVP_MD_CTX_new failed");
        return 1;
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1) {
        perror("EVP_DigestInit_ex failed");
        EVP_MD_CTX_free(ctx);
        return 1;
    }

    if (EVP_DigestUpdate(ctx, buffer, strlen(buffer)) != 1) {
        perror("EVP_DigestUpdate failed");
        EVP_MD_CTX_free(ctx);
        return 1;
    }

    if (EVP_DigestFinal_ex(ctx, hash, &hash_len) != 1) {
        perror("EVP_DigestFinal_ex failed");
        EVP_MD_CTX_free(ctx);
        return 1;
    }

    EVP_MD_CTX_free(ctx);
    // --- End of EVP SHA256 ---

    printf("\n");
    // Change: Print as unsigned decimal integers
    for (unsigned int i = 0; i < hash_len; i++) {
        printf("%u ", hash[i]);
    }
    printf("\n");

    return 0;
