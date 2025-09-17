Ciphers Project
- A C++ program that encodes and decodes text using classical cryptography methods.

How It Works
- Caesar Cipher: Shifts letters by a fixed number (e.g., A→C with shift 2)

- Substitution Cipher: Maps each letter to a random different letter

- Decryption:

- Caesar: Tries all 25 shifts, checks against dictionary

- Substitution: Uses hill-climbing algorithm with quadgram scoring to find best match

Commands
C - Caesar encrypt

D - Caesar decrypt

A - Apply random substitution

S - Decrypt substitution cipher

F - Decrypt from file

E - Score English text

R - Set random seed

Run
bash - make run_ciphers
