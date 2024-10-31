#include <iostream>
#include <time.h>
using namespace std;

int main()
{
	//srand((unsigned)time(NULL)); //添加当前时间为随机数种子
	int a = 5;
	while (a-- > 0)
	{
		//rand()产生的随机数对 100 取余得到 0-99 的范围
		int n = rand() % 100;	//获取 0-99 的随机数
		cout << "n = " << n << endl;
	}
	return 0;
}
