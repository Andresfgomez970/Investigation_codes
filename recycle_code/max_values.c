int main(int argc, char const *argv[])
{
    printf("%d", sizeof(int));
    printf("Maximum int: %d \n", __INT32_MAX__);
    printf("Maximum size_t: %zu\n", __SIZE_MAX__);
    printf("%16.15g\n", 20.12345678912345);
    printf("%16.15g\n", 6.1); 
    
    return 0;
}
