# Museum-Simulator
Undergraduate Operating Systems Project
Used semaphores to simulate museum and visitor entrances to a museum based on assigned entrance policies.
Entrance policies: 
• a visitor cannot tour a museum without a tour guide, 
• a tour guide cannot open the museum without a visitor, 
• a tour guide leaves when no more visitors are in the museum, 
• a tour guide cannot leave until all visitors in the museum leave, 
• at most two tour guides can be in the museum at a time,
• each tour guide provides a tour for at most ten visitors.

Testing Conventions:
-m: number of visitors
-k: number of tour guides
-pv: probability of a visitor immediately following another visitor
-dv: delay in seconds when a visitor does not immediately follow another visitor
-sv: random seed for the visitor arrival process
-pg: probability of a tour guide immediately following another tour guide
-dg: delay in seconds when a tour guide does not immediately follow another tour guide
-sg: random seed for the tour guide arrival process

Example test case:
2 tour guides and 11 visitors, no visitor delay, 3 sec tour guide delay
./museumsim -m 11 -k 2 -pv 100 -dv 1 -sv 1 -pg 0 -dg 3 -sg 2 &> output.txt
The museum is now empty.
Visitor 1 arrives at time 0.
Visitor 2 arrives at time 0.
Visitor 3 arrives at time 0.
Visitor 4 arrives at time 0.
Visitor 5 arrives at time 0.
Visitor 6 arrives at time 0.
Visitor 7 arrives at time 0.
Visitor 8 arrives at time 0.
Visitor 0 arrives at time 0.
Tour guide 0 arrives at time 0.
Tour guide 0 opens museum for tours at time 0.
Visitor 0 tours museum at time 0.
Visitor 1 tours museum at time 0.
Visitor 2 tours museum at time 0.
Visitor 3 tours museum at time 0.
Visitor 4 tours museum at time 0.
Visitor 5 tours museum at time 0.
Visitor 6 tours museum at time 0.
Visitor 7 tours museum at time 0.
Visitor 8 tours museum at time 0.
Visitor 9 arrives at time 0.
Visitor 9 tours museum at time 0.
Visitor 10 arrives at time 0.
Visitor 0 leaves the museum at time 2.
Visitor 1 leaves the museum at time 2.
Visitor 2 leaves the museum at time 2.
Visitor 3 leaves the museum at time 2.
Visitor 4 leaves the museum at time 2.
Visitor 5 leaves the museum at time 2.
Visitor 6 leaves the museum at time 2.
Visitor 7 leaves the museum at time 2.
Visitor 8 leaves the museum at time 2.
Visitor 9 leaves the museum at time 2.
Tour guide 0 leaves the museum at time 2.
The museum is now empty.
Tour guide 1 arrives at time 3.
Tour guide 1 opens museum for tours at time 3.
Visitor 10 tours museum at time 3.
Visitor 10 leaves the museum at time 5.
Tour guide 1 leaves the museum at time 5.
The museum is now empty.
