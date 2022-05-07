int main() {
    int A[700];
    int B[700];
    int C[700];
    int n1;
    int m1;
    int n2;
    int m2;
	scanf("%d%d", n1, m1);
    int i = 0;
    int j;
    while(i < n1) {
        j = 0;
        while (j < m1) {
            scanf("%d", A[i*m1+j]);
            j = j + 1;
        }
        i = i + 1;
    }
	
	scanf("%d%d", n2, m2);
    i = 0;
    while(i < n2) {
        j = 0;
        while (j < m2) {
            scanf("%d", B[i*m2+j]);
            j = j + 1;
        }
        i = i + 1;
    }

	if ( m1 != n2 ) {
		printf("Incompatible Dimensions\n");
		return 0;
	}

    i = 0;
    while (i < n1) {
        j = 0;
        while (j < m2) {
            C[i*m2+j] = 0;
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
                C[i*m2+k] = C[i*m2+k] + A[i*m1+j] * B[j*m2+k];
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
            printf("%10d ", C[i*m2+j]);
            j = j + 1;
        }
        printf("\n");
        i = i + 1;
    }


	return 0;
}
