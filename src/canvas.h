// stores ur drawing
class Canvas {
    private:
        int width;
        int height;
        double *background; // the paper texture, a height map
        double *wax;        // how much wax is deposited on the canvas at different spots

        virtual void draw () const;
        
    public:
        Canvas (int width, int height);
        ~Canvas ();

        void generate_background (); // generates the background txture
};
