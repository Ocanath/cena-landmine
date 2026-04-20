## Cena Landmine




## Note on Audio Asset Bundling

Super cool tool called `xxd` that hexdumps a file to a C byte array.
Tool use example:

```bash
xxd -i python/sounds/notification.wav > src/notification.h
```

Creates unsigned char array. 