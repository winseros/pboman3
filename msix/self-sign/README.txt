## Preambule

A trusted authority must sign an MSIX package before it may be installed on a system.

A legitimate digital signature can be acquired in a variety of methods.

- Make the bundle available via the Microsoft Store.
- Purchase a code-signing certificate.

But I have a problem here:)

I'm in Russia, and as I write this, sanctions are in effect there. And thus far, none of those methods are available to me because they call for making a payment outside of Russia.

## Remedy

I'll provide you two options for installing MSIX.

1) If you know how to deal with .X509/PowerShell/OpenSSL. Create a self-signed certificate, sign the MSIX file, and add the certificate to the "Cert:\\LocalMachine\Root" store. Profit. 
    These files are for assistance:
    https://github.com/winseros/pboman3/blob/develop/msix/self-sign/GenerateSigningCert.ps1
    https://github.com/winseros/pboman3/blob/develop/msix/self-sign/SignWithCert.ps1

2) If not, I created my own self-signed certificate. You may install it on your machine, and the machine will accept the MSIX.
    - Install the "PBO Manager Trust Cert.cer" to the "Cert:\\LocalMachine\Root". Other kinds of stores won't work.
    - Then install the SIGNED version of the MSIX, i.e., PBOManager.xxx.SIGNED.msix.
    - Profit
