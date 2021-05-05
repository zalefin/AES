# Advanced Encryption Standard
A C implementation of the 128-bit version of the Advanced Encryption Standard algorithm also known as
Rijndael.

This repository includes the source for both `aes_encrypt` and `aes_decrypt` which act
as working tools to encrypt and decrypt arbitrary sized files on the local file system.

## Building
- Running `make` in the repository root directory will compile the primary `aes_encrypt` and
`aes_decrypt` binaries.
- Running `make tests` will compile the `tests` binary which tries to encrypt and decrypt
some random data a large number of times, checking for any inconsistency.

## Usage
- The encryptor binary can be executed by running `./aes_encrypt [PATH] [KEY]`.
The encrypted contents will be saved to `[PATH].AESCIPHER`
- The decryptor binary can be executed by running `./aes_encrypt [PATH] [KEY]`.
The decrypted contents will be saved to `[PATH].AESPLAIN`
