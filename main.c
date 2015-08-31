#include <stdio.h>
#include <gmp.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>


/* Key size in bits */
#define KEY_SIZE 1024
/* Max source data size in bytes */
#define MAX_SOURCE_SIZE ((KEY_SIZE - 2) / 8)
/* Public exponent */
#define PUB_EXP 65537



int get_seed()
{
    static increment = 0;
    return time(NULL) + increment++;
}


/* Converts key {X, n} to string */
void get_key(char *key, const mpz_t e, const mpz_t n)
{
    sprintf(key, "%s %s", mpz_get_str(NULL, 36, e), mpz_get_str(NULL, 36, n));
}


/* Converts string key to key pair */
void set_key(const char *key, mpz_t e, mpz_t n)
{
    /* TODO: remove hardcoded size */
    char buffer1[1024];
    char buffer2[1024];
    sscanf(key, "%s %s", buffer1, buffer2);
    mpz_set_str(e, buffer1, 36);
    mpz_set_str(n, buffer2, 36);
}


/* Generates public and private keys */
void generate_keys(mpz_t public_exponent,
        mpz_t private_exponent, mpz_t modulus)
{
    mpz_t p;
    mpz_t q;
    mpz_t phi;
    mpz_init(p);
    mpz_init(q);
    mpz_init(phi);

    gmp_randstate_t rs;
    gmp_randinit_default(rs);

    /* Generate p and q values.
    They should be:
    1) At least (KEY_SIZE / 2) bits
    2) Greater bit must be set to 1
    3) Prime numbers */
    gmp_randseed_ui(rs, get_seed());
    mpz_urandomb(p, rs, KEY_SIZE / 2);
    mpz_setbit(p, (KEY_SIZE / 2) - 1);
    do {
        mpz_nextprime(p, p);
    } while (mpz_probab_prime_p(p, 1000) == 0);

    gmp_randseed_ui(rs, get_seed());
    mpz_urandomb(q, rs, KEY_SIZE / 2);
    mpz_setbit(q, (KEY_SIZE / 2) - 1);
    do {
        mpz_nextprime(q, q);
    } while (mpz_probab_prime_p(q, 1000) == 0);


    /* Modulus */
    mpz_mul(modulus, p, q);

    /* Euler function */
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(phi, p, q);

    /* Public exponent */
    mpz_set_ui(public_exponent, PUB_EXP);

    /* Private exponent */
    mpz_invert(private_exponent, public_exponent, phi);

    gmp_randclear(rs);

    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(phi);
}


/* Encrypts source with key and writes it to buffer */
void encrypt(char *buffer, const char *source, const char *key)
{
    mpz_t e;
    mpz_t n;
    mpz_t number;
    mpz_init(e);
    mpz_init(n);
    mpz_init(number);

    /* Convert string key to {e; n} pair */
    set_key(key, e, n);

    /* Read source bytes as one big number */
    mpz_import(number, strlen(source) + 1, 1, sizeof(char), 1, 0, source);

    /* Encrypt number by {e; n} pair */
    mpz_powm(number, number, e, n);

    /* Put encrypted data to buffer as number with base 36 */
    strcpy(buffer, mpz_get_str(NULL, 36, number));

    mpz_clear(e);
    mpz_clear(n);
    mpz_clear(number);
}


void decrypt(char *buffer, const char *ciphertext, const char *key)
{
    mpz_t d;
    mpz_t n;
    mpz_t number;
    mpz_init(d);
    mpz_init(n);
    mpz_init(number);

    /* Convert string key to {d; n} pair */
    set_key(key, d, n);

    /* Read ciphertext as big number with base 36 */
    mpz_set_str(number, ciphertext, 36);

    /* Encrypt number by {d; n} pair */
    mpz_powm(number, number, d, n);

    /* Store decrypted number to buffer */
    mpz_export(buffer, NULL, 1, strlen("This is source text. Looks like it works fine!") + 1, 1, 0, number);

    mpz_clear(d);
    mpz_clear(n);
    mpz_clear(number);
}


int main()
{
    mpz_t n;
    mpz_t e;
    mpz_t d;
    mpz_init(n);
    mpz_init(e);
    mpz_init(d);

    generate_keys(e, d, n);

    char public_key[2048];
    char private_key[2048];

    get_key(public_key, e, n);
    get_key(private_key, d, n);

    printf("Public key:\n%s\n\n", public_key);
    printf("Private key:\n%s\n\n", private_key);


    const char *str = "This is source text. Looks like it works fine!";

    printf("Source text:\n%s\n\n", str);

    char encrypted_buffer[2048];
    char decrypted_buffer[2048];

    encrypt(encrypted_buffer, str, public_key);
    printf("Cyphertext:\n%s\n\n", encrypted_buffer);
    decrypt(decrypted_buffer, encrypted_buffer, private_key);
    printf("Decoded text:\n%s\n\n", decrypted_buffer);

    mpz_clear(n);
    mpz_clear(e);
    mpz_clear(d);

    return 0;
}
