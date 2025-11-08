/* This is a multi-line comment
   testing the parser */

int main() {
    int a, b, c;
    float x = 10.5;
    
    // Single line comment
    a = 5;
    b = 10;
    c = a + b * 2;
    
    /* Conditional statements */
    if (a < b) {
        c = a + b;
    } else {
        c = a - b;
    }
    
    /* While loop */
    while (a < 10) {
        a++;
        b--;
    }
    
    /* For loop */
    for (int i = 0; i < 10; i++) {
        c = c + i;
    }
    
    /* Do-while loop */
    do {
        a = a + 1;
    } while (a < 20);
    
    /* Expressions with various operators */
    a += 5;
    b *= 2;
    c = (a == b) && (c != 0);
    
    /* Nested conditions */
    if (a > 5 || b <= 10) {
        if (c >= 0) {
            a = a % 3;
        }
    }
    
    /* Array operations */
    int arr[10];
    arr[0] = 100;
    
    /* Function calls */
    printf("Value: %d", a);
    
    return 0;
}
