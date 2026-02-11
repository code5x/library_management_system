#ifndef LOGIN_H
#define LOGIN_H

#define LOGIN_USERNAME "Admin@123"

/* SHA-512 hash of password */
#define LOGIN_PASSWORD_HASH \
"$6$jbgY8w7Gg7sGMnj7$oTYzwf9Kl94E3g5CPHkHQ6JZupOJPatLSx/poajPDEXbnc6Z2bQlUFP7zt02HxCTYQTcV0lg4Rtf1AH58HqAZ1"
/* Actual password is "Pass#code123" */

int login();

#endif
