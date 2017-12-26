#include <iostream>

using namespace std;

class Tree {
protected:
    int value;
    int childs;
    Tree* child;

public:
    Tree() {child = NULL; childs = 0;}
    Tree(int childs)  {
        this->childs = childs;
        child = new Tree[childs];
    }
    void set_childs(int childs) {
        this->childs = childs;
    }
    void set_child(Tree* t) {
        child = t;
    }
    int get_childs() {
        return childs;
    }
    Tree* get_child() {
        return child;
    }
    Tree& operator = (const int val) {
        this->value = val;
        return *this;
    }

    Tree& operator[](int i) {
        return *(child + i);
    }
    friend ostream& operator<< (ostream&, const Tree&);
};
ostream& operator<<(ostream& os, const Tree& dt)  {
    os << dt.value;
    return os;
}
class DoubleArray : public Tree {
public:
    DoubleArray() : Tree() {}
    DoubleArray(int childs) : Tree(childs) {}
    DoubleArray(int x, int y) {
        child = new DoubleArray(x);
        childs = x;
        for (int i = 0; i < x; i++) {
            child[i].set_childs(y);
            child[i].set_child(new DoubleArray[y]);
        }
    }
};

int main() {
    const int n = 10;
    const int m = 1;
    DoubleArray arr = DoubleArray(n, m);
    for (int i = 0, s = 1; i < n; i++) {
        for (int j = 0; j < m; j++,s++) {
            arr[i][j] = s;
        }
    }
    for(int i = 0; i < n; i++, cout << '\n') {
        for (int j = 0; j < m; j++) {
            cout << arr[i][j] << ' ';
        }
    }
    return 0;
}
