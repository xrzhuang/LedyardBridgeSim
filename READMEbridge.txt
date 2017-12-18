cs58 max_z

compilation and clean:
run “make” to compile
run “make clean” to delete executable from compilation

run program:
./bridge num_cars
run any number of “cars” you want as random threads given random directions and random speeds. These threads running will cross the Ledyard Bridge in a synchronized, efficient, safe, and lively fashion.

output:
leaves behind print statements of what is happening on the bridge, which cars arrive when, which cars get on the bridge when, and which cars exit, as well as total number of cars exited.
files:
bridge.c
	main and helper for OneVehicle which is immediately executed by each string 	created, these helper functions are broken up into 3 different phases, 		arriving to the bridge, getting on the bridge, and exiting the bridge.
bridge.h
	car_t struct is used to in an array of structs to retain each car’s randomly	generated properties (license number, speed, direction)

assignment:
	safety - prohibits bad interleaving - cars cannot go in oppo, cannot have		 	  more than max cars on bridge at one time
	liveness - cars get on and cross, if bridge is empty car will get on
	efficiency - makes use of bridge capacity allowing next car to get on bridge 		     if below capacity
	prevents starvation - automatically change flow of traffic every so and so 				       number of cars so no direction starves
