# The Sound Player Proxy

This proxy enables Donnie to play audio files formats such as wav, mp3, among others.

# Installation

- 1st) Download [Player](https://github.com/lsa-pucrs/Player) source code and apply the appropriate patch located in https://github.com/lsa-pucrs/Player/tree/master/patch/install; 
- 2nd) Apply the Player Proxies (such as this one) to Player;
- 3rd) Compile Player;
- 4th) Compile Donnie's drivers.

To execute the 2nd step, just move this patch file to player's home directory and run:

```bash
patch -p1 -i instalationProxySound.patch
```
