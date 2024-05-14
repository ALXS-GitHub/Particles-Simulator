# Notes

## 14/05/2024

- I tried to compare compiling with g++ and MSVC, and here are the results on the same simulation (at the time not optimized):

How many particles until the fps goes under 30 :

    - MSVC : 500
    - g++ : 950

So, g++ is more performant than MSVC, that's why I will use it for the future.