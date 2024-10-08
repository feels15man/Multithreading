# Multithreading
•	Вычисления (**expressions**) – expr.cpp(ОС Linux, Mutex).

•	Быстрая сортировка (**quickort**) – qsort.cpp(ОС Windows, Semaphore, Crit.section).

•	Сортировка слиянием (**mergesort**) – msort.cpp(ОС Windows, Mutex, Event).

•	Задача о философах (**philosophers**) – phil.cpp(ОС Linux, Semaphore, способ – состояние философов).

## Compile 
 On Windows using MSVC no extra flags
 ```
cl file.cpp -o file
```
 On Linux 
 ```
g++ file.cpp -o file -pthread --std=c++11
```

 On MacOS 
 ```
g++ file.cpp -o file
```

## file formats
  ### input.txt
  1 line - count of threads T                    
  2 line - count of elements of expression or elems to sort N (length _array_)                      
  3 line - _array_ (for sorts N nums separated with space; for expressions N + 1 nums also seperated with space where sum of first N nums = N + 1 num)
  
  ### output.txt
  1 line - count of threads T                          
  2 line - count of elements of expression or elems to sort N (length _array_)                          
  3 line - result (for sorts it is sorted _array_ ; for expressions it a number of possible sign(+/-) placements )

  ### time.txt
  1 line - time in ms
