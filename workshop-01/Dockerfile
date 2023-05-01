FROM pmoros/ns3:latest

WORKDIR /usr/ns3/ns-3.26

# Copy the simulation code to the container
COPY ./simulation.cc ./scratch/my-simulation.cc

# Build the simulation
RUN ./waf --run scratch-simulator

# Run the simulation as entrypoint
ENTRYPOINT ["./waf", "--run", "my-simulation"]

