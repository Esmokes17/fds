# FileD
This is a file hashing distribution

You could give it a file then it gives you list of your files hashes from
the Merkletree and also you could prove the file with that hashes and index bits.
(also in future will be added network to it)

Note: this project in development

## Quick start
```console
PS > ./build.bat
PS > cat .\test.txt
some text for stupid test

PS > .\main.exe .\temp.txt 1
**************************
Hash 0 '0347f3d5652e28ca37330f67d8fe87b5f4492c96ada04ec966c6b9a88c9dcb42' is written!
Hash 1 '7b70eeeba1b0a648b12137976ca2a1e3e24e5bd0d6f985da89e2a0a7825956f0' is written!
Hash 2 '2582cfe117dff6fc99f0d2038d443bf70fd10b24271ee9f0bd24b53e35ce64f4' is written!
Root hash 'ec4c75be343f9df38a455fa49a987a3c38334169d2c30cf5029fe68511bb197b' is written!
Index '01011' is written!

PS > .\test.exe
Hash 0 '0347f3d5652e28ca37330f67d8fe87b5f4492c96ada04ec966c6b9a88c9dcb42' is read!
Hash 1 '7b70eeeba1b0a648b12137976ca2a1e3e24e5bd0d6f985da89e2a0a7825956f0' is read!
Hash 2 '2582cfe117dff6fc99f0d2038d443bf70fd10b24271ee9f0bd24b53e35ce64f4' is read!
Root hash 'ec4c75be343f9df38a455fa49a987a3c38334169d2c30cf5029fe68511bb197b' is read!
index '01011' is read!
Proof root hash: ec4c75be343f9df38a455fa49a987a3c38334169d2c30cf5029fe68511bb197b
```

## TODO
[ ] Compatible for unix
[ ] better interface
[ ] network