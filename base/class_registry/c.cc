class A {};
class B {};

template <typename T>
A* f(A* p) {
    return p;
}

template <typename T>
A* f(B* p) {
    return p;
}
