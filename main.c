#include <stdio.h>
#include <gmp.h>
#include <time.h>
#include <string.h>


/* Key size in bits */
#define KEY_SIZE 16
/* Max source data size in bytes */
#define MAX_SOURCE_SIZE ((KEY_SIZE - 2) / 8)



int get_seed()
{
    static increment = 0;
    return time(NULL) + increment++;
}


int main()
{
    mpz_t p;
    mpz_t q;
    mpz_t n;
    mpz_t phi;
    mpz_init(p);
    mpz_init(q);
    mpz_init(n);
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
    mpz_mul(n, p, q);

    /* Euler function */
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(phi, p, q);



    gmp_randclear(rs);

    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(n);
    mpz_clear(phi);

    return 0;
}
