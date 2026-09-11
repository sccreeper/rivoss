int main() {

    int x = 4;
    int y = 3;

    for (int i = 0; i < 100; i++) {
        x = x * y;
        y += 3;
    }

    return 0;
}
