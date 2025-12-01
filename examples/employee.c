#include <stdio.h>
#include "../ssal.c"

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
