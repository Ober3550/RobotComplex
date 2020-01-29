// My modulo function that works with negatives in the way I want it to
int MyMod(int a, int b)
{
	int temp = a % b;
	if (a < 0 && temp < 0)
		temp += b;
	return temp;
}
float MyMod(float a, float b)
{
	if (a > b)
	{
		return MyMod(a - b, b);
	}
	else
	{
		return a;
	}
}