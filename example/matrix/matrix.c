int main() {
    int n1;
    int m1;
    int n2;
    int m2;
    scanf("%d%d", n1, m1);
    int A[30][30];
    int i = 0;
    int j;
    while(i < n1) {
        j = 0;
        while (j < m1) {
            scanf("%d", A[i][j]);
            j = j + 1;
        }
        i = i + 1;
    }

    scanf("%d%d", n2, m2);
    int B[30][30];
    i = 0;
    while(i < n2) {
        j = 0;
        while (j < m2) {
            scanf("%d", B[i][j]);
            j = j + 1;
        }
        i = i + 1;
    }

    if ( m1 != n2 ) {
        printf("Incompatible Dimensions\n");
        return 0;
    }

    i = 0;
    int C[30][30];
    while (i < n1) {
        j = 0;
        while (j < m2) {
            C[i][j] = 0;
            j = j + 1;
        }
        i = i + 1;
    }

    i = 0;
    int k = 0;
    while (i < n1) {
        j = 0;
        while (j < m1) {
            k = 0;
            while (k < m2) {
                C[i][k] = C[i][k] + A[i][j] * B[j][k];
                k = k + 1;
            }
            j = j + 1;
        }
        i = i + 1;
    }

    i = 0;
    while (i < n1) {
        j = 0;
        while (j < m2) {
            printf("%10d", C[i][j]);
            j = j + 1;
        }
        printf("\n");
        i = i + 1;
    }


    return 0;
}
