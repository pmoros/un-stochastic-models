# NS3 simulation for ad-hoc networks
## Configure the project
`docker pull ryankurte/docker-ns3`
`docker run --rm -it -v pwd:/work ryankurte/docker-ns3  `

## Run an example
In the folder `examples` you can find some examples of simulations, these are written in C++. Once you executed the commands above to configure the project, get inside the NS3 folder with `cd /usr/ns3/ns-3.26`. Once in the NS3 folder there is a `waf` executable, this compiles, runs and does many things for NS3.

In order to run a simulation you gotta put your `*.cc` file with the simulation code inside a folder `scratch\my-simulation`, this must have the same name as your `.cc` file. So as to build it you run `./waf --run scratch-simulator`, once it builds, you run it using `./waf --run my-simulation`.

## Extra

You can modify the Dockerfile to make this easy to follow. The example code comes from https://www.youtube.com/watch?v=WaMK5Af_qh0
