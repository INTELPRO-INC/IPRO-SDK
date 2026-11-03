/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

/*
 * PSA Crypto RPC of PSA_HANDLE_CRYPTO, shared by the NSPE client and
 * the SPE service. The psa_call type is an enum psa_crypto_fn id. An
 * NSPE re-launch ends every op handle and the client's volatile keys.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/crypto.h"

/* Multipart operations (hash ... kd, aead, pake) the SPE holds at once */
#define PSA_CRYPTO_OP_SLOTS 8u

/*
 * Per id: req fields / in_vec[1..] / out_vec[1] ("-" none). h is
 * op_handle, k key_id; "-> h", "-> k": rsp field set on success.
 */
enum psa_crypto_fn {
    CRYPTO_FN_INIT = 1,              /* - / - / - */
    CRYPTO_FN_KEY_IMPORT,            /* attr / data / - -> k */
    CRYPTO_FN_KEY_DESTROY,           /* k / - / - */
    CRYPTO_FN_KEY_GET_ATTRIBUTES,    /* k / - / psa_key_attributes_t */
    CRYPTO_FN_KEY_EXPORT,            /* k / - / data */
    CRYPTO_FN_KEY_EXPORT_PUBLIC,     /* k / - / data */
    CRYPTO_FN_KEY_GENERATE,          /* attr / - / - -> k */
    CRYPTO_FN_KEY_COPY,              /* k attr / - / - -> k */
    CRYPTO_FN_KEY_PURGE,             /* k / - / - */
    CRYPTO_FN_HASH_COMPUTE,          /* alg / input / hash */
    CRYPTO_FN_HASH_COMPARE,          /* alg / input, hash / - */
    CRYPTO_FN_HASH_SETUP,            /* alg h=0 / - / - -> h */
    CRYPTO_FN_HASH_UPDATE,           /* h / input / - */
    CRYPTO_FN_HASH_FINISH,           /* h / - / hash; end */
    CRYPTO_FN_HASH_VERIFY,           /* h / hash / -; end */
    CRYPTO_FN_HASH_ABORT,            /* h / - / -; end */
    CRYPTO_FN_HASH_CLONE,            /* h / - / - -> h of the clone */
    CRYPTO_FN_MAC_COMPUTE,           /* k alg / input / mac */
    CRYPTO_FN_MAC_VERIFY,            /* k alg / input, mac / - */
    CRYPTO_FN_MAC_SIGN_SETUP,        /* k alg h=0 / - / - -> h */
    CRYPTO_FN_MAC_VERIFY_SETUP,      /* k alg h=0 / - / - -> h */
    CRYPTO_FN_MAC_UPDATE,            /* h / input / - */
    CRYPTO_FN_MAC_SIGN_FINISH,       /* h / - / mac; end */
    CRYPTO_FN_MAC_VERIFY_FINISH,     /* h / mac / -; end */
    CRYPTO_FN_MAC_ABORT,             /* h / - / -; end */
    CRYPTO_FN_CIPHER_ENCRYPT,        /* k alg / input / output */
    CRYPTO_FN_CIPHER_DECRYPT,        /* k alg / input / output */
    CRYPTO_FN_CIPHER_ENCRYPT_SETUP,  /* k alg h=0 / - / - -> h */
    CRYPTO_FN_CIPHER_DECRYPT_SETUP,  /* k alg h=0 / - / - -> h */
    CRYPTO_FN_CIPHER_GENERATE_IV,    /* h / - / iv */
    CRYPTO_FN_CIPHER_SET_IV,         /* h / iv / - */
    CRYPTO_FN_CIPHER_UPDATE,         /* h / input / output */
    CRYPTO_FN_CIPHER_FINISH,         /* h / - / output; end */
    CRYPTO_FN_CIPHER_ABORT,          /* h / - / -; end */
    CRYPTO_FN_AEAD_ENCRYPT,          /* k alg / nonce, ad, plain / out */
    CRYPTO_FN_AEAD_DECRYPT,          /* k alg / nonce, ad, cipher / out */
    /* multipart AEAD: mbedtls 3.6 or TF-PSA-Crypto SPE, else NOT_SUPPORTED */
    CRYPTO_FN_AEAD_ENCRYPT_SETUP,    /* k alg h=0 / - / - -> h */
    CRYPTO_FN_AEAD_DECRYPT_SETUP,    /* k alg h=0 / - / - -> h */
    CRYPTO_FN_AEAD_GENERATE_NONCE,   /* h / - / nonce */
    CRYPTO_FN_AEAD_SET_NONCE,        /* h / nonce / - */
    CRYPTO_FN_AEAD_SET_LENGTHS,      /* h / lengths / - */
    CRYPTO_FN_AEAD_UPDATE_AD,        /* h / input / - */
    CRYPTO_FN_AEAD_UPDATE,           /* h / input / output */
    CRYPTO_FN_AEAD_FINISH,           /* h aux=ct cap / - / ct, tag; end */
    CRYPTO_FN_AEAD_VERIFY,           /* h / tag / plaintext; end */
    CRYPTO_FN_AEAD_ABORT,            /* h / - / -; end */
    CRYPTO_FN_SIGN_HASH,             /* k alg / hash / signature */
    CRYPTO_FN_VERIFY_HASH,           /* k alg / hash, signature / - */
    CRYPTO_FN_SIGN_MESSAGE,          /* k alg / input / signature */
    CRYPTO_FN_VERIFY_MESSAGE,        /* k alg / input, signature / - */
    CRYPTO_FN_ASYM_ENCRYPT,          /* k alg / input, salt / output */
    CRYPTO_FN_ASYM_DECRYPT,          /* k alg / input, salt / output */
    CRYPTO_FN_KD_SETUP,              /* alg h=0 / - / - -> h */
    CRYPTO_FN_KD_GET_CAPACITY,       /* h / - / -; rsp.length */
    CRYPTO_FN_KD_SET_CAPACITY,       /* h aux=capacity / - / - */
    CRYPTO_FN_KD_INPUT_BYTES,        /* h step / data / - */
    CRYPTO_FN_KD_INPUT_KEY,          /* h step k / - / - */
    CRYPTO_FN_KD_KEY_AGREEMENT,      /* h step k / peer key / - */
    CRYPTO_FN_KD_OUTPUT_BYTES,       /* h / - / output, len = request */
    CRYPTO_FN_KD_OUTPUT_KEY,         /* h attr / - / - -> k */
    CRYPTO_FN_KD_ABORT,              /* h / - / -; end */
    CRYPTO_FN_RAW_KEY_AGREEMENT,     /* k alg / peer key / output */
    CRYPTO_FN_GENERATE_RANDOM,       /* - / - / output, len = request */
    CRYPTO_FN_KD_INPUT_INTEGER,      /* h step / u64 value / - */
    /* API 1.2: TF-PSA-Crypto SPE only, else NOT_SUPPORTED */
    CRYPTO_FN_KEY_AGREEMENT,         /* k alg attr / peer key / - -> k */
    CRYPTO_FN_PAKE_SETUP,            /* k h=0 / suite / - -> h */
    CRYPTO_FN_PAKE_SET_ROLE,         /* h aux=role / - / - */
    CRYPTO_FN_PAKE_SET_USER,         /* h / user id / - */
    CRYPTO_FN_PAKE_SET_PEER,         /* h / peer id / - */
    CRYPTO_FN_PAKE_SET_CONTEXT,      /* h / context / - */
    CRYPTO_FN_PAKE_OUTPUT,           /* h step / - / output */
    CRYPTO_FN_PAKE_INPUT,            /* h step / input / - */
    CRYPTO_FN_PAKE_GET_SHARED_KEY,   /* h attr / - / -; end -> k */
    CRYPTO_FN_PAKE_ABORT,            /* h / - / -; end */
    CRYPTO_FN_LAST = CRYPTO_FN_PAKE_ABORT,
};

