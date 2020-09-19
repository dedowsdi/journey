
struct Vec2
{
    float x;
    float y;
};

class Rectangle
{
public:

    // use case 1: solve parameter ambiguity

    // This conflict, no way to distinguish the send parameter
    // Recttangle(const Vec2& lb, const Vec2& rt);
    // Recttangle(const Vec2& lb, const Vec2& size);

    static Rectangle lb_rt(const Vec2& lb, const Vec2& rt);

    static Rectangle lb_size(const Vec2& lb, const Vec2& rt);

    // use case 2: make sure object created on heap.
    static Rectangle* create()
    {
        return new Rectangle;
    }

private:

    Rectangle(){}
};

int main( int argc, char *argv[] )
{

    return 0;
}
