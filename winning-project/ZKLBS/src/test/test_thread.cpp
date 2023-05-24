#include <iostream>
#include <thread>
using namespace std;

void output(int i)
{
    cout << i << endl;
}

int main()
{    
    for (int i = 0; i < 4; i++)
    {
        thread t(output, i);
        t.detach();    
    }        
    getchar();   //换行符
    return 0;
}