/*
 * Key attributes on the wire: the public fields of
 * psa_key_attributes_t, the same layout for mbedtls 2.28 and 3.6
 */
struct psa_crypto_attr {
    uint32_t type;               /* psa_key_type_t */
    uint32_t bits;               /* psa_key_bits_t */
    uint32_t lifetime;
    uint32_t id;
    uint32_t usage;
    uint32_t alg;
    uint32_t alg2;               /* enrollment algorithm */
};

/*
 * in_vec[0] of every call. key_id must name a key the client made,
 * else PSA_ERROR_INVALID_HANDLE. aux2 is 0, step and aux are 0 unless
 * the id reads them, else PSA_ERROR_INVALID_ARGUMENT.
 */
struct psa_crypto_req {
    uint32_t key_id;
    uint32_t alg;
    uint32_t op_handle;          /* 1..PSA_CRYPTO_OP_SLOTS, 0 = none */
    uint32_t step;               /* psa_key_derivation_step_t */
    uint32_t aux;                /* KD_SET_CAPACITY: the capacity */
    uint32_t aux2;               /* reserved, 0 */
    struct psa_crypto_attr attr; /* A_IN ids */
};

/*
 * out_vec[0] of every call, copied out on PSA_SUCCESS only. "end"
 * ids free h whatever the status; the client then holds h = 0.
 */
