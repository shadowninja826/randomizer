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

// Function Prototypes
void print_help(void);
unsigned long long get_nanoseconds(void);

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
    // Use CLOCK_REALTIME on systems without CLOCK_MONOTONIC_RAW (or similar high-res)
    // CLOCK_REALTIME is generally sufficient for entropy seeding
    clock_gettime(CLOCK_REALTIME, &ts);
    return (unsigned long long)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
#endif
}

// Function to print help instructions
void print_help(void) {
    printf("\n");
    printf("Randomizer: SHA-256 Hash-based Seed Generator\n");
    printf("---------------------------------------------\n");
    printf("This program generates a high-entropy seed by hashing the current time in nanoseconds.\n");
    printf("\n");
    printf("Usage:\n");
    printf("  ./randomizer [OPTIONS]\n");
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help      Display this help message and exit.\n");
    printf("  -n, --nospace        Output the decimal bytes as a single, contiguous number (no spaces).\n");
    printf("\n");
    printf("Default Output:\n");
    printf("  32 space-separated decimal integers (0-255).\n");
    printf("Example: 201 153 63 17 144 ...\n");
    printf("\n");
}


int main(int argc, char *argv[]) {
    // Determine if spaces should be omitted (default is 1 for space, 0 for nospace)
    int use_space = 1; 

    // Command-line argument parsing
    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_help();
            return 0; // Exit after displaying help
        }
        if (strcmp(argv[1], "-n") == 0 || strcmp(argv[1], "--nospace") == 0) {
            use_space = 0; // Set flag to omit spaces
        }
    }
    
    // --- Program Logic Starts Here ---

    // Seed RNG (optional, but kept from original)
    time_t currentTime = time(NULL);
    srand((unsigned int)currentTime);

    // Get high-resolution time
    unsigned long long ns = get_nanoseconds();

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%llu", ns);

    // SHA256 using EVP functions
    unsigned char hash[EVP_MAX_MD_SIZE]; 
    unsigned int hash_len;

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        perror("EVP_MD_CTX_new failed");
        return 1;
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1 ||
        EVP_DigestUpdate(ctx, buffer, strlen(buffer)) != 1 ||
        EVP_DigestFinal_ex(ctx, hash, &hash_len) != 1) 
    {
        perror("SHA256 digest operation failed");
        EVP_MD_CTX_free(ctx);
        return 1;
    }

    EVP_MD_CTX_free(ctx);
    // --- End of EVP SHA256 ---

    printf("\n");
    // Print the hash bytes as decimal integers
    for (unsigned int i = 0; i < hash_len; i++) {
        printf("%u", hash[i]); 
        // Print a space only if the use_space flag is set AND it's not the last byte
        if (use_space && i < hash_len - 1) {
            printf(" ");
        }
    }
    printf("\n");

    return 0;
}
