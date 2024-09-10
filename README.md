![Banner image](.github/images/screenshot.png)

# Tetragon

This is the repository where I learn OpenGL using C++. In no way 
should this be considered a serious project with any purpose beside learning.

## Building

...

## Log icons
Tetragon utilizes emojis in its logs message format. To see them, you can install
and use [Nerd fonts](https://www.nerdfonts.com/) in your terminal.
Alternatively you can remove emojis by enabling `NO_LOG_EMOJIS` CMake option.

```sh
cd build
cmake -DNO_LOG_EMOJIS=ON .. && make
```

### Normal:
![Screenshot of log message emojis](.github/images/screenshot_emojis.png)

### No emojis:
![Screenshot of log messages without emojis](.github/images/screenshot_no_emojis.png)
