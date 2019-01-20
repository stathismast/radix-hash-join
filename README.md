## Ευαγγελίου Παναγιώτης - 1115201500039
## Μαστοράκης Ευστάθιος-Ανδρέας - 1115201500092
## Σπίθας Ευάγγελος - 1115201500147

# Radix Hash Join

## Compile
```
make
```

## Execute with 'small' workload
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
