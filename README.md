# 🎲 High-Entropy Randomizer (randomizer)


A simple C utility that generates a highly unpredictable seed by hashing the current system time at nanosecond precision using the cryptographic **SHA-256** algorithm. The resulting 32-byte hash is outputted as a sequence of space-separated **decimal integers** (0-255).

## 🚀 Why Use This?

Standard C `rand()` functions often use low-entropy seeds like `time(NULL)`. This program is designed to create a much more unpredictable, high-quality seed by leveraging:

1.  **Nanosecond Precision:** Capturing the full time value (including the nanosecond component, not just seconds) to capture minute, difficult-to-predict variations in system timing.
2.  **Cryptographic Hashing (SHA-256):** Ensuring even tiny changes in the input (time) result in a drastically different, well-distributed 32-byte output.
3.  **Decimal Output:** The 32 decimal values are easy to parse and use as a source of randomness in other scripts or applications.

## 🛠️ Installation & Setup

This program requires the **GCC compiler** and the **OpenSSL Development Libraries** (version 3.0+ recommended).

### Prerequisites (Debian/Ubuntu)

Use the following command to install the required dependencies:

```
sudo apt update && sudo apt install build-essential libssl-dev -y
```
### Build the Program
Navigate to the project directory and compile using GCC, linking against the OpenSSL libraries (libssl and libcrypto):

```
gcc -o randomizer randomizer.c -lssl -lcrypto
```
### 💻 Usage
Simply execute the compiled binary:

```
./randomizer [OPTIONS]

-h, --help Displays detailed usage instructions and exits.
-n, --nospace Outputs the decimal bytes as a single, contiguous, large number (removes spaces).
```
### Example Output
The program will output 32 decimal integers, each representing one byte of the SHA-256 hash:
```
201 153 63 17 144 67 187 170 59 114 33 43 51 243 41 234 62 111 201 234 9 248 3 194 104 157 20 49 196 228 80 212
or
20115363171446718717059114334351243412346211120123492483194104157204919622880212
```

### Using the Output
You can easily pipe this output into a script or another program. For example, to use the first integer as a seed for another process:
```
SEED=$(./randomizer | awk '{print $1}')
echo "Using seed: $SEED"
```

### 📜 Technical Details
**Entropy Source:** The total number of nanoseconds elapsed since the epoch (using `clock_gettime(CLOCK_REALTIME)` on Linux or `GetSystemTimePreciseAsFileTime` on Windows). Hashing this large, high-resolution number ensures maximum input variability.

**Hashing Algorithm:** SHA-256 via the **OpenSSL EVP** interface.

**Output Format:** 32 decimal bytes (0-255), separated by spaces by default.
