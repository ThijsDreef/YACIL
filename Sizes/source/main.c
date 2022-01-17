struct Circle {
    int radius;
    int x;
    int y;
};

struct Rectangle {
    int x;
    int y;
    int w;
    int h;
};

enum ShapeVariant {
    RECTANGLE = 44,
    CIRCLE = 34,
};

union ShapeUnion {
    struct Circle circle;
    struct Rectangle rectangle;
};

struct Shape {
    enum ShapeVariant variant;
    union ShapeUnion shape;
};

int checkCollision(struct Shape one, struct Shape two) { return 0; }
void updateShapes(struct Shape component) { }