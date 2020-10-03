# waitd
A daemon to monitor user input (keyboard or mouse).

## What does it do
* After a certain period of inactivity the daemon will run any command/script supplied.
* Will run a command upon recieving the first input after the period of inactivity.

Note: daemon can execute a command after not recieving input, after finally recieving input or both (or neither).

## Features
* Lightweight
* Does not depend on X to operate.
