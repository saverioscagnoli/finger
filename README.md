# finger

A replica of the finger command in linux.
The finger command shows info for the specified user, and all the users online if not specified.

This was made as a part of the Operative Systems II computer science course at Sapienza Università di Roma.

## Installation

```
git clone https://github.com/saverioscagnoli/finger
cd finger
```

## Usage

compile it using any C compiler. Example with gcc:

```
gcc -o ./bin/finger src/main.c src/list.c src/user.c src/utils.c
```

and to run it:

```
./bin/finger
```

output:

```
Login     Name      Tty    Idle     Login Time         Office   Office Phone
svscagn   Saverio   tty1   3h 50m   Wed Jun 26 17:57   *        *
```

## Documentation

For additional info and use of the flags see https://linux.die.net/man/1/finger

## License

MIT License 2024 Saverio Scagnoli
