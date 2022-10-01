#waitd

Run a command after a period of inactivity.

## Run

``` sh
waitd [OPTIONS]
```

* `-w` or  `--wait`
  * how long to wait for until running the first command, in seconds.
  * Required
* `-e` or  `--exec-on-wait`
  * command to be executed after the period of not receiving input.
  * Required
* `-p` or  `--poll-rate`
  * how long to wait before checking if there has been any input.
  * Optional


## Build and install

In order to compile and instal run `make install` (as root if needed). Use the
`PREFIX` environment variable to choose installation directory.