struct psa_crypto_rsp {
    uint32_t op_handle;
    uint32_t key_id;
    size_t length;               /* bytes in out_vec[1] */
    size_t length2;              /* AEAD_FINISH: ciphertext bytes */
};

/*
 * AEAD_FINISH out_vec[1]: req.aux bytes for the ciphertext (unused
 * bytes 0), then the tag; length = req.aux + tag length
 */

/* AEAD_SET_LENGTHS in_vec[1] */
struct psa_crypto_aead_lengths {
    uint32_t ad_length;
    uint32_t plaintext_length;
};

/* PAKE_SETUP in_vec[1]: psa_pake_cipher_suite_t through its accessors */
struct psa_crypto_pake_suite {
    uint32_t alg;
    uint32_t primitive;          /* psa_pake_primitive_t */
    uint32_t key_confirmation;
};

static inline void psa_crypto_attr_pack(struct psa_crypto_attr *w,
                                        const psa_key_attributes_t *a)
{
    w->type = psa_get_key_type(a);
    w->bits = psa_get_key_bits(a);
    w->lifetime = psa_get_key_lifetime(a);
    w->id = (uint32_t)psa_get_key_id(a);
    w->usage = psa_get_key_usage_flags(a);
    w->alg = psa_get_key_algorithm(a);
    w->alg2 = psa_get_key_enrollment_algorithm(a);
}

/* Through the setters: a volatile lifetime clears the id */
static inline void psa_crypto_attr_unpack(psa_key_attributes_t *a,
                                          const struct psa_crypto_attr *w)
{
    *a = psa_key_attributes_init();
    psa_set_key_type(a, (psa_key_type_t)w->type);
    psa_set_key_bits(a, (size_t)w->bits);
    psa_set_key_id(a, (mbedtls_svc_key_id_t)w->id);
    psa_set_key_lifetime(a, w->lifetime);
    psa_set_key_usage_flags(a, w->usage);
    psa_set_key_algorithm(a, w->alg);
    psa_set_key_enrollment_algorithm(a, w->alg2);
}

_Static_assert(sizeof(struct psa_crypto_attr) == 28u, "psa_crypto_attr size");
_Static_assert(offsetof(struct psa_crypto_req, attr) == 24u,
               "psa_crypto_req.attr offset");
_Static_assert(sizeof(struct psa_crypto_req) == 52u, "psa_crypto_req size");
_Static_assert(sizeof(struct psa_crypto_pake_suite) == 12u,
               "psa_crypto_pake_suite size");
