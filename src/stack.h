#pragma once

#include <wax.h>
#include <iostream>

using namespace std;

const int max_layers = 10;

// a layer
struct Layer {
    Wax *wax;
    double thickness;

    Layer (Wax *wax = nullptr, double thickness = 0)
        : wax (wax), thickness (thickness) {}

    void clear () {
        wax = nullptr;
        thickness = 0;
    }
};

// a stack of wax layers
struct Stack {
    Layer layers[max_layers];

    void clear () {
        for (int i = 0; i < max_layers; i++)
            layers[i].clear ();
    }

    double thickness () {
        double total = 0;
        for (int i = 0; i < max_layers; i++)
            total += layers[i].thickness;
        return total;
    }

    void deposit (Wax &wax, double amount) {
        // TODO: actuall do
        layers[0].thickness += amount;
        layers[0].wax = &wax;
    }

    // put a given stack onto this stack
    void deposit_stack (Stack &stack) {
        // TODO: for real
        layers[0].thickness += stack.layers[0].thickness;
        layers[0].wax = stack.layers[0].wax;
    }

    // take a portion of this stack
    Stack take (double amount) {
        // TODO: for real
        Stack taken;
        taken.layers[0].thickness = amount;
        taken.layers[0].wax = layers[0].wax;
        layers[0].thickness -= amount;
        return taken;
    }
};
