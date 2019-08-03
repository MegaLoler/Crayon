#pragma once

#include <wax.h>
#include <vec.h>
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

    bool empty () {
        return wax == nullptr || thickness == 0;
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

    int get_top () {
        int top;
        for (int i = max_layers - 1; i >= 0; i-- ) {
            top = i;
            if (!layers[i].empty ())
                break;
        }
        return top;
    }

    void deposit (Wax *wax, double amount) {
        // first find the topmost layer
        int top = get_top ();

        // if its empty init it for this wax
        // if its non empty with different wax use the layer above
        if (layers[top].empty ()) {
            layers[top].thickness = 0;
        } else if (layers[top].wax != wax) {
            top++;
        }

        // add wax to this layer...
        layers[top].thickness += amount;
        layers[top].wax = wax;

        // TODO: deal with ovelflow and layers too thin....
    }

    // put a given stack onto this stack
    void deposit_stack (Stack &stack) {
        for (int i = 0; i < max_layers; i++) {
            Layer &layer = stack.layers[i];
            if (!layer.empty ())
                deposit (layer.wax, layer.thickness);
        }
    }

    // flip the order of the layers
    Stack flip () {
        Stack flipped;
        for (int i = max_layers - 1; i >= 0; i--) {
            Layer &layer = layers[i];
            if (!layer.empty ())
                flipped.deposit (layer.wax, layer.thickness);
        }
        return flipped;
    }

    // take a portion of this stack
    Stack take (double amount) {
        // gather the topmost layers upside down
        Stack stack;
        int top = get_top ();
        while (amount > 0 && top >= 0) {
            double layer_amount;
            if (layers[top].thickness >= amount)
                layer_amount = amount;
            else
                layer_amount = layers[top].thickness;
            amount -= layer_amount;
            stack.deposit (layers[top].wax, layer_amount);
            layers[top].thickness -= layer_amount;
            top--;
        }
        if (amount > 0) {
            cerr << "Tried to take too much wax! " << amount << endl;
        }
        return stack.flip ();

        //Stack taken;
        //taken.layers[0].thickness = amount;
        //if (amount)
        //    taken.layers[0].wax = layers[0].wax;
        //layers[0].thickness -= amount;
        //return taken;
    }

    Vec render (Vec bg_color) {
        Vec color = bg_color;
        for (int i = 0; i < max_layers; i++) {
            if (layers[i].empty ())
                continue;
            Wax *wax = layers[i].wax;
            Vec wax_color = wax->color;
            double wax_thickness = layers[i].thickness;
            Vec unit (1, 1, 1);
            Vec transmittance = (wax_color * wax->t).pow (wax_thickness) * color;
            Vec reflectance = unit - (unit - wax_color).pow (wax_thickness * wax->s);
            color = reflectance + transmittance;
        }
        return color;
    }
};
