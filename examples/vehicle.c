#include <assert.h>
#include <stdio.h>
#include "../ssal.c"

class(Vehicle)
class(Car)

interface(Vehicle
  , virtual_method(accelerate, void ,(const *this), { assert(!"unreachable"); }
  , virtual_method(brake, void ,(const *this), { assert(!"unreachable"); }

  , method(startEngine, void ,(const *this) {
    puts("Engine started!");
  })
)

interface(Car, extends(Vehicle)
  , method(accelerate, void ,(const *this) {
    puts("Car: Pressing gas pedal...");
  })
  , static_method(_accelerate, void ,(const Vehicle *this) {
    Car_accelerate(dynamic_cast(Vehicle, Car)(this));
  })

  , method(brake, void ,(const *this) {
    puts("Car: Applying brakes...");
  })
  , static_method(_brake, void ,(const Vehicle *this) {
    Car_brake(dynamic_cast(Vehicle, Car)(this));
  })

  , static_property(vTable, static const Car_VTable, {
    as Vehicle_VTable.accelerate = Car__accelerate,
    as Vehicle_VTable.brake = Car__brake,
  })
)

int main(void) {
  Vehicle_Ptr myCar = {&(Car){} as Vehicle, &Car_vTable as Vehicle_VTable};
  Vehicle_startEngine(myCar._);
  myCar.v->accelerate(myCar._);
  myCar.v->brake(myCar._);
}
