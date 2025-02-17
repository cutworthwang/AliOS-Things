src = Split('''
        ./libalicrypto/mbed/cipher/aes.c
        ./libalicrypto/mbed/hash/hash.c
        ./libalicrypto/mbed/asym/rsa.c
        ./libalicrypto/mbed/mac/hmac.c 		
        ./libalicrypto/sw/ali_crypto_rand.c
        ./libalicrypto/ali_crypto.c 			
        ./mbedtls/library/aes.c
        ./mbedtls/library/md5.c 						
        ./mbedtls/library/sha1.c 					
        ./mbedtls/library/sha256.c 					
        ./mbedtls/library/hash_wrap.c 				
        ./mbedtls/library/bignum.c 					
        ./mbedtls/library/oid.c 						
        ./mbedtls/library/rsa.c 						
        ./mbedtls/library/threading.c 				
        ./mbedtls/library/mbedtls_alt.c 				
        ./mbedtls/library/asn1parse.c 				
        ./mbedtls/library/hmac.c 					
        ./libalicrypto/test/ali_crypto_test.c 		
        ./libalicrypto/test/ali_crypto_test_comm.c 		
        ./libalicrypto/test/ali_crypto_test_aes.c 		
        ./libalicrypto/test/ali_crypto_test_hash.c 		
        ./libalicrypto/test/ali_crypto_test_rand.c 		
        ./libalicrypto/test/ali_crypto_test_rsa.c 		
        ./libalicrypto/test/ali_crypto_test_hmac.c 
''')

incs = Split('''
        ./mbedtls/include/mbedtls
        ./libalicrypto/mbed/inc
        ./libalicrypto/sw
        ./mbedtls/include
	./libalicrypto/test/inc
''')

gincs = Split('''
        ./libalicrypto/inc
''')

comp = aos_component('alicrypto', src)
comp.add_includes(*incs)
comp.add_global_includes(*gincs)
comp.add_macros('CONFIG_CRYPT_MBED=1')
comp.add_macros('CONFIG_DBG_CRYPT=1')
comp.add_global_macros('CONFIG_ALICRYPTO')

