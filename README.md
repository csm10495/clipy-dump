# clipy-dump

This is a weird little C++ executable to dump the realm database of the Clipy app on mac. I have no idea why there is no easy way to do this otherwise.

# Build:

Make sure you have cmake installed, then:

In the root directory of this repo, run:

```
mkdir build
cd build
cmake ..
```

Then after that run (in the build directory):
```
make -j8
```

and you should have a clipy-dump file in build/.

Run it like:

```
./clipy-dump <PATH_TO_CLIPY_REALM_FILE>
```

# License:
MIT License - Charles Machalow