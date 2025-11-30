# ssal.c
Object-Oriented macro abuse in C 

## Usage
`ssal.c` is header-only (despite the name); copy the file somewhere and include it in your project.

## Overview
`ssal.c` defines a plethora of C macros to give shortcuts for writing Object-Oriented flavor code in C, regardless of it being a good idea or not.

Inspired by Objective-C, declaring a class is split across 3 keyword macros:

```c
class(Foo) // typedefs the class, its v-table, and its fat pointer
interface(Foo, ...) // declares the actual structs, definitions, as well as implementation if desired
implementation(Foo, ...) // only defines methods; useful when class definition is split across header and source file
```

There are also 2 keyword macros for casting purposes:

```c
bar as Foo // cast a subclass to a superclass (synonymous to .)
dynamic_cast(Foo, Bar)(&foo) // cast a superclass to a subclass
```

There are also 7 context-sensitive keyword macros to be used within class definitions. This means they do not possess definition outside of a class' direct body, so you can still use these names in code. Use...

```c
extends // ...for extending fields and v-tables of specified superclasses
method // ...for declaring class-named functions that take a pointer to the class
property // ...for declaring fields of the class
static_method // ...for declaring class-named functions w/o a default class-pointer argument (not actually static; storage can be specified by the user)
static_property // ...for declaring class-named global variables (not actually static)
type // ...for declaring class-named subtypes (mostly for shorthand/convenience)
virtual_method // ...for declaring class-named functions that take a pointer to the class as well as become fields within the class' v-table
```

Actual usage varies wildly between context-sensitive keywords. Here are a couple examples of valid usage:

```c
extends(Foo, Bar) // has the current class inherit fields and v-tables from classes Foo and Bar

// defines a function Baz_write that takes a const Baz * and a FILE *
method(write, void ,(const *this, FILE *stream) {
  fprintf(stream, "Baz@%p", (void *)this);
})

property(x, int) // declares a field x of type int

// defines a function Vector2_make that takes 2 floats
static_method(make, Vector2 ,(float x, float y) {
  return (Vector2){.x = x, .y = y};
})

static_property(active, Allocator *, NULL) // defines a global variable Allocator_active of type Allocator * with value NULL
type(Iterator, char *) // defines a typedef String_Iterator of type char *

// defines a function Baz_write like above, but also puts a field "write" inside Baz_VTable
// (note the comma after the arguments; this change from normal methods is an annoying quirk that's sadly required)
virtual_method(write, void ,(const *this, FILE *stream), {
  fprintf(stream, "Baz@%p", (void *)this);
})
```

## Examples
Below are a couple of classic OOP examples rewritten from C++ to C with `ssal.c`. Original C++ source code can be found on [GeeksforGeeks](https://www.geeksforgeeks.org/cpp/object-oriented-programming-in-cpp/).
### Employee
```c
#include <stdio.h>
#include "ssal.c"

class(Employee)

interface(Employee
  , property(_name, const char *)
  , property(_salary, float)

  , method(init, void ,(*this, const char *name, float salary) {
    this->_name = name;
    this->_salary = salary;
  })

  , method(getName, const char * ,(const *this) { return this->_name; })
  , method(getSalary, float ,(const *this) { return this->_salary; })

  , method(setName, void ,(*this, const char *name) { this->_name = name; })
  , method(setSalary, void ,(*this, float salary) { this->_salary = salary; })

  , method(displayDetails, void ,(const *this) {
    printf("Employee: %s\n", this->name);
    printf("Salary: %g\n", this->salary);
  })
)

int main(void) {
  Employee emp; Employee_init(&emp, "Geek", 10000.0f);
  Employee_displayDetails(&emp);

  Employee_setSalary(&emp, 101.0f);
  Employee_setName("Keeg");

  printf("Employee Salary: %g\n", Employee_getSalary(&emp));
  printf("Employee Name: %s\n", Employee_getName(&emp));
}
```

### Vehicle
```c
#include <assert.h>
#include <stdio.h>
#include "ssal.c"

class(Vehicle)
class(Car)

interface(Vehicle
  , virtual_method(accelerate, void ,(const *this), { assert(!"unreachable"); })
  , virtual_method(brake, void ,(const *this), { assert(!"unreachable"); })

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
```

### Animal
```c
#include <stdio.h>
#include "ssal.c"

class(Animal)
class(Dog)

interface(Animal
  , method(eat, void ,(const *this) {
    puts("Animal is eating...");
  })

  , method(sleep, void ,(const *this) {
    puts("Animal is sleeping...");
  })
)

interface(Dog, extends(Animal)
  , method(bark, void ,(const *this) {
    puts("Dog is barking!");
  })
)

int main(void) {
  Dog myDog;
  Animal_eat(&myDog as Animal);
  Animal_sleep(&myDog as Animal);
  Dog_bark(&myDog);
}
```

### Car (split interface/implementation)
Car.h
```c
#ifndef CAR_H
#define CAR_H

#include "Vehicle.h"

class(Car)

interface(Car, extends(Vehicle)
  , method(accelerate, static inline void ,(const *this) {
    puts("Car: Pressing gas pedal...");
  })
  , method(brake, void ,(const *this))
  , static_property(vTable, extern const Car_VTable)
)

#endif
```
Car.c
```c
#include "Car.h"

implementation(Car
  , static_method(_accelerate, void ,(const Vehicle *this) {
    Car_accelerate(dynamic_cast(Vehicle, Car)(this));
  })

  , method(brake, void ,(const *this) {
    puts("Car: Applying brakes...");
  })
  , static_method(_brake, void ,(const Vehicle *this) {
    Car_brake(dynamic_cast(Vehicle, Car)(this));
  })

  , static_property(vTable, const Car_VTable, {
    as Vehicle_VTable.accelerate = Car__accelerate,
    as Vehicle_VTable.brake = Car__brake,
  })
)
```
