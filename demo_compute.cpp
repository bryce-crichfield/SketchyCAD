#include "Task.h"
#include <iostream>


int main() {
    Task::Context context;

    auto task = []() { std::cout << "Hello World!" << std::endl; };

    context.Submit(task);

    context.Wait();
}