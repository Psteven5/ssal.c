#include <stdio.h>
#include "../ssal.c"

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
