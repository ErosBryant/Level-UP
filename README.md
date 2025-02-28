```
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
```


```
./db_bench --benchmarks="fillrandom,readrandom,stats" --zip=1
```