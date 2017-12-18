// bridge struct

#define MAX_CARS 3
const char *DIRECTION[] = {"TO_NORWICH", "TO_HANOVER"};

typedef struct car {
int licenseplate;
int direction;
int speed;
} car_t;

void *OneVehicle(void *car);
