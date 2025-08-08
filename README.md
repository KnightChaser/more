# More: A Minimal `more(1)` Clone
> A lightweight, minimal implementation of the classic Unix `more` command, written in C.

This project was started as a personal exercise to get familiar with low-level C programming, 
particularly involving direct terminal I/O, file handling, and signal processing. 
I quickly discovered that writing programs like this is far more challenging than it looks(!). 
It has given me a newfound respect for the developers who built the original Unix utilities.

The goal was never to replicate `more(1)` completely, but rather to "taste" this kind of programming. 
As such, this program implements a small but core subset of `more`'s features, and then I respectfully ran away(^_^...;).
Anyway, this project implemented highlights of `more(1)` command, anyway.

<img width="800" height="600" alt="image" src="https://github.com/user-attachments/assets/706ed979-f9e2-4bdc-b703-fbf2e363a0c2" />

(When my program runs like: `ls -al /usr/bin | ./src/more`)

## Build

This project uses Meson, the C/C++ buildd system.
```
meson setup builddir --native-file=clang.ini
cd builddir
meson compile
meson test
```

The final executable will be located at `build/src/more`.

## Usage

You can provide input to "my version of `more(?)`" in two ways, like a real `more(1)` command.

1.  **By file path:**
    ```sh
    ./build/src/more <your-file-name>
    ```

2.  **Via a pipe (`|`):**
    ```sh
    cat <your-file-name> | ./build/src/more
    ls -la | ./build/src/more
    ```
    *Note: When using a pipe, features that require seeking (like percentage display and searching) are disabled.*

### Interactive Commands

Once the viewer is open, you can use the following keys to navigate:

| Key         | Action                               |
|-------------|--------------------------------------|
| **Space**   | Show the next full page of text.     |
| **Enter**   | Show the next single line of text.   |
| **/**       | Search forward for a pattern.        |
| **q**       | Quit the program.                    |

The display also automatically adjusts if you resize the terminal window.
