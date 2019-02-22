# Radix Hash Join

## SIGMOD 2018 Programming Contest Task
- Implementation includes multi-threaded query execution and query optimization

## Task page
http://sigmod18contest.db.in.tum.de/task.shtml

## Compilation
```
make
```

## To execute with 'small' workload
```
./main < input.txt
```

- If you want to suppress the output of everything but the sums, use:
```
./main < input.txt 2> /dev/null
```

## Execute with harness
```
cd workload
make harness
sh runHarness.sh
```

## Contributors
https://github.com/PanosEvange

https://github.com/stathismast

https://github.com/VaggelisSpi
