# fl
fl is a command line tool for using IO functions on set of files

# Building fl

```
$ make
```

# Invoking fl

```
fl [file1] [file2]
```

# Commands
| Command | Args | Example | Description | stdio |
| ------- | --------- | ------- | ----------- | ------------- |
| help | _none_ | | Show help | |
| info | _none_ | | Show info about the files | |
| open | ID&nbsp;MODE | open&nbsp;1&nbsp;r+ | Open the file. **MODE** is passed directly to fopen() call, so all modes are supported | fopen() |
| lock | ID&nbsp;OPERATION | lock&nbsp;1&nbsp;shnb | Lock the file. Valid operations are **sh**, **ex**, **un**. **nb** can be added to any operation | flock() |
| close | ID | close 1 | Close the file | fclose() |
| seek | ID&nbsp;OFFSET&nbsp;WHENCE | Set the file position indicator | fseek() |
| tell | ID | tell&nbsp;ID | Show the file position indicator | ftell() |
| write | | | Not implemented yet | fwrite() |
| read | ID&nbsp;x&nbsp;LENGTH | read 0 x 13 | Read **LENGTH** bytes from the file. **x** is optional. If **x** is set, output will be HEX-formatted like **hexdump -c** does | fread() |
| truncate | ID&nbsp;LENGTH | truncate 0 100 | Set file size to **LENGTH** bytes | ftruncate() |
