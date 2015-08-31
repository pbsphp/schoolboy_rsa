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
    sprintf(key, "%s;%s", mpz_get_str(NULL, 36, e), mpz_get_str(NULL, 36, n));
}


/* Converts string key to key pair */
void set_key(const char *key, mpz_t e, mpz_t n)
{
    /* TODO: remove hardcoded size */
    char buffer1[512];
    char buffer2[512];
    sscanf(key, "%s;%s", buffer1, buffer2);
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
    mpz_nextprime(p, p);
    mpz_setbit(p, (KEY_SIZE / 2) - 1);
    gmp_randseed_ui(rs, get_seed());
    mpz_urandomb(q, rs, KEY_SIZE / 2);
    mpz_setbit(q, (KEY_SIZE / 2) - 1);
    mpz_nextprime(q, q);


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


int main()
{
    mpz_t n;
    mpz_t e;
    mpz_t d;
    mpz_init(n);
    mpz_init(e);
    mpz_init(d);

    generate_keys(e, d, n);

    char buffer1[512];
    char buffer2[512];

    get_key(buffer1, e, n);
    get_key(buffer2, d, n);

    printf("%s\n%s\n", buffer1, buffer2);

    mpz_clear(n);
    mpz_clear(e);
    mpz_clear(d);

    return 0;
}
