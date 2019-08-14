make:
	gcc -o otp_enc_d otp_enc_d.c -Wall
	gcc -o otp_enc otp_enc.c -Wall

edit:
	vim otp_enc.c
editd:
	vim otp_enc_d.c
