# NS3 simulation for ad-hoc networks
## Configure the project
### Using commands

`docker pull pmoros/ns3`
`docker run --rm -it -v pwd:/work pmoros/ns3  `

*You need to execute the cd to reach the NS3 directory*

### Using the Dockerfile

`docker build -t my-tag .`

`docker run --rm -it -v my-tag`

## Adding a new simulation
In the folder `examples` you can find some examples of simulations, these are written in C++. Once you executed the commands above to configure the project, get inside the NS3 folder `/usr/ns3/ns-3.26`. Once in the NS3 folder there is a `waf` executable, which compiles, runs and does many things for NS3.

In order to run a simulation you gotta put your `*.cc` file with the simulation code inside a folder `scratch\my-simulation`, this must have the same name as your `.cc` file. So as to build it you run `./waf --run scratch-simulator`, once it builds, you run it using `./waf --run my-simulation`.

## Example

In the Docker image there is already a simulation example in the scratch folder `manet-routing-compare`

* `cd /usr/ns3/ns-3.26`
* `./waf --run scratch-simulator`
* `./waf --run manet-routing-compare`

This example simulation will run for 200 seconds and outputs the results to a CSV in the working directory.

## Extra

You can modify the Dockerfile to make this easy to follow. The example code comes from https://www.youtube.com/watch?v=WaMK5Af_qh0
