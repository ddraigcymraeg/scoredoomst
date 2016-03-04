//GHK 
/*Define the 'seed' to be incorporated into the global md5 hash to
be used to verify both the gzscoredoom.exe and gzscoredoom.pk3 files with the
 client when using global hi scores. Basically this just salts the code hash.

INSTRUCTIONS:

Once SDMD5SEED is defined below, compile and rename exe to gzscoredoom.exe 
from gzdoom.exe (doesnt really matter though). Using an md5 utility, get the 
checksum of gzscoredoom.exe, then of gzscoredoom.pk3 and concatenate them. 
Then to this string concatenate the seed string you defined below. Now run 
an md5 encrypter on this combined string to get the global md5 hash of your build.
This global md5 hash is used internally and on the (your) hi score server to verify the 
authenticity of the client. *KEEP THE SEED PRIVATE*

PURPOSES:
A private seed definition makes it much harder for cheaters/hackers to use the source 
code to fake hi score postings using a web client and just the md5s of the exe and the pk3.
This should be sufficient for the player base Scoredoom will have, though through the use of 
packet sniffing, it will be possible to get the generated global md5 hash.

The hi score servers will therefore probably support administration users which can add
trusted players and give them passwords for their scoredoom client (as well as where to point 
to). 

The next step is to use timestamp encryption where the clients must ensure their clocks match 
the hi score server (and vice versa).

*/
#ifndef __SD_MD5SEED_H__
#define __SD_MD5SEED_H__

#define SDMD5SEED "CHANGEME"

#endif