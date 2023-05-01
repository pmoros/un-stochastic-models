# NS3 simulation for ad-hoc networks

# Use the dev container

Oneliner to run the project.

`cd /workspaces/un-stochastic-models/workshop-01/ && export LAST_DIR=$(pwd); cp simulation.cc /usr/ns3/ns-3.26/scratch/my-simulation.cc; cd /usr/ns3/ns-3.26/; (./waf --run scratch-simulator && ./waf --run my-simulation); cd ${LAST_DIR}`

## Configure the project

### Using commands

`docker pull pmoros/ns3`
`docker run --rm -it -v pwd:/work pmoros/ns3  `

_You need to execute the cd to reach the NS3 directory_

### Using the Dockerfile

The Dockerfile uses the `simulation.cc` file and copies it to the scratch folder. You can modify the Dockerfile to use your own simulation. The container prints the results to the console, which you can redirect to a file so as to save the results. In the console it appears when the build finishes, however it takes a while for the simulation to run and the results to be printed.

`docker build -t my-tag .`

`docker run --rm -it -v my-tag`

## Adding a new simulation

In the folder `examples` you can find some examples of simulations, these are written in C++. Once you executed the commands above to configure the project, get inside the NS3 folder `/usr/ns3/ns-3.26`. Once in the NS3 folder there is a `waf` executable, which compiles, runs and does many things for NS3.

In order to run a simulation you gotta put your `*.cc` file with the simulation code inside a folder `scratch\my-simulation`, this must have the same name as your `.cc` file. So as to build it you run `./waf --run scratch-simulator`, once it builds, you run it using `./waf --run my-simulation`.

## Example

In the Docker image there is already a simulation example in the scratch folder `manet-routing-compare`

- `cd /usr/ns3/ns-3.26`
- `./waf --run scratch-simulator`
- `./waf --run manet-routing-compare`

This example simulation will run for 200 seconds and outputs the results to a CSV in the working directory.

## Extra

You can modify the Dockerfile to make this easy to follow. The example code comes from https://www.youtube.com/watch?v=WaMK5Af_qh0
