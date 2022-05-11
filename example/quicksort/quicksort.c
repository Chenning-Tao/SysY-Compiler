int quick_sort(int a[], int low, int high)
{
	int i = low;	
	int j = high;	
	int key = a[i]; 

	while (i < j)
	{
		while (i < j)
		{
			if (a[j] < key) {
				break;
			}
			
			j = j - 1;
		}
		a[i] = a[j];

		while (i < j)
		{
			if (a[i] > key) {
				break;
			}
			
			i = i + 1;
		}
		a[j] = a[i];
	}
	a[i] = key;
	if (i - 1 > low)
	{
		quick_sort(a, low, i - 1);
	}

	if (i + 1 < high)
	{
		quick_sort(a, i + 1, high);
	}

	return 0;

}


int main()
{

	int N;

	scanf("%d", N);

	int a[10001];

	int i;
	i = 0;
	while (i < N)
	{
		scanf("%d", a[i]);
		i = i + 1;
	}

	
	quick_sort(a, 0, N - 1);

	i = 0;
	while (i < N)
	{
		printf("%d\n", a[i]);
		i = i + 1;
	}
	

	return 0;
}