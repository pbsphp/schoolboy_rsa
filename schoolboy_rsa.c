#include <stdio.h>
#include <gmp.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "schoolboy_rsa.h"


/* Public exponent */
#define PUB_EXP 65537
/* Use number with given base for string keys */
#define KEY_NUMBER_BASE 36


static int get_seed()
{
    static int increment = 0;
    return time(NULL) + increment++;
}


/* Converts key {X, n} to string */
static void get_key(char *key, const mpz_t exponent, const mpz_t modulo)
{
    sprintf(key, "%.2048s %.2048s",
            mpz_get_str(NULL, KEY_NUMBER_BASE, exponent),
            mpz_get_str(NULL, KEY_NUMBER_BASE, modulo));
}


/* Converts string key to key pair */
static void set_key(const char *key, mpz_t exponent, mpz_t modulo)
{
    char buf_a[KEY_STRING_SIZE];
    char buf_b[KEY_STRING_SIZE];
    sscanf(key, "%s %s", buf_a, buf_b);
    mpz_set_str(exponent, buf_a, KEY_NUMBER_BASE);
    mpz_set_str(modulo, buf_b, KEY_NUMBER_BASE);
}


/* Generates public and private mpz_t keys */
static void generate_mpz_keys(mpz_t public_exponent,
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
    mpz_import(number, MAX_SOURCE_SIZE, 1, sizeof(char), 1, 0, source);

    /* Encrypt number by {e; n} pair */
    mpz_powm(number, number, e, n);

    /* Put encrypted data to buffer as number with base KEY_NUMBER_BASE */
    strncpy(buffer, mpz_get_str(NULL, KEY_NUMBER_BASE, number), KEY_SIZE);

    mpz_clear(e);
    mpz_clear(n);
    mpz_clear(number);
}


/* Decrypts cyphertext with key and writes it to buffer */
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
    mpz_set_str(number, ciphertext, KEY_NUMBER_BASE);

    /* Encrypt number by {d; n} pair */
    mpz_powm(number, number, d, n);

    /* Store decrypted number to buffer */
    mpz_export(buffer, NULL, 1, MAX_SOURCE_SIZE, 1, 0, number);

    mpz_clear(d);
    mpz_clear(n);
    mpz_clear(number);
}


/* Generates key pair */
void generate_keys(char *public, char *private)
{
    mpz_t pub;
    mpz_t prv;
    mpz_t modulus;
    mpz_init(pub);
    mpz_init(prv);
    mpz_init(modulus);

    generate_mpz_keys(pub, prv, modulus);
    get_key(public, pub, modulus);
    get_key(private, prv, modulus);

    mpz_clear(pub);
    mpz_clear(prv);
    mpz_clear(modulus);
}
