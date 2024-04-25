
class CXXTest
{
    __declspec(property(get=get_A, put=put_A))
    int A;

    void put_A(int val);
    int get_A();
};

int main()
{
    return 0;
}
