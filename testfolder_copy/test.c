
int main() {

    int test = 0;

    #if FEATURE_B && FEATURE_C && \
    	gcdENABLE_2D
    
       test = 1;
    #else
    
        test = 2;
    #endif


    #if gcdENABLE_2D
    
        test = 3;
    #else
    
        test = 4;
    #endif
    
    return 0;
}