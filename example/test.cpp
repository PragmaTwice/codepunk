int foo(int y) {
    int x = 0;

    if(y < 10) return 0;

    while(x < y) {
        x ++;
        y -= 2;
    }

    return x * y;
}